#include "start.h"
#ifndef FIGHT_H
#define FIGHT_H

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
Monster **loadFightScene(Player *p, int *nbrMonster, int idToFight[]);
void fightMonster(Player *p, Monster **m, int *nbrMonster);
void clearLinesFrom(int startLine);
void defeat();
void rewards(Player *p, Monster **m);
void levelUp(Player *p);
void normalAttack(Player *p, Monster *m);
void monsterAttack(Player *p, Monster *m);
int showPlayerSpells(Player *p);
void usePlayerSpell(Player *p, Monster *m, int spellId);
void monsterSpell(Player *p, Monster *m);
Spell *setMonsterSpell(int idSpell);
void printLifeBar(Player *p, Monster **m, int *nbrMonster, int mana);
void monsterTurn(int *nbrMonster, Monster *m[], Player *p);
int monsterAlive(int nbMonster, Monster **m);
int attackWithSpell(int maxLines, int nbrMonster, Monster **m, Player p);
void attackWithNormalAttack(int maxLines, int nbrMonster, Monster **m, Player p);
void printCombatInterface(int nbrMonster, int damageNormalAttack);

#endif // FIGHT_H