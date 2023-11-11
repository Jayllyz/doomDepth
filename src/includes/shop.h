#include "start.h"
#ifndef SHOP_H
#define SHOP_H

typedef struct {
    int id;
    char *name;
    char *description;
    int attack;
    int defense;
    int life;
    int mana;
    int grade;
    int gold;
    char *type;
    int effect;
    int isEquip;
} stuff;

void printLine();

void printShopDealerAnsiiWay();

void printShopAnsiiWay();

void printDealerAnsiiWay();

void printStuffAnsiiWay(int x, int y);

void printStuffs(stuff *stuffsList, int stuffCount);

stuff *getStuffFromShop(int *stuffCount);

stuff *getStuffOfPLayer(int *stuffCount, int idPlayer);

int getStuffprice(int idStuff);

int getPlayerGold(int idPlayer);

int checkStuffIsInPlayerStuff(int idStuff, int idPlayer);

void addStuffToPlayerStuff(int idStuff, int idPlayer);

void removeStuffFromPlayerStuff(int idStuff, int idPlayer);

char *getStuffType(int idStuff);

void removeStatsStuff(int idSuff, int idPlayer);

void addStatsStuff(int idSuff, int idPlayer);

void removeGoldToPlayer(int gold, int idPlayer);

void addGoldToPlayer(int gold, int idPlayer);

void printPlayerGold(int idPlayer);

void buyStuffInit();

void sellStuffInit();

void initShop(int idPlayer);

void equipStuff(int idPlayer, int stuffId);

void unequipStuff(int idPlayer, int stuffId);
#endif