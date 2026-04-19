#ifndef DYN_BUF_H
#define DYN_BUF_H
#include <stddef.h>

typedef struct{
    char *data;
    size_t len;
    size_t cap;
}DynBuf;

void dbuf_init(DynBuf *b);
void dbuf_free(DynBuf *b);
void dbuf_reset(DynBuf *b);
void dbuf_append(DynBuf *b, const char *s);
void dbuf_append_line(DynBuf *b, const char *line);
void dbuf_copy(DynBuf *dest, const DynBuf *src);
#endif
