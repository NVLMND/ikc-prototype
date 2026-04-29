#include<stdlib.h>
#include<string.h>
#include"unitlist.h"

void ulist_init(UnitList *ul){
    ul->items=NULL;
    ul->count=0;
    ul->cap=0;
}

void ulist_add(UnitList *ul, const char *line){
    if(ul->count==ul->cap){
        ul->cap=ul->cap ? ul->cap * 2:8;
        ul->items=realloc(ul->items, ul->cap*sizeof(char*));
    }
    ul->items[ul->count++]=strdup(line);
}

void ulist_replace(UnitList *ul, int idx, const char *line){
    if(idx<0||idx>=ul->count) return;
    free(ul->items[idx]);
    ul->items[idx]=strdup(line);
}

void ulist_free(UnitList *ul){
    for(int i=0; i<ul->count; i++){
        free(ul->items[i]);
    }
    free(ul->items);
}
