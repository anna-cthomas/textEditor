# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <termios.h>
# include <unistd.h>

/*
    ---------
    | NOTES |
    ---------

    - By default, terminal starts in canonical/cooked mode. Keyboard input is only sent to a program after enter is pressed.
    - Goal: raw mode. It will allow the text editor to read live input.
        - Turn off ECHO. ECHO prints each typed character to the terminal. Turned off e.g. typing a password when using sudo
        - Turn off ICANON. ICANON reads input line-by-line. Flipping it off allows for reading byte-by-byte. This means the program will close as soon as 'q' is pressed
*/

/*
    ------------
    | GLOSSARY |
    ------------

    - TCSAFLUSH : Says to wait for all pending output to be written to the terminal and flush any input that has not been read
    - ~ : Bitwise-NOT operator. Gives the inverse of a set of bits. Used with bitwise-AND '&' to flip desired bits on/off.
    - c_lflag : Local flags
    - Control Character : A non-printable character
*/

/*
    ------------------
    | DEBUGGING TIPS |
    ------------------

    - Reset Terminal: ctrl+c, type 'reset', hit enter. Restart terminal emulator if fix fails
    - ctrl+z will send the program to the background. Bring it back with the 'fg' command. May immediately quit after
*/

struct termios originalTermios;

// Return to original terminal state
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
}

// Turns off the ECHO feature
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &originalTermios);
    // Call disableRawMode once the program exits
    atexit(disableRawMode);

    // Initialize rawTermios as a copy of the original terminal state
    struct termios rawTermios = originalTermios;

    // Modify the terminal's attributes
    // Flip only the ECHO and ICANON bitflags off
    rawTermios.c_lflag &= ~(ECHO | ICANON);

    // Write the terminal's newly modified attributes back out
    // Second argument specifies when to apply terminal modifications
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawTermios);
}

// Starting point
int main() {
    enableRawMode();

    char readChar;
    // Read 1 byte from the standard input into 'readChar' and do so until all bytes have been read
    // read() returns the number of bytes read and will return 0 when it reaches the end of a file
    while(read(STDIN_FILENO, &readChar, 1) == 1 && readChar != 'q') {
        // Check if input is a control character
        if (iscntrl(readChar)) {
            printf("%d\n", readChar);
        } else {
            // Format the byte as a decimal number and write it out as a character
            printf("%d ('%c')\n", readChar, readChar);
        }
    }

    return 0;
}
