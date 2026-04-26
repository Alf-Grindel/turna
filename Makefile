CFLAGS = -Wall -Wextra
tmst: main.c
	cc ${CFLAGS} main.c -o tmst
