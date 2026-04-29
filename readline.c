#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "readline.h"
#include "colors.h"
#include"state.h"
#include"history.h"
#define CTRL(X) ((X) & 0X1f)

static void redraw_from_cursor(const char *prompt, const char *buf, int len, int cursor)
{
    char seq[64];

    write(STDOUT_FILENO, "\r\033[K", 4);
    write(STDOUT_FILENO, prompt, strlen(prompt));
    write(STDOUT_FILENO, buf, len);

    int pos = len - cursor;
    if (pos > 0) {
        snprintf(seq, sizeof(seq), "\033[%dD", pos);
        write(STDOUT_FILENO, seq, strlen(seq));
    }
}

int ikc_readline(BufferState *state, char *out, size_t maxlen, const char *prompt)
{

    char buf[4096]={0};
    int len = 0;
    int cursor = 0;
    int history_active=0;
    char original[4096]={0};
    if (maxlen > sizeof(buf))
        maxlen = sizeof(buf);
    redraw_from_cursor(prompt, buf, len, cursor);
    while(1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1)
            return -1;

        /* ENTER */
        if (c == '\n' || c == '\r') {
            buf[len] = '\0';
            write(STDOUT_FILENO, "\n", 1);
            snprintf(out, maxlen, "%s", buf);
            return len;
        }

        /* TAB = 4 spaces */
        if (c == '\t') {
            for (int i = 0; i < 4; i++) {
                if (len + 1 >= (int)maxlen)
                    break;
                memmove(buf + cursor + 1, buf + cursor, len - cursor);
                buf[cursor] = ' ';
                cursor++;
                len++;
                history_active=0;
            }
            redraw_from_cursor(prompt, buf, len, cursor);
            continue;
        }

        /* ESC */
        if (c == 27) {
            char next;
            ssize_t n = read(STDIN_FILENO, &next, 1);
            if (n == 0) {
                /* Esc alone */
                return -1;
            }
            if (next != '[') {
                return -1;
            }
            char final;
            if (read(STDIN_FILENO, &final, 1) != 1) {
                return -1;
            }
            switch (final) {
                case 'A':{
                    if(!history_active){
                        strcpy(original, buf);
                        history_active=1;
                    }
                    const char *h=history_up(&state->history);
                    if(h){
                        snprintf(buf, sizeof(buf), "%s", h);
                        len=cursor=strlen(buf);
                        redraw_from_cursor(prompt, buf, len, cursor);
                    }
                    break;
                }
                case 'B':{
                    const char *h=history_down(&state->history);
                    if(h && *h){
                        snprintf(buf, sizeof(buf), "%s", h);
                        len=cursor=strlen(buf);
                    }else{
                        snprintf(buf, sizeof(buf), "%s", original);
                        len=cursor=strlen(buf);
                        history_active=0;
                    }
                    redraw_from_cursor(prompt, buf, len, cursor);
                    break;
                }
                case 'C': /* right */
                    if (cursor < len) {
                        write(STDOUT_FILENO, "\033[C", 3);
                        cursor++;
                    }
                    break;
                case 'D': /* left */
                    if (cursor > 0) {
                        write(STDOUT_FILENO, "\033[D", 3);
                        cursor--;
                    }
                    break;
                default:
                    break;
            }
            continue;
        }
        /* backspace */
        if (c == 127 || c == CTRL('h')) {
            if (cursor > 0) {
                memmove(buf + cursor -1, buf + cursor, len - cursor);
                cursor--;
                len--;
                write(STDOUT_FILENO, "\033[D", 3);
                history_active=0;
                redraw_from_cursor(prompt, buf, len, cursor);
            }
            continue;
        }
        /* printable char */
        if (isprint((unsigned char)c)) {
            if (len + 1 >= (int)maxlen)
                continue;

            memmove(buf + cursor + 1, buf + cursor, len - cursor);
            buf[cursor] = c;
            cursor++;
            len++;
            history_active=0;
            redraw_from_cursor(prompt, buf, len, cursor);
        }
    }
}

