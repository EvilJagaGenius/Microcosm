#include <stdio.h>
#include <stdlib.h>

#include <curses.h>
#include "mcosm_tile.h"
#include "mcosm_mapgen.h"

void generateColors() {
    for (int i=0; i<8; i++) {  // FG color
        for (int j=0; j<8; j++) {  // BG color
            init_pair(1 + (i*8) + j, i, j);
        }
    }
}

void changeColor(short fgColor, short bgColor) {
    if (fgColor >= 8) {
        attron(A_BOLD);
        fgColor -= 8;
    } else {
        attroff(A_BOLD);
    }
    if (bgColor >= 8) {
        attron(A_BLINK);
        bgColor -= 8;
    } else {
        attroff(A_BLINK);
    }
    attron(COLOR_PAIR(1 + (fgColor * 8) + bgColor));
}

void renderTile(int row, int col, mcosm_tile tile) {
    if (tile.unit != NULL) {
        changeColor(8+tile.unit->owner, tile.terrain.color1);
        mvaddch(row, col, tile.unit->unit_type.symbol);
    } else {
        changeColor(tile.terrain.color2, tile.terrain.color1);
        mvaddch(row, col, '_');
    }
    changeColor(tile.terrain.color2, tile.terrain.color1);
    mvaddch(row, col+1, tile.terrain.symbol);
    mvaddch(row+1, col, '_');
    /*if (tile.feature != NULL) {
        mvaddch(row+1, col, tile.feature.symbol);
    } else {
        mvaddch(row+1, col, '_');
    }*/
    mvaddch(row+1, col+1, '+');
}



/*typedef struct {  // Represents a player/civilization in the game.
    int color;
    // C strings scare me
    char civName[16];
    char rulerName[16];
    int currentResearch;
    int science;
    int gold;
    // How should we keep track of who has discovered what in the tech tree?
} mcosm_civ;

typedef struct {
    char cityName[16];
    int owner;
    int producing;
    int currentProduction;
    char currentProductionType;  // Unit, improvement, wonder, other stuff
    int size;
} mcosm_city;

typedef struct {
    int prereq1;
    int prereq2;
    int type;
    bool civsDiscovered[7];  // We can only have 6 civs plus the barbarians.  Who has discovered this tech?  Maybe find a better solution
} mcosm_tech;*/

int main()
{
	initscr();			/* Start curses mode 		  */
	start_color();
	generateColors();

	//printw("Hello World !!!");	/* Print Hello World		  */
	//attroff(A_BOLD);
	//printf("Generating map\n");
    generateMap();
    //printf("Map generated\n");
    for (int row=0; row<15; row++) {
        for (int col=0; col<60; col++) {
            renderTile(row*2, col*2, WORLD[(row * WORLD_X) + col]);
        }
    }

	refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */
	//printf("COLOR_PAIRS:  %d\n", COLOR_PAIRS);
	//printf("COLORS:  %d\n", COLORS);
	printf("Freeing WORLD\n");
	if (WORLD != NULL) {
        free(WORLD);
    }
	printf("WORLD freed\n");

	return 0;
}
