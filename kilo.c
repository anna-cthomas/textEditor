# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <termios.h>
# include <unistd.h>

/*
    ---------
    | NOTES |
    ---------

    * By default, terminal starts in canonical/cooked mode. Keyboard input is only sent to a program after enter is pressed.
    * Goal: raw mode. It will allow the text editor to read live input
        - Turn off ECHO (Disables directly printing typed keys)
        - Turn off ICANON (Reads input byte-by-byte)
        - Turn off ISIG (Disables ctrl+c, ctrl+z)
        - Turn off IXON (Disables ctrl+s, ctrl+q)
        - Turn off IEXTEN (Disables ctrl+v)
        - Turn off ICRNL (Fixes ctrl+m)
        - Turn off OPOST (Fixes newlines)
        - Turn off BRKINT, INPCK, ISTRIP, CS8 (Misc.)

*/

/*
    ------------
    | GLOSSARY |
    ------------

    * TCSAFLUSH : Says to wait for all pending output to be written to the terminal and flush any input that has not been read
    * ~ : Bitwise-NOT operator. Gives the inverse of a set of bits. Used with bitwise-AND '&' to flip desired bits on/off
    * c_lflag : Local flags ("miscellaneous flags")
    * c_iflag : Input flags
    * c_oflag : Output flags
    * c_cflag : Control flags
    * Control Character : A non-printable character
    * CS8 : Bitmask with multiple bits. Sets character size to 8 bits per byte
    * Bitflags
        - ECHO : Prints each typed character to the terminal. Turned off example: typing a password when using sudo
        - ICANON : Reads input line-by-line. Flipping it off allows for reading byte-by-byte. This means the program will close as soon as 'q' is pressed
        - ISIG : Represents signals such as ctrl+z (sends suspension signal to process) and ctrl+c (sends termination signal to process). When flipped off, these shortcuts can be read as byte inputs
        - IXON : For software flow control signals, like ctrl+s  (stops transmission of data to terminal) and ctrl+q (resumes transmission of data to terminal)
        - IEXTEN : Controls commands like ctrl+v (waits for a character to be typed then sends said character literally) and ctrl+o (set to be discarded by the terminal driver on MacOS by default)
        - ICRNL : Controls ctrl+m (translates carriage returns into newlines)
        - OPOST : Translates each newline into a newline + carriage return
        - BRKINT, INPCK, ISTRIP : Most if not all are likely already turned off or don't apply to modern terminal emulators.

*/

/*
    ------------------
    | DEBUGGING TIPS |
    ------------------

    * Reset Terminal: ctrl+c, type 'reset', hit enter. Restart terminal emulator if fix fails
    * ctrl+z will send the program to the background. Bring it back with the 'fg' command. May immediately quit after

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
    // Flip IXON, ICRNL, OPOST, ECHO, ICANON, ISIG, and IEXTEN bitflags off
    rawTermios.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    rawTermios.c_oflag &= ~(OPOST);
    rawTermios.c_cflag |= (CS8);
    rawTermios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

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
            printf("%d\r\n", readChar);
        } else {
            // Format the byte as a decimal number and write it out as a character
            printf("%d ('%c')\r\n", readChar, readChar);
        }
    }

    return 0;
}
