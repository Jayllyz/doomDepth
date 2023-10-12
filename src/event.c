#include "includes/map.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/utils.h"
#include <stdlib.h>
#include <string.h>


void event(){
    clearScreen();
    FILE * fp = fopen("ascii/event.txt", "r");
    char * line = readFileContent(fp);
    changeTextColor("yellow");
    printf("%s", line);
    changeTextColor("reset");
    printf("\n");
    printf("Press any key to continue...\n");
    fgetc(stdin);
}