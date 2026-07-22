#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>


typedef struct {
    char* name;
    int mask;
} FileMask;


int get_file_mask(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_mode & 0777;
}
FileMask* collect_masks(const char* dirpath, int* out_count)
{
    DIR* dir = opendir(dirpath);
    FileMask* arr = NULL;
    int count = 0;
    int capacity = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        int mask = get_file_mask(fullpath);

        if (count == capacity)
        {
            capacity = (capacity == 0) ? 4 : capacity * 2;
            FileMask* tmp = realloc(arr, capacity * sizeof(FileMask));
            arr = tmp;
        }

        arr[count].name = malloc(strlen(entry->d_name) + 1);
        strcpy(arr[count].name, entry->d_name);
        arr[count].mask = mask;
        count++;
    }

    closedir(dir);
    *out_count = count;
    return arr;
}
void displaymaskwithnewline(int mask, bool newLine)
{
    for (int i = 8; i >= 0; i--)
    {
        if ((mask >> i) % 2)
            if (i % 3 == 2)
            {
                printf("r");
            }
            else if (i % 3 == 1)
            {
                printf("w");
            }
            else
            {
                printf("x");
            }
        else
        {
            printf("-");
        }
    }
    printf(" ");
    for (int i = 0; i < 8; i++) {
        printf("%d", (mask >> (8 - i)) % 2);
    }
    printf(" ");
    printf("%d", mask);
    if (newLine) {
        printf("\n");
    }
}
void displaymask(int mask) {
    displaymaskwithnewline(mask, true);
}
void display_all_files(FileMask* all_files, int count)
{
    for (int i = 0; i < count; i++)
    {
        displaymaskwithnewline(all_files[i].mask, false);
        printf("\t%s\n", all_files[i].name);
    }
}
int readmaskfromint(int maskt)
{
    int mask = 0;
    for (int i = 0; i < 3; i++)
    {
        int part = (maskt % (int)pow(10, i + 1)) / (int)pow(10, i);
        mask |= part << (i * 3);

    }
    return mask;
}
bool isnumber(const char* str) {
    if (str == NULL || *str == '\0') return false;

    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}
int readmaskfromstring(const char* inp, char* oper, int* maskequal)
{
    bool u = false, g = false, o = false;
    int tmask = 0;
    for (int i = 0; inp[i]; i++)
    {
        switch (inp[i])
        {
            case 'a':
                u = true;
                g = true;
                o = true;
                break;
            case 'u':
                u = true;
                break;
            case 'g':
                g = true;
                break;
            case 'o':
                o = true;
                break;
            case '=':
            case '+':
            case '-':
                *oper = inp[i];
                break;
            case 'r':
                tmask |= 1 << 2;
                break;
            case 'w':
                tmask |= 1 << 1;
                break;
            case 'x':
                tmask |= 1;
                break;
        }
    }
    int nmask = 0;
    if (!(u || g || o))
    {
        u = true;
        g = true;
        o = true;
    }
    if (o)
    {
        nmask += tmask;
        *maskequal += 7;
    }
    if (g)
    {
        nmask += tmask << 3;
        *maskequal += 7 << 3;
    }
    if (u)
    {
        nmask += tmask << 6;
        *maskequal += 7 << 6;
    }
    return nmask;
}
int main() {
    char oper = '=';
    char folder[] = ".";

    int maskscount = 0;
    FileMask* all_masks = collect_masks(folder, &maskscount);
    display_all_files(all_masks, maskscount);

    int file_mask = get_file_mask(".gitignore");
    displaymask(file_mask);
    while (true)
    {
        char filename[100];
        char cmd[10];
        scanf_s("%s %s", filename, (unsigned)sizeof(filename), cmd, (unsigned)sizeof(cmd));
        FileMask* chosenFile = &all_masks[0];
        for (int i = 0; i < maskscount; i++)
        {
            if (strcmp(filename, all_masks[i].name) == 0) {
                chosenFile = &all_masks[i];
            }
        }
        int mask = chosenFile->mask;
        if (isnumber(cmd))
        {
            mask = readmaskfromint(atoi(cmd));
        }
        else
        {
            int maskequal = 0;
            int nmask = readmaskfromstring(cmd, &oper, &maskequal);
            switch (oper)
            {
                case '+':
                    mask |= nmask;
                    break;
                case '=':
                    mask = (mask & (~maskequal)) | nmask;
                    break;
                case '-':
                    mask &= ~nmask;
                    break;
            }
        }
        chosenFile->mask = mask;
        printf("\n");
        display_all_files(all_masks, maskscount);
    }
}

