typedef struct {
    int map_top;
    int map_left;
    int map_width;
    int map_height;
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
void eventHandler(char sign, Map m);
void movRight(int *x, int *y, char *map, Map m);
void movLeft(int *x, int *y, char *map, Map m);
void movUp(int *x, int *y, char *map, Map m);
void movDown(int *x, int *y, char *map, Map m);
int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top);