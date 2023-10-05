#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "db/doomdepth.sqlite"

int createPlayer(char *name, int classId, Player *p)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    int level = 1;
    int experience = 0;
    int life = 50;
    int attack = 10;
    int defense = 10;
    int mana = 25;
    int gold = 0;

    switch (classId) {
    case 1: // Guerrier
        attack += 2;
        defense += 2;
        break;
    case 2: // Mage
        attack += 2;
        mana += 2;
        break;
    case 3: // Archer
        attack += 3;
        life += 1;
        break;
    }

    char *sql
        = sqlite3_mprintf("INSERT INTO PLAYER (name, level, experience, life, attack, defense, mana, gold, class_id) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d);",
            name, level, experience, life, attack, defense, mana, gold, classId);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    p->name = name;
    p->level = level;
    p->experience = experience;
    p->life = life;
    p->attack = attack;
    p->defense = defense;
    p->mana = mana;
    p->gold = gold;
    p->classId = classId;

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 0;
}

int eraseDatabase()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = sqlite3_mprintf("DELETE FROM PLAYER; DELETE FROM PLAYER_STUFF; DELETE FROM PLAYER_SPELL;");

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to delete data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 0;
}

int playerSetup(Player *p)
{
    int erase = eraseDatabase();
    if (erase != 0)
        return 1;

    char *name;
    int choice;
    printf("Entrez votre nom de joueur : ");
    name = getInputString(30);
    clearScreen();
    printf("Bonjour %s\n\n", name);

    do {
        printf("Choisissez votre classe :\n");
        printf("1. Guerrier   | +2 Attaque  | +2 Defense\n");
        printf("2. Mage       | +2 Attaque  | +2 Mana\n");
        printf("3. Archer     | +3 Attaque  | +1 Vie\n");
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > 3);

    return createPlayer(name, choice, p);
}
