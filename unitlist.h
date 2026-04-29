#ifndef UNITLIST_H
#define UNITLIST_H

typedef struct{
    char **items;
    int count;
    int cap;
}UnitList;

void ulist_init(UnitList *ul);
void ulist_add(UnitList *ul, const char *line);
void ulist_replace(UnitList *ul, int idx, const char *line);
void ulist_free(UnitList *ul);
#endif
