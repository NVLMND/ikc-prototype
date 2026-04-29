#include<string.h>
#include"history.h"

void history_init(History *h){
    h->count=0;
    h->cursor=-1;
    h->selected_index=-1;
    h->modified=0;
}

void history_add(History *h, const char *line, int buffer_index){
    if(h->count>=MAX_HISTORY) return;
    strcpy(h->entries[h->count].line, line);
    h->entries[h->count].buffer_index=buffer_index;
    h->count++;
}

const char *history_up(History *h){
    if(h->count==0) return NULL;
    if(h->cursor>0) h->cursor--;
    h->selected_index=h->cursor;
    return h->entries[h->cursor].line;
}

const char *history_down(History *h){
    if(h->count==0) return NULL;
    if(h->cursor<h->count-1){
        h->cursor++;
        h->selected_index=h->cursor;
        return h->entries[h->cursor].line;
    }
    h->cursor=h->count;
    h->selected_index=-1;
    return NULL;
}

int history_get_selected(History *h){
    return h->selected_index;
}

int history_get_buffer_index(History *h, int hist_index){
    if(hist_index<0||hist_index>=h->count) return -1;
    return h->entries[hist_index].buffer_index;
}

void history_reset_navigation(History *h){
    h->cursor=h->count;
    h->selected_index=-1;
    h->modified=0;
}
