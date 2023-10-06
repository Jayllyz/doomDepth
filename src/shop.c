#include "includes/shop.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHOP "ascii/shop.txt"
#define ITEM_SWORD "ascii/shop/1.txt"
#define DB_FILE "db/doomdepth.sqlite"
void printShopAnsiiWay()
{
    FILE *fp = fopen(SHOP, "r");

    if (fp == NULL) {
        printf("Fichier du shop introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(10, 0, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

void printItemAnsiiWay()
{
    FILE *fp = fopen(ITEM_SWORD, "r");

    if (fp == NULL) {
        printf("Fichier de l'item introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(0, 6, content);
    changeTextColor("reset");
    printf("\n");

    free(content);
    fclose(fp);
}

shopItems *getShopItems(int *itemCount)
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

    shopItems *itemsList = NULL;
    *itemCount = 0;

    while (sqlite3_step(res) == SQLITE_ROW) {
        itemsList = (shopItems *)realloc(itemsList, (*itemCount + 1) * sizeof(shopItems));
        shopItems *currentItem = &itemsList[*itemCount];

        currentItem->id = sqlite3_column_int(res, 0);

        currentItem->name = strdup((const char *)sqlite3_column_text(res, 1));
        currentItem->description = strdup((const char *)sqlite3_column_text(res, 2));
        currentItem->attack = sqlite3_column_int(res, 3);
        currentItem->defense = sqlite3_column_int(res, 4);
        currentItem->grade = sqlite3_column_int(res, 5);
        currentItem->gold = sqlite3_column_int(res, 6);
        currentItem->type = strdup((const char *)sqlite3_column_text(res, 7));

        (*itemCount)++;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return itemsList;
}

void printItems(shopItems *itemsList, int itemCount)
{
    for (int i = 0; i < itemCount; i++) {
        printf("id : %d\n", itemsList[i].id);
        printf("name : %s\n", itemsList[i].name);
        printf("description : %s\n", itemsList[i].description);
        printf("attack : %d\n", itemsList[i].attack);
        printf("defense : %d\n", itemsList[i].defense);
        printf("grade : %d\n", itemsList[i].grade);
        printf("gold : %d\n", itemsList[i].gold);
        printf("type : %s\n", itemsList[i].type);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    clearScreen();
    printShopAnsiiWay();

    int itemCount;
    shopItems *itemsList = getShopItems(&itemCount);
    if (itemsList) {
        printItems(itemsList, itemCount);
        free(itemsList);
    }
    else {
        printf("Aucun élément trouvé dans la base de données.\n");
    }

    return 0;
}
