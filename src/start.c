#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "db/doomdepth.sqlite"
#define MAX_PLAYER_SPELL 2

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
    int attack = 99;
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
        printf("Erreur lors de la création du joueur\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    int id = sqlite3_last_insert_rowid(db);

    p->spell = (Spell **)malloc(MAX_PLAYER_SPELL * sizeof(Spell *));
    for (int i = 0; i < MAX_PLAYER_SPELL; i++) {
        p->spell[i] = (Spell *)malloc(sizeof(Spell));
        p->spell[i]->id = -1;
    }

    switch (classId) {
    case 1: // Guerrier
        p->spell[0] = affectSpellToPlayer(id, 2);
        break;
    case 2: // Mage
        p->spell[0] = affectSpellToPlayer(id, 1);
        break;
    case 3: // Archer
        p->spell[0] = affectSpellToPlayer(id, 3);
        break;
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

Spell *affectSpellToPlayer(int playerId, int spellId)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    char *sql = sqlite3_mprintf("INSERT INTO PLAYER_SPELL (player_id, spell_id) VALUES (%d, %d);", playerId, spellId);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert data\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_free(sql);

    sqlite3_stmt *select;
    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, grade, mana, type FROM SPELL WHERE id = ?;", -1, &select, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(select, 1, spellId);
    sqlite3_step(select);

    Spell *s = (Spell *)malloc(sizeof(Spell));

    s->id = sqlite3_column_int(select, 0);
    s->name = (char *)malloc(sizeof(char) * (strlen((const char *)sqlite3_column_text(select, 1)) + 1));
    strcpy(s->name, (const char *)sqlite3_column_text(select, 1));
    s->description = (char *)malloc(sizeof(char) * (strlen((const char *)sqlite3_column_text(select, 2)) + 1));
    strcpy(s->description, (const char *)sqlite3_column_text(select, 2));
    s->attack = sqlite3_column_int(select, 3);
    s->grade = sqlite3_column_int(select, 4);
    s->mana = sqlite3_column_int(select, 5);
    s->type = (char *)malloc(sizeof(char) * (strlen((const char *)sqlite3_column_text(select, 6)) + 1));
    strcpy(s->type, (const char *)sqlite3_column_text(select, 6));

    sqlite3_finalize(select);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return s;
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

    do {
        clearScreen();
        printf("Bonjour %s\n\n", name);
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
