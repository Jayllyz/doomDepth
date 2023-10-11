#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/map.h"
#include "includes/utils.h"
#include <stdlib.h>
#include <string.h>

const char *arrowKeyGetName(arrowKey_t arrowKey)
{
    const char *arrowKeyName = "TBD";

    switch (arrowKey) {
    case ARROWKEY_UNKNOWN:
        arrowKeyName = "ARROWKEY_UNKNOWN";
        break;
    case ARROWKEY_UP:
        arrowKeyName = "ARROWKEY_UP";
        break;
    case ARROWKEY_DOWN:
        arrowKeyName = "ARROWKEY_DOWN";
        break;
    case ARROWKEY_LEFT:
        arrowKeyName = "ARROWKEY_LEFT";
        break;
    case ARROWKEY_RIGHT:
        arrowKeyName = "ARROWKEY_RIGHT";
        break;
    }
    return arrowKeyName;
}

arrowKey_t readArrowKeyPress()
{
    arrowKey_t arrowKeyPressed = ARROWKEY_UNKNOWN;

    const char *cmd = "bash -c 'read -s -t .1 -n3 c && printf \"%s\" \"$c\"'";
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("\nError opening pipe!\n");
        return arrowKeyPressed;
    }

    char buf[BUFSIZE] = {0};
    char *retval1 = fgets(buf, BUFSIZE, fp);
    if (retval1 == NULL) {
        return arrowKeyPressed;
    }

    int retval2 = pclose(fp);
    if (retval2 == -1) {
        printf("\nError obtaining the cmd's exit status code.\n");
        return arrowKeyPressed;
    }
    else if (retval2 != 0) {
        printf("\nCommand exited with exit status code %i.\n", retval2);
        return arrowKeyPressed;
    }

    // map the readings to arrow keys
    if ((buf[0] == 27) && (buf[1] == 91) && (buf[2] == 65)) {
        arrowKeyPressed = ARROWKEY_UP;
    }
    else if ((buf[0] == 27) && (buf[1] == 91) && (buf[2] == 66)) {
        arrowKeyPressed = ARROWKEY_DOWN;
    }
    else if ((buf[0] == 27) && (buf[1] == 91) && (buf[2] == 67)) {
        arrowKeyPressed = ARROWKEY_RIGHT;
    }
    else if ((buf[0] == 27) && (buf[1] == 91) && (buf[2] == 68)) {
        arrowKeyPressed = ARROWKEY_LEFT;
    }

    return arrowKeyPressed;
}

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

void printMapInterface(int map_left, int map_top, char *map)
{
    printMapAtCoordinate(map_left, map_top, map);
    printf("\n\n");
    saveCursorPos();
}

void printPlayerAtCoordinate(int x, int y)
{
    changeTextColor("blue");
    printCharAtCoordinate(x, y, 'x');
    changeTextColor("reset");
}

void updateMap(Map *m)
{
    int clear = 1;
    int isMonster = 0;
    int i = 0;
    // clear last player starting pos
    while (clear) {
        if (m->map[i] == '9') {
            m->map[i] = '0';
            clear = 0;
        }
        i++;
    }

    i = 0;

    while (i < strlen(m->map - 1)) {
        if (m->map[i] == '2') {
            isMonster = 1;
            break;
        }
        i++;
    }

    if (!isMonster) {
        printf("\nNext map unlocked /!WIP!\\");
    }

    // update starting pos
    printf("x:%d, y%d\n", (m->player_x - m->map_left) / 2, m->player_y - m->map_top);
    //printf ("%s", m->map);

    m->map[(m->map_width / 2 + 1) * (m->player_y - m->map_top) + (m->player_x - m->map_left) / 2] = '9';
}

void eventHandler(char sign, Map m, Player *p)
{
    int *nbrMonster = (int *)malloc(sizeof(int));
    switch (sign) {
    case '0':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("                            ");
        break;
    case '2':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);

        printf("Fight begins!");
        fightMonster(p, loadFightScene(p, nbrMonster), nbrMonster);
        clearScreen();
        updateMap(&m);
        printMapInterface(m.map_left, m.map_top, m.map);
        mov(&m, p);

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

void movRight(int *x, int *y, char *map, Map m, Player *p)
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
    m.player_x = *x;
    eventHandler(signAtCoordinate(map, *x, *y, m), m, p);
}

void movLeft(int *x, int *y, char *map, Map m, Player *p)
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
    m.player_x = *x;
    eventHandler(signAtCoordinate(map, *x, *y, m), m, p);
}

void movUp(int *x, int *y, char *map, Map m, Player *p)
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
    m.player_y = *y;
    eventHandler(signAtCoordinate(map, *x, *y, m), m, p);
}

void movDown(int *x, int *y, char *map, Map m, Player *p)
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
    m.player_y = *y;
    eventHandler(signAtCoordinate(map, *x, *y, m), m, p);
}

void mov(Map *m, Player *p)
{
    printf("Press any arrow key. Press Ctrl + C to quit.\n");
    fflush(stdout);

    saveCursorPos();
    while (1) {
        arrowKey_t arrowKeyPressed = readArrowKeyPress();
        if (arrowKeyPressed == ARROWKEY_UNKNOWN) {
            continue;
        }
        if (arrowKeyPressed == ARROWKEY_UP) {
            movUp(&m->player_x, &m->player_y, m->map, *m, p);
        }
        else if (arrowKeyPressed == ARROWKEY_DOWN) {
            movDown(&m->player_x, &m->player_y, m->map, *m, p);
        }
        else if (arrowKeyPressed == ARROWKEY_LEFT) {
            movLeft(&m->player_x, &m->player_y, m->map, *m, p);
        }
        else if (arrowKeyPressed == ARROWKEY_RIGHT) {
            movRight(&m->player_x, &m->player_y, m->map, *m, p);
        }

        restoreCursorPos();
        printf(("                               \n"));
        restoreCursorPos();
        printf("Key pressed = %d\n", arrowKeyPressed);
    }
    return;
}

int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top, Player *p)
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
    m.player_x = x;
    m.player_y = y;

    int ans = 0;

    m.map = readFileContent(fp);

    printMapInterface(m.map_left, m.map_top, m.map);

    mov(&m, p);

    fclose(fp);
    return 0;
}
