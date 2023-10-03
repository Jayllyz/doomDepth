#include "includes/ansii_print.h"
#include <string.h>
#include <stdlib.h>


const char *colors[] = {
        "red",
        "green",
        "yellow",
        "blue",
        "reset"
};

const char *colorCodes[] = {
        "\033[31m", // Red
        "\033[32m", // Green
        "\033[33m", // Yellow
        "\033[34m", // Blue
        "\033[0m"   // Reset
};

void changeTextColor(const char *color) {
    if (color != NULL) {
        int numColors = sizeof(colors) / sizeof(colors[0]);
        int i;
        for (i = 0; i < numColors; i++) {
            if (strcmp(color, colors[i]) == 0) {
                printf("%s", colorCodes[i]);
                return;
            }
        }
        printf("Possible colors: ");
        for (i = 0; i < numColors; i++) {
            printf("%s", colors[i]);
            if (i < numColors - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }
}

void printCharAtCoordinate(int x, int y, char c) {
    if (x >= 0 && y >= 0) {
        printf("\033[%d;%dH%c", y, x, c);
    } else {
        printf("x or y is negative\n");
    }
}

void movCursor(int x, int y){
    printf("\033[%d;%dH", y, x);
}

void printStringAtCoordinate(int x, int y, char *s) {
    if (x >= 0 && y >= 0 && s != NULL) {
        movCursor(x, y);
        while (*s) {
            if (*s == '\n') {
                y++;
                movCursor(x, y);
            } else {
                printf("%c", *s);
            }
            s++;
        }
    } else {
        printf("x or y is negative or s is NULL\n");
    }
}

char* readFileContent(FILE* fp) {

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* content = (char*)malloc(file_size + 1);

    fread(content, 1, file_size, fp);

    content[file_size] = '\0';
    return content;
}
