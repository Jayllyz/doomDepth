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

#define SHOP "ascii/shop.txt"
#define ITEMS "ascii/shop/items.txt"
#define DB_FILE "db/doomdepth.sqlite"

#define NB_ITEMS_EACH_ROW 2 //number of items on each row
#define NB_COL_ITEMS 6 //number of columns for the ascii item element
#define NB_ROW_ITEMS 10 //number of rows for an item in the shop

#define ID_USER 1 //DEV PURPOSE

/**
 * @brief print a simple line
 * @return void
*/
void printLine()
{
    printf("--------------------");
    changeTextColor("green");
    printf("--------------------");
    changeTextColor("reset");
    printf("--------------------");
    changeTextColor("green");
    printf("--------------------");
    changeTextColor("reset");
    printf("--------------------");
    changeTextColor("green");
    printf("--------------------");
    changeTextColor("reset");
    printf("--------------------");
    changeTextColor("green");
    printf("--------------------");
    changeTextColor("reset");
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
void printItemAnsiiWay(int x, int y)
{
    FILE *fp = fopen(ITEMS, "r");

    if (fp == NULL) {
        printf("Fichier de l'item introuvable\n");
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
 * @brief Get all stuff of the shop from the database
 * @param int *itemCount The number of items
 * @return stuff * The list of items
*/
stuff *getStuffFromShop(int *itemCount)
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

    stuff *itemsList = NULL;
    *itemCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        itemsList = (stuff *)realloc(itemsList, (*itemCount + 1) * sizeof(stuff));
        stuff *currentItem = &itemsList[*itemCount];

        currentItem->id = sqlite3_column_int(res, 0);

        currentItem->name = strdup((const char *)sqlite3_column_text(res, 1));
        currentItem->description = strdup((const char *)sqlite3_column_text(res, 2));
        currentItem->attack = sqlite3_column_int(res, 3);
        currentItem->defense = sqlite3_column_int(res, 4);
        currentItem->grade = sqlite3_column_int(res, 5);
        currentItem->gold = sqlite3_column_int(res, NB_COL_ITEMS);
        currentItem->type = strdup((const char *)sqlite3_column_text(res, 7));

        (*itemCount)++;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return itemsList;
}

/**
 * @brief Print all items of the shop
 * @param stuff *itemsList The list of items
 * @param int itemCount The number of items
 * @return void
 * @todo pb in positioning of the items (NB_COL_ITEMs * i) is not necessary but necessary for printItemAnsiiWay
*/
void printItems(stuff *itemsList, int itemCount)
{
    int length = 0;
    int itemsOnCurrentLine = 0; // Compteur d'articles sur la ligne actuelle
    int ligne = 0;

    for (int i = 0; i < itemCount; i++) {

        if (i == 0) {
            length = 0;
        }
        else if (itemsOnCurrentLine % NB_ITEMS_EACH_ROW == 0) {
            length = 0;
        }
        else {
            length += strlen(itemsList[i].description) + 30;
        }

        if (itemsOnCurrentLine % NB_ITEMS_EACH_ROW == 0 && i != 0) {
            ligne += 10;
        }

        printItemAnsiiWay(NB_COL_ITEMS * i + length, 11 + ligne);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 11 + ligne);
        printf("Identifiant: %d", itemsList[i].id);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 12 + ligne);
        printf("Nom: %s", itemsList[i].name);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 13 + ligne);
        printf("Description: %s", itemsList[i].description);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 14 + ligne);
        printf("Attaque: %d", itemsList[i].attack);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 15 + ligne);
        printf("Défense: %d", itemsList[i].defense);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 16 + ligne);
        printf("Grade: %d", itemsList[i].grade);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 17 + ligne);
        printf("Type: %s", itemsList[i].type);

        movCursor((NB_COL_ITEMS * i) + NB_COL_ITEMS + length, 18 + ligne);
        printf("Prix: %d", itemsList[i].gold);

        itemsOnCurrentLine++;
    }
    printf("\n\n");
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

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

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
 * @brief Add an item to the player's stuff
 * @param int idItem The id of the item
 * @return void
*/
void addItemToPlayerStuff(int idItem)
{
    //Table  = PLAYER_STUFF

    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, "INSERT INTO PLAYER_STUFF (id_player, id_stuff) VALUES (?, ?);", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(res, 1, ID_USER);
    sqlite3_bind_int(res, 2, idItem);

    sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

/**
 * @brief Remove an item to the player's stuff
 * @param int idItem The id of the item
 * @return void
*/
void removeItemFromPlayerStuff(int idItem)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, "DELETE FROM PLAYER_STUFF WHERE id_player = ? AND id_stuff = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(res, 1, ID_USER);
    sqlite3_bind_int(res, 2, idItem);

    sqlite3_step(res);

    sqlite3_finalize(res);
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
 * @brief Init the shop 
 * @return void
*/
void initShop()
{
    clearScreen();

    printShopAnsiiWay();

    movCursor(60, 8);

    changeTextColor("yellow");
    printf("Gold: %d", getplayerGold());
    changeTextColor("reset");

    int itemCount;
    stuff *itemsList = getStuffFromShop(&itemCount);
    if (itemsList) {
        printItems(itemsList, itemCount);
        free(itemsList);
    }
    else {
        printf("Aucun élément trouvé dans la base de données.\n");
    }

    printLine();

    printf("\n\n");

    printf("Que voulez-vous faire ?\n");
    printf("1. Acheter un item\n");
    printf("2. Vendre un item\n");
    printf("3. Quitter\n");

    int choice = getInputInt();

    while (choice < 1 || choice > 3) {
        printf("Veuillez entrer un choix valide\n");
        choice = getInputInt();
    }

    switch (choice) {
    case 1:
        //buyItem();
        break;
    case 2:
        //sellItem();
        break;
    case 3:
        return;
    }

    printf("\n\n");
}

int main(int argc, char **argv)
{
    initShop();
}