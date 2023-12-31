#include "includes/map.h"
#include "includes/ansii_print.h"
#include "includes/event.h"
#include "includes/fight.h"
#include "includes/items.h"
#include "includes/shop.h"
#include "includes/smith.h"
#include "includes/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAP_FINISHED 2

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
    case INVENTORY:
        arrowKeyName = "INVENTORY";
        break;
    }
    return arrowKeyName;
}

arrowKey_t readArrowKeyPress()
{
    arrowKey_t arrowKeyPressed = ARROWKEY_UNKNOWN;

    /* Detailed explanation :
    This command bash -c 'read -s -t .1 -n3 c && printf "%s" "$c"' will read a single character from the keyboard and print it to stdout.
        The -s option will make the read command silent, so the character pressed will not be echoed to the terminal.
        The -t option will make the read command timeout after .1 seconds.
        The -n3 option will make the read command return after reading 3 characters.
        The printf "%s" "$c" will print the character read to stdout.
        The command will return immediately if no character is pressed, or after .1 seconds if a character is pressed.
        The command will return with a non-zero exit status if the read command times out.
     */

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
    else if ((buf[0] == 27) && (buf[1] == 79) && (buf[2] == 80)) {
        arrowKeyPressed = INVENTORY;
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
    case '?':
        changeTextColor("yellow");
        printf("? ");
        changeTextColor("reset");
        break;
    case '+':
        changeTextColor("green");
        printf("+ ");
        changeTextColor("reset");
        break;
    case '!':
        changeTextColor("yellow");
        printf("! ");
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

void printSignAtCoordinate(char *map, int x, int y, Map *m)
{
    setSignColor((signAtCoordinate(map, x, y, *m)));
    printCharAtCoordinate(x, y, convertSigntoChar(signAtCoordinate(map, x, y, *m)));
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

int updateMap(Map *m)
{
    int clear = 1;
    int isMonster = 0;
    int i = 0;
    while (clear) {
        if (m->map[i] == '9') {
            m->map[i] = '0';
            clear = 0;
        }
        i++;
    }
    m->map[(m->map_width / 2 + 1) * (m->player_y - m->map_top) + (m->player_x - m->map_left) / 2] = '9';
    saveMapToFile(m->map, "ascii/map_save");

    i = 1;

    while (i < 97) {

        if (m->map[i] == '2') {
            isMonster = 1;
            break;
        }
        i++;
    }

    if (!isMonster) {
        return MAP_FINISHED;
    }
    return 0;
}

void saveMapToFile(const char *map, const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Error opening the file");
        return;
    }

    fprintf(file, "%s", map);
    fclose(file);
}

int eventHandler(char sign, Map m, Player *p)
{
    int idToFight[1] = {-1};
    int *nbrMonster = (int *)malloc(sizeof(int));
    //*nbrMonster = 3;

    switch (sign) {
    case '0':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("                            ");
        break;
    case '2':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);

        printf("Fight begins!");
        fightMonster(p, loadFightScene(p, nbrMonster, idToFight), nbrMonster);
        clearScreen();
        if (updateMap(&m) == MAP_FINISHED) {
            return MAP_FINISHED;
        }
        printMapInterface(m.map_left, m.map_top, m.map);
        mov(&m, p);

        break;
    case '3':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Happy shopping!");

        initShop(p->id);

        clearScreen();
        if (updateMap(&m) == MAP_FINISHED) {
            return MAP_FINISHED;
        }
        printMapInterface(m.map_left, m.map_top, m.map);

        break;
    case '4':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Welcome to the Smith!");

        initSmith(p);

        clearScreen();
        if (updateMap(&m) == MAP_FINISHED) {
            return MAP_FINISHED;
        }
        printMapInterface(m.map_left, m.map_top, m.map);

        break;

    case '+':
        printStringAtCoordinate(2, 19, "You got a heal !");
        p->life = p->maxLife;
        updatePlayerInfo(p);
        updateMap(&m);
        break;

    case '!':
        printStringAtCoordinate(2, 19, "You found a treasure !");
        rewardStuff(p);
        updateMap(&m);
        break;

    case '?':
        movCursor(m.map_width / 2 + m.map_left - m.map_width / 2, m.map_top + m.map_height + 1);
        printf("Here is a random event!");
        int event_result = event();
        switch (event_result) {
        case DEATH:
            changeTextColor("red");
            printf("You died !\n");
            defeat();
            break;
        case BONUS:
            changeTextColor("green");
            printf("You got a bonus !\n");
            p->attack += 15;
            updatePlayerInfo(p);
            break;
        case MALUS:
            changeTextColor("red");
            printf("You got a malus !\n");
            p->attack /= 2;
            updatePlayerInfo(p);
            break;
        case REWARD:
            changeTextColor("green");
            printf("You got a reward !\n");
            rewardStuff(p);
            break;
        case GOLD:
            changeTextColor("yellow");
            printf("You got 100 gold !\n");
            p->gold += 100;
            updatePlayerInfo(p);
            break;
        case GOLD_LOST:
            changeTextColor("red");
            printf("You lost half of your gold !\n");
            p->gold /= 2;
            updatePlayerInfo(p);
            break;
        default:
            changeTextColor("blue");
            printf("End of side quest.\n");
            break;
        }
        changeTextColor("reset");
        sleep(3); // @TODO replace it with a key press
        clearScreen();
        if (updateMap(&m) == MAP_FINISHED) {
            return MAP_FINISHED;
        }
        printMapInterface(m.map_left, m.map_top, m.map);

        break;
    }

    free(nbrMonster);
    return 0;
}

int movRight(int *x, int *y, char *map, Map m, Player *p)
{
    printSignAtCoordinate(map, *x, *y, &m);
    if (signAtCoordinate(map, *x + 2, *y, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return 0;
    }
    *x = *x + 2;
    if (*x > m.map_left + m.map_width - 4) {
        *x = m.map_left - 2 + m.map_width - 2; // -2 come from map border
    }
    printPlayerAtCoordinate(*x, *y);
    m.player_x = *x;
    if (eventHandler(signAtCoordinate(map, *x, *y, m), m, p) == MAP_FINISHED) {
        return MAP_FINISHED;
    }
    return 0;
}

int movLeft(int *x, int *y, char *map, Map m, Player *p)
{
    printSignAtCoordinate(map, *x, *y, &m);
    if (signAtCoordinate(map, *x - 2, *y, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return 0;
    }
    *x = *x - 2;
    if (*x < m.map_left + 2) {
        *x = m.map_left + 2;
    }
    printPlayerAtCoordinate(*x, *y);
    m.player_x = *x;
    if (eventHandler(signAtCoordinate(map, *x, *y, m), m, p) == MAP_FINISHED) {
        return MAP_FINISHED;
    }
    return 0;
}

int movUp(int *x, int *y, char *map, Map m, Player *p)
{
    printSignAtCoordinate(map, *x, *y, &m);

    if (signAtCoordinate(map, *x, *y - 1, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return 0;
    }
    *y = *y - 1;
    if (*y < m.map_top + 1) {
        *y = m.map_top + 1;
    }
    printPlayerAtCoordinate(*x, *y);
    m.player_y = *y;
    if (eventHandler(signAtCoordinate(map, *x, *y, m), m, p) == MAP_FINISHED) {
        return MAP_FINISHED;
    }
    return 0;
}

int movDown(int *x, int *y, char *map, Map m, Player *p)
{
    printSignAtCoordinate(map, *x, *y, &m);

    if (signAtCoordinate(map, *x, *y + 1, m) == '1') {
        printPlayerAtCoordinate(*x, *y);
        return 0;
    }

    *y = *y + 1;

    if (*y + 1 > m.map_top + m.map_height - 2) {
        *y = m.map_top + m.map_height - 2;
    }
    printPlayerAtCoordinate(*x, *y);
    m.player_y = *y;
    if (eventHandler(signAtCoordinate(map, *x, *y, m), m, p) == MAP_FINISHED) {
        return MAP_FINISHED;
    }
    return 0;
}

int mov(Map *m, Player *p)
{
    printStringAtCoordinate(2, 20, "Press any arrow key | F1 to open inventory | Press Ctrl + C to quit.\n");

    saveCursorPos();
    while (1) {
        arrowKey_t arrowKeyPressed = readArrowKeyPress();
        if (arrowKeyPressed == ARROWKEY_UNKNOWN) {
            continue;
        }
        if (arrowKeyPressed == ARROWKEY_UP) {
            if (movUp(&m->player_x, &m->player_y, m->map, *m, p) == MAP_FINISHED) {
                return MAP_FINISHED;
            }
        }
        else if (arrowKeyPressed == ARROWKEY_DOWN) {
            if (movDown(&m->player_x, &m->player_y, m->map, *m, p) == MAP_FINISHED) {
                return MAP_FINISHED;
            }
        }
        else if (arrowKeyPressed == ARROWKEY_LEFT) {
            if (movLeft(&m->player_x, &m->player_y, m->map, *m, p) == MAP_FINISHED) {
                return MAP_FINISHED;
            }
        }
        else if (arrowKeyPressed == ARROWKEY_RIGHT) {
            if (movRight(&m->player_x, &m->player_y, m->map, *m, p) == MAP_FINISHED) {
                return MAP_FINISHED;
            }
        }
        else if (arrowKeyPressed == INVENTORY) {
            initInventory(1);
            printMapInterface(m->map_left, m->map_top, m->map);
        }

        restoreCursorPos();
        printf(("                               \n"));
        restoreCursorPos();
    }
}

void findStartingPos(Map *m)
{
    for (int i = 0; i < strlen(m->map); i++) {
        if (m->map[i] == '9') {

            int x = (i % 11) * 2 + m->map_left;
            int y = (i / 11) + m->map_top;
            m->player_x = x;
            m->player_y = y;
            break;
        }
    }
    return;
}

int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top, Player *p)
{
    Map m = {map_top, map_left, map_width, map_height, NULL, 0, 0};

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
    printStringAtCoordinate((int)((map_left + map_width) * 1.5), 0, dragon);
    changeTextColor("reset");

    int x = map_width / 2 + map_left;

    int y = map_height / 2 + map_top;

    m.player_x = x;
    m.player_y = y;

    m.map = readFileContent(fp);
    findStartingPos(&m);
    int isMapFinished = updateMap(&m);
    if (isMapFinished == MAP_FINISHED) {
        return MAP_FINISHED;
    }

    printMapInterface(m.map_left, m.map_top, m.map);

    mov(&m, p);

    free(dragon);
    free(m.map);
    fclose(fp2);
    fclose(fp);
    return 0;
}
