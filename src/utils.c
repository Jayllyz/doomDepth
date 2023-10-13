#include "includes/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 50

int getInputInt()
{
    int value;
    if (1 == scanf("%d", &value))
        return value;
    return -1;
}

char getInputChar()
{
    char *line = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    char c;

    if (fgets(line, BUFFER_SIZE, stdin)) {
        if (1 == sscanf(line, "%c", &c)) {
            free(line);
            return c;
        }
    }

    free(line);
    return '\0';
}

char *getInputString(unsigned size)
{
    char *line = (char *)malloc(sizeof(char) * size + 1);
    if (!line)
        return NULL;

    if (fgets(line, size, stdin)) {
        line[strcspn(line, "\n")] = 0;
        return line;
    }
    free(line);
    return NULL;
}

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[1;1H");
#endif
}