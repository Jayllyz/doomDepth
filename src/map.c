#include "includes/ansii_print.h"
#include "includes/map.h"
#include "includes/utils.h"

void saveCursorPos()
{
    printf("\033[s");
}

void restoreCursorPos()
{
    printf("\033[u");
}

void clearLine()
{
    printf("\033[K");
}

char signAtCoordinate(const char *map, int x, int y, Map m)
{
    return map[(y - m.map_top) * 11 + (x - m.map_left) / 2];
}

char convertSigntoChar(char sign)
{
    switch (sign) {
    case '0':
        return '.';
    case '1':
        return '#';
    case '2':
        return 'X';
    case '3':
        return '$';
    case '4':
        return '=';
    case '9':
        return '.';
    default:
        return ' ';
    }
}

void setSignColor(char sign)
{
    switch (sign) {
    case '2':
        changeTextColor("red");
        break;
    case '3':
        changeTextColor("green");
        break;
    case '4':
        changeTextColor("yellow");
        break;
    case '9':
        changeTextColor("blue");
        break;
    }
}

void printSignWithColor(char sign)
{
    switch (sign) {
    case '0':
        printf(". ");
        break;
    case '1':
        printf("# ");
        break;
    case '2':
        changeTextColor("red");
        printf("X ");
        changeTextColor("reset");
        break;
    case '3':
        changeTextColor("green");
        printf("$ ");
        changeTextColor("reset");
        break;
    case '4':
        changeTextColor("yellow");
        printf("= ");
        changeTextColor("reset");
        break;
    case '9':
        changeTextColor("blue");
        printf("x ");
        changeTextColor("reset");
        break;
    default:
        printf("  ");
        break;
    }
}

void printMapAtCoordinate(int x, int y, char *m)
{
    if (x >= 0 && y >= 0 && m != NULL) {
        printf("\033[%d;%dH", y, x);
        while (*m) {
            if (*m == '\n') {
                y++;
                printf("\033[%d;%dH", y, x);
            }
            else {
                printSignWithColor(*m);
            }
            m++;
        }
    }
    else {
        printf("x or y is negative or s is NULL\n");
    }
}

void printSignAtCoordinate(char *map, int x, int y, Map m)
{
    setSignColor((signAtCoordinate(map, x, y, m)));
    printCharAtCoordinate(x, y, convertSigntoChar(signAtCoordinate(map, x, y, m)));
    changeTextColor("reset");
}

void printPlayerAtCoordinate(int x, int y)
{
    changeTextColor("blue");
    printCharAtCoordinate(x, y, 'x');
    changeTextColor("reset");
}

void eventHandler(char sign, Map m)
{
    switch (sign) {
    case '0':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("                            ");
        break;
    case '2':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Fight begins!");
        break;
    case '3':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Happy shopping!");
        break;
    case '4':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Welcome to the Smith!");
        break;
    }
}

void movRight(int *x, int *y, char *map, Map m)
{
    printSignAtCoordinate(map, *x, *y, m);
    if (signAtCoordinate(map, *x + 2, *y, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return;
    }
    *x = *x + 2;
    if (*x > m.map_left + m.map_width - 4) {
        *x = m.map_left - 2 + m.map_width - 2; // -2 come from map border
    }
    printPlayerAtCoordinate(*x, *y);
    eventHandler(signAtCoordinate(map, *x, *y, m), m);
}

void movLeft(int *x, int *y, char *map, Map m)
{
    printSignAtCoordinate(map, *x, *y, m);
    if (signAtCoordinate(map, *x - 2, *y, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return;
    }
    *x = *x - 2;
    if (*x < m.map_left + 2) {
        *x = m.map_left + 2;
    }
    printPlayerAtCoordinate(*x, *y);

    eventHandler(signAtCoordinate(map, *x, *y, m), m);
}

void movUp(int *x, int *y, char *map, Map m)
{
    printSignAtCoordinate(map, *x, *y, m);

    if (signAtCoordinate(map, *x, *y - 1, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return;
    }
    *y = *y - 1;
    if (*y < m.map_top + 1) {
        *y = m.map_top + 1;
    }
    printPlayerAtCoordinate(*x, *y);
    eventHandler(signAtCoordinate(map, *x, *y, m), m);
}

void movDown(int *x, int *y, char *map, Map m)
{
    printSignAtCoordinate(map, *x, *y, m);

    if (signAtCoordinate(map, *x, *y + 1, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return;
    }

    *y = *y + 1;

    if (*y + 1 > m.map_top + m.map_height - 2) {
        *y = m.map_top + m.map_height - 2;
    }
    printPlayerAtCoordinate(*x, *y);
    eventHandler(signAtCoordinate(map, *x, *y, m), m);
}

int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top)
{
    Map m = {map_top, map_left, map_width, map_height};

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        return 1;
    }
    FILE *fp2 = fopen(monster, "r");
    if (fp2 == NULL) {
        printf("Could not open file %s\n", monster);
        fclose(fp);
        return 1;
    }

    char *dragon = readFileContent(fp2);

    changeTextColor("red");
    printStringAtCoordinate((map_left + map_width) * 1.5, 0, dragon);
    changeTextColor("reset");

    int x = map_width / 2 + map_left;
    if (x % 2 != map_width % 2) {
        //x++;
    }
    int y = map_height / 2 + map_top;

    int ans = 0;

    char *map;
    map = readFileContent(fp);

    printMapAtCoordinate(map_left, map_top, map);

    printf("\n\n");

    printf("1 ... Up    2 ... Down\n3 ... Left    4 ... Right\n-1 ... Quit\n");
    saveCursorPos();

    do {
        restoreCursorPos();
        clearLine();
        ans = getInputInt();
        clearBuffer();

        switch (ans) {
        case 1:
            movUp(&x, &y, map, m);
            break;
        case 2:
            movDown(&x, &y, map, m);
            break;
        case 3:
            movLeft(&x, &y, map, m);
            break;
        case 4:
            movRight(&x, &y, map, m);
            break;
        }

    } while (ans != -1);

    fclose(fp);

    return 0;
}
