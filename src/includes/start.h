#ifndef START_H
#define START_H

typedef struct {
    int id;
    char *name;
    int level;
    int experience;
    int life;
    int attack;
    int defense;
    int mana;
    int gold;
    int classId;
} Player;

int createPlayer(char *name, int classId, Player *p);
int eraseDatabase();
int playerSetup(Player *p);

#endif /* START_H */