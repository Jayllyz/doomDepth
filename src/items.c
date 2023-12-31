#include "includes/items.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/map.h"
#include "includes/shop.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DB_FILE "db/doomdepth.sqlite"
#define INVENTORY "ascii/inventory.txt"
#define ID_USER 1
#define NB_ITEMS_EACH_ROW 2 //number of stuffs on each row
#define NB_COL_ITEMS 6 //number of columns for the ascii stuff element
#define NB_COl_TEXT 90 //number of columns for the text of the stuff element

stuff *getStuffInfo(int id)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, defense, life, mana, type, grade, effect FROM STUFF WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(-1);
    }

    sqlite3_bind_int(res, 1, id);
    sqlite3_step(res);

    stuff *s = (stuff *)malloc(sizeof(stuff));

    s->id = id;
    const char *name = (const char *)sqlite3_column_text(res, 1);
    if (name == NULL) {
        s->name = NULL;
    }
    else {
        s->name = strdup(name);
    }
    const char *description = (const char *)sqlite3_column_text(res, 2);
    if (description == NULL) {
        s->description = NULL;
    }
    else {
        s->description = strdup(description);
    }
    s->attack = sqlite3_column_int(res, 3);
    s->defense = sqlite3_column_int(res, 4);
    s->life = sqlite3_column_int(res, 5);
    s->mana = sqlite3_column_int(res, 6);
    const char *type = (const char *)sqlite3_column_text(res, 7);
    if (type == NULL) {
        s->type = NULL;
    }
    else {
        s->type = strdup(type);
    }

    s->grade = sqlite3_column_int(res, 8);
    s->effect = sqlite3_column_int(res, 9);
    rc = sqlite3_prepare_v2(db, "SELECT isEquip FROM PLAYER_STUFF WHERE player_id = ? AND stuff_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        free(s);
        sqlite3_close(db);
        exit(-1);
    }

    sqlite3_bind_int(res, 1, ID_USER);

    sqlite3_bind_int(res, 2, id);

    sqlite3_step(res);

    s->isEquip = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);
    return s;
}

int countSuffUsable(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    sqlite3_close_v2(db);
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM STUFF JOIN PLAYER_STUFF ON STUFF.id = PLAYER_STUFF.stuff_id WHERE STUFF.type = 'Consumable' AND PLAYER_STUFF.player_id = 1;", -1, &res,
        NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_step(res);

    int count = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);
    return count;
}

stuff **getUsableStuffs(int idPlayer, int count)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(
        db, "SELECT id FROM STUFF JOIN PLAYER_STUFF ON STUFF.id = PLAYER_STUFF.stuff_id WHERE STUFF.type = 'Consumable' AND PLAYER_STUFF.player_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    rc = sqlite3_step(res);
    int i = 0;

    stuff **stuffs = (stuff **)malloc(sizeof(stuff *) * count);
    while (rc == SQLITE_ROW) {
        int stuff_id = sqlite3_column_int(res, i);
        stuffs[i] = getStuffInfo(stuff_id);
        i++;
        rc = sqlite3_step(res);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
    return stuffs;
}

enum stuffEffect { NOTHING = 0, DAMAGE = 1, BONUS = 2, MALUS = 3 };

int showPlayerInventory(Player *p, Monster **m, int nbrMonster, int maxLines)
{
    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 7);
    int choice;
    int count = countSuffUsable(p->id);
    if (count <= 0)
        return -2;

    stuff **stuffs = getUsableStuffs(p->id, count);

    if (stuffs == NULL)
        return -1;

    do {
        printf("Votre inventaire :\n");
        printf("0 - Retour\n");
        for (int i = 0; i < count; i++) {
            if (stuffs[i]->id != -1)
                printf("%d - %s\n", i + 1, stuffs[i]->name);
        }
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
        if (choice == 0)
            return -1;
    } while (choice < 1 || choice > 10);

    int effect = stuffs[choice - 1]->effect;
    choice--;

    if (strcmp(stuffs[choice]->type, "Consumable") == 0 && effect != NOTHING) {
        switch (effect) {
        case DAMAGE:
            clearLinesFrom(maxLines + 4);
            movCursor(0, maxLines + 7);
            int target = chooseMonster(m, nbrMonster);
            if (target == -1)
                return -1;
            m[target]->life -= stuffs[choice]->attack;
            printItemsLogs(p, stuffs[choice], target, m, maxLines);
            break;
        case BONUS:
            if (stuffs[choice]->attack > 0) {
                p->attack += stuffs[choice]->attack;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }

            else if (stuffs[choice]->defense > 0) {
                p->defense += stuffs[choice]->defense;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }
            else if (stuffs[choice]->life > 0)

                if (p->life < p->maxLife) {
                    clearLinesFrom(maxLines + 4);
                    //movCursor(0, maxLines + 21);
                    //printf("Vous avez utilisé %s\n", stuffs[choice]->name);
                    movCursor(100, 35);
                    clearLine(); // @TODO if cursor moved reset it
                    printf("\033[0;33m[ITEMS] \033[0m%s used %s\033[0m", p->name, stuffs[choice]->name);
                    movCursor(100, 36);
                    clearLine();
                    printf("\033[0;32m[HEAL] \033[0m%s: \033[0m", p->name);

                    if (p->life + stuffs[choice]->life > p->maxLife) {
                        p->life = p->maxLife;
                        updateMainLifeBars(maxLines, nbrMonster, m, p);
                        printf("Restore 100%c HP \033[0m", 37);
                    }
                    else {
                        p->life += stuffs[choice]->life;
                        updateMainLifeBars(maxLines, nbrMonster, m, p);
                        printf("+ %d HP \033[0m", stuffs[choice]->life);

                        //printf("Vous avez récupéré %d points de vie\n", stuffs[choice]->life);
                    }
                    printf("\n");
                    sleep(2);
                }
                else if (stuffs[choice]->mana > 0) {
                    p->mana += stuffs[choice]->mana;
                    printItemsLogs(p, stuffs[choice], target, m, maxLines);
                }
            break;
        case MALUS:
            if (stuffs[choice]->attack > 0) {
                p->attack -= stuffs[choice]->attack;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }
            else if (stuffs[choice]->defense > 0) {
                p->defense -= stuffs[choice - 1]->defense;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }
            else if (stuffs[choice]->life > 0) {
                p->life -= stuffs[choice]->life;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }
            else if (stuffs[choice]->mana > 0) {
                p->mana -= stuffs[choice]->mana;
                printItemsLogs(p, stuffs[choice], target, m, maxLines);
            }
            break;
        }

        removeStuffFromPlayerStuff(stuffs[choice]->id, p->id);
    }
    else {
        return -2;
    }

    return 0;
}

void printItemsLogs(Player *p, stuff *s, int nbrMonster, Monster **m, int maxLines)
{
    clearLinesFrom(maxLines + 4);
    //movCursor(0, maxLines + 21);
    saveCursorPos();
    //movCursor(100, 37);
    //changeTextColor("red");
    //printf("\033[0;32m[DMG] \033[0m%s: -\033[0;32m%d HP\033[0m", m->name, damage);
    //printf("Vous avez utilisé %s", s->name);

    if (s->effect == DAMAGE) {
        movCursor(100, 35);
        clearLine(); // @TODO if cursor moved reset it
        printf("\033[0;33m[ITEMS] \033[0m%s used %s\033[0m", p->name, s->name);
        movCursor(100, 36);
        clearLine();
        printf("\033[0;34m[DMG] \033[0m%s: - %d\033[0m", m[nbrMonster]->name, s->attack);

        if (s->attack > m[nbrMonster]->maxLife)
            s->attack = m[nbrMonster]->maxLife;

        removeHP(nbrMonster * 50 + 50 + m[nbrMonster]->life + s->attack - 1, 8 - (nbrMonster * 1), s->attack);

        //printf("Vous avez infligé %d dégats à %s\n", s->attack, m[nbrMonster]->name);
    }
    else if (s->effect == BONUS) {
        if (s->attack > 0)
            printf("Vous avez gagné %d points d'attaque\n", s->attack);
        else if (s->defense > 0)
            printf("Vous avez gagné %d points de défense\n", s->defense);
        else if (s->mana > 0)
            printf("Vous avez gagné %d points de mana\n", s->mana);
    }
    else if (s->effect == MALUS) {
        if (s->attack > 0)
            printf("Vous avez perdu %d points d'attaque\n", s->attack);
        else if (s->defense > 0)
            printf("Vous avez perdu %d points de défense\n", s->defense);
        else if (s->life > 0)
            printf("Vous avez perdu %d points de vie\n", s->life);
        else if (s->mana > 0)
            printf("Vous avez perdu %d points de mana\n", s->mana);
    }
    updateMainLifeBars(maxLines, nbrMonster, m, p);
}

int countPlayerStuff(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    sqlite3_close_v2(db);
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM PLAYER_STUFF WHERE player_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    sqlite3_step(res);

    int count = sqlite3_column_int(res, 0);

    sqlite3_finalize(res);
    sqlite3_close(db);
    return count;
}

stuff **selectStuffFromPlayer(int idPlayer)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    sqlite3_close_v2(db);
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT stuff_id FROM PLAYER_STUFF WHERE player_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_free(res);
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(res, 1, idPlayer);
    rc = sqlite3_step(res);
    int i = 0;

    stuff **s = (stuff **)malloc(sizeof(stuff *) * countPlayerStuff(idPlayer));
    while (rc == SQLITE_ROW) {
        int stuff_id = sqlite3_column_int(res, 0);
        s[i] = getStuffInfo(stuff_id);
        i++;
        rc = sqlite3_step(res);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
    return s;
}

void printStuffsInventory(stuff **stuffsList, int stuffCount)
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
        printf("Identifiant: %d", stuffsList[i]->id);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 12 + ligne);
        printf("Nom: %s", stuffsList[i]->name);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 13 + ligne);
        printf("Description: %s", stuffsList[i]->description);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 14 + ligne);
        printf("Attaque: %d", stuffsList[i]->attack);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 15 + ligne);
        printf("Défense: %d", stuffsList[i]->defense);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 16 + ligne);
        printf("Grade: %d", stuffsList[i]->grade);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 17 + ligne);
        printf("Type: %s", stuffsList[i]->type);

        movCursor(NB_COl_TEXT * col + NB_COL_ITEMS, 18 + ligne);
        printf("Equipé: %s", stuffsList[i]->isEquip == 1 ? "Oui" : "Non");

        col++;
    }
    printf("\n\n");
}

void printInventoryAnsiiWay()
{
    FILE *fp = fopen(INVENTORY, "r");

    if (fp == NULL) {
        printf("Fichier de l\'inventaire introuvable\n");
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

void initInventory(int idPlayer)
{
    int count = countPlayerStuff(idPlayer);
    if (count <= 0)
        return;
    int choice;
    do {
        clearScreen();

        printInventoryAnsiiWay();
        printLine();

        stuff **s = selectStuffFromPlayer(idPlayer);

        printStuffsInventory(s, count);

        printf("0 - Retour\n");
        printf("1 - Changer d'arme\n");
        printf("2 - Changer d'armure\n");
        printf("3 - Changer de casque\n");
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();

        switch (choice) {
        case 1:
            changeEquip(idPlayer, s, "Weapon", count);
            break;
        case 2:
            changeEquip(idPlayer, s, "Armor", count);
            break;
        case 3:
            changeEquip(idPlayer, s, "Helmet", count);
            break;
        }
        free(s);

    } while (choice != 0);

    clearScreen();
}

void changeEquip(int idPlayer, stuff **s, const char *type, int count)
{
    printf("\nQuel équipement voulez-vous équiper ? (-1 pour quitter)\n");

    int choice = -1;
    short int found = 0;
    saveCursorPos();
    do {
        restoreCursorPos();
        clearLine();
        choice = getInputInt();
        clearBuffer();

        if (choice < 0)
            return;

        for (int i = 0; i < count; i++) {
            if (s[i]->id == choice && strcmp(s[i]->type, type) == 0) {
                found = 1;
                choice = i;

                if (s[choice]->isEquip == 1)
                    found = 0;

                break;
            }
        }

    } while (found == 0);

    for (int i = 0; i < count; i++) {
        if (s[i]->isEquip == 1 && strcmp(s[i]->type, type) == 0) {
            removeStatsStuff(s[i]->id, idPlayer);
            unequipStuff(idPlayer, s[i]->id);
        }
    }

    equipStuff(idPlayer, s[choice]->id);

    addStatsStuff(s[choice]->id, idPlayer);

    printf("Vous avez équipé %s\n", s[choice]->name);
    sleep(2);
}
