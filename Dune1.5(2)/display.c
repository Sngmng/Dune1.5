#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION system_msg_pos = { MAP_HEIGHT + 2, 0 };
const POSITION unit_status_pos = { 1, MAP_WIDTH + 5 };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_unit_status(OBJECT_SAMPLE obj);
void display_system_message(const char* message);

void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    OBJECT_SAMPLE obj,
    const char* system_message
) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
    display_unit_status(obj);
    display_system_message(system_message);
}

void display_resource(RESOURCE resource) {
    set_color(COLOR_RESOURCE);
    gotoxy(resource_pos);
    printf("spice = %d/%d, population=%d/%d\n",
        resource.spice, resource.spice_max,
        resource.population, resource.population_max
    );
}

// 자원 및 지도를 화면에 표시
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            for (int k = 0; k < N_LAYER; k++) {
                if (src[k][i][j] >= 0) {
                    dest[i][j] = src[k][i][j];
                }
            }
        }
    }
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (frontbuf[i][j] != backbuf[i][j]) {
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
            }
            frontbuf[i][j] = backbuf[i][j];
        }
    }
}

// 커서 위치 표시
void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    char ch = frontbuf[prev.row][prev.column];
    printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

    ch = frontbuf[curr.row][curr.column];
    printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

// 유닛 상태 정보 표시
void display_unit_status(OBJECT_SAMPLE obj) {
    set_color(COLOR_DEFAULT);
    gotoxy(unit_status_pos);
    printf("유닛 상태:\n");
    printf(" 위치: (%d, %d)\n", obj.pos.row, obj.pos.column);
    printf(" 목표 위치: (%d, %d)\n", obj.dest.row, obj.dest.column);
    printf(" 체력: %d\n", 100);  // 예시 값, 실제 체력 변수가 있으면 사용
    printf(" 공격력: %d\n", 20);  // 예시 값, 실제 공격력 변수가 있으면 사용
    printf(" 다음 이동 시점: %d ms\n", obj.next_move_time);
}

// 시스템 메시지 출력
void display_system_message(const char* message) {
    set_color(COLOR_DEFAULT);
    gotoxy(system_msg_pos);
    printf("시스템 메시지: %s", message);
}
