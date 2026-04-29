#ifndef HISTORY_H
#define HISTORY_H
#include"common.h"
typedef struct{
    char line[LINE_MAX];
    int buffer_index;
}HistoryEntry;

typedef struct{
    HistoryEntry entries[MAX_HISTORY];
    int count;
    int cursor;
    int selected_index;
    int modified;
}History;

void history_init(History *h);
void history_add(History *h, const char *line, int buffer_index);
const char *history_up(History *h);
const char *history_down(History *h);
int history_get_selected(History *h);
int history_get_buffer_index(History *h, int hist_index);
void history_reset_navigation(History *h);
#endif
