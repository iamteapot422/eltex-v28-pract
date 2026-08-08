#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
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

msgbuf* create_message(long mtype, int action, int pid, char topic[50], char payload[200])
{
    msgbuf* msg = malloc(sizeof(msgbuf));
    msg->mtype = mtype;
    msg->action = action;
    msg->pid = pid;
    strncpy(msg->topic, topic, 50);
    char* mtext = "test payload";
    strncpy(msg->payload, payload, 200);
    return msg;
}

int get_msqid()
{
    key_t key = ftok("test", 65);
    int msqid = msgget(key, 0666 | IPC_CREAT);
    return msqid;
}

void broker()
{
    int msqid = get_msqid();
    int nsubs = 0;
    subscription* subs;
    while (1)
    {
        msgbuf message;
        msgrcv(msqid, &message, 512, 1, 0);
        if (message.action == SUBSCRIBE)
        {
            char fifo[512];
            snprintf(fifo, "%d", message.pid);
            nsubs++;
            subs = realloc(subs, sizeof(subscription) * nsubs);
            subscription* sub = malloc(sizeof(subscription));
            sub->pid = message.pid;
            strncpy(sub->topic, message.topic, 50);
            subs[nsubs - 1] = *sub;
            printf("subscribed %d %s\n", message.pid, message.payload);
        }
        else if (message.action == UNSUBSCRIBE)
        {
            int found = 0;
            for (int i = 0; i < nsubs; i++)
            {
                if ((subs[i].pid == message.pid) && (strcmp(subs[i].topic, message.topic) == 0))
                {
                    found = i;
                    break;
                }
            }
            for (int i = found + 1; i < nsubs; i++)
            {
                subs[i - 1] = subs[i];
            }

            nsubs--;
            printf("unsubscribed %d \n", nsubs);
            break;
        }
    }
    msgctl(msqid, IPC_RMID, NULL);
}

void provider()
{
    int msqid = get_msqid();
    msgbuf* msg; 
    msg = create_message(1, SUBSCRIBE, getpid(), "test topic", "test payload");
    msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
    msg = create_message(1, UNSUBSCRIBE, getpid(), "test topic", "test payload");
    msgsnd(msqid, msg, sizeof(msgbuf) - sizeof(long), 0);
}

int main(int argc, char* argv[])
{
    int pid = fork();
    if (pid == 0)
    {
        broker();
    }
    else
    {
        provider();
    }

    return 0;
}
