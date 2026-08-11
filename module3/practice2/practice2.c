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

#define SUBSCRIBE 1
#define UNSUBSCRIBE 2
#define SEND 3

typedef struct
{
    long mtype;
    int action;
    int pid;
    char topic[50];
    char payload[200];
} msgbuf;

typedef struct
{
    int pid;
    char topic[50];
} subscription;

int g_stop = 0;

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

msgbuf* create_message(long mtype, int action, int pid, const char* topic, const char* payload)
{
    msgbuf* msg = malloc(sizeof(msgbuf));
    msg->mtype = mtype;
    msg->action = action;
    msg->pid = pid;
    strncpy(msg->topic, topic, 50);
    strncpy(msg->payload, payload, 200);
    return msg;
}

int get_msqid()
{
    key_t key = ftok("test", 65);
    int msqid = msgget(key, 0666 | IPC_CREAT);
    return msqid;
}

void add_subscription(subscription** subs, int* nsubs, int pid, const char* topic)
{
    (*nsubs)++;
    *subs = realloc(*subs, sizeof(subscription) * (*nsubs));
    (*subs)[*nsubs - 1].pid = pid;
    strncpy((*subs)[*nsubs - 1].topic, topic, 50);
}

void remove_subscription(subscription* subs, int* nsubs, int pid, const char* topic)
{
    int found = -1;
    for (int i = 0; i < *nsubs; i++)
    {
        if ((subs[i].pid == pid) && (strcmp(subs[i].topic, topic) == 0))
        {
            found = i;
            break;
        }
    }
    if (found < 0) return;
    for (int i = found + 1; i < *nsubs; i++)
    {
        subs[i - 1] = subs[i];
    }
    (*nsubs)--;
}

ssize_t read_string(int fd, char* buf, size_t maxlen)
{
    size_t i = 0;
    while (i < maxlen - 1)
    {
        ssize_t r = read(fd, buf + i, 1);
        if (r <= 0) return r;
        if (buf[i] == '\0') return i;
        i++;
    }
    buf[i] = '\0';
    return i;
}

void broker()
{
    int msqid = get_msqid();
    int nsubs = 0;
    subscription* subs = NULL;

    printf("broker: started, msqid=%d\n", msqid);
    fflush(stdout);

    while (!g_stop)
    {
        msgbuf message;
        int r = msgrcv(msqid, &message, sizeof(msgbuf) - sizeof(long), 1, 0);
        if (r < 0)
        {
            break;
        }

        if (message.action == SUBSCRIBE)
        {
            add_subscription(&subs, &nsubs, message.pid, message.topic);
            printf("broker: subscribed %d to \"%s\"\n", message.pid, message.topic);
            fflush(stdout);
        }
        else if (message.action == UNSUBSCRIBE)
        {
            remove_subscription(subs, &nsubs, message.pid, message.topic);
            printf("broker: unsubscribed %d from \"%s\"\n", message.pid, message.topic);
            fflush(stdout);
        }
        else if (message.action == SEND)
        {
            for (int i = 0; i < nsubs; i++)
            {
                if (strcmp(subs[i].topic, message.topic) != 0) continue;

                msgbuf* msg = create_message(subs[i].pid, SEND, getpid(), message.topic, message.payload);
                msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
                free(msg);
            }
        }
    }

    msgctl(msqid, IPC_RMID, NULL);
    free(subs);
    printf("broker: stopped\n");
}

void provider(const char* topic)
{
    int msqid = get_msqid();
    int counter = 0;

    printf("provider: sending to \"%s\", pid=%d\n", topic, getpid());
    fflush(stdout);

    while (!g_stop)
    {
        char payload[200];
        snprintf(payload, sizeof(payload), "message #%d from %d", counter++, getpid());

        msgbuf* msg = create_message(1, SEND, getpid(), topic, payload);
        int r = msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
        free(msg);

        if (r < 0)
        {
            break;
        }

        printf("provider: sent \"%s\" to \"%s\"\n", payload, topic);
        fflush(stdout);

        sleep(1);
    }

    printf("provider: stopped\n");
}

void subscriber(char** topics, int ntopics)
{
    int msqid = get_msqid();

    for (int i = 0; i < ntopics; i++)
    {
        msgbuf* msg = create_message(1, SUBSCRIBE, getpid(), topics[i], "");
        msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
        free(msg);
        printf("subscriber: subscribed to \"%s\"\n", topics[i]);
    }
    fflush(stdout);

    while (!g_stop)
    {
        msgbuf message;
        int r = msgrcv(msqid, &message, sizeof(msgbuf) - sizeof(long), getpid(), 0);
        if (r < 0)
        {
            break;
        }

        printf("subscriber [%d] %s: %s\n", getpid(), message.topic, message.payload);
        fflush(stdout);
    }

    for (int i = 0; i < ntopics; i++)
    {
        msgbuf* msg = create_message(1, UNSUBSCRIBE, getpid(), topics[i], "");
        msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
        free(msg);
    }

    printf("subscriber: stopped\n");
}

int main(int argc, char* argv[])
{
    install_signal_handlers();

    if (strcmp(argv[1], "-b") == 0)
    {
        broker();
    }
    else if (strcmp(argv[1], "-p") == 0)
    {
        if (argc < 3)
        {
            printf("too little arguments");
            return 1;
        }
        provider(argv[2]);
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        if (argc < 3)
        {
            printf("too little arguments");
            return 1;
        }
        subscriber(argv + 2, argc - 2);
    }


    return 0;
}