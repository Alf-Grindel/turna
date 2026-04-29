#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 5
#define LEN 20 /* buf size for single instruction, have white space */

typedef enum {
	L,
	R,
} SHIFT;

typedef char* STATE;
typedef char* SYMBOL;

typedef struct {
	STATE current; /* current state */
	SYMBOL read; /* tape symbol */
	SYMBOL write; /* write symbol */
	SHIFT move; /* move tape */
	STATE next; /* next state */
} TURNA;

TURNA parseTurna(char *filepath, char* buf, int line_index) {
	int capacity = CAPACITY;
	int size = 0;
	char *tokens[capacity];
	buf[strcspn(buf, "\n")] = '\0';
	for (char *token = strtok(buf, " "); token; token = strtok(NULL, " ")) {
		if (size >= capacity) {
			fprintf(stderr, "ERROR: file->%s: line->%d: a single turna is expected to have 5 tokens\n", filepath, line_index);
			exit(1);
		}
		tokens[size++] = token;
	}

	if (size != capacity) {
		fprintf(stderr, "ERROR: file->%s: line->%d: a single turna is expected to have 5 tokens\n", filepath, line_index);
		exit(1);
	}

	TURNA turna;
	turna.current = strdup(tokens[0]); 
	turna.read = strdup(tokens[1]);
	turna.write = strdup(tokens[2]);
	switch(*tokens[3]) {
		case 'L':
			turna.move = L;
			break;
		case 'R':
			turna.move = R;
			break;
		default:
			fprintf(stderr, "ERROR: file->%s: line->%d: %c is not a correct step. Expected `L` or `R`\n", filepath, line_index, *tokens[3]);
			exit(1);
	}
	turna.next = strdup(tokens[4]);
	return turna;
}

int main(int argc, char* argv[])
{
	int args_index = 1;

	if (args_index >= argc) {
		fprintf(stderr, "ERROR: not turna input file is provided\n");
		exit(1);
	}
	char *turna_filepath = argv[args_index++];

	FILE *turna_fp = fopen(turna_filepath, "r");

	if (!turna_fp) {
		fprintf(stderr, "ERROR: open turna input file failed\n");
		exit(1);
	}

	/* instruction set */
	TURNA *turnas = (TURNA *) malloc(CAPACITY * sizeof(TURNA));
	int capacity = CAPACITY;
	int size = 0; /* check have how many instructions, to realloc memory size */
	int line_number = 0; /* to index each instruction line number */

	/* single instruction read from input file and store in buf tmporary array */
	char buf[LEN];

	while(fgets(buf, LEN, turna_fp)) {
		if (size >= capacity) {
			capacity *= 2; 
			turnas = (TURNA *) realloc(turnas, capacity * sizeof(TURNA));
		}
		line_number++;
		if (*buf == '\n') continue; 
		TURNA turna = parseTurna(turna_filepath, buf, line_number);
		turnas[size++] = turna;
	}

	for(int i = 0; i < size; i++) {
		TURNA turna = turnas[i];
		char *move = turna.move ? "R" : "L";
		printf("turnas: current: %s\tread: %s\twrite: %s\tmove: %s\tnext: %s\n", turna.current, turna.read, turna.write, move, turna.next);
	}

	free(turnas);
	
	return 0;
}
