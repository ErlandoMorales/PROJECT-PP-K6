#include <ncurses.h>
#define WIDTH 30
#define HEIGHT 10

char map[HEIGHT][WIDTH + 1] = {
    "##############################",
    "#        #                   #",
    "# ###### # ##### ####### ### #",
    "# #      #     #       #     #",
    "# # ########## ####### ##### #",
    "# #                  #      K#",
    "# ############ # ### ####### #",
    "#              #   #         #",
    "#E################ ######### #",
    "#              #             #",
    "##############################"
};

void init_ui(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors() == TRUE) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Pemain '@'
        init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Dinding '#'
        init_pair(3, COLOR_GREEN, COLOR_BLACK);  // Kunci 'K' & Pintu Keluar 'E'
    }
}