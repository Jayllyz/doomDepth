#include "includes/items.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/shop.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DB_FILE "db/doomdepth.sqlite"
#define ID_USER 1

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

    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, defense, life, mana, type, effect FROM STUFF WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {

        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return NULL;
    }

    sqlite3_bind_int(res, 1, id);
    sqlite3_step(res);

    stuff *s = (stuff *)malloc(sizeof(stuff));

    s->id = sqlite3_column_int(res, 0);
    s->name = strdup((const char *)sqlite3_column_text(res, 1));
    s->description = strdup((const char *)sqlite3_column_text(res, 2));
    s->attack = sqlite3_column_int(res, 3);
    s->defense = sqlite3_column_int(res, 4);
    s->life = sqlite3_column_int(res, 5);
    s->mana = sqlite3_column_int(res, 6);
    s->type = strdup((const char *)sqlite3_column_text(res, 7));
    s->effect = sqlite3_column_int(res, 8);

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
                    movCursor(0, maxLines + 21);
                    printf("Vous avez utilisé %s\n", stuffs[choice]->name);
                    if (p->life + stuffs[choice]->life > p->maxLife) {
                        p->life = p->maxLife;
                        updateMainLifeBars(maxLines, nbrMonster, m, p);
                        printf("Vous avez récupéré tous vos points de vie\n");
                    }
                    else {
                        p->life += stuffs[choice]->life;
                        updateMainLifeBars(maxLines, nbrMonster, m, p);
                        printf("Vous avez récupéré %d points de vie\n", stuffs[choice]->life);
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
    movCursor(0, maxLines + 21);
    printf("Vous avez utilisé %s\n", s->name);
    if (s->effect == DAMAGE) {
        printf("Vous avez infligé %d dégats à %s\n", s->attack, m[nbrMonster]->name);
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