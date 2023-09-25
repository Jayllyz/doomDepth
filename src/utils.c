#include "includes/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

int getInputInt()
{
    char line[BUFFER_SIZE];
    int i;
    if (fgets(line, sizeof(line), stdin)) {
        if (1 == sscanf(line, "%d", &i)) {
            return i;
        }
    }
    return -1;
}

char getInputChar()
{
    char line[BUFFER_SIZE];
    char c;
    if (fgets(line, sizeof(line), stdin)) {
        if (1 == sscanf(line, "%c", &c)) {
            return c;
        }
    }
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