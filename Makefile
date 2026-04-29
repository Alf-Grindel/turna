CFLAGS = -Wall -Wextra
turna: main.c
	cc ${CFLAGS} main.c -o turna

turna-db: main.c
	cc -g main.c -o turna-db
