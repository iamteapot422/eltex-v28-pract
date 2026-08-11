#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/wait.h>


volatile int g_stop = 0;

void handle_stop_signal(int sig)
{
    g_stop = 1;
}

void install_signal_handlers()
{
    struct sigaction sa = { .sa_handler = handle_stop_signal };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

mqd_t create_queue(char* name, bool is_write)
{
    mqd_t ds;
    struct mq_attr queue_attr = {
            .mq_maxmsg = 10,
            .mq_msgsize = 256
        };
    if (is_write)
    {
        ds = mq_open(name, O_CREAT | O_RDWR, 0666, &queue_attr);
    }
    else
    {
        ds = mq_open(name, O_CREAT | O_RDONLY, 0666, &queue_attr);
    }
    if (ds < 0)
    {
        printf("error creating a queue");
    }
    return ds;
}

void p2p(char* queue_in_name, char* queue_out_name)
{
    mqd_t queue_out = create_queue(queue_out_name, true);
    mqd_t queue_in = create_queue(queue_in_name, false);
    struct mq_attr queue_in_attr;
    mq_getattr(queue_in, &queue_in_attr);

    int i = 0;
    while (!g_stop)
    {
        char payload_out[256];
        snprintf(payload_out, sizeof(payload_out), "[%d] Message #%d: My output queue is %s", getpid(), i, queue_out_name);
        int r = mq_send(queue_out, payload_out, strlen(payload_out) + 1, 1);
        if (r < 0)
        {
            printf("Can't send the message\n");
            break;
        }

        char payload_in[queue_in_attr.mq_msgsize];
        unsigned prio;
        int n = mq_receive(queue_in, payload_in, queue_in_attr.mq_msgsize, &prio);
        if (n < 0)
        {
            printf("Can't receive a message\n");
            break;
        }

        if (strncmp(payload_in, "end", 3) == 0)
        {
            printf("Received 'end' message\n");
            fflush(stdout);
            break;
        }

        printf("New message: %s \n", payload_in);
        fflush(stdout);
        i++;
        sleep(1);
    }

    char payload_out[256];
    snprintf(payload_out, sizeof(payload_out), "end");
    mq_send(queue_out, payload_out, strlen(payload_out) + 1, 1);

    mq_close(queue_in);
    mq_close(queue_out);
    mq_unlink(queue_out_name);
}

int main(int argc, char* argv[])
{
    install_signal_handlers();
    char* name = argv[1];
    char name1[32];
    char name2[32];
    snprintf(name1, sizeof(name1), "/%s_1", name);
    snprintf(name2, sizeof(name2), "/%s_2", name);

    int pid = fork();
    if (pid == 0)
    {
        p2p(name1, name2);
    }
    else
    {
        p2p(name2, name1);
        int status;
        waitpid(pid, &status, 0);
    }
    
    return 0;
}