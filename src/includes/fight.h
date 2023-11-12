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
    int isBoss;
    Spell **spell;
} Monster;

int randomMonster(int level);
void selectPlayerInfo(Player *p);
void updatePlayerInfo(Player *p);
Monster **loadFightScene(Player *p, int *nbrMonster, const int idToFight[]);
void fightMonster(Player *p, Monster **m, int *nbrMonster);
void defeat();
void rewardStuff(Player *p);
void rewards(Player *p, Monster **m, int nbrMonster);
char *levelUp(Player *p);
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
int chooseMonster(Monster **m, int nbrMonster);
int attackWithSpell(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife);
void updateMainLifeBars(int maxLines, int nbrMonster, Monster **m, Player *p);
void attackWithNormalAttack(int maxLines, int nbrMonster, Monster **m, Player *p, const int *maxLife);
void printCombatInterface(int nbrMonster, int damageNormalAttack);
void getCursorPosition(int *row, int *col);

#endif // FIGHT_H
