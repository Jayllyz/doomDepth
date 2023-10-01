#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "db/doomdepth.sqlite"

int createPlayer(char *name, int classId)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = sqlite3_mprintf(
        "INSERT INTO PLAYER(name, level, experience, life, attack, defense, mana, gold, class_id) VALUES('%s', 1, 0, 50, 10, 10, 10, 0, %d);", name, classId);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

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

int playerSetup()
{
    int erase = eraseDatabase();
    if (erase != 0)
        return 1;

    char *name;
    int choice;
    printf("Entrez votre nom de joueur : ");
    name = getInputString(30);
    printf("Bonjour %s\n", name);

    do {
        printf("Choisissez votre classe :\n");
        printf("1. Warrior\n");
        printf("2. Mage\n");
        printf("3. Rogue\n");
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > 3);

    return createPlayer(name, choice);
}
