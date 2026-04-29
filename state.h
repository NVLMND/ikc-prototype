#ifndef STATE_H
#define STATE_H
#include"dyn_buf.h"
#include"history.h"
#include"unitlist.h"

typedef struct {
    DynBuf last_globals;
    DynBuf last_main_body;
    UnitList globals;
    UnitList main_body;
    History history;
}BufferState;

void init_state(BufferState *s);
void free_state(BufferState *s);

#endif
