#include "start.h"

typedef struct {
    int map_top;
    int map_left;
    int map_width;
    int map_height;
    char *map;
    int player_x;
    int player_y;
} Map;



void saveCursorPos();
void restoreCursorPos();
void clearLine();
char signAtCoordinate(const char *map, int x, int y, Map m);
char convertSigntoChar(char sign);
void setSignColor(char sign);
void printSignWithColor(char sign);
void printMapAtCoordinate(int x, int y, char *m);
void movCursor(int x, int y);
void printSignAtCoordinate(char *map, int x, int y, Map m);
void printPlayerAtCoordinate(int x, int y);
void eventHandler(char sign, Map m, Player *p);
void movRight(int *x, int *y, char *map, Map m, Player *p);
void movLeft(int *x, int *y, char *map, Map m, Player *p);
void movUp(int *x, int *y, char *map, Map m, Player *p);
void movDown(int *x, int *y, char *map, Map m, Player *p);
void mov(Map *m, Player *p);
int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top, Player *p);