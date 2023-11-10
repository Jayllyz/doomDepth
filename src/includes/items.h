#ifndef ITEMS_H
#define ITEMS_H
#include "fight.h"
#include "shop.h"

stuff *getStuffInfo(int id);
int countSuffUsable(int idPlayer);
stuff **getUsableStuffs(int idPlayer, int count);
int showPlayerInventory(Player *p, Monster **m, int nbrMonster, int maxLines);
void printItemsLogs(Player *p, stuff *s, int nbrMonster, Monster **m, int maxLines);
int countPlayerStuff(int idPlayer);
void initInventory(int idPlayer);
void changeEquip(int idPlayer, stuff **s, char *type);
stuff **selectStuffFromPlayer(int idPlayer);

#endif // ITEMS_H
