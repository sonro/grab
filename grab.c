const char *USAGE = "USAGE: grab <search_term> [file]";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// linked list for traversing file names in directory
struct node 
{
    const char *file;
    struct node *next;
};

// prototypes
void cleanup_list(struct node *head);

int main(int argc, const char* argv[])
{
    // validate args
    if (argc < 2 || argc > 3)
    {
        printf("%s\n", USAGE);
        return 1;
    }

    // foramt args
    const char *key_str = argv[1];

    struct node *head = malloc(sizeof(struct node));
    if (!head)
    {
        fprintf(stderr, "memory allocation error\n");
        return 2;
    }

    DIR *dp;

    if (argc == 2)
    {
        dp = opendir(".");
        struct dirent *dir;
        if (!dp)
        {
            fprintf(stderr, "could not access current directory\n");
            return 3;
        }

        struct node *ptr = head;
        int first = 1;

        while ((dir = readdir(dp)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                if (first)
                {
                    ptr->file = dir->d_name;
                    first = 0;
                }
                else
                {
                    if (!(ptr->next = malloc(sizeof(struct node))))
                    {
                        fprintf(stderr, "memory allocation error\n");
                        closedir(dp);
                        cleanup_list(head);
                        return 2;
                    }


                    ptr = ptr->next;
                    ptr->file = dir->d_name;
                }
            }
        }

        ptr->next = NULL;
    }
    else
    {
        head->file = argv[2];
        head->next = NULL;
    }


    for (struct node *ptr = head; ptr; ptr = ptr->next)
    {
        FILE *fp = fopen(ptr->file, "r");

        int c;
        char buffer[BUFSIZ];

        int i = 0, line = 1;

        while ((c = fgetc(fp)) != EOF && i < BUFSIZ-1)
        {
            if (c == '\n')
            {
                buffer[i++] = '\0';
                if (strstr(buffer, key_str))
                {
                    printf("%s: %3d: %s\n", ptr->file, line, buffer);
                }
                line++;
                i = 0;
            }
            else
            {
                buffer[i++] = c;
            }
        }

        // close file
        fclose(fp);
    }

    cleanup_list(head);
    if (dp) closedir(dp);
    return 0;
}

void cleanup_list(struct node *head)
{
    if (!head) return;

    struct node *ptr = head->next;
    free(head);
    while (ptr)
    {
        head = ptr;
        ptr = ptr->next;
        free(head);
    }
}
