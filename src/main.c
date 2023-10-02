#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 50
#define WIDTH 150

void printGameMenu()
{
    FILE *fp = fopen("ascii/menu.txt", "r");

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
        playerSetup();
        printf("Votre personnage a bien ete cree\n");
        break;
    case 2:
        clearScreen();
        printf("Charger une partie\n");
        break;
    case 3:
        printf("Quitter\n");
        return EXIT_SUCCESS;
        break;
    }

    return 0;
}