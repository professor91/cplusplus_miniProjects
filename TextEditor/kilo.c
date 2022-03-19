/*** includes ***/

#include <ctype.h>
#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// prints an error message and exits the program
void die (const char* s) {
    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);      // comes from stdio.h
    exit(1);        // comes from stdlib.h
}

void disableRawMode () {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

// gets us fully into raw mode
void enableRawMode () {

    if ( tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    
    /*  struct termios, tcgetattr(), tcsetattr(), ECHO, TCSAFLUSH, VMIN, VTIME
     *  not input flags  -  ICANON ISIG
     *  input flags      -  IXON ICRNL
     *  output flags     -  OPOST
     * 
     *  all comes from termios.h
     */
    struct termios raw= orig_termios;

    /*  ECHO causes each key you type to be printed to the terminal, 
     *  so you can see what you're typing.
     *  We turned it off in this line
     * 
     *  ICANON allows us to turn off caninical mode
     *  this means we will be reading input byte-by-byte, instead of line-by-line
     *  since we are now reading input byte-by-byte, program will quit as soon as
     *  q is pressed
     * 
     *  ISIG turns off Ctrl-C (quits process) Ctrl-Z (takes process to background) signals
     * 
     *  IXON turns off Ctrl-S and Ctrl-Q, software flow controls
     *  Ctrl-S stops data from being transmitted to the terminal until you press Ctrl-Q
     * 
     *  IEXTEN turns off Ctrl-V flag, it makes termminal wait for you to type another
     *  character and then sends that character literally
     * 
     *  ICRNL turns of '\r'- carriage returns and '\n'- newlines
     * 
     *  BRKINT, INPCK, ISTRIP, CS8 are misc flags coz they are either already turned off
     *  or are not used in modern terminal emulators
     */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    // cc stands for control characters
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey () {
    int nread;
    char c;

    while ((nread == read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)  die("read");
    }

return c;
}

/*** input ***/

void editorProcessKeypress () {
    char c = editorReadKey();

    switch (c) {

        case CTRL_KEY('q'):   
                            exit(0);    
                            break;

    }
}


/*** output ***/

void editorDrawRows () {
    int y;
    for (y= 0; y< 24; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen () {
    /*  write comes from unistd.h
     *  4 in write() function means we are writing 4 bytes out to the terminal.
     *  The 1st byte is \x1b which is the escape character, or 27 in decimal
     *  the other 3 bytes are [2j
     * 
     *  Escape sequences instructs the terminal to do various text formatting tasks
     *  such as coloring text, moving cursor around and clearing parts of screen
    */
    write(STDOUT_FILENO, "\x1b[2j", 4);
    
    // reposition the curson
    write(STDIN_FILENO, "\x1b[H", 3);

    editorDrawRows();

    write(STDOUT_FILENO, "\x1b[H", 3);
}


/*** init ***/

int main() {

    enableRawMode();

    /*  STDIN_FILENO comes from unistd.h
     *  
     *  here we are asking read() to read
     *  1 byte from the standard input
     *  into c variable 'c' until 
     *  there are no more bytes to read
     *  it will return 0 when it reaches EOF
     * 
     *  iscntrl comes from ctype.h
     *  In ASCII table  -   0-31 & 127 are contorl characters and
     *                      32-126 are printable characters
     * 
     *  iscntrl tests whether a character is a control character or not
     */
    while (1) {
        editorRefreshScreen();

        /*  It's job is to wait for one keypress, and return it
         *  
         *  
         * 
        */
        editorProcessKeypress();
    }

return 0;
}
