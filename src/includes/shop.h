#include "start.h"
#ifndef SHOP_H
#define SHOP_H

typedef struct {
    int id;
    char *name;
    char *description;
    int attack;
    int defense;
    int grade;
    int gold;
    char *type;
} stuff;

void printLine();

void printShopDealerAnsiiWay();

void printShopAnsiiWay();

void printDealerAnsiiWay();

void printStuffAnsiiWay(int x, int y);

void printStuffs(stuff *stuffsList, int stuffCount);

stuff *getStuffFromShop(int *stuffCount);

stuff *getStuffOfPLayer(int *stuffCount);

int getStuffprice(int idStuff);

int getplayerGold();

int checkStuffIsInPlayerStuff(int idStuff);

void addStuffToPlayerStuff(int idStuff);

void removeStuffFromPlayerStuff(int idStuff);

void removeStatsStuff(int idSuff);

void addStatsStuff(int idSuff);

void removeGoldToPlayer(int gold);

void addGoldToPlayer(int gold);

void printPlayerGold();

void buyStuffInit();

void sellStuffInit();

void initShop();
#endif