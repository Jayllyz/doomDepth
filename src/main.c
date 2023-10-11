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
    printf("Press enter to continue\n");
    fgetc(stdin);
    clearScreen();
    srand(time(NULL));
    Player *p = (Player *)malloc((sizeof(Player)));
    int choice;
    char monster[25];
    char filename[25];

    do {
        clearScreen();
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

        for (int i = 1; i < 4; i++) {
            printf("loading map %d\n", i);
            fgetc(stdin);
            sprintf(filename, "ascii/map%d.txt", i);
            sprintf(monster, "ascii/monster/%d.txt", i);
            int isMap = map(filename, monster, MAP_WIDTH, MAP_HEIGHT, MAP_LEFT, MAP_TOP, p);
            if (isMap == 1) {
                printf("File %s or %s not found\n", filename, monster);
                return 1;
            }
        }
        printf("Vous avez fini le jeu, bravo !\n");

        break;
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
