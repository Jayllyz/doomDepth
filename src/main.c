#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 50
#define WIDTH 150
#define MENU_FILE "ascii/menu.txt"

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

int main(int argc, char **argv)
{
    clearScreen();
    Player *p = (Player *)malloc((sizeof(Player)));
    int choice;

    do {
        printGameMenu();
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > 3);

    switch (choice) {
    case 1:
        clearScreen();
        printf("Nouvelle partie\n");
        playerSetup(p);
        printf("Votre personnage a bien ete cree, vous etes niveau %d\n", p->level);
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
