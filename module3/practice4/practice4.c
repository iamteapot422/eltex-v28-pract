#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define TOTAL_INTS 50
#define MAX_BLOCK_LEN 5
#define SLEEP_SEC 2

union semun
{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

int get_shmid()
{
    key_t key = ftok("test", 80);
    return shmget(key, TOTAL_INTS * sizeof(int), IPC_CREAT | 0666);
}

int get_semid()
{
    key_t key = ftok("test", 81);
    return semget(key, 1, IPC_CREAT | 0666);
}

void producer()
{
    int shmid = get_shmid();
    int semid = get_semid();

    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    int* mem = shmat(shmid, NULL, 0);

    struct sembuf lock = { 0, -1, 0 };
    struct sembuf unlock = { 0,  1, 0 };

    semop(semid, &lock, 1);

    int pos = 1;
    int prev = -1;
    int nblocks = 0;

    while (pos + 2 < TOTAL_INTS)
    {
        int room = TOTAL_INTS - pos - 2;
        int max_len = room < MAX_BLOCK_LEN ? room : MAX_BLOCK_LEN;
        int count = 1 + rand() % max_len;

        mem[pos] = count;
        mem[pos + 1] = 0;
        for (int i = 0; i < count; i++)
        {
            mem[pos + 2 + i] = rand() % 100;
        }

        if (prev != -1) mem[prev + 1] = pos;
        prev = pos;
        nblocks++;
        pos = pos + 2 + count;
    }

    mem[0] = nblocks > 0 ? 1 : 0;

    semop(semid, &unlock, 1);

    printf("producer: generated %d sets, %d ints used\n", nblocks, pos);

    while (1)
    {
        semop(semid, &lock, 1);

        int offset = mem[0];
        int done = 1;
        while (offset != 0)
        {
            if (mem[offset] != 0) { done = 0; break; }
            offset = mem[offset + 1];
        }

        semop(semid, &unlock, 1);

        if (done)
        {
            printf("producer: all sets processed\n");
            break;
        }

        sleep(SLEEP_SEC);
    }

    shmdt(mem);
}

void consumer()
{
    int shmid = get_shmid();
    int semid = get_semid();
    int* mem = shmat(shmid, NULL, 0);

    struct sembuf lock = { 0, -1, 0 };
    struct sembuf unlock = { 0,  1, 0 };

    while (1)
    {
        semop(semid, &lock, 1);

        int offset = mem[0];
        int found = -1;
        while (offset != 0)
        {
            if (mem[offset] != 0) { found = offset; break; }
            offset = mem[offset + 1];
        }

        int count = 0, mn = 0, mx = 0;
        if (found != -1)
        {
            count = mem[found];
            mn = mx = mem[found + 2];
            for (int i = 1; i < count; i++)
            {
                int v = mem[found + 2 + i];
                if (v < mn) mn = v;
                if (v > mx) mx = v;
            }
            mem[found] = 0;
        }

        semop(semid, &unlock, 1);

        if (found == -1)
        {
            printf("consumer[%d]: all sets processed, exiting\n", getpid());
            break;
        }

        printf("consumer[%d]: set@%d count=%d min=%d max=%d\n", getpid(), found, count, mn, mx);
        sleep(SLEEP_SEC);
    }

    shmdt(mem);
}

int main(int argc, char* argv[])
{
    srand(time(NULL) ^ getpid());

    if (argc < 2)
    {
        printf("usage: %s -p | -c\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-p") == 0) producer();
    else if (strcmp(argv[1], "-c") == 0) consumer();

    return 0;
}