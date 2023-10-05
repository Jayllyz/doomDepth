#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DB_FILE "db/doomdepth.sqlite"
#define WIN_FILE "ascii/win.txt"
#define DEFEAT_FILE "ascii/defeat.txt"
#define MAX_PLAYER_SPELL 2

int randomMonster(int level)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT id FROM MONSTER WHERE level = ? ORDER BY RANDOM() LIMIT 1;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data\n");
        sqlite3_close(db);

        return -1;
    }
    int id;

    sqlite3_bind_int(res, 1, level);
    sqlite3_step(res);

    id = sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    sqlite3_close(db);

    return id;
}

Monster *getMonsterInfo(int id)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    rc = sqlite3_prepare_v2(db, "SELECT id, name, attack, defense, life, level FROM MONSTER WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {

        printf("Failed to select data\n");
        sqlite3_close(db);

        return NULL;
    }

    sqlite3_bind_int(res, 1, id);
    sqlite3_step(res);

    Monster *m = (Monster *)malloc(sizeof(Monster));
    m->id = sqlite3_column_int(res, 0);

    m->name = (char *)malloc(sizeof(char) * (strlen((const char *)sqlite3_column_text(res, 1)) + 1));
    strcpy(m->name, (const char *)sqlite3_column_text(res, 1));
    m->attack = sqlite3_column_int(res, 2);
    m->defense = sqlite3_column_int(res, 3);
    m->life = sqlite3_column_int(res, 4);
    m->level = sqlite3_column_int(res, 5);

    sqlite3_finalize(res);
    sqlite3_close(db);
    return m;
}

Monster *loadFightScene(Player *p)
{
    //TODO : Print player

    clearScreen();
    printf("%s \nNiveau %d \nattack : %d \ndefense : %d\n", p->name, p->level, p->attack, p->defense);
    int monsterId = randomMonster(p->level);

    Monster *m = getMonsterInfo(monsterId);

    char *file = (char *)malloc(sizeof(char) * 100);
    sprintf(file, "ascii/monster/%d.txt", monsterId);
    FILE *fp = fopen(file, "r");
    char *content = readFileContent(fp);

    printStringAtCoordinate(50, 1, content);
    int lines = countLines(file);
    movCursor(0, lines + 1);

    printf("---------------------------------------------------------------------------------\n");

    printf("Vous avez rencontré un %s de niveau %d\n", m->name, m->level);

    free(file);
    fclose(fp);
    return m;
}

void normalAttack(Player *p, Monster *m)
{
    int damage = p->attack - m->defense;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 15) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    if (damage < 0) {
        damage = 0;
    }

    m->life -= damage;

    if (m->life < 0) {
        m->life = 0;
    }

    printf("Vous avez infligé \033[0;32m%d\033[0m dégats au monstre\n", damage);
    printf("Il lui reste \033[0;31m%02d\033[0m points de vie\n", m->life);
}

void monsterAttack(Player *p, Monster *m)
{
    int randomAttack = (rand() % m->attack) + 1;
    int damage = randomAttack - p->defense;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 15) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    if (damage < 0) {
        damage = 0;
    }

    p->life -= damage;

    if (p->life < 0) {
        p->life = 0;
    }

    printf("Le monstre vous a infligé \033[0;31m%d\033[0m dégats\n", damage);
    printf("Il vous reste \033[0;32m%02d\033[0m points de vie\n", p->life);
}

void levelUp(Player *p)
{
    p->level++;
    p->life += 10;
    p->attack += 5;
    p->defense += 5;
    p->experience = 0;
}

void rewards(Player *p, Monster *m)
{
    int xp = m->level * 10;
    int gold = m->level * 5;

    if (p->experience + xp >= 50)
        levelUp(p);

    FILE *fp = fopen(WIN_FILE, "r");

    if (fp == NULL) {
        printf("Fichier de victoire introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("green");
    printStringAtCoordinate(0, 0, content);
    changeTextColor("reset");

    printf("Vous avez gagné %d points d'expérience\n", xp);
    printf("Progression exp : %d/%d\n", p->experience + xp, 50);
    printf("Vous avez gagné %d pièces d'or\n", gold);
    printf("Total or : %d\n", p->gold + gold);

    p->experience += xp;
    p->gold += gold;
    printf("Appuyez sur entrée pour continuer\n");
    getInputChar();
    free(content);
}

void defeat()
{
    FILE *fp = fopen(DEFEAT_FILE, "r");

    if (fp == NULL) {
        printf("Fichier de défaite introuvable\n");
        return;
    }

    char *content = readFileContent(fp);
    changeTextColor("red");
    printStringAtCoordinate(0, 0, content);
    changeTextColor("reset");

    printf("Vous êtes mort\n");
}

void clearLinesFrom(int startLine)
{
    printf("\033[%d;1H", startLine);
    printf("\033[J");
}

void usePlayerSpell(Player *p, Monster *m, int spellId)
{
    if (p->mana < p->spell[spellId]->mana) {
        printf("Vous n'avez pas assez de mana pour utiliser ce sort\n");
        return;
    }

    int damage = p->spell[spellId]->attack - m->defense;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 10) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    if (damage < 0) {
        damage = 0;
    }

    m->life -= damage;

    if (m->life < 0) {
        m->life = 0;
    }

    p->mana -= p->spell[spellId]->mana;
    printf("Vous avez utilisé le sort %s\n", p->spell[spellId]->name);
    printf("Vous avez infligé \033[0;32m%d\033[0m dégats au monstre\n", damage);
    printf("Il lui reste \033[0;31m%02d\033[0m points de vie\n", m->life);
}

int showPlayerSpells(Player *p)
{
    int choice;

    do {
        printf("Vos sorts :\n");
        printf("Mana : \033[0;34m%02d\033[0m\n", p->mana);
        printf("0 - Retour\n");
        for (int i = 0; i < MAX_PLAYER_SPELL; i++) {
            if (p->spell[i]->id != -1) {
                printf("%d - %s | %d mana\n", i + 1, p->spell[i]->name, p->spell[i]->mana);
            }
        }
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
        if (choice == 0)
            return -1;
    } while (choice < 1 || choice > MAX_PLAYER_SPELL);

    return choice - 1;
}

int fightMonster(Player *p, Monster *m)
{
    int damageNormalAttack = p->attack - m->defense;
    int choice;
    int spellChoice;
    char *filePath = (char *)malloc(sizeof(char) * 50);
    sprintf(filePath, "ascii/monster/%d.txt", m->id);
    int lines = countLines(filePath);
    while (p->life > 0 && m->life > 0) {

        do {
            movCursor(0, lines + 4);
            changeTextColor("green");
            printf("%s HP : %02d\n", p->name, p->life);
            changeTextColor("red");
            printf("%s HP : %02d\n\n", m->name, m->life);
            changeTextColor("reset");
            printf("Choisissez une action :\n");
            printf("1 - Attaque normal (%d dégats)\n", damageNormalAttack);
            printf("2 - Utiliser une compétence - Mana : \033[0;34m%02d\033[0m\n", p->mana);
            printf("3 - Utiliser un objet (coming soon)\n");
            printf("4 - Abandonner\n");
            printf("Votre choix : ");
            choice = getInputInt();
            clearBuffer();
        } while (choice < 1 || choice > 4);

        clearLinesFrom(lines + 9);
        movCursor(0, lines + 14);

        switch (choice) {
        case 1:
            normalAttack(p, m);
            monsterAttack(p, m);
            break;
        case 2:
            clearLinesFrom(lines + 4);
            movCursor(0, lines + 7);

            spellChoice = showPlayerSpells(p);
            if (spellChoice == -1) {
                choice = 0;
                continue;
            }
            clearLinesFrom(lines + 9);
            movCursor(0, lines + 14);
            usePlayerSpell(p, m, spellChoice);
            break;

        case 4:
            p->life = 0;
            break;

        default:
            break;
        }

        choice = 0;
        spellChoice = 0;
    }

    if (p->life <= 0) {
        clearScreen();
        defeat();
        return 0;
    }
    else {
        clearScreen();
        printf("Vous avez vaincu le monstre\n");
        rewards(p, m);
        return 1;
    }
}