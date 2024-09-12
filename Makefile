# kilo is what we want to build and kilo.c is needed to build it
kilo: kilo.c
# What to run to actually build kilo
# $(CC) is a variable that make expands to cc by default
# -Wall stands for "All Warnings", warns against not technically wrong code, but bad or questionable usage
# -Wextra and -pedantic provide further warnings, such that only "unused variable" warnings should be returned in this project
# -stdc=99 specifies that the standard version of C being used is C99. It allows for variables to be declared anywhere in a function, which ANSI C does not
	$(CC) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99