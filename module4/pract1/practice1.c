#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define CHUNK_SIZE 256

ssize_t read_exact(int fd, void* buf, size_t n)
{
    size_t total = 0;
    while (total < n)
    {
        ssize_t r = read(fd, (char*)buf + total, n - total);
        if (r <= 0) return r;
        total += r;
    }
    return total;
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

void send_command(int fd, const char* cmd)
{
    write(fd, cmd, strlen(cmd) + 1);
}

void child_process(int pipe_in, int pipe_out)
{
    char inbuf[512];
    char current_name[512];
    char chunk[CHUNK_SIZE];
    int chunk_size;
    int out_fd = -1;

    send_command(pipe_out, "ready_for_input");

    while (1)
    {
        read_string(pipe_in, inbuf, sizeof(inbuf));

        if (strcmp(inbuf, "prepare_for_filename") == 0)
        {
            read_string(pipe_in, current_name, sizeof(current_name));

            char out_name[600];
            snprintf(out_name, sizeof(out_name), "%s.copy", current_name);

            out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0)
            {
                printf("Error: can't create %s\n", out_name);
            }
        }
        else if (strcmp(inbuf, "prepare_for_size") == 0)
        {
            read_exact(pipe_in, &chunk_size, sizeof(int));

            if (chunk_size == 0)
            {
                if (out_fd >= 0)
                {
                    close(out_fd);
                    out_fd = -1;
                    printf("Copied: %s.copy\n", current_name);
                }
            }
        }
        else if (strcmp(inbuf, "prepare_for_message") == 0)
        {
            read_exact(pipe_in, chunk, chunk_size);
            if (out_fd >= 0)
            {
                write(out_fd, chunk, chunk_size);
            }
        }
        else if (strcmp(inbuf, "end_of_transmission") == 0)
        {
            break;
        }
    }
}

void parent_process(int pipe_in, int pipe_out, int file_count, char* files[])
{
    char inbuf[512];

    read_string(pipe_in, inbuf, sizeof(inbuf));
    if (strcmp(inbuf, "ready_for_input") != 0)
    {
        printf("error, wrong ready_for_input\n");
    }

    for (int i = 0; i < file_count; i++)
    {
        int in_fd = open(files[i], O_RDONLY);
        if (in_fd < 0)
        {
            printf("Error: can't open %s, skipping\n", files[i]);
            continue;
        }

        send_command(pipe_out, "prepare_for_filename");
        write(pipe_out, files[i], strlen(files[i]) + 1);

        char chunk[CHUNK_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(in_fd, chunk, CHUNK_SIZE)) > 0)
        {
            send_command(pipe_out, "prepare_for_size");
            int size = (int)bytes_read;
            write(pipe_out, &size, sizeof(int));

            send_command(pipe_out, "prepare_for_message");
            write(pipe_out, chunk, bytes_read);
        }

        send_command(pipe_out, "prepare_for_size");
        int zero = 0;
        write(pipe_out, &zero, sizeof(int));

        close(in_fd);
    }

    send_command(pipe_out, "end_of_transmission");
}

int main(int argc, char* argv[])
{
    char* pipe_name = NULL;
    char* files[64];
    int file_count = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: -p requires a pipe name argument\n");
                exit(1);
            }
            pipe_name = argv[i + 1];
            i++;
        }
        else
        {
            files[file_count++] = argv[i];
        }
    }

    if (file_count == 0)
    {
        exit(1);
    }

    int use_named = (pipe_name != NULL);
    char fifo1[512];
    char fifo2[512];

    int p1[2];
    int p2[2];

    if (use_named)
    {
        snprintf(fifo1, sizeof(fifo1), "%s_p1", pipe_name);
        snprintf(fifo2, sizeof(fifo2), "%s_p2", pipe_name);

        if (mkfifo(fifo1, 0666) < 0 && errno != EEXIST)
        {
            printf("Error: can't create fifo %s\n", fifo1);
            exit(1);
        }
        if (mkfifo(fifo2, 0666) < 0 && errno != EEXIST)
        {
            printf("Error: can't create fifo %s\n", fifo2);
            exit(1);
        }
    }
    else
    {
        if (pipe(p1) < 0)
        {
            printf("Error");
            exit(1);
        }
        if (pipe(p2) < 0)
        {
            printf("Error");
            exit(1);
        }
    }

    int child_id = fork();
    if (child_id == 0)
    {
        int pipe_in, pipe_out;

        if (use_named)
        {
            pipe_in = open(fifo1, O_RDONLY);
            pipe_out = open(fifo2, O_WRONLY);
        }
        else
        {
            close(p1[1]);
            close(p2[0]);
            pipe_in = p1[0];
            pipe_out = p2[1];
        }

        child_process(pipe_in, pipe_out);
        close(pipe_in);
        close(pipe_out);
    }
    else
    {
        int pipe_in, pipe_out;

        if (use_named)
        {
            pipe_out = open(fifo1, O_WRONLY);
            pipe_in = open(fifo2, O_RDONLY);
        }
        else
        {
            close(p1[0]);
            close(p2[1]);
            pipe_in = p2[0];
            pipe_out = p1[1];
        }

        parent_process(pipe_in, pipe_out, file_count, files);
        close(pipe_in);
        close(pipe_out);
        wait(NULL);

        if (use_named)
        {
            unlink(fifo1);
            unlink(fifo2);
        }
    }
    return 0;
}