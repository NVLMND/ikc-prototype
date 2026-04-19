#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dyn_buf.h"

static void dbuf_grow(DynBuf *b, size_t n){
    if(b->cap>=n) return;
    if(b->cap==0)
        b->cap=128;

    while(b->cap<n)
        b->cap*=2;
    char *new_data=realloc(b->data, b->cap);
    if(!new_data){
        perror("realloc");
        exit(1);
    }
    b->data=new_data;
}

void dbuf_init(DynBuf *b){
    b->cap=4096;
    b->len=0;
    b->data=malloc(b->cap);
    if (!b->data) {
        perror("malloc");
        exit(1);
    }
    b->data[0]='\0';
}

void dbuf_free(DynBuf *b){
    free(b->data);
    b->data=NULL;
    b->len=0;
    b->cap=0;
}

void dbuf_append(DynBuf *b, const char *s){
    size_t n=strlen(s);
    dbuf_grow(b, b->len+n+1);
    memcpy(b->data+b->len, s, n);
    b->len+=n;
    b->data[b->len]='\0';
}

void dbuf_append_line(DynBuf *b, const char *line){
    dbuf_append(b, line);
    if(b->len==0||b->data[b->len-1]!='\n')
        dbuf_append(b, "\n");
}

void dbuf_reset(DynBuf *b){
    b->len=0;
    if(b->data)
        b->data[0]='\0';
}

void dbuf_copy(DynBuf *dest, const DynBuf *src){
    dbuf_reset(dest);
    if(!src->data||src->len==0) return;
    if(dest==src) return;

    dbuf_grow(dest, src->len+1);
    memcpy(dest->data, src->data, src->len);
    dest->len=src->len;
    dest->data[dest->len]='\0';
}
