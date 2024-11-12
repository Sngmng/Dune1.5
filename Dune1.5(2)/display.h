#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 표시할 색상 정의
#define COLOR_DEFAULT 15
#define COLOR_CURSOR 112
#define COLOR_RESOURCE 112

void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    OBJECT_SAMPLE obj,
    const char* system_message
);

#endif
