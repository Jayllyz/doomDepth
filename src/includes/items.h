#ifndef ITEMS_H
#define ITEMS_H
#include "shop.h"
#include "fight.h"

stuff *getStuffInfo(int id);
int countSuffUsable(int idPlayer);
stuff **getUsableStuffs(int idPlayer, int count);
int showPlayerInventory(Player *p, Monster **m, int nbrMonster, int maxLines);
void printItemsLogs(Player *p, stuff *s, int nbrMonster, Monster **m, int maxLines);

#endif // ITEMS_H
