#include "includes/event.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/map.h"
#include "includes/utils.h"
#include <stdlib.h>
#include <string.h>

/**
 * RPG like adventure
 * How does it work ?
 * 1 - Village / Super Node / Forest
 * The player can move from village to village, village is a super node that is a place AND a village.
 * It allows us to move horizontally and vertically, in the village linked list or in the place linked list.
 * The player can only move to another village if he is at the super node village.
 *
 * 2 - Place / Node / Tree
 * Each village has a circular linked list of places.
 * Each node have 2 scenarios.
 *
 * 3 - Scenario / Branch
 * A scenario is bound to a place.
 * When the player reach a place, a random number is generated between 1 and 2 to chose which scenario it will play.
 * A scenario is a sequence of scenario -> TreeChoice -> scenario -> ...
 *
 *
 *
 */

void event()
{
    clearScreen();
    FILE *fp = fopen("ascii/event.txt", "r");
    char *line = readFileContent(fp);

    changeTextColor("yellow");
    printf("%s", line);
    changeTextColor("reset");

    printf("\n");
    printf("Press any key to continue...\n");
    fgetc(stdin);

    // @TODO Quest system RPG like

    TreeChoice *blue_pill0_1 = malloc(sizeof(TreeChoice));
    blue_pill0_1->id = 1;
    blue_pill0_1->context = "You took the blue pill, then a red pill";
    blue_pill0_1->situation = "You are feeling super weird, the god bestowed you with a new power !";
    blue_pill0_1->event = BONUS;
    blue_pill0_1->blue_pill = NULL;
    blue_pill0_1->red_pill = NULL;

    TreeChoice *blue_pill0_0 = malloc(sizeof(TreeChoice));
    blue_pill0_0->id = 1;
    blue_pill0_0->context = "You take the blue pill a second time";
    blue_pill0_0->situation = "Too bad you can't take the blue pill twice !";
    blue_pill0_0->event = DEATH;
    blue_pill0_0->blue_pill = NULL;
    blue_pill0_0->red_pill = NULL;

    TreeChoice *red_pill1_1 = malloc(sizeof(TreeChoice));
    red_pill1_1->id = 2;
    red_pill1_1->context = "You took the red pill a second time";
    red_pill1_1->situation = "You feel like you could fight a dragon !";
    red_pill1_1->event = FIGHT;
    red_pill1_1->blue_pill = NULL;
    red_pill1_1->red_pill = NULL;

    TreeChoice *red_pill1_0 = malloc(sizeof(TreeChoice));
    red_pill1_0->id = 2;
    red_pill1_0->context = "You took the red pill, then a blue pill";
    red_pill1_0->situation = "This allow you to see the matrix, you can now see the code of the matrix ! You can come back to the 1st time you took a pill !";
    red_pill1_0->blue_pill = NULL;
    red_pill1_0->red_pill = NULL;

    TreeChoice *blue_pill = malloc(sizeof(TreeChoice));
    blue_pill->id = 1;
    blue_pill->context = "You take the blue pill";
    blue_pill->situation = "You wake up in your bed and believe whatever you want to believe";
    blue_pill->blue_pill = blue_pill0_0;
    blue_pill->red_pill = blue_pill0_1;

    TreeChoice *red_pill = malloc(sizeof(TreeChoice));
    red_pill->id = 2;
    red_pill->context = "You take the red pill";
    red_pill->situation = "You stay in Wonderland and I show you how deep the rabbit-hole goes";
    red_pill->blue_pill = NULL;
    red_pill->red_pill = NULL;

    red_pill1_0->blue_pill = blue_pill;
    red_pill1_0->red_pill = red_pill;

    TreeChoice *start_point_village = malloc(sizeof(TreeChoice));
    start_point_village->id = 0;
    start_point_village->context = "You arrived to the village";
    start_point_village->situation = "A strange guy came to you and ask you to take a pill";
    start_point_village->blue_pill = blue_pill;
    start_point_village->red_pill = red_pill;

    TreeChoice *start_point_bar = malloc(sizeof(TreeChoice));
    start_point_bar->id = 1;
    start_point_bar->context = "You arrived to the bar";
    start_point_bar->situation = "A strange guy came to you and ask you to take a pill";
    start_point_bar->blue_pill = blue_pill;
    start_point_bar->red_pill = red_pill;

    TreeChoice *start_point_forest = malloc(sizeof(TreeChoice));
    start_point_forest->id = 2;
    start_point_forest->context = "You arrived to the forest";
    start_point_forest->situation = "A strange guy came to you and ask you to take a pill";
    start_point_forest->blue_pill = blue_pill;
    start_point_forest->red_pill = red_pill;

    Scenario *scenario_village = malloc(sizeof(Scenario));
    scenario_village->id = 0;
    scenario_village->choice = start_point_village;

    Scenario *scenario_bar = malloc(sizeof(Scenario));
    scenario_bar->id = 1;
    scenario_bar->choice = start_point_bar;

    Scenario *scenario_forest = malloc(sizeof(Scenario));
    scenario_forest->id = 2;
    scenario_forest->choice = start_point_forest;

    Village *village = malloc(sizeof(Village));
    village->id = 0;
    village->name = "Village";
    village->description = "The village is a small village with a few houses";
    village->scenario = scenario_village;
    village->next_place = NULL;

    Place *bar = malloc(sizeof(Place));
    bar->id = 1;
    bar->name = "Bar";
    bar->description = "The bar stinks of alcohol and cigarettes";
    bar->scenario = scenario_bar;
    bar->next = NULL;

    Place *forest = malloc(sizeof(Place));
    forest->id = 2;
    forest->name = "Forest";
    forest->description = "The forest is a dark and dangerous place";
    forest->scenario = scenario_forest;
    forest->next = NULL;

    village->next_place = bar;
    bar->next = forest;
    forest->next = bar;

    Protagonist *protagonist = malloc(sizeof(Protagonist));
    protagonist->village = NULL;
    protagonist->place = NULL;

    // Starting point is defined randomly
    int random = rand() % 2;
    if (random == 0) {
        protagonist->village = village;
        protagonist->scenario = scenario_village;
        protagonist->choice = scenario_village->choice;
    }
    else if (random == 1) {
        protagonist->place = bar;
        protagonist->scenario = scenario_bar;
        protagonist->choice = scenario_bar->choice;
    }
    else if (random == 2) {
        protagonist->place = forest;
        protagonist->scenario = scenario_forest;
        protagonist->choice = scenario_forest->choice;
    }

    int choice = 0;
    while (protagonist->choice->blue_pill != NULL && protagonist->choice->red_pill != NULL) {
        printf("%s\n", protagonist->choice->context);
        printf("%s\n", protagonist->choice->situation);
        printf("1 - %s\n", protagonist->choice->blue_pill->context);
        printf("2 - %s\n", protagonist->choice->red_pill->context);
        printf("Your choice : \n");
        scanf("%d", &choice);

        if (choice == 1)
            protagonist->choice = protagonist->choice->blue_pill;
        else if (choice == 2)
            protagonist->choice = protagonist->choice->red_pill;
    }

    printf("%s\n", protagonist->choice->context);
    printf("%s\n", protagonist->choice->situation);

    switch (protagonist->choice->event) {
    case DEATH:
        changeTextColor("red");
        printf("You died !\n");
        break;
    case BONUS:
        changeTextColor("green");
        printf("You got a bonus !\n");
        break;
    case FIGHT:
        changeTextColor("yellow");
        printf("You got a fight !\n");
        break;
    default:
        changeTextColor("blue");
        printf("End of side quest.\n");
        break;
    }
    changeTextColor("reset");
    scanf("%d", &choice);
}
