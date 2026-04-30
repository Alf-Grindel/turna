#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 5
#define LEN 20 /* buf size for single instruction, have white space */
#define BEGIN "A"

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

typedef struct {
	SYMBOL* tapes;
	int head;
	STATE state;
} MACHINE;

int next(MACHINE *machine, TURNA *pr, int pr_size, int tape_size) {
	for (int i = 0; i < pr_size; i++) {
		if (!strcmp(pr[i].current, machine->state) && !strcmp(pr[i].read, machine->tapes[machine->head])) {
			machine->tapes[machine->head] = strdup(pr[i].write);
			switch (pr[i].move) {
				case L:
					if(machine->head == 0) machine->head = tape_size - 1;
					else machine->head -= 1;
					break;
				case R:
					machine->head = (machine->head + 1) % tape_size;
					break;
			}
			machine->state = strdup(pr[i].next);
			return 1;
		}
	}
	return 0;
}

void dump_tape(MACHINE *machine, int tape_size) {
	for (int i = 0; i < tape_size; i++) {
		printf("%s ", machine->tapes[i]);
	}
	printf("\n");
}

void debug_dump(MACHINE *machine, int tape_size) {
	printf("STATE: %s\n", machine->state);
	dump_tape(machine, tape_size);
	for (int i = 0; i < machine->head; i++) {
		printf("  ");
	}
	printf("^\n");
}

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

	if (args_index >= argc) {
		fprintf(stderr, "ERROR: not tape input file is provided\n");
		exit(1);
	}
	char *tape_filepath = argv[args_index];

	FILE *turna_fp = fopen(turna_filepath, "r");
	FILE *tape_fp = fopen(tape_filepath, "r");

	if (!turna_fp) {
		fprintf(stderr, "ERROR: open turna input file failed\n");
		exit(1);
	}

	if (!tape_fp) {
		fprintf(stderr, "ERROR: open tape input file failed\n");
		exit(1);
	}

	/* instruction set */
	TURNA *turnas = (TURNA *) malloc(CAPACITY * sizeof(TURNA));
	int turna_capacity = CAPACITY;
	int turna_size = 0; /* check have how many instructions, to realloc memory size */
	int line_number = 0; /* to index each instruction line number */

	/* single instruction read from input file and store in buf tmporary array */
	char buf[LEN];

	while(fgets(buf, LEN, turna_fp)) {
		if (turna_size >= turna_capacity) {
			turna_capacity *= 2; 
			turnas = (TURNA *) realloc(turnas, turna_capacity * sizeof(TURNA));
		}
		line_number++;
		if (*buf == '\n') continue; 
		TURNA turna = parseTurna(turna_filepath, buf, line_number);
		turnas[turna_size++] = turna;
	}
	fclose(turna_fp);

	SYMBOL *tapes = (SYMBOL *) malloc(CAPACITY * sizeof(SYMBOL));
	int tape_capacity = CAPACITY;
	int tape_size = 0;

	char buff[2];
	
	while(fgets(buff, 2, tape_fp)) {
		if (tape_size >= tape_capacity) {
			tape_capacity *= 2;
			tapes = (SYMBOL *) realloc(tapes, tape_capacity * sizeof(SYMBOL));
		}
		if (*buff == ' ') continue;
		if (*buff == '\n') break;
		tapes[tape_size++] = strdup(buff);
	}
	fclose(tape_fp);

	MACHINE *machine = (MACHINE *) malloc(sizeof(MACHINE));
	machine->tapes = tapes;
	machine->head = 0;
	machine->state =  BEGIN; 

	while(next(machine, turnas, turna_size, tape_size)) {
		debug_dump(machine, tape_size);
	}
	
	for (int i = 0; i < turna_size; i++) {
		free(turnas[i].current);
		free(turnas[i].read);
		free(turnas[i].write);
		free(turnas[i].next);
	}
	free(turnas);
	for (int i = 0; i < tape_size; i++) {
		free(tapes[i]);
	}
	free(tapes);
	free(machine->state);
	free(machine);
	
	return 0;
}
