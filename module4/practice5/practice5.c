#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SHM_NAME "/pc_shm"
#define SEM_NAME "/pc_sem"
#define TOTAL_INTS 50
#define MAX_BLOCK_LEN 5
#define SLEEP_SEC 2

int* open_shared_memory()
{
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, TOTAL_INTS * sizeof(int));
    int* mem = mmap(NULL, TOTAL_INTS * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return mem;
}

sem_t* open_semaphore()
{
    return sem_open(SEM_NAME, O_CREAT, 0666, 1);
}

void producer()
{
    int* mem = open_shared_memory();
    sem_t* sem = open_semaphore();

    sem_wait(sem);

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

    sem_post(sem);

    printf("producer: generated %d sets, %d ints used\n", nblocks, pos);

    while (1)
    {
        sem_wait(sem);

        int offset = mem[0];
        int done = 1;
        while (offset != 0)
        {
            if (mem[offset] != 0) { done = 0; break; }
            offset = mem[offset + 1];
        }

        sem_post(sem);

        if (done)
        {
            printf("producer: all sets processed\n");
            break;
        }

        sleep(SLEEP_SEC);
    }

    munmap(mem, TOTAL_INTS * sizeof(int));
    sem_close(sem);
}

void consumer()
{
    int* mem = open_shared_memory();
    sem_t* sem = open_semaphore();

    while (1)
    {
        sem_wait(sem);

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

        sem_post(sem);

        if (found == -1)
        {
            printf("consumer[%d]: all sets processed, exiting\n", getpid());
            break;
        }

        printf("consumer[%d]: set@%d count=%d min=%d max=%d\n", getpid(), found, count, mn, mx);
        sleep(SLEEP_SEC);
    }

    munmap(mem, TOTAL_INTS * sizeof(int));
    sem_close(sem);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    if (argc < 2)
    {
        printf("usage: %s -p | -c\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-p") == 0) producer();
    else if (strcmp(argv[1], "-c") == 0) consumer();

    return 0;
}
