#ifndef UTILS_H
#define UTILS_H

int getInputInt();
char getInputChar();
char *getInputString(int size);
void clearBuffer();
void clearScreen();
void clearLinesFrom(int startLine);
char *getClassName(int classId);

#endif /* UTILS_H */