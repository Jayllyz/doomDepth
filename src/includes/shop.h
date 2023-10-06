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
} shopItems;

void printShopAnsiiWay();

void printItemAnsiiWay();

#endif