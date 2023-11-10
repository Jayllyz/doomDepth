#include "includes/ansii_print.h"
#include "includes/shop.h"
#include "includes/utils.h"
#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SMITH "ascii/smith/smith.txt"
#define ITEMS "ascii/shop/stuff.txt"
#define DB_FILE "db/doomdepth.sqlite"
#define TAUX_AMELIORATION 0.1

/**
 * @brief Read the content of a file and print it
 * @return void
*/
void printSmithAnsiiWay()
{
    FILE *fp = fopen(SMITH, "r");

    if (fp == NULL) {
        printf("Fichier du shop introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(50, 0, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

/**
 * @brief Get the pstuff info
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return stuff The stuff info
*/
stuff getStuffOfPlayerById(int idStuff, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_prepare_v2(db, "SELECT * FROM STUFF WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
    }

    sqlite3_bind_int(res, 1, idStuff);

    stuff stuff;

    while (sqlite3_step(res) == SQLITE_ROW) {

        stuff.name = strdup((const char *)sqlite3_column_text(res, 1));
        stuff.description = strdup((const char *)sqlite3_column_text(res, 2));
        stuff.attack = sqlite3_column_int(res, 3);
        stuff.defense = sqlite3_column_int(res, 4);
        stuff.grade = sqlite3_column_int(res, 5);
        stuff.gold = sqlite3_column_int(res, 6);
        stuff.type = strdup((const char *)sqlite3_column_text(res, 7));
    }

    sqlite3_finalize(res);

    sqlite3_close(db);

    return stuff;
}

/**
 * @brief Get the stuff level of a player
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return int The level of the stuff
*/
int getStuffLevelOfPlayerById(int idStuff, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_prepare_v2(db, "SELECT level FROM PLAYER_STUFF WHERE player_id = ? AND stuff_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_bind_int(res, 2, idStuff);

    int level = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        level = sqlite3_column_int(res, 0);
    }

    sqlite3_finalize(res);

    sqlite3_close(db);

    return level;
}

/**
 * @brief Init the smith 
 * @param int idPlayer The id of the player
 * @return void
*/
void initSmith(int idPlayer)
{
    clearScreen();

    printSmithAnsiiWay();
    printPlayerGold(idPlayer);

    printf("\n\n");
    printLine();
    printf("\n\n");

    int stuffCount;
    stuff *stuffsList = getStuffOfPLayer(&stuffCount, idPlayer);
    if (stuffsList) {
        printStuffs(stuffsList, stuffCount);
        free(stuffsList);
    }
    else {
        changeTextColor("orange");
        printf("Mince ! Vous n'avez rien à vendre \n");
        changeTextColor("reset");
    }

    printf("\n\n");
    printLine();
    printf("\n\n");

    printf("Entrer le numéro du stuff que vous voulez améliorer\n");
    printf("Entrer 0 pour quitter\n");

    int choice = getInputInt();

    while (choice < 0) {
        printf("Veuillez entrer un choix valide\n");
        choice = getInputInt();
    }

    if (choice == 0) {
        return;
    }

    stuff stuff = getStuffOfPlayerById(choice, idPlayer);
    int level = getStuffLevelOfPlayerById(choice, idPlayer);

    printf("Vous avez choisi %s\n", stuff.name);
    printf("Votre stuff est de niveau %d\n", level);

    int price = stuff.gold * (1 + level * TAUX_AMELIORATION);

    printf("Le prix de l'amélioration est de %d\n", price);

    if (price > getPlayerGold(idPlayer)) {
        printf("Vous n'avez pas assez d'argent\n");
        return;
    }
}

int main()
{
    initSmith(1);
}
