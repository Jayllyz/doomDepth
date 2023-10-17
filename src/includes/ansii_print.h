#ifndef ANSii_PRINT_H
#define ANSii_PRINT_H
#include <stdio.h>

void changeTextColor(const char* color);
void printCharAtCoordinate(int x, int y, char c);
void movCursor(int x, int y);
void printStringAtCoordinate(int x, int y, char* s);
char* readFileContent(FILE* fp);
int countLines(char* filePath);

#endif