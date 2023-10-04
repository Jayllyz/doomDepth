#include "includes/fight.h"
#include "includes/ansii_print.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DB_FILE "db/doomdepth.sqlite"

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
    int monsterId = randomMonster(p->level);

    Monster *m = getMonsterInfo(monsterId);

    char *file = (char *)malloc(sizeof(char) * 100);
    sprintf(file, "ascii/monster/%d.txt", monsterId);
    FILE *fp = fopen(file, "r");
    char *content = readFileContent(fp);

    printStringAtCoordinate(50, 0, content);
    movCursor(0, 25);

    printf("Vous avez rencontré un %s de niveau %d\n", m->name, m->level);

    free(file);
    fclose(fp);
    return m;
}

int normalAttack(Player *p, Monster *m)
{
    int damage = p->attack - m->defense;

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

    printf("Vous avez infligé %d dégats au monstre\n", damage);
    printf("Il lui reste %d points de vie\n", m->life);

    return m->life;
}

int monsterAttack(Player *p, Monster *m)
{
    int randomAttack = rand() % m->attack;
    int damage = randomAttack - p->defense;

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

    printf("Le monstre vous a infligé %d dégats\n", damage);
    printf("Il vous reste %d points de vie\n", p->life);

    return p->life;
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

    printf("Vous avez gagné %d points d'expérience\n", xp);
    printf("Progression : %d/%d\n", p->experience + xp, 50);
    printf("Vous avez gagné %d pièces d'or\n", gold);

    p->experience += xp;
    p->gold += gold;
    printf("Appuyez sur entrée pour continuer\n");
    getInputChar();
}

void clearLinesFrom(int startLine)
{

    printf("\033[%d;1H", startLine);
    printf("\033[J");
}

int fightMonster(Player *p, Monster *m)
{
    int playerLife = p->life;
    int monsterLife = m->life;
    int damageNormalAttack = p->attack;
    int choice;

    while (playerLife > 0 && monsterLife > 0) {

        do {
            movCursor(0, 26);
            printf("(Vous) HP : %d\n", playerLife);
            printf("(%s) HP : %d\n", m->name, monsterLife);
            printf("Choisissez une action :\n");
            printf("1 - Attaque normal (%d dégats)\n", damageNormalAttack);
            printf("2 - Utiliser une compétence (coming soon)\n");
            printf("3 - Utiliser un objet (coming soon)\n");
            printf("4 - Abandonner\n");
            printf("Votre choix : ");
            choice = getInputInt();
            clearBuffer();
        } while (choice < 1 || choice > 4);

        clearLinesFrom(30);
        movCursor(0, 40);

        switch (choice) {
        case 1:
            monsterLife = normalAttack(p, m);
            playerLife = monsterAttack(p, m);
            break;

        default:
            break;
        }

        choice = 0;
    }

    if (playerLife <= 0) {
        printf("Vous êtes mort\n");
        return 0;
    }
    else {
        clearScreen();
        printf("Vous avez vaincu le monstre\n");
        rewards(p, m);
        return 1;
    }
}