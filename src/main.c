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
            printf("Donjon %d\n", i);
            // fgetc(stdin);
            p->mana = p->maxMana;
            p->life = p->maxLife;

            if (snprintf(filename, 25, "ascii/map%d.txt", i) < 0) {
                printf("Error while loading map %d\n", i);
                free(p);
                return 0;
            }
            if (snprintf(monster, 25, "ascii/monster/%d.txt", i) < 0) {
                printf("Error while loading monster %d\n", i);
                free(p);
                return 0;
            }

            int isMap = map(filename, monster, MAP_WIDTH, MAP_HEIGHT, MAP_LEFT, MAP_TOP, p);
            if (isMap == 1) {
                printf("File %s or %s not found\n", filename, monster);
                return 1;
            }
        }

        clearScreen();
        changeTextColor("red");
        printf("HERE IS THE FINAL BOSS\n");
        fgetc(stdin);
        changeTextColor("reset");
        int *idToFight = (int *)malloc(sizeof(int));
        idToFight[0] = 5;
        int *nbMonster = (int *)malloc(sizeof(int));
        *nbMonster = 1;
        fightMonster(p, loadFightScene(p, nbMonster, idToFight), nbMonster);
        printf("Vous avez fini le jeu, bravo !\n");
        free(idToFight);
        free(nbMonster);
        break;
    case 2:
        clearScreen();
        continueGame(p);
        printf("Votre personnage a bien ete charge, %s\n", p->name);

        char *save_file = (char *)malloc(sizeof(char) * 25);
        strcpy(save_file, "ascii/map_save");
        FILE *fp = fopen(save_file, "r");
        if (fp == NULL) {
            printf("Fichier de sauvegarde introuvable\n");
            return 1;
        }

        char *map_save = readFileContent(fp);
        fclose(fp);
        // convert char to int

        for (int i = map_save[0] - '0'; i < 4; i++) {
            printf("loading map %d\n", i);
            // fgetc(stdin);

            if (snprintf(filename, 25, "ascii/map%d.txt", i) < 0) {
                printf("Error while loading map %d\n", i);
                free(p);
                return 0;
            }

            if (snprintf(monster, 25, "ascii/monster/%d.txt", i) < 0) {
                printf("Error while loading monster %d\n", i);
                free(p);
                return 0;
            }

            if (i == map_save[0] - '0') {
                int isMap = map(save_file, monster, MAP_WIDTH, MAP_HEIGHT, MAP_LEFT, MAP_TOP, p);
                if (isMap == 1) {
                    printf("File %s or %s not found\n", filename, monster);
                    return 1;
                }
            }
            else {
                int isMap = map(filename, monster, MAP_WIDTH, MAP_HEIGHT, MAP_LEFT, MAP_TOP, p);
                if (isMap == 1) {
                    printf("File %s or %s not found\n", filename, monster);
                    return 1;
                }
            }
        }

        clearScreen();
        changeTextColor("red");
        printf("HERE IS THE FINAL BOSS\n");
        fgetc(stdin);
        changeTextColor("reset");
        fightMonster(p, loadFightScene(p, nbMonster, idToFight), nbMonster);
        printf("Vous avez fini le jeu, bravo !\n");
        free(idToFight);
        free(nbMonster);
        break;
    case 3:
        printf("Quitter\n");
        free(p);
        return EXIT_SUCCESS;
    }

    free(p);
    return 0;
}
