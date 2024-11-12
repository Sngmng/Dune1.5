#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

/* ================= system parameters =================== */
#define TICK 10
#define N_LAYER 2
#define MAP_WIDTH 60
#define MAP_HEIGHT 18
#define COLOR_DEFAULT 15
#define COLOR_CURSOR 112
#define COLOR_RESOURCE 14
#define COLOR_UNIT 13
#define COLOR_BUILDING 9

/* ================= 위치와 방향 =================== */
typedef struct { int row, column; } POSITION;

typedef struct { POSITION previous; POSITION current; } CURSOR;

typedef enum { k_none = 0, k_up, k_right, k_left, k_down, k_quit, k_place_unit, k_place_building, k_undef } KEY;

typedef enum { d_stay = 0, d_up, d_right, d_left, d_down } DIRECTION;

POSITION padd(POSITION p1, POSITION p2) { return (POSITION) { p1.row + p2.row, p1.column + p2.column }; }
POSITION psub(POSITION p1, POSITION p2) { return (POSITION) { p1.row - p2.row, p1.column - p2.column }; }

#define is_arrow_key(k)    (k_up <= (k) && (k) <= k_down)
#define ktod(k)            (DIRECTION)(k)
POSITION dtop(DIRECTION d) { static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} }; return direction_vector[d]; }
#define pmove(p, d)        (padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct { int spice; int spice_max; int population; int population_max; } RESOURCE;

typedef struct { POSITION pos; POSITION dest; char repr; int move_period; int next_move_time; } OBJECT_SAMPLE;

int sys_clock = 0;
CURSOR cursor = { {1, 1}, {1, 1} };
RESOURCE resource = { 100, 200, 10, 50 };
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };
OBJECT_SAMPLE obj = { {1, 1}, {MAP_HEIGHT - 2, MAP_WIDTH - 2}, 'o', 300, 300 };

/* ================= I/O Functions =================== */
void gotoxy(POSITION pos) {
    COORD coord = { pos.column, pos.row };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printc(POSITION pos, char ch, int color) {
    set_color(color);
    gotoxy(pos);
    printf("%c", ch);
    set_color(COLOR_DEFAULT);
}

KEY get_key(void) {
    if (!_kbhit()) return k_none;
    int ch = _getch();
    switch (ch) {
    case 'q': return k_quit;
    case 'p': return k_place_unit;
    case 'b': return k_place_building;
    case 224:
        ch = _getch();
        if (ch == 72) return k_up;
        if (ch == 80) return k_down;
        if (ch == 75) return k_left;
        if (ch == 77) return k_right;
    }
    return k_undef;
}

/* ================= Display Functions =================== */
void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy((POSITION) { 0, 0 });
    printf("spice = %d/%d, population = %d/%d\n", resource.spice, resource.spice_max, resource.population, resource.population_max);
}

void display_map(void) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char ch = map[0][i][j] ? map[0][i][j] : map[1][i][j];
            int color = (ch == 'R') ? COLOR_RESOURCE : (ch == 'P') ? COLOR_UNIT : (ch == 'H') ? COLOR_BUILDING : COLOR_DEFAULT;
            POSITION pos = { i + 1, j };
            printc(pos, ch ? ch : ' ', color);
        }
    }
}

void display_cursor(void) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;
    printc(prev, map[0][prev.row][prev.column] ? map[0][prev.row][prev.column] : ' ', COLOR_DEFAULT);
    printc(curr, 'C', COLOR_CURSOR);
}

void display(const char* system_message) {
    display_resource(resource);
    display_map();
    display_cursor();
    set_color(COLOR_DEFAULT);
    gotoxy((POSITION) { MAP_HEIGHT + 2, 0 });
    printf("시스템 메시지: %s", system_message);
}

/* ================= Game Control Functions =================== */
void init(void) {
    for (int j = 0; j < MAP_WIDTH; j++) {
        map[0][0][j] = '#';
        map[0][MAP_HEIGHT - 1][j] = '#';
    }
    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        map[0][i][0] = '#';
        map[0][i][MAP_WIDTH - 1] = '#';
        for (int j = 1; j < MAP_WIDTH - 1; j++) map[0][i][j] = ' ';
    }
    map[1][obj.pos.row][obj.pos.column] = 'o';
}

void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);
    if (1 <= new_pos.row && new_pos.row < MAP_HEIGHT - 1 && 1 <= new_pos.column && new_pos.column < MAP_WIDTH - 1) {
        cursor.previous = cursor.current;
        cursor.current = new_pos;
    }
}

void place_unit(void) {
    // 유닛을 커서 위치에 생성, 커서가 있는 곳에만 생성
    if (map[1][cursor.current.row][cursor.current.column] == ' ') {
        map[1][cursor.current.row][cursor.current.column] = 'P';
        display("유닛 생성");
    }
}

void place_building(void) {
    // 빌딩을 커서 위치에 생성, 커서가 있는 곳에만 생성
    if (map[1][cursor.current.row][cursor.current.column] == ' ') {
        map[1][cursor.current.row][cursor.current.column] = 'H';
        display("빌딩 생성");
    }
}

void sample_obj_move(void) {
    if (sys_clock < obj.next_move_time) return;
    map[1][obj.pos.row][obj.pos.column] = ' ';
    obj.pos = (obj.pos.row == obj.dest.row && obj.pos.column == obj.dest.column) ? (POSITION) { 1, 1 } : (POSITION) { obj.pos.row + 1, obj.pos.column + 1 };
    map[1][obj.pos.row][obj.pos.column] = obj.repr;
    obj.next_move_time = sys_clock + obj.move_period;
}

/* ================= main() =================== */
int main(void) {
    srand((unsigned int)time(NULL));
    init();
    const char* system_message = "게임 시작";

    while (1) {
        KEY key = get_key();
        if (is_arrow_key(key)) {
            cursor_move(ktod(key));
        }
        else if (key == k_place_unit) {
            place_unit();
            system_message = "유닛 생성";
        }
        else if (key == k_place_building) {
            place_building();
            system_message = "빌딩 생성";
        }
        else if (key == k_quit) {
            printf("게임 종료\n");
            break;
        }
        sample_obj_move();
        display(system_message);
        Sleep(TICK);
        sys_clock += TICK;
    }
}