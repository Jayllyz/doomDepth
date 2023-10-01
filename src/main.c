#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 50
#define WIDTH 150

void printGameSize()
{
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("*");
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    clearScreen();
    int choice;

    do {
        printf("=====================================\n");
        printf("Bienvenue dans le jeu doomDepth\n");
        printf("1. Nouvelle partie\n");
        printf("2. Charger une partie\n");
        printf("3. Quitter\n");
        printf("=====================================\n");
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