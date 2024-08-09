#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

uint8_t color = 0;

uint8_t wheel1_idx = 0;
uint8_t wheel2_idx = 0;
uint8_t wheel3_idx = 0;

uint32_t bet = 1;

uint32_t credits = 100;

static enum {CHERRY, BELL, LEMON, GRAPES, BAR, SEVEN} symbols;

static char* cherry = "        \n"
                      "  /\\    \n"
                      " /  \\   \n"
                      " | (  ) \n"
                      "(  )    \n"
                      "        \n";

static char* bell = "        \n"
                    "    _   \n"
                    "   / \\  \n"
                    "  /   \\ \n"
                    " (_____)\n"
                    "        \n";

static char* lemon = "        \n"
                     "  ____  \n"
                     " /    \\ \n"
                     " \\____/ \n"
                     "        \n"
                     "        \n";

static char* grapes = "        \n"
                      "   _    \n"
                      " ()()() \n"
                      "  ()()  \n"
                      "   ()   \n"
                      "        \n";

static char* bar = "        \n"
                   "  ___   \n"
                   " |BAR|  \n"
                   " |___|  \n"
                   "        \n"
                   "        \n";

static char* seven = "        \n"
                     " _____  \n"
                     " |_  /  \n"
                     "  / /   \n"
                     " /_/    \n"
                     "        \n";

static uint8_t wheel1[15] = {CHERRY, BELL, LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON};
static uint8_t wheel2[15] = {BELL, LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON, GRAPES};
static uint8_t wheel3[15] = {LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON, GRAPES, BAR, SEVEN, CHERRY, BELL, LEMON, GRAPES, BAR};

static void draw_symbol(WINDOW* win, int y, int x, uint8_t symbol) {
    char* s;
    switch (symbol) {
        case CHERRY:
            if(color){
                wattron(win, COLOR_PAIR(1));
            }
            s = cherry;
            break;
        case BELL:
            if(color){
                wattron(win, COLOR_PAIR(2));
            }
            s = bell;
            break;
        case LEMON:
            if(color){
                wattron(win, COLOR_PAIR(2));
            }
            s = lemon;
            break;
        case GRAPES:
            if(color){
                wattron(win, COLOR_PAIR(3));
            }
            s = grapes;
            break;
        case BAR:
            if(color){
                wattron(win, COLOR_PAIR(4));
            }
            s = bar;
            break;
        case SEVEN:
            if(color){
                wattron(win, COLOR_PAIR(1));
            }
            s = seven;
            break;
    }
    // Calculate the width and height of the symbol
    int symbol_width = 8; // All symbols are 8 characters wide
    int symbol_height = 6; // All symbols are 6 lines tall

    // Calculate the starting position to center the symbol
    int start_y = (8 - symbol_height) / 2;
    int start_x = (12 - symbol_width) / 2;

    // Print the symbol at the calculated position
    // Manually print each line of the symbol string
    for (int i = 0; i < symbol_height; i++) {
        mvwprintw(win, y + start_y + i, x + start_x, "%.*s", symbol_width, s + i * (symbol_width + 1));
    }


    if(color){
        wattroff(win, COLOR_PAIR(1));
        wattroff(win, COLOR_PAIR(2));
        wattroff(win, COLOR_PAIR(3));
        wattroff(win, COLOR_PAIR(4));
    }

    wrefresh(win);
}


static void draw_wheel(WINDOW* w, int y, int x, uint8_t* wheel, uint8_t idx) {
    draw_symbol(w, y, x, wheel[idx]);
    box(w, 0, 0);
    wrefresh(w);
}


/*
PAYOUT


2 of the same symbol: 2x bet
2 of the same symbol and 1 seven: 5x bet
3 of the same symbol: 10x bet
3 BARs: 20x bet
3 SEVENs: 50x bet

*/
int calculate_payout(uint8_t* wheel1, uint8_t* wheel2, uint8_t* wheel3, uint8_t idx1, uint8_t idx2, uint8_t idx3) {
    uint8_t symbol1 = wheel1[idx1];
    uint8_t symbol2 = wheel2[idx2];
    uint8_t symbol3 = wheel3[idx3];

    if(symbol1 == symbol2 && symbol2 == symbol3){
        switch(symbol1){
            case CHERRY:
                return 10;
                break;
            case BELL:
                return 10;
                break;
            case LEMON:
                return 10;
                break;
            case GRAPES:
                return 10;
                break;
            case BAR:
                return 20;
                break;
            case SEVEN:
                return 50;
                break;
        }
    } else if(symbol1 == symbol2 || symbol2 == symbol3 || symbol1 == symbol3){
        if(symbol1 == SEVEN || symbol2 == SEVEN || symbol3 == SEVEN){
            return 5;
        } else {
            return 2;
        }
    } else {
        return 0;
    }
}

void spin_wheel(WINDOW* w, int y, int x, uint8_t* wheel, uint8_t* idx) {
    for(int i = 0; i < rand() % 15 + 15; i++){
        draw_wheel(w, y, x, wheel, *idx);
        *idx = (*idx + 1) % 15;
        napms(100);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();

    //hide cursor
    curs_set(0);

    //init color
    if(has_colors()){
        color = 1;
        start_color();
    }

    //init color pairs
    if(color){
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
    }

    printw("2 of the same symbol: 2x bet \n\
    2 of the same symbol and 1 seven: 5x bet\n\
    3 of the same symbol: 10x bet\n\
    3 BARs: 20x bet\n\
    3 SEVENs: 50x bet");

    refresh();

    //init random
    srand(time(NULL));

    // Get screen dimensions
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if(rows < 12 || cols < 64){
        endwin();
        printf("Terminal too small\n");
        return 1;
    }

    // Calculate center coordinates
    int center_y = rows / 2;
    int center_x = cols / 2;

    int window_y = center_y - 4;

    int w1_x = center_x - 25;
    int w2_x = center_x - 5;
    int w3_x = center_x + 15;


    //init windows
    WINDOW* w1 = newwin(8, 12, window_y, w1_x);
    WINDOW* w2 = newwin(8, 12, window_y, w2_x);
    WINDOW* w3 = newwin(8, 12, window_y, w3_x);

    //draw border
    box(w1, 0, 0);
    box(w2, 0, 0);
    box(w3, 0, 0);

    wrefresh(w1);
    wrefresh(w2);
    wrefresh(w3);

    wheel1_idx = rand() % 15;
    wheel2_idx = rand() % 15;
    wheel3_idx = rand() % 15;

    //draw wheels
    draw_wheel(w1, 0, 0, wheel1, wheel1_idx);
    draw_wheel(w2, 0, 0, wheel2, wheel2_idx);
    draw_wheel(w3, 0, 0, wheel3, wheel3_idx);
    
    mvprintw(center_y + 4, center_x - 31, "Press any key to spin the wheels, +/- to change bet, q to quit");
    mvprintw(center_y + 6, center_x - 16, "Credits: %d", credits);
    mvprintw(center_y + 7, center_x - 16, "Bet: %d", bet);

    // Spin the wheels
    char ch;
    while((ch = getch()) != 'q'){
        if(ch == '+'){
            bet++;
            mvprintw(center_y + 7, center_x - 16, "Bet: %d", bet);
            refresh();
            continue;
        } else if(ch == '-' && bet > 1){
            bet--;
            mvprintw(center_y + 7, center_x - 16, "Bet: %d", bet);
            refresh();
            continue;
        }
        spin_wheel(w1, 0, 0, wheel1, &wheel1_idx);
        spin_wheel(w2, 0, 0, wheel2, &wheel2_idx);
        spin_wheel(w3, 0, 0, wheel3, &wheel3_idx);
        int payout_mul = calculate_payout(wheel1, wheel2, wheel3, wheel1_idx, wheel2_idx, wheel3_idx);
        credits -= bet;
        credits += bet * payout_mul;
        if(credits <= 0){
            mvprintw(center_y, center_x, "Game Over!");
        }
        mvprintw(center_y + 5, center_x - 16, "Payout: %d", bet * payout_mul);
        mvprintw(center_y + 6, center_x - 16, "Credits: %d", credits);
        refresh();
    }

    delwin(w1);
    delwin(w2);
    delwin(w3);
    endwin();
    
    return 0;
}