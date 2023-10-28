#ifndef FIGHT_H
#define FIGHT_H
#include "start.h"

typedef struct {
    int id;
    char *name;
    int level;
    int life;
    int attack;
    int defense;
    Spell **spell;
} Monster;

int randomMonster(int level);
void selectPlayerInfo(Player *p);
void updatePlayerInfo(Player *p);
Monster **loadFightScene(Player *p, int *nbrMonster, const int idToFight[]);
void fightMonster(Player *p, Monster **m, int *nbrMonster);
void clearLinesFrom(int startLine);
void defeat();
void rewards(Player *p, Monster **m, int nbrMonster);
void levelUp(Player *p);
int normalAttack(Player *p, Monster *m);
void monsterAttack(Player *p, Monster *m);
int showPlayerSpells(Player *p);
int usePlayerSpell(Player *p, Monster *m, int spellId);
void monsterSpell(Player *p, Monster *m);
Spell *setMonsterSpell(int idSpell);
void printLifeBar(Player *p, Monster **m, const int nbrMonster, int mana);
void clearLifeBar(int nbrMonster);
void monsterTurn(const int *nbrMonster, Monster **m, Player *p);
int monsterAlive(int nbMonster, Monster **m);
int attackWithSpell(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife);
void attackWithNormalAttack(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife);
void printCombatInterface(int nbrMonster, int damageNormalAttack);

#endif // FIGHT_H
