#include "includes/fight.h"
#include "includes/ansii_print.h"
#include "includes/map.h"
#include "includes/utils.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DB_FILE "db/doomdepth.sqlite"
#define WIN_FILE "ascii/win.txt"
#define DEFEAT_FILE "ascii/defeat.txt"
#define MAX_PLAYER_SPELL 2
#define MAX_MONSTER_SPELL 1

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
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
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
    //TODO : clean function (big and unclear)
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

        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return NULL;
    }

    sqlite3_bind_int(res, 1, id);
    sqlite3_step(res);

    Monster *m = (Monster *)malloc(sizeof(Monster));
    m->id = sqlite3_column_int(res, 0);

    m->name = strdup((const char *)sqlite3_column_text(res, 1));
    m->attack = sqlite3_column_int(res, 2);
    m->defense = sqlite3_column_int(res, 3);
    m->life = sqlite3_column_int(res, 4);
    m->level = sqlite3_column_int(res, 5);

    m->spell = (Spell **)malloc(MAX_MONSTER_SPELL * sizeof(Spell *));
    for (int i = 0; i < MAX_MONSTER_SPELL; i++) {
        m->spell[i] = (Spell *)malloc(sizeof(Spell));
        m->spell[i]->id = -1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT spell_id FROM MONSTER_SPELL WHERE monster_id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select MONSTER_SPELL: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        free(m);
        return NULL;
    }

    sqlite3_bind_int(res, 1, id);
    rc = sqlite3_step(res);
    int i = 0;

    while (rc == SQLITE_ROW) {
        int spell_id = sqlite3_column_int(res, i);
        m->spell[i] = setMonsterSpell(spell_id);
        i++;
        rc = sqlite3_step(res);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
    return m;
}

Spell *setMonsterSpell(int idSpell)
{
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_stmt *select;
    rc = sqlite3_prepare_v2(db, "SELECT id, name, description, attack, grade, mana, type FROM SPELL WHERE id = ?;", -1, &select, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_bind_int(select, 1, idSpell);
    sqlite3_step(select);

    Spell *s = (Spell *)malloc(sizeof(Spell));

    s->id = sqlite3_column_int(select, 0);
    s->name = strdup((const char *)sqlite3_column_text(select, 1));
    s->description = strdup((const char *)sqlite3_column_text(select, 2));
    s->attack = sqlite3_column_int(select, 3);
    s->grade = sqlite3_column_int(select, 4);
    s->mana = sqlite3_column_int(select, 5);
    s->type = strdup((const char *)sqlite3_column_text(select, 6));

    return s;
}

void printLifeBarAtCoordinate(int life, int x, int y)
{
    changeTextColor("red");
    for (int i = 0; i < life; i++) {
        printCharAtCoordinate(x, y, '#');
        x++;
    }
    changeTextColor("reset");
}

void removeHP(int lastHP_x, int y, int life_to_remove)
{
    saveCursorPos();
    int i;
    for (i = 0; i < life_to_remove; i++) {
        printCharAtCoordinate(lastHP_x - i, y, 'X');
    }
    movCursor(100, 0);
    printf("lastHP_x: %d", lastHP_x);
    restoreCursorPos();
}

int getMonsterWidth(int id)
{
    char *file = (char *)malloc(sizeof(char) * 25);
    if (snprintf(file, 25, "ascii/monster/%d.txt", id) < 0) {
        printf("Fichier introuvable\n");
        free(file);
        return -1;
    }
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Fichier introuvable\n");
        free(file);
        return -1;
    }
    char *content = readFileContent(fp);
    int line_width = 0;
    while (content[line_width] != '\n')
        line_width++;
    free(file);
    free(content);
    return line_width;
}

void selectPlayerInfo(Player *p)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_prepare_v2(db, "SELECT level, attack, defense, experience, life, mana, gold FROM PLAYER WHERE id = ?;", -1, &res, NULL);

    if (rc != SQLITE_OK) {
        printf("Failed to select data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(res, 1, 1);
    sqlite3_step(res);

    p->level = sqlite3_column_int(res, 0);
    p->attack = sqlite3_column_int(res, 1);
    p->defense = sqlite3_column_int(res, 2);
    p->experience = sqlite3_column_int(res, 3);
    p->life = sqlite3_column_int(res, 4);
    p->mana = sqlite3_column_int(res, 5);
    p->gold = sqlite3_column_int(res, 6);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

void updatePlayerInfo(Player *p)
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(DB_FILE, &db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = sqlite3_mprintf("UPDATE PLAYER SET level = %d, attack = %d, defense = %d, experience = %d, life = %d, mana = %d, gold = %d WHERE id = 1;", p->level,
        p->attack, p->defense, p->experience, p->life, p->mana, p->gold);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to update data: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    sqlite3_free(sql);
    sqlite3_close(db);
}

Monster **loadFightScene(Player *p, int *nbrMonster, const int idToFight[])
{
    FILE *fplayer;
    clearScreen();
    selectPlayerInfo(p);
    printf("%s \nNiveau %d \nattack : %d \ndefense : %d\nxp : %d/50", p->name, p->level, p->attack, p->defense, p->experience);

    int nbMonster;

    if (idToFight[0] == -1)
        nbMonster = rand() % 3 + 1;
    else
        nbMonster = *nbrMonster;

    if (p->classId == 1)
        fplayer = fopen("ascii/player/warrior.txt", "r");
    else if (p->classId == 2)
        fplayer = fopen("ascii/player/mage.txt", "r");
    else
        fplayer = fopen("ascii/player/archer.txt", "r");

    char *contentPlayer = readFileContent(fplayer);
    changeTextColor("blue");
    printStringAtCoordinate(1, 7, contentPlayer);
    changeTextColor("reset");
    fclose(fplayer);

    int y = 50;

    Monster **monsters = (Monster **)malloc(sizeof(Monster *) * nbMonster);

    char *file = (char *)malloc(sizeof(char) * 25);
    for (int i = 0; i < nbMonster; i++) {
        FILE *fp;
        monsters[i] = (Monster *)malloc(sizeof(Monster));
        if (idToFight[0] == -1)
            monsters[i]->id = randomMonster(p->level);
        else
            monsters[i]->id = idToFight[i];

        monsters[i] = getMonsterInfo(monsters[i]->id);

        if (snprintf(file, 25, "ascii/monster/%d.txt", monsters[i]->id) < 0) {
            printf("Error during memory allocation\n");
            free(monsters);
            free(file);
            return NULL;
        }

        fp = fopen(file, "r");
        if (fp == NULL) {
            printf("Fichier introuvable\n");
            free(monsters);
            free(file);
            return NULL;
        }

        int line_width = getMonsterWidth(monsters[i]->id);

        changeTextColor("red");
        printStringAtCoordinate((int)(y + line_width / 2 - (strlen(monsters[i]->name) / 2)), 7, monsters[i]->name);
        printLifeBarAtCoordinate(monsters[i]->life, y, 8);
        printStringAtCoordinate(y, 10, readFileContent(fp));
        fclose(fp);

        y += 50;
    }

    *nbrMonster = nbMonster;
    free(contentPlayer);
    free(file);

    if (monsters == NULL)
        return NULL;
    else
        return monsters;
}

int normalAttack(Player *p, Monster *m)
{
    int damage = p->attack - m->defense;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 15) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    m->life -= damage;

    if (m->life < 0)
        m->life = 0;

    printf("Vous avez infligé \033[0;32m%d\033[0m dégats au %s\n", damage, m->name);
    printf("Il reste \033[0;31m%02d\033[0m points de vie au %s\n", m->life, m->name);

    return damage;
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

    p->life -= damage;

    if (p->life < 0) {
        p->life = 0;
    }

    printf("Le %s vous a infligé \033[0;31m%d\033[0m dégats\n", m->name, damage);
    printf("Il vous reste \033[0;32m%02d\033[0m points de vie\n", p->life);
}

void monsterSpell(Player *p, Monster *m)
{
    int damage = m->spell[0]->attack;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 10) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    p->life -= damage;

    if (p->life < 0)
        p->life = 0;

    printf("Le %s a utilisé le sort %s\n", m->name, m->spell[0]->name);
    printf("Il vous a infligé \033[0;32m%d\033[0m dégats\n", damage);
    printf("Il vous reste \033[0;31m%02d\033[0m points de vie\n", p->life);
}

void levelUp(Player *p)
{
    p->level++;
    p->life += 10;
    p->attack += 5;
    p->defense += 5;
    p->experience = 0;
}

void rewards(Player *p, Monster **m, int nbrMonster)
{
    int xp = (m[0]->level * 10) * nbrMonster;
    int gold = (m[0]->level * 5) * nbrMonster;

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
    printf("Il vous reste \033[0;32m%02d\033[0m points de vie\n", p->life);

    p->experience += xp;
    p->gold += gold;
    printf("Appuyez sur entrée pour continuer\n");
    getInputChar();
    free(content);
    free(m);
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
    exit(0);
}

int usePlayerSpell(Player *p, Monster *m, int spellId)
{
    if (p->mana < p->spell[spellId]->mana) {
        printf("Vous n'avez pas assez de mana pour utiliser ce sort\n");
        return 0;
    }

    int damage = p->spell[spellId]->attack - m->defense;
    if (damage <= 0)
        damage = 1;

    int randomCC = rand() % 100;
    if (randomCC < 10) {
        damage *= 2;
        printf("Coup critique !\n");
    }

    m->life -= damage;

    if (m->life < 0)
        m->life = 0;

    p->mana -= p->spell[spellId]->mana;
    printf("Vous avez utilisé le sort %s\n", p->spell[spellId]->name);
    printf("Vous avez infligé \033[0;32m%d\033[0m dégats au %s\n", damage, m->name);
    printf("Il reste \033[0;31m%02d\033[0m points de vie au %s\n", m->life, m->name);
    return damage;
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
                if (p->spell[i]->mana > p->mana)
                    changeTextColor("red");

                if (strcmp(p->spell[i]->type, "Soin") == 0)
                    printf("%d - %s | %d mana (Soin de %d) \n", i + 1, p->spell[i]->name, p->spell[i]->mana, p->spell[i]->attack);
                else
                    printf("%d - %s | %d mana (%d dégats)\n", i + 1, p->spell[i]->name, p->spell[i]->mana, p->spell[i]->attack);

                changeTextColor("reset");
            }
        }
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
        if (choice == 0)
            return -1;
    } while (choice < 1 || choice > MAX_PLAYER_SPELL);

    if (p->spell[choice - 1]->mana > p->mana)
        return -1;

    return choice - 1;
}

int chooseMonster(Monster **m, int nbrMonster)
{
    int choice;
    do {
        printf("Choisissez le monstre cibler :\n");
        for (int i = 0; i < nbrMonster; i++) {

            if (m[i]->life > 0)
                printf("%d - %s | HP %d\n", i + 1, m[i]->name, m[i]->life);
        }
        printf("Votre choix : ");
        choice = getInputInt();
        clearBuffer();
    } while (choice < 1 || choice > nbrMonster);

    return choice - 1;
}

void printLifeBar(Player *p, Monster **m, const int nbrMonster, int mana)
{
    int lifeBar = (p->life * 10) / 10;
    changeTextColor("green");
    printf("%s HP : ", p->name);
    for (int i = 0; i < lifeBar; i++)
        printf("\033[0;32m█\033[0m");
    for (int i = 0; i < 10 - lifeBar; i++)
        printf("\033[0;31m█\033[0m");
    printf("\033[0;32m %d \033[0m", p->life);
    printf("\n\n");

    if (mana == 1) {
        int manaBar = (p->mana * 10) / 10;
        changeTextColor("blue");
        printf("%s Mana : ", p->name);
        for (int i = 0; i < manaBar; i++)
            printf("\033[0;34m█\033[0m");
        for (int i = 0; i < 10 - manaBar; i++)
            printf("\033[0;31m█\033[0m");
        printf("\033[0;34m %d \033[0m", p->mana);
        printf("\n\n");
    }

    for (int i = 0; i < nbrMonster; i++) {
        changeTextColor("red");
        if (m[i]->life == 0) {
            printf("%s %d est mort\n\n", m[i]->name, i + 1);
            continue;
        }
        lifeBar = (m[i]->life * 10) / 10;
        printf("%s %d HP : ", m[i]->name, i + 1);
        for (int j = 0; j < lifeBar; j++)
            printf("\033[0;31m█\033[0m");
        printf("\033[0;31m %d \033[0m", m[i]->life);
        printf("\n\n");
    }

    changeTextColor("reset");
}

void clearLifeBar(int nbrMonster)
{
    for (int i = 0; i < nbrMonster + 5; i++) {
        printf("\033[K");
    }
}

void printCombatInterface(int nbrMonster, int damageNormalAttack)
{
    printf("Choisissez une action :\n");
    if (nbrMonster == 1)
        printf("1 - Attaque normal (%d dégats)\n", damageNormalAttack);
    else
        printf("1 - Attaque normal\n");
    printf("2 - Utiliser une compétence\n");
    printf("3 - Utiliser un objet (coming soon)\n");
    printf("4 - Abandonner\n");
}

void attackWithNormalAttack(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife)
{
    int target;

    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 7);

    if (nbrMonster == 1)
        target = 0;
    else
        target = chooseMonster(m, nbrMonster);

    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 21);

    int damage = normalAttack(p, m[target]);
    int max_hp = maxLife[target];
    removeHP(target * 50 + 50 + m[target]->life + damage, 8, damage > max_hp ? max_hp : damage);

    saveCursorPos();
    movCursor(0, maxLines + 4);
    clearLifeBar(nbrMonster);
    movCursor(0, maxLines + 4);
    printLifeBar(p, m, nbrMonster, 1);
    sleep(2);
    restoreCursorPos();
}

int attackWithSpell(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife)
{
    int target, spellChoice;

    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 7);

    spellChoice = showPlayerSpells(p);

    if (spellChoice == -1)
        return 0;

    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 4);

    if (nbrMonster == 1)
        target = 0;
    else
        target = chooseMonster(m, nbrMonster);

    clearLinesFrom(maxLines + 4);
    movCursor(0, maxLines + 21);

    int damage = usePlayerSpell(p, m[target], spellChoice);
    int max_hp = maxLife[target];
    removeHP(target * 50 + 50 + m[target]->life + damage, 8, damage > max_hp ? max_hp : damage);

    saveCursorPos();
    movCursor(0, maxLines + 4);
    clearLifeBar(nbrMonster);
    movCursor(0, maxLines + 4);
    printLifeBar(p, m, nbrMonster, 1);
    sleep(2);
    restoreCursorPos();

    return 1;
}

int monsterAlive(int nbMonster, Monster **m)
{
    int lifeTester = 0;
    for (int i = 0; i < nbMonster; i++) {
        if (m[i]->life <= 0)
            lifeTester++;
    }
    return lifeTester;
}

void monsterTurn(const int *nbrMonster, Monster **m, Player *p)
{
    int randomMob = rand() % *nbrMonster;
    int randomAttack = rand() % 10;

    while (m[randomMob]->life <= 0)
        randomMob = rand() % *nbrMonster;

    if (randomAttack % 2 == 0)
        monsterAttack(p, m[randomMob]);
    else
        monsterSpell(p, m[randomMob]);
}

void fightMonster(Player *p, Monster **m, int *nbrMonster)
{
    int *maxLife = (int *)malloc(sizeof(int) * *nbrMonster);
    for (int i = 0; i < *nbrMonster; i++) {
        maxLife[i] = m[i]->life;
    }
    int damageNormalAttack = p->attack - m[0]->defense;
    int choice;
    int maxLines = 0;
    char *filePath = (char *)malloc(sizeof(char) * 25);
    for (int i = 0; i < *nbrMonster; i++) {
        if (snprintf(filePath, 25, "ascii/monster/%d.txt", m[i]->id) < 0) {
            printf("Erreur lors de l'allocation de la mémoire\n");
            free(maxLife);
            free(filePath);
            return;
        }
        int lines = countLines(filePath);
        if (lines > maxLines)
            maxLines = lines;
    }
    free(filePath);
    maxLines += 10;
    int startPrint = maxLines + 4;
    int combatLog = maxLines + 21;

    while (p->life > 0) {
        do {
            movCursor(0, startPrint);
            clearLifeBar(*nbrMonster);

            movCursor(0, startPrint);
            printLifeBar(p, m, *nbrMonster, 1);

            printCombatInterface(*nbrMonster, damageNormalAttack);

            printf("Votre choix : ");
            choice = getInputInt();
            clearBuffer();
        } while (choice < 1 || choice > 4);

        clearLinesFrom(startPrint + 4);
        movCursor(0, combatLog);
        int validInput = 1;

        switch (choice) {
        case 1:
            attackWithNormalAttack(maxLines, *nbrMonster, m, p, maxLife);
            break;
        case 2:
            if (attackWithSpell(maxLines, *nbrMonster, m, p, maxLife) == 0)
                validInput = 0;

            break;
        case 4:
            p->life = 0;
            break;

        default:
            break;
        }

        int lifeTester = monsterAlive(*nbrMonster, m);

        if (lifeTester == *nbrMonster && p->life > 0)
            break;

        if (validInput == 1)
            monsterTurn(nbrMonster, m, p);
        else
            clearLinesFrom(startPrint);

        updatePlayerInfo(p);
    }

    if (p->life <= 0) {
        clearLinesFrom(maxLines + 4);
        movCursor(0, maxLines + 6);

        printLifeBar(p, m, *nbrMonster, 0);
        printf("Vous êtes mort\n");
        printf("Appuyez sur entrée pour continuer\n");
        getInputChar();

        clearScreen();
        free(m);
        free(maxLife);
        defeat();
    }
    else {
        clearScreen();
        rewards(p, m, *nbrMonster);
        free(maxLife);
    }
}
