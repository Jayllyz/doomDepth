#include "includes/shop.h"
#include "includes/ansii_print.h"
#include "includes/items.h"
#include "includes/map.h"
#include "includes/utils.h"
#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SHOP "ascii/shop/shop.txt"
#define DEALER "ascii/shop/dealer.txt"
#define SHOPDEALER "ascii/shop/shopdealer.txt"
#define ITEMS "ascii/shop/stuff.txt"
#define DB_FILE "db/doomdepth.sqlite"

#define NB_ITEMS_EACH_ROW 2 //number of stuffs on each row
#define NB_COL_ITEMS 6 //number of columns for the ascii stuff element
#define NB_COl_TEXT 90 //number of columns for the text of the stuff
#define PLAYER_STUFF_LIMIT 6 //number of stuffs that the player can have

#define ID_USER 1 //DEV PURPOSE

/**
 * @brief print a simple line
 * @return void
*/
void printLine()
{
    int i;
    for (i = 0; i < 150; i++) {
        if (i % 15 == 0) {
            changeTextColor("green");
        }
        else if (i % 15 == 7) {
            changeTextColor("reset");
        }
        printf("-");
    }
    changeTextColor("reset");
    printf("\n");
}

/**
 * @brief Read the content of a file and print it
 * @return void
*/
void printShopDealerAnsiiWay()
{
    FILE *fp = fopen(SHOPDEALER, "r");

    if (fp == NULL) {
        printf("Fichier du shop introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(28, 0, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

/**
 * @brief Read the content of a file and print it
 * @return void
*/
void printShopAnsiiWay()
{
    FILE *fp = fopen(SHOP, "r");

    if (fp == NULL) {
        printf("Fichier du shop introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(57, 0, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

/**
 * @brief Read the content of a file and print it
 * @return void
*/
void printDealerAnsiiWay()
{
    FILE *fp = fopen(DEALER, "r");

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
 * @brief Read the content of a file and print it
 * @return void
*/
void printStuffAnsiiWay(int x, int y)
{
    FILE *fp = fopen(ITEMS, "r");

    if (fp == NULL) {
        printf("Fichier de stuff introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(x, y, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

/**
 * @brief Print all stuffs of the shop
 * @param stuff *stuffsList The list of stuffs
 * @param int stuffCount The number of stuffs
 * @return void
 * @todo pb in positioning of the stuffs (NB_COL_ITEMs * i) is not necessary but necessary for printStuffAnsiiWay
*/
void printStuffs(stuff *stuffsList, int stuffCount)
{
    int ligne = 0;
    int col = 0;
    for (int i = 0; i < stuffCount; i++) {

        if (col % NB_ITEMS_EACH_ROW == 0 && i != 0) {
            ligne += 10;
            col = 0;
        }

        printStuffAnsiiWay(NB_COl_TEXT * col, 11 + ligne);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 11 + ligne);
        printf("Identifiant: %d", stuffsList[i].id);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 12 + ligne);
        printf("Nom: %s", stuffsList[i].name);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 13 + ligne);
        printf("Description: %s", stuffsList[i].description);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 14 + ligne);
        printf("Attaque: %d", stuffsList[i].attack);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 15 + ligne);
        printf("Défense: %d", stuffsList[i].defense);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 16 + ligne);
        printf("Grade: %d", stuffsList[i].grade);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 17 + ligne);
        printf("Type: %s", stuffsList[i].type);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 18 + ligne);
        printf("Prix: %d", stuffsList[i].gold);

        col++;
    }
    printf("\n\n");
}

/**
 * @brief Get all stuff of the shop from the database
 * @param int *stuffCount The number of stuffs
 * @return stuff * The list of stuffs
*/
stuff *getStuffFromShop(int *stuffCount)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, defense, grade, gold, type FROM STUFF ORDER BY RANDOM() LIMIT 6;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    stuff *stuffsList = NULL;
    stuff *safe = NULL;
    *stuffCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        safe = (stuff *)realloc(stuffsList, (*stuffCount + 1) * sizeof(stuff));
        if (safe == NULL) {
            printf("Erreur d'allocation mémoire\n");
            return NULL;
        }
        stuffsList = safe;
        stuff *currentStuff = &stuffsList[*stuffCount];

        currentStuff->id = sqlite3_column_int(res, 0);

        currentStuff->name = strdup((const char *)sqlite3_column_text(res, 1));
        currentStuff->description = strdup((const char *)sqlite3_column_text(res, 2));
        currentStuff->attack = sqlite3_column_int(res, 3);
        currentStuff->defense = sqlite3_column_int(res, 4);
        currentStuff->grade = sqlite3_column_int(res, 5);
        currentStuff->gold = sqlite3_column_int(res, 6);
        currentStuff->type = strdup((const char *)sqlite3_column_text(res, 7));

        (*stuffCount)++;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return stuffsList;
}

/**
 * @brief Get all stuff of the shop from the database of the player
 * @param int *stuffCount The number of stuffs
 * @param int idPlayer The id of the player
 * @return stuff * The list of stuffs
*/
stuff *getStuffOfPLayer(int *stuffCount, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db,
        "SELECT S.id, S.name, S.description, S.attack, S.defense, S.grade, S.gold, S.type FROM STUFF AS S INNER JOIN PLAYER_STUFF AS PS ON S.id = PS.stuff_id WHERE "
        "PS.player_id = ?;",
        -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(res, 1, idPlayer);

    stuff *stuffsList = NULL;
    stuff *safe = NULL;
    *stuffCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        safe = (stuff *)realloc(stuffsList, (*stuffCount + 1) * sizeof(stuff));
        if (safe == NULL) {
            printf("Erreur d'allocation mémoire\n");
            return NULL;
        }
        stuffsList = safe;
        stuff *currentStuff = &stuffsList[*stuffCount];

        currentStuff->id = sqlite3_column_int(res, 0);

        currentStuff->name = strdup((const char *)sqlite3_column_text(res, 1));
        currentStuff->description = strdup((const char *)sqlite3_column_text(res, 2));
        currentStuff->attack = sqlite3_column_int(res, 3);
        currentStuff->defense = sqlite3_column_int(res, 4);
        currentStuff->grade = sqlite3_column_int(res, 5);
        currentStuff->gold = sqlite3_column_int(res, NB_COL_ITEMS);
        currentStuff->type = strdup((const char *)sqlite3_column_text(res, 7));

        (*stuffCount)++;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return stuffsList;
}

/**
 * @brief Get the price of a stuff from the database
 * @param int idStuff The id of the stuff
 * @return int The price of the stuff
*/
int getStuffprice(int idStuff)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT gold FROM STUFF WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idStuff);

    int gold = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        gold = sqlite3_column_int(res, 0);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return gold;
}

/**
 * @brief Get the gold of the player from the database
 * @param int idPlayer The id of the player
 * @return int The gold of the player
*/
int getPlayerGold(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT gold FROM PLAYER WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_step(res);

    int gold = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);

    return gold;
}

/**
 * @brief Get the number of stuff in the player's stuff
 * @param int idPlayer The id of the player
 * @return int The number of stuff
*/
int getNbStuffInPlayerStuff(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);
    int nbStuff = 0;

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM PLAYER_STUFF WHERE player_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_step(res);

    nbStuff = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);

    return nbStuff;
}

/**
 * @brief Check if a stuff is in the player's stuff
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return int 0 if the stuff is not in the player's stuff, 1 otherwise
*/
int checkStuffIsInPlayerStuff(int idStuff, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(stuff_id) FROM PLAYER_STUFF WHERE player_id = ? AND stuff_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_bind_int(res, 2, idStuff);

    int count = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        count = sqlite3_column_int(res, 0);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return count;
}

/**
 * @brief Add a stuff to the player's stuff
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return void
*/
void addStuffToPlayerStuff(int idStuff, int idPlayer)
{
    if (countPlayerStuff(idPlayer) >= PLAYER_STUFF_LIMIT)
        return;

    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = sqlite3_mprintf("INSERT INTO PLAYER_STUFF (player_id, stuff_id) VALUES (%d, %d);", idPlayer, idStuff);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);

    char *type = getStuffType(idStuff);

    if (strcmp(type, "Consumable") != 0) {
        sqlite3_stmt *res;
        rc = sqlite3_open(DB_FILE, &db);

        if (rc != SQLITE_OK) {
            printf("Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM PLAYER_STUFF WHERE player_id = ? AND stuff_id IN (SELECT id FROM STUFF WHERE type = ?);", -1, &res, NULL);

        if (rc != SQLITE_OK) {
            printf("Failed to insert data: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        sqlite3_bind_int(res, 1, idPlayer);
        sqlite3_bind_text(res, 2, type, -1, SQLITE_STATIC);

        int count = 0;

        if (sqlite3_step(res) == SQLITE_ROW) {
            count = sqlite3_column_int(res, 0);
        }

        sqlite3_finalize(res);
        sqlite3_close(db);

        if (count == 1)
            equipStuff(idPlayer, idStuff);
    }
}

/**
 * @brief Remove a stuff to the player's stuff
 * @param int idStuff The id of the stuff
 * @param int idPlayer The id of the player
 * @return void
*/
void removeStuffFromPlayerStuff(int idStuff, int idPlayer)
{
    sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    rc = sqlite3_prepare_v2(db, "SELECT isEquip FROM PLAYER_STUFF WHERE player_id = ? AND stuff_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    sqlite3_bind_int(res, 1, idPlayer);

    int isEquip = 0;

    if (sqlite3_step(res) == SQLITE_ROW)
        isEquip = sqlite3_column_int(res, 0);

    if (isEquip)
        removeStatsStuff(idPlayer, idStuff);

    sqlite3_finalize(res);

    char *sql = sqlite3_mprintf("DELETE FROM PLAYER_STUFF WHERE player_id = %d AND stuff_id = %d;", idPlayer, idStuff);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to delete data %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(0);
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
}

char *getStuffType(int idStuff)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    char *type = NULL;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));

        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT type FROM STUFF WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(res, 1, idStuff);

    while (sqlite3_step(res) == SQLITE_ROW) {
        type = strdup((const char *)sqlite3_column_text(res, 0));
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return type;
}

/**
 * @brief Remove the stats of a stuff to the player
 * @param int idSuff The id of the stuff
 * @param int idPlayer The id of the player
 */
void removeStatsStuff(int idSuff, int idPlayer)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = sqlite3_mprintf(
        "UPDATE PLAYER SET attack = attack - (SELECT attack FROM STUFF WHERE id = %d), defense = defense - (SELECT defense FROM STUFF WHERE id = %d) WHERE id = %d;",
        idSuff, idSuff, idPlayer);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to update data: %s\n", sqlite3_errmsg(db));
        exit(-1);
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
}

/**
 * @brief Add the stats of a stuff to the player
 * @param int idSuff The id of the stuff
 * @param int idPlayer The id of the player
 */
void addStatsStuff(int idSuff, int idPlayer)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = sqlite3_mprintf(
        "UPDATE PLAYER SET attack = attack + (SELECT attack FROM STUFF WHERE id = %d), defense = defense + (SELECT defense FROM STUFF WHERE id = %d) WHERE id = %d;",
        idSuff, idSuff, idPlayer);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to update data: %s\n", sqlite3_errmsg(db));
        exit(-1);
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
}

/**
 * @brief remove gold to the player
 * @param int gold The gold to remove
 * @param int idPlayer The id of the player
 * @return void
*/
void removeGoldToPlayer(int gold, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, "UPDATE PLAYER SET gold = gold - ? WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(res, 1, gold);
    sqlite3_bind_int(res, 2, idPlayer);

    sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

/**
 * @brief Add gold to the player
 * @param int gold The gold to add
 * @param int idPlayer The id of the player
 * @return void
*/
void addGoldToPlayer(int gold, int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, "UPDATE PLAYER SET gold = gold + ? WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(res, 1, gold);
    sqlite3_bind_int(res, 2, idPlayer);

    sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

/**
 * @brief Print the gold of the player
 * @param int idPlayer The id of the player
 * @return void
*/
void printPlayerGold(int idPlayer)
{
    movCursor(65, 8);

    changeTextColor("orange");
    printf("Gold: %d", getPlayerGold(idPlayer));
    changeTextColor("reset");
}

/**
 * @brief Display the buy stuff menu and handle the choice of the user
 * @param int idPlayer The id of the player
 * @return void
*/
void buyStuffInit(int idPlayer)
{
    clearScreen();
    printShopAnsiiWay();
    printPlayerGold(idPlayer);
    printf("\n\n");
    printLine();

    int stuffCount;
    stuff *stuffsList = getStuffFromShop(&stuffCount);
    int biggestId = biggestIdStuff(idPlayer);
    if (stuffsList) {
        printStuffs(stuffsList, stuffCount);
        free(stuffsList);
    }
    else {
        printf("Aucun élément trouvé dans la base de données.\n");
    }

    int choice;
    saveCursorPos();
    do {
        restoreCursorPos();
        clearLine();
        printf("Entrer le numéro du stuff que vous voulez acheter\n");
        printf("Entrer 0 pour quitter\n");
        clearLine();
        choice = getInputInt();
        clearBuffer();

        if (choice > 0 && choice <= biggestId) {

            int nbStuffPlayer = getNbStuffInPlayerStuff(idPlayer);

            if (nbStuffPlayer >= PLAYER_STUFF_LIMIT) {
                changeTextColor("red");
                clearLine();
                printf("Vous avez atteint la limite de stuffs dans votre inventaire\n");
                changeTextColor("reset");
                continue;
            }

            int price = getStuffprice(choice);

            if (checkStuffIsInPlayerStuff(choice, idPlayer)) {
                changeTextColor("orange");
                clearLine();
                printf("Vous avez déjà ce stuff\n\n");
                changeTextColor("reset");
                continue;
            }

            if (price > getPlayerGold(idPlayer)) {
                changeTextColor("red");
                clearLine();
                printf("Vous n'avez pas assez d'or pour acheter ce stuff\n");
                changeTextColor("reset");
                continue;
            }

            removeGoldToPlayer(price, idPlayer);
            addStuffToPlayerStuff(choice, idPlayer);

            changeTextColor("green");
            clearLine();
            printf("Vous avez acheté le stuff avec succès\n\n");
            changeTextColor("reset");
        }
    } while (choice != 0);

    return;
}

/**
 * @brief Sell a stuff of the player to the shop
 * @param int idPlayer The id of the player
 * @return void
*/
void sellStuffInit(int idPlayer)
{
    clearScreen();

    int choice;
    do {
        printDealerAnsiiWay();
        printPlayerGold(idPlayer);
        printf("\n\n");
        printLine();

        int stuffCount;
        stuff *stuffsList = getStuffOfPLayer(&stuffCount, idPlayer);
        if (stuffsList) {
            printStuffs(stuffsList, stuffCount);
            free(stuffsList);
        }
        else {
            clearScreen();
            printDealerAnsiiWay();
            printPlayerGold(idPlayer);
            printf("\n\n");
            printLine();

            changeTextColor("orange");
            printf("Mince ! Vous n'avez rien à vendre \n");
            changeTextColor("reset");
        }

        printf("Entrer le numéro du stuff que vous voulez vendre\n");
        printf("Entrer 0 pour quitter\n");

        choice = getInputInt();
        clearBuffer();

        if (!checkStuffIsInPlayerStuff(choice, idPlayer)) {
            printf("Vous ne possédez pas ce stuff\n");
            continue;
        }

        removeStuffFromPlayerStuff(choice, idPlayer);
        addGoldToPlayer(getStuffprice(choice), idPlayer);
        changeTextColor("green");
        printf("Vous avez vendu le stuff avec succès\n\n");
        changeTextColor("reset");
    } while (choice != 0);

    return;
}

/**
 * @brief Init the shop 
 * @param int idPlayer The id of the player
 * @return void
*/
void initShop(int idPlayer)
{
    clearScreen();

    printShopDealerAnsiiWay();
    printPlayerGold(idPlayer);
    printf("\n\n");
    printLine();
    printf("\n\n");

    printf("Que voulez-vous faire ?\n");
    printf("1. Acheter un stuff\n");
    printf("2. Vendre un stuff\n");
    printf("3. Quitter\n");

    int choice;
    do {
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > 3);

    switch (choice) {
    case 1:
        buyStuffInit(idPlayer);
        break;
    case 2:
        sellStuffInit(idPlayer);
        break;
    case 3:
        return;
    }

    initShop(idPlayer);

    printf("\n\n");
}

void equipStuff(int idPlayer, int stuffId)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    if (strcmp(getStuffType(stuffId), "Consumable") == 0) {
        printf("Vous ne pouvez pas équiper un objet consommable\n");
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE PLAYER_STUFF SET isEquip = 1 WHERE player_id = %d AND stuff_id = %d;", idPlayer, stuffId);

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

    addStatsStuff(idPlayer, stuffId);
}

void unequipStuff(int idPlayer, int stuffId)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE PLAYER_STUFF SET isEquip = 0 WHERE player_id = %d AND stuff_id = %d;", idPlayer, stuffId);

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

    removeStatsStuff(idPlayer, stuffId);
}

int biggestIdStuff(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);
    int biggestId = 0;

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT MAX(id) FROM STUFF;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_step(res);

    biggestId = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);

    return biggestId;
}