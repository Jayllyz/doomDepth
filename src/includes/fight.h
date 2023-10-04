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
} Monster;

int randomMonster(int level);
Monster *loadFightScene(Player *p);
int fightMonster(Player *p, Monster *m);

#endif // FIGHT_H