#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/map.h"
#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HEIGHT 50
#define WIDTH 150
#define MENU_FILE "ascii/menu.txt"
#define DRAGON "ascii/monster/5.txt"

#define MAP_LEFT 5
#define MAP_TOP 5
#define MAP_WIDTH 20
#define MAP_HEIGHT 9

void printGameMenu()
{
    FILE *fp = fopen(MENU_FILE, "r");

    if (fp == NULL) {
        printf("Fichier du menu introuvable\n");
        return;
    }

    int c = fgetc(fp);
    while (c != EOF) {
        printf("%c", c);
        c = fgetc(fp);
    }

    fclose(fp);
}

void printDragonAnsiiWay()
{
    FILE *fp = fopen(DRAGON, "r");

    if (fp == NULL) {
        printf("Fichier du dragon introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("red");
    printStringAtCoordinate(100, 0, content);
    changeTextColor("reset");
    printf("\n"); // To avoid the cursor to be on the last line of the file which will cut the dragon

    free(content);
    fclose(fp);
}

int main(int argc, char **argv)
{
    clearScreen();
    int isMap;

    srand(time(NULL));
    Player *p = (Player *)malloc((sizeof(Player)));
    int choice;

    do {
        printGameMenu();
        printDragonAnsiiWay();
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > 3);

    switch (choice) {
    case 1:
        clearScreen();
        printf("Nouvelle partie\n");
        playerSetup(p);
        clearScreen();
        printf("Votre personnage a bien ete cree, \nvous etes niveau %d\n", p->level);
        isMap = map("ascii/map.txt", DRAGON, MAP_WIDTH, MAP_HEIGHT, MAP_LEFT, MAP_TOP, p);
        if (isMap == 1) {
            printf("Error in map function");
            return 1;
        }
        //while (p->life > 0)
        //    fightMonster(p, loadFightScene(p));
        //break;
    case 2:
        clearScreen();
        printf("Charger une partie\n");
        //Load player
        break;
    case 3:
        printf("Quitter\n");
        free(p);
        return EXIT_SUCCESS;
    }

    free(p);
    return 0;
}
