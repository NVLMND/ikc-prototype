#include "state.h"
void init_state(BufferState *s){
    ulist_init(&s->globals);
    ulist_init(&s->main_body);
    dbuf_init(&s->last_globals);
    dbuf_init(&s->last_main_body);
    history_init(&s->history);
}

void free_state(BufferState *s){
    ulist_free(&s->globals);
    ulist_free(&s->main_body);
    dbuf_free(&s->last_globals);
    dbuf_free(&s->last_main_body);
}
