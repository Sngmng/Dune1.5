#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);

int sys_clock = 0;
CURSOR cursor = { {1, 1}, {1, 1} };
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
RESOURCE resource = { 0, 0, 0, 0 };
OBJECT_SAMPLE obj = { {1, 1}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 'o', 300, 300 };

int main(void) {
    srand((unsigned int)time(NULL));
    init();
    intro();
    const char* system_message = "게임 시작";

    while (1) {
        KEY key = get_key();
        if (is_arrow_key(key)) {
            cursor_move(ktod(key));
        }
        else if (key == k_quit) {
            outro();
        }

        sample_obj_move();
        display(resource, map, cursor, obj, system_message);

        Sleep(TICK);
        sys_clock += TICK;
    }
}

void init(void) {
    for (int j = 0; j < MAP_WIDTH; j++) {
        map[0][0][j] = '#';
        map[0][MAP_HEIGHT - 1][j] = '#';
    }
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        map[0][i][0] = '#';
        map[0][i][MAP_WIDTH - 1] = '#';
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';
        }
    }
    map[1][obj.pos.row][obj.pos.column] = 'o';
}

void intro(void) {
    printf("DUNE 1.5\n");
    Sleep(2000);
    system("cls");
}

void outro(void) {
    printf("exiting...\n");
    exit(0);
}

void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);
    if (1 <= new_pos.row && new_pos.row < MAP_HEIGHT - 1 &&
        1 <= new_pos.column && new_pos.column < MAP_WIDTH - 1) {
        cursor.previous = cursor.current;
        cursor.current = new_pos;
    }
}

void sample_obj_move(void) {
    if (sys_clock < obj.next_move_time) return;
    map[1][obj.pos.row][obj.pos.column] = -1;
    obj.pos = sample_obj_next_position();
    map[1][obj.pos.row][obj.pos.column] = obj.repr;
    obj.next_move_time = sys_clock + obj.move_period;
}
