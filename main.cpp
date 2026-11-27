#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 44
#define HEIGHT 15
#define NUM_LEVELS 2
#define NUM_ENEMIES 2
#define MAX_TIPS 3

// [ARRAY 2D] Peta beberapa level:
// '#' dinding | ' ' jalan | 'K' kunci | 'E' pintu keluar
char levels[NUM_LEVELS][HEIGHT][WIDTH + 1] = {
    {
        "############################################",
        "#       #                 #                #",
        "####### # ############### # ############## #",
        "#       #       #         #       #        #",
        "# ############# # ####### ####### # ###### #",
        "# #           # #   #           # #      # #",
        "# # ######### # ### # ######### # # ###### #",
        "#           # #     #     #     # #        #",
        "########### # ########### # ##### ######## #",
        "#           #       #     #       #        #",
        "# ################# # ########### # ###### #",
        "#       #           #             #     K# #",
        "# ##### # ####################### ######## #",
        "#E      #                                  #",
        "############################################"
    },
    {
        "############################################",
        "#           #                 #            #",
        "######### # # ############### # ########## #",
        "#         # #       #         #          # #",
        "# ######### ####### # ############### ## # #",
        "# #               # #               #    # #",
        "# # ############# # ############### ###### #",
        "# #             # #       #              # #",
        "# ############# # ##### # ############## # #",
        "#               #     # #       #        # #",
        "# ################### # ##### # # ######## #",
        "# #                 # #     # # #       K# #",
        "# # ############### # ##### # # ######## #",
        "#E                  #       #            ###",
        "############################################"
    }
};

// [ARRAY 1D] Menyimpan daftar petunjuk di panel bawah layar
const char *game_tips[MAX_TIPS] = {
    "[WASD / Panah] Gerak   |   [R] Ulang Level   |   [Q] Keluar Menu",
    "Ambil kunci 'K' (Hijau) lalu menuju pintu 'E' (Hijau) sebelum langkah habis!",
    "Hindari musuh 'X' (Merah)! Menyentuh atau berpapasan dengan 'X' mengurangi 1 nyawa."
};

// [ARRAY 2D] Posisi awal & arah gerak musuh per level
int enemy_y[NUM_LEVELS][NUM_ENEMIES] = {
    {7, 9},
    {5, 9}
};
int enemy_x[NUM_LEVELS][NUM_ENEMIES] = {
    {15, 28},
    {12, 24}
};
int enemy_dx[NUM_LEVELS][NUM_ENEMIES] = {
    {1, -1},
    {1, -1}
};

// [FUNCTION] Mengecek apakah koordinat berada di dalam batas array peta
int is_within_bounds(int y, int x) {
    return (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH);
}

// [FUNCTION] Inisialisasi ncurses & warna
void init_ncurses_colors(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors() == FALSE) {
        endwin();
        printf("Terminal tidak mendukung warna.\n");
        exit(1);
    }

    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // Pemain '@'
    init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Dinding '#'
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   // Kunci 'K' & Pintu 'E'
    init_pair(4, COLOR_RED, COLOR_BLACK);     // Musuh 'X' & Peringatan
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Judul / Dekorasi
}

// [FUNCTION] Menyalin peta level aktif ke array kerja agar bisa di-reset
void load_level(int level_idx, char current_map[HEIGHT][WIDTH + 1],
                int *py, int *px, int *has_key, int *moves_left,
                int ey[NUM_ENEMIES], int ex[NUM_ENEMIES], int edx[NUM_ENEMIES]) {
    for (int y = 0; y < HEIGHT; y++) {
        strcpy(current_map[y], levels[level_idx][y]);
    }

    *py = 1;
    *px = 1;
    *has_key = 0;
    *moves_left = (level_idx == 0) ? 140 : 180;

    for (int i = 0; i < NUM_ENEMIES; i++) {
        ey[i] = enemy_y[level_idx][i];
        ex[i] = enemy_x[level_idx][i];
        edx[i] = enemy_dx[level_idx][i];
    }
}

// [FUNCTION] Menampilkan layar panduan singkat
void show_instructions(void) {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(2, 6, "=== CARA BERMAIN FIND THE KEY IN MAZE ===");
    attroff(COLOR_PAIR(5));

    mvprintw(5, 6, "1. Kendalikan karakter '@' (Kuning) memakai WASD atau tombol panah.");
    mvprintw(6, 6, "2. Cari dan ambil kunci 'K' (Hijau) terlebih dahulu.");
    mvprintw(7, 6, "3. Setelah punya kunci, masuk ke pintu keluar 'E' (Hijau).");
    mvprintw(8, 6, "4. Hindari patroli musuh 'X' (Merah) dan perhatikan sisa langkah.");
    mvprintw(10, 6, "Tekan tombol apa saja untuk kembali ke menu...");
    refresh();
    getch();
}

// [FUNCTION] Menampilkan menu utama dan mengembalikan pilihan pengguna
int show_main_menu(void) {
    int choice;

    while (1) {
        clear();
        attron(COLOR_PAIR(5));
        mvprintw(2, 8, "========================================");
        mvprintw(3, 8, "     FIND THE KEY IN MAZE (NCURSES)     ");
        mvprintw(4, 8, "========================================");
        attroff(COLOR_PAIR(5));

        mvprintw(7, 12, "[1] Mulai Permainan");
        mvprintw(8, 12, "[2] Cara Bermain");
        mvprintw(9, 12, "[3] Keluar");
        mvprintw(12, 12, "Pilih menu (1-3): ");
        refresh();

        choice = getch();

        if (choice == '1' || choice == '2' || choice == '3') {
            return choice - '0';
        }
    }
}

// [FUNCTION] Menggambar arena game, musuh, pemain, dan panel informasi
void render_screen(char current_map[HEIGHT][WIDTH + 1], int level_idx,
                   int py, int px, int has_key, int score, int lives,
                   int moves_left, int ey[NUM_ENEMIES], int ex[NUM_ENEMIES],
                   const char *status_msg) {
    clear();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char tile = current_map[y][x];
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

    attron(COLOR_PAIR(4));
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (is_within_bounds(ey[i], ex[i])) {
            mvprintw(ey[i], ex[i], "X");
        }
    }
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(1));
    if (is_within_bounds(py, px)) {
        mvprintw(py, px, "@");
    }
    attroff(COLOR_PAIR(1));

    mvprintw(HEIGHT + 1, 0, "Level: %d/%d | Skor: %d | Nyawa: %d | Sisa Langkah: %3d",
             level_idx + 1, NUM_LEVELS, score, lives, moves_left);
    mvprintw(HEIGHT + 2, 0, "Kunci: %-28s",
             has_key ? "ADA (Menuju pintu 'E')" : "BELUM ADA (Cari 'K')");

    for (int i = 0; i < MAX_TIPS; i++) {
        mvprintw(HEIGHT + 3 + i, 0, "%s", game_tips[i]);
    }

    if (status_msg != NULL) {
        attron(COLOR_PAIR(4));
        mvprintw(HEIGHT + 7, 0, "Info: %-68s", status_msg);
        attroff(COLOR_PAIR(4));
    }

    refresh();
}

// [FUNCTION] Menggerakkan musuh secara bolak-balik di jalur kosong
void update_enemies(char current_map[HEIGHT][WIDTH + 1],
                    int ey[NUM_ENEMIES], int ex[NUM_ENEMIES], int edx[NUM_ENEMIES]) {
    for (int i = 0; i < NUM_ENEMIES; i++) {
        int next_x = ex[i] + edx[i];

        if (!is_within_bounds(ey[i], next_x) || current_map[ey[i]][next_x] != ' ') {
            edx[i] = -edx[i];
            next_x = ex[i] + edx[i];
        }

        if (is_within_bounds(ey[i], next_x) && current_map[ey[i]][next_x] == ' ') {
            ex[i] = next_x;
        }
    }
}

// [FUNCTION] Mengecek tabrakan langsung atau berpapasan (swap) dengan musuh
int hit_enemy(int prev_py, int prev_px, int py, int px,
              int prev_ey[NUM_ENEMIES], int prev_ex[NUM_ENEMIES],
              int ey[NUM_ENEMIES], int ex[NUM_ENEMIES]) {
    for (int i = 0; i < NUM_ENEMIES; i++) {
        int same_tile = (py == ey[i] && px == ex[i]);
        int swapped_tiles = (py == prev_ey[i] && px == prev_ex[i] &&
                             ey[i] == prev_py && ex[i] == prev_px);
        if (same_tile || swapped_tiles) {
            return 1;
        }
    }
    return 0;
}

// [FUNCTION] Menampilkan layar akhir (Menang atau Kalah)
void show_end_screen(int won, int final_score) {
    clear();
    if (won) {
        attron(COLOR_PAIR(3));
        mvprintw(5, 8, "SELAMAT! ANDA MENYELESAIKAN SEMUA LEVEL!");
        attroff(COLOR_PAIR(3));
    } else {
        attron(COLOR_PAIR(4));
        mvprintw(5, 8, "GAME OVER! ANDA KEHABISAN NYAWA / LANGKAH.");
        attroff(COLOR_PAIR(4));
    }
    attron(COLOR_PAIR(1));
    mvprintw(7, 8, "Skor Akhir Anda: %d", final_score);
    attroff(COLOR_PAIR(1));
    mvprintw(10, 8, "Tekan tombol apa saja untuk kembali ke menu utama...");
    refresh();
    getch();
}

int main(void) {
    init_ncurses_colors();

    while (1) {
        int menu_choice = show_main_menu();

        if (menu_choice == 2) {
            show_instructions();
            continue;
        }
        if (menu_choice == 3) {
            break;
        }

        char current_map[HEIGHT][WIDTH + 1];
        int ey[NUM_ENEMIES], ex[NUM_ENEMIES], edx[NUM_ENEMIES];
        int level_idx = 0;
        int py, px, has_key, moves_left;
        int score = 0;
        int lives = 3;
        int game_over = 0;
        int victory = 0;
        const char *status_msg = "Selamat datang di Level 1!";

        load_level(level_idx, current_map, &py, &px, &has_key, &moves_left, ey, ex, edx);

        while (!game_over && !victory) {
            render_screen(current_map, level_idx, py, px, has_key, score,
                          lives, moves_left, ey, ex, status_msg);

            int ch = getch();

            if (ch == 'q' || ch == 'Q') {
                game_over = 1;
                break;
            }
            if (ch == 'r' || ch == 'R') {
                load_level(level_idx, current_map, &py, &px, &has_key, &moves_left, ey, ex, edx);
                status_msg = "Level diulang dari posisi awal (nyawa tetap).";
                continue;
            }

            int ny = py, nx = px;
            int moved_input = 1;

            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'W':
                    ny--;
                    break;
                case KEY_DOWN:
                case 's':
                case 'S':
                    ny++;
                    break;
                case KEY_LEFT:
                case 'a':
                case 'A':
                    nx--;
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'D':
                    nx++;
                    break;
                default:
                    moved_input = 0;
                    break;
            }

            if (!moved_input) {
                status_msg = "Tombol tidak dikenali. Gunakan WASD / Panah / R / Q.";
                continue;
            }

            if (!is_within_bounds(ny, nx)) {
                status_msg = "Batas peta tercapai!";
                continue;
            }

            int prev_py = py;
            int prev_px = px;
            int prev_ey[NUM_ENEMIES];
            int prev_ex[NUM_ENEMIES];
            for (int i = 0; i < NUM_ENEMIES; i++) {
                prev_ey[i] = ey[i];
                prev_ex[i] = ex[i];
            }

            char target = current_map[ny][nx];
            if (target == '#') {
                status_msg = "Terhalang dinding '#'!";
            } else {
                py = ny;
                px = nx;
                moves_left--;
                status_msg = "Bergerak ke jalur kosong.";

                if (target == 'K') {
                    has_key = 1;
                    current_map[ny][nx] = ' ';
                    score += 100;
                    status_msg = "Kunci 'K' diambil! (+100 skor) Cari pintu 'E'.";
                } else if (target == 'E') {
                    if (has_key) {
                        score += moves_left * 5 + 200;
                        level_idx++;

                        if (level_idx < NUM_LEVELS) {
                            load_level(level_idx, current_map, &py, &px, &has_key,
                                       &moves_left, ey, ex, edx);
                            status_msg = "Naik ke Level berikutnya! (+200 + bonus langkah)";
                        } else {
                            victory = 1;
                        }
                    } else {
                        status_msg = "Pintu 'E' terkunci! Ambil kunci 'K' dulu.";
                    }
                }
            }

            if (!victory && !game_over) {
                update_enemies(current_map, ey, ex, edx);

                if (hit_enemy(prev_py, prev_px, py, px, prev_ey, prev_ex, ey, ex)) {
                    lives--;
                    status_msg = "Tertangkap atau berpapasan dengan musuh 'X'! Nyawa -1.";
                    py = 1;
                    px = 1;
                    if (lives <= 0) {
                        game_over = 1;
                    }
                }
            }

            if (!victory && moves_left <= 0) {
                game_over = 1;
            }
        }

        show_end_screen(victory, score);
    }

    endwin();
    return 0;
}