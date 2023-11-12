#ifndef SMITH_H
#define SMITH_H

#include "shop.h"

void printSmithAnsiiWay();

stuff getStuffOfPlayerById(int idStuff, int idPlayer);

int getStuffLevelOfPlayerById(int idStuff, int idPlayer);

void updateStuffStats(int level, int idStuff);

void initSmith(Player *p);
#endif