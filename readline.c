#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "readline.h"
#include "colors.h"
#define CTRL(X) ((X) & 0X1f)

static void redraw_from_cursor(const char *buf, int len, int cursor)
{
    char seq[64];

    write(STDOUT_FILENO, "\r\033[K", 4);    // clear line
    write(STDOUT_FILENO, ">>> ", 4);      // redraw prompt
    write(STDOUT_FILENO, buf, len);       // redraw buffer

    int pos = len - cursor;
    if (pos > 0) {
        snprintf(seq, sizeof(seq), "\033[%dD", pos);
        write(STDOUT_FILENO, seq, strlen(seq));
    }
}

int ikc_readline(char *out, size_t maxlen)
{

    char buf[4096];
    int len = 0;
    int cursor = 0;

    if (maxlen > sizeof(buf))
        maxlen = sizeof(buf);

    write(STDOUT_FILENO, "\r\033[k", 4);
    write(STDOUT_FILENO, PROMPT">>> "C_RESET, strlen(PROMPT">>> " C_RESET));   
    while(1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1)
            return -1;

        /* ENTER */
        if (c == '\n' || c == '\r') {
            buf[len] = '\0';
            write(STDOUT_FILENO, "\n", 1);
            strncpy(out, buf, maxlen);
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
            }
            redraw_from_cursor(buf, len, cursor);
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
                case 'A': /* up */
                    /* ignore for now */
                    break;
                case 'B': /* down */
                    break;
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
                redraw_from_cursor(buf, len, cursor);
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
            redraw_from_cursor(buf, len, cursor);
        }
    }
}

