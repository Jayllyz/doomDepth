#include "includes/smith.h"
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
#define MAX_STUFF_LEVEL 10

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
        printf("Failed to prepare statement\n");
        sqlite3_close(db);
    }

    sqlite3_bind_int(res, 1, idStuff);

    stuff result;

    result.name = NULL;
    result.description = NULL;
    result.type = NULL;

    if (sqlite3_step(res) == SQLITE_ROW) {
        result.name = strdup((const char *)sqlite3_column_text(res, 1));
        result.description = strdup((const char *)sqlite3_column_text(res, 2));
        result.attack = sqlite3_column_int(res, 3);
        result.defense = sqlite3_column_int(res, 4);
        result.grade = sqlite3_column_int(res, 5);
        result.gold = sqlite3_column_int(res, 6);
        result.type = strdup((const char *)sqlite3_column_text(res, 7));
    }
    else {
        printf("No data found for id = %d\n", idStuff);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return result;
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
 * @brief Upgrade the player stuff
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return void
*/
void upgradePlayerStuff(int idStuff, int idPlayer)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE PLAYER_STUFF SET level = level + 1 WHERE player_id = %d AND stuff_id = %d;", idPlayer, idStuff);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to update data: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
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
        printf("Mince ! Vous n'avez rien dans votre inventaire\n");
        changeTextColor("reset");
    }

    printf("\n\n");
    printLine();
    printf("\n\n");

    int choice;
    do {
        printf("Entrer le numéro du stuff que vous voulez améliorer\n");
        printf("Entrer 0 pour quitter\n");

        choice = getInputInt();
        clearBuffer();
    } while (choice < 0);

    if (choice != 0) {

        stuff stuff = getStuffOfPlayerById(choice, idPlayer);
        int level = getStuffLevelOfPlayerById(choice, idPlayer);

        printf("Vous avez choisi %s\n", stuff.name);

        if (level >= MAX_STUFF_LEVEL) {
            changeTextColor("orange");
            printf("Votre stuff est déjà au niveau maximum\n");
            changeTextColor("reset");

            printf("\n\n");
            printf("Appuyer sur entrer pour continuer\n");
            clearBuffer();

            initSmith(idPlayer);
        }
        else {
            printf("Votre stuff est de niveau %d\n", level);
        }

        int price = stuff.gold * (1 + level * TAUX_AMELIORATION);

        printf("\n\n");

        printf("Le prix de l'amélioration est de %d", price);
        changeTextColor("orange");
        printf(" gold\n");
        changeTextColor("reset");

        printf("\n\n");

        changeTextColor("red");
        printf("Voulez-vous améliorer ce stuff ?\n");
        changeTextColor("reset");
        printf("1. Oui\n");
        printf("2. Non\n");

        int choice2;
        do {
            choice2 = getInputInt();
            clearBuffer();
        } while (choice2 < 1 || choice2 > 2);

        if (choice2 == 1) {
            int gold = getPlayerGold(idPlayer);
            if (price > gold) {
                changeTextColor("red");
                printf("Vous n'avez pas assez d'argent\n");
                changeTextColor("reset");

                printf("\n\n");
                printf("Appuyer sur entrer pour continuer\n");
                clearBuffer();

                initSmith(idPlayer);
            }

            removeGoldToPlayer(price, idPlayer);
            upgradePlayerStuff(choice, idPlayer);
            changeTextColor("green");
            printf("Votre stuff a été amélioré\n");
            changeTextColor("reset");
        }

        printf("\n\n");
        printf("Appuyer sur entrer pour continuer\n");
        clearBuffer();

        initSmith(idPlayer);
    }

    return;
}
