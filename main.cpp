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
void draw_maze(int py, int px, int has_key) {
    clear();

   
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char tile = map[y][x];

            if (tile == '#') {
                attron(COLOR_PAIR(2));
                mvprintw(y, x, "%c", tile);
                attroff(COLOR_PAIR(2));
            } else if (tile == 'K' || tile == 'E') {
                attron(COLOR_PAIR(3));
                mvprintw(y, x, "%c", tile);
                attroff(COLOR_PAIR(3));
            } else {
                mvprintw(y, x, "%c", tile);
            }
        }
    }

    attron(COLOR_PAIR(1));
    mvprintw(py, px, "@");
    attroff(COLOR_PAIR(1));

    mvprintw(HEIGHT + 1, 0, "Kunci: %s",
             has_key ? "SUDAH DIAMBIL (Menuju 'E')" : "BELUM ADA (Cari 'K')");
    mvprintw(HEIGHT + 2, 0, "Gerak: WASD / Panah | Keluar: Q");
    refresh();
}

void show_win_screen(void) {
    clear();
    attron(COLOR_PAIR(3));
    mvprintw(HEIGHT / 2 - 1, 3, "SELAMAT! ANDA BERHASIL KELUAR!");
    attroff(COLOR_PAIR(3));
    mvprintw(HEIGHT / 2 + 1, 3, "Tekan tombol apa saja untuk selesai...");
    refresh();
    getch();
}
int main(void) {
    int py = 1;
    int px = 1;
    int has_key = 0;
    int won = 0;
    int ch;

    init_ui();

    while (1) {
        draw_maze(py, px, has_key);

        ch = getch();

        if (ch == 'q' || ch == 'Q') {
            break;
        }

        int ny = py;
        int nx = px;

        if (ch == KEY_UP || ch == 'w' || ch == 'W') {
            ny--;
        } else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            ny++;
        } else if (ch == KEY_LEFT || ch == 'a' || ch == 'A') {
            nx--;
        } else if (ch == KEY_RIGHT || ch == 'd' || ch == 'D') {
            nx++;
        }

        char target = map[ny][nx];

        if (target != '#') {
            if (target == 'K') {
                has_key = 1;
                map[ny][nx] = ' '; // Hapus kunci dari peta setelah diambil
            } else if (target == 'E') {
                if (has_key == 1) {
                    py = ny;
                    px = nx;
                    won = 1;
                    break;
                } else {
                    mvprintw(HEIGHT + 3, 0, "Pintu 'E' terkunci! Ambil kunci 'K' dulu.");
                    refresh();
                    getch();
                }
            }

            if (won == 0) {
                py = ny;
                px = nx;
            }
        }
    }

    if (won == 1) {
        show_win_screen();
    }

    endwin();
    return 0;
    void draw_maze(int py, int px, int has_key) {
    clear();
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char tile = map[y][x];

            if (tile == '#') {
                attron(COLOR_PAIR(2));
                mvprintw(y, x, "%c", tile);
                attroff(COLOR_PAIR(2));
            } else if (tile == 'K' || tile == 'E') {
                attron(COLOR_PAIR(3));
                mvprintw(y, x, "%c", tile);
                attroff(COLOR_PAIR(3));
            } else {
                mvprintw(y, x, "%c", tile);
            }
        }
    }

    attron(COLOR_PAIR(1));
    mvprintw(py, px, "@");
    attroff(COLOR_PAIR(1));

    mvprintw(HEIGHT + 1, 0, "Kunci: %s",
             has_key ? "SUDAH DIAMBIL (Menuju 'E')" : "BELUM ADA (Cari 'K')");
    mvprintw(HEIGHT + 2, 0, "Gerak: WASD / Panah | Keluar: Q");
    refresh();
}

void show_win_screen(void) {
    clear();
    attron(COLOR_PAIR(3));
    mvprintw(HEIGHT / 2 - 1, 3, "SELAMAT! ANDA BERHASIL KELUAR!");
    attroff(COLOR_PAIR(3));
    mvprintw(HEIGHT / 2 + 1, 3, "Tekan tombol apa saja untuk selesai...");
    refresh();
    getch();