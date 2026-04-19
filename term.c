#include "term.h"
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios orig_termios;

void term_disable_raw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void term_enable_raw(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(term_disable_raw);

    struct termios raw = orig_termios;

    /* input modes */
    raw.c_lflag &= ~(ICANON | ECHO | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);

    /* output modes */
    raw.c_oflag &= ~(OPOST);

    /* control modes */
    raw.c_cflag |= (CS8);

    /* read settings */
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


