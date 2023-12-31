#ifndef START_H
#define START_H

typedef struct {
    int id;
    char *name;
    char *description;
    int attack;
    int grade;
    int mana;
    char *type;
} Spell;

typedef struct {
    int id;
    char *name;
    int level;
    int experience;
    int life;
    int maxLife;
    int attack;
    int defense;
    int mana;
    int maxMana;
    int gold;
    int classId;
    Spell **spell;
} Player;

int createPlayer(char *name, int classId, Player *p);
int eraseDatabase();
int playerSetup(Player *p);
void continueGame(Player *p);
Spell *affectSpellToPlayer(int playerId, int spellId);
Spell **loadPlayerSpells(int playerId);
int getRandomSpellId(char *type);
void resetStuffTable();

#endif /* START_H */