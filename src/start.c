#include "includes/start.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "db/doomdepth.sqlite"
#define MAX_PLAYER_SPELL 4

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
    int life = 99;
    int attack = 20;
    int defense = 10;
    int mana = 25;
    int gold = 100;
    int maxLife = 99;
    int maxMana = 25;

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

    char *sql = sqlite3_mprintf("INSERT INTO PLAYER (id, name, level, experience, life, attack, defense, mana, gold, class_id, maxLife, maxMana) VALUES ('%d', '%s', %d, "
                                "%d, %d, %d, %d, %d, %d, %d, %d, %d);",
        1, name, level, experience, life, attack, defense, mana, gold, classId, maxLife, maxMana);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Erreur lors de la création du joueur\n");
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    int id = (int)sqlite3_last_insert_rowid(db);

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

    p->id = 1;
    p->name = name;
    p->level = level;
    p->experience = experience;
    p->life = life;
    p->attack = attack;
    p->defense = defense;
    p->mana = mana;
    p->gold = gold;
    p->classId = classId;
    p->maxLife = maxLife;
    p->maxMana = maxMana;

    sqlite3_free(sql);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 0;
}

int getRandomSpellId(char *type)
{
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_stmt *select;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM SPELL WHERE type = ? ORDER BY RANDOM() LIMIT 1;", -1, &select, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(select, 1, type, -1, SQLITE_STATIC);

    sqlite3_step(select);
    int id = sqlite3_column_int(select, 0);

    sqlite3_finalize(select);
    sqlite3_close(db);
    return id;
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
        printf("Failed to insert data %s\n", err_msg);
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
    s->name = strdup((const char *)sqlite3_column_text(select, 1));
    s->description = strdup((const char *)sqlite3_column_text(select, 2));
    s->attack = sqlite3_column_int(select, 3);
    s->grade = sqlite3_column_int(select, 4);
    s->mana = sqlite3_column_int(select, 5);
    s->type = strdup((const char *)sqlite3_column_text(select, 6));

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

void continueGame(Player *p)
{
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_stmt *select;
    rc = sqlite3_prepare_v2(
        db, "SELECT id, name, level, experience, life, attack, defense, mana, gold, class_id, maxLife, maxMana FROM PLAYER WHERE id = ?;", -1, &select, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(select, 1, 1);
    sqlite3_step(select);

    p->id = sqlite3_column_int(select, 0);
    p->name = strdup((const char *)sqlite3_column_text(select, 1));
    p->level = sqlite3_column_int(select, 2);
    p->experience = sqlite3_column_int(select, 3);
    p->life = sqlite3_column_int(select, 4);
    p->defense = sqlite3_column_int(select, 6);
    p->mana = sqlite3_column_int(select, 7);
    p->classId = sqlite3_column_int(select, 9);
    p->maxLife = sqlite3_column_int(select, 10);
    p->maxMana = sqlite3_column_int(select, 11);
    p->spell = loadPlayerSpells(p->id);

    sqlite3_finalize(select);

    sqlite3_close(db);
}

Spell **loadPlayerSpells(int playerId)
{
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_stmt *select;

    rc = sqlite3_prepare_v2(db,
        "SELECT COUNT(*), SPELL.id, SPELL.name, SPELL.description, SPELL.attack, SPELL.grade, SPELL.mana, SPELL.type FROM SPELL INNER JOIN PLAYER_SPELL ON SPELL.id = "
        "PLAYER_SPELL.spell_id WHERE PLAYER_SPELL.player_id = ?;",
        -1, &select, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(select, 1, playerId);
    sqlite3_step(select);

    int nbrSpell = sqlite3_column_int(select, 0);

    Spell **spells = (Spell **)malloc(nbrSpell * sizeof(Spell *));

    for (int i = 0; i < nbrSpell; i++) {
        spells[i] = (Spell *)malloc(sizeof(Spell));
        spells[i]->id = sqlite3_column_int(select, 1);
        spells[i]->name = strdup((const char *)sqlite3_column_text(select, 2));
        spells[i]->description = strdup((const char *)sqlite3_column_text(select, 3));
        spells[i]->attack = sqlite3_column_int(select, 4);
        spells[i]->grade = sqlite3_column_int(select, 5);
        spells[i]->mana = sqlite3_column_int(select, 6);
        spells[i]->type = strdup((const char *)sqlite3_column_text(select, 7));
        sqlite3_step(select);
    }

    sqlite3_finalize(select);
    sqlite3_close(db);
    return spells;
}