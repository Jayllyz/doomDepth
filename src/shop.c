#include "includes/shop.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/utils.h"
#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHOP "ascii/shop/shop.txt"
#define DEALER "ascii/shop/dealer.txt"
#define SHOPDEALER "ascii/shop/shopdealer.txt"
#define ITEMS "ascii/shop/stuff.txt"
#define DB_FILE "db/doomdepth.sqlite"

#define NB_ITEMS_EACH_ROW 2 //number of stuffs on each row
#define NB_COL_ITEMS 6 //number of columns for the ascii stuff element
#define NB_ROW_ITEMS 10 //number of rows for a stuff in the shop

#define ID_USER 1 //DEV PURPOSE

/**
 * @brief print a simple line
 * @return void
*/
#include <stdio.h>

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
        printf("Fichier de l'stuff introuvable\n");
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
    int length = 0;
    int stuffsOnCurrentLine = 0; // Compteur d'articles sur la ligne actuelle
    int ligne = 0;

    for (int i = 0; i < stuffCount; i++) {

        if (i == 0) {
            length = 0;
        }
        else if (stuffsOnCurrentLine % NB_ITEMS_EACH_ROW == 0) {
            length = 0;
        }
        else {
            length += strlen(stuffsList[i].description) + 30;
        }

        if (stuffsOnCurrentLine % NB_ITEMS_EACH_ROW == 0 && i != 0) {
            ligne += 10;
        }

        printStuffAnsiiWay(NB_COL_ITEMS * i + length, 11 + ligne);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 11 + ligne);
        printf("Identifiant: %d", stuffsList[i].id);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 12 + ligne);
        printf("Nom: %s", stuffsList[i].name);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 13 + ligne);
        printf("Description: %s", stuffsList[i].description);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 14 + ligne);
        printf("Attaque: %d", stuffsList[i].attack);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 15 + ligne);
        printf("Défense: %d", stuffsList[i].defense);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 16 + ligne);
        printf("Grade: %d", stuffsList[i].grade);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 17 + ligne);
        printf("Type: %s", stuffsList[i].type);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 18 + ligne);
        printf("Prix: %d", stuffsList[i].gold);

        stuffsOnCurrentLine++;
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

    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, defense, grade, gold, type FROM STUFF;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    stuff *stuffsList = NULL;
    *stuffCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        stuffsList = (stuff *)realloc(stuffsList, (*stuffCount + 1) * sizeof(stuff));
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
 * @brief Get all stuff of the shop from the database of the player
 * @param int *stuffCount The number of stuffs
 * @return stuff * The list of stuffs
*/
stuff *getStuffOfPLayer(int *stuffCount)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT S.id, S.name, S.description, S.attack, S.defense, S.grade, S.gold, S.type FROM STUFF AS S INNER JOIN PLAYER_STUFF AS PS ON S.id = PS.stuff_id WHERE PS.player_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(res, 1, ID_USER);

    stuff *stuffsList = NULL;
    *stuffCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        stuffsList = (stuff *)realloc(stuffsList, (*stuffCount + 1) * sizeof(stuff));
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
 * @return int The gold of the player
*/
int getplayerGold()
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

    sqlite3_bind_int(res, 1, 1);

    int gold = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        gold = sqlite3_column_int(res, 0);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return gold;
}

/**
 * @brief Check if a stuff is in the player's stuff
 * @param int idStuff The id of the stuff
 * @return int 0 if the stuff is not in the player's stuff, 1 otherwise
*/
int checkStuffIsInPlayerStuff(int idStuff)
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

    sqlite3_bind_int(res, 1, ID_USER);
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
 * @return void
*/
void addStuffToPlayerStuff(int idStuff)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = sqlite3_mprintf("INSERT INTO PLAYER_STUFF (player_id, stuff_id) VALUES (%d, %d);", ID_USER, idStuff);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }

    sqlite3_free(sql);
}

/**
 * @brief Remove a stuff to the player's stuff
 * @param int idStuff The id of the stuff
 * @return void
*/
void removeStuffFromPlayerStuff(int idStuff)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    char *sql = sqlite3_mprintf("DELETE FROM PLAYER_STUFF WHERE player_id = %d AND stuff_id = %d;", ID_USER, idStuff);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to delete data\n");
    }

    sqlite3_close(db);
}

/**
 * @brief remove gold to the player
 * @param int gold The gold to remove
 * @return void
*/
void removeGoldToPlayer(int gold)
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
    sqlite3_bind_int(res, 2, ID_USER);

    sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

/**
 * @brief Add gold to the player
 * @param int gold The gold to add
 * @return void
*/
void addGoldToPlayer(int gold)
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
    sqlite3_bind_int(res, 2, ID_USER);

    sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

/**
 * @brief Print the gold of the player
 * @return void
*/
void printPlayerGold()
{
    movCursor(65, 8);

    changeTextColor("yellow");
    printf("Gold: %d", getplayerGold());
    changeTextColor("reset");
}

/**
 * @brief Display the buy stuff menu and handle the choice of the user
 * @return void
*/
void buyStuffInit()
{
    clearScreen();
    printShopAnsiiWay();
    printPlayerGold();
    printf("\n\n");
    printLine();

    int stuffCount;
    stuff *stuffsList = getStuffFromShop(&stuffCount);
    if (stuffsList) {
        printStuffs(stuffsList, stuffCount);
        free(stuffsList);
    }
    else {
        printf("Aucun élément trouvé dans la base de données.\n");
    }

    int choice = -1;

    while(choice != 0){

        printf("Entrer le numéro du stuff que vous voulez acheter\n");
        printf("Entrer 0 pour quitter\n");

        choice = getInputInt();

        while (choice < 0) {
            printf("Veuillez entrer un choix valide\n");
            choice = getInputInt();
        }

        if (choice == 0) {
            return;
        }

        int price = getStuffprice(choice);

        if(checkStuffIsInPlayerStuff(choice)) {
            changeTextColor("orange");
            printf("Vous avez déjà ce stuff\n\n");
            changeTextColor("reset");
            continue;
        }

        if (price > getplayerGold()) {
            changeTextColor("red");
            printf("Vous n'avez pas assez d'or pour acheter ce stuff\n");
            changeTextColor("reset");
            continue;
        }

        removeGoldToPlayer(price);
        addStuffToPlayerStuff(choice);

        changeTextColor("green");
        printf("Vous avez acheté le stuff avec succès\n\n");
        changeTextColor("reset");
    }

    return;
}

/**
 * @brief Sell a stuff of the player to the shop
 * @return void
*/
void sellStuffInit()
{
    clearScreen();

    int choice = -1;

    while (choice != 0) {
        printDealerAnsiiWay();
        printPlayerGold();
        printf("\n\n");
        printLine();

        int stuffCount;
        stuff *stuffsList = getStuffOfPLayer(&stuffCount);
        if (stuffsList) {
            printStuffs(stuffsList, stuffCount);
            free(stuffsList);
        }
        else {
            changeTextColor("orange");
            printf("Mince ! Vous n'avez rien à vendre \n");
            changeTextColor("reset");
        }

        printf("Entrer le numéro du stuff que vous voulez vendre\n");
        printf("Entrer 0 pour quitter\n");

        choice = getInputInt();        

        while (choice < 0) {
            printf("Veuillez entrer un choix valide\n");
            choice = getInputInt();
        }

        if (choice == 0) {
            return;
        }

        if(!checkStuffIsInPlayerStuff(choice)) {
            printf("Vous ne possédez pas ce stuff\n");
            continue;
        }

        removeStuffFromPlayerStuff(choice);
        addGoldToPlayer(getStuffprice(choice));
        changeTextColor("green");
        printf("Vous avez vendu le stuff avec succès\n\n");
        changeTextColor("reset");
    }

    return;
}

/**
 * @brief Init the shop 
 * @return void
*/
void initShop()
{
    clearScreen();

    printShopDealerAnsiiWay();
    printPlayerGold();
    printf("\n\n");
    printLine();
    printf("\n\n");

    printf("Que voulez-vous faire ?\n");
    printf("1. Acheter un stuff\n");
    printf("2. Vendre un stuff\n");
    printf("3. Quitter\n");

    int choice = getInputInt();

    while (choice < 1 || choice > 3) {
        printf("Veuillez entrer un choix valide\n");
        choice = getInputInt();
    }

    switch (choice) {
    case 1:
        buyStuffInit();
        break;
    case 2:
        sellStuffInit();
        break;
    case 3:
        return;
    }

    initShop();

    printf("\n\n");
}

int main(int argc, char **argv)
{
    initShop();
}