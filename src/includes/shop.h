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

void printShopAnsiiWay();

void printItemAnsiiWay(int x, int y);

void printItems(stuff *itemsList, int itemCount);

stuff *getStuffFromShop(int *itemCount);

#endif