/*
    Copyright 2019 Topala Andrei  <topala.andrei@gmail.com>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"

#define MAX_LINE_LEN 1000
#define MAX_CMD_ARGS 10

int cmd_read(int *args_count, char *args[MAX_CMD_ARGS]);
void cmd_execute(int args_count, char *args[MAX_CMD_ARGS],
                unsigned char **arena, int *END_OF_APP);
void cmd_free(int args_count, char *args[MAX_CMD_ARGS]);
void gestion_modify(unsigned char *arena, int byte_index, int value);
int ascii_to_int(char *string_number);

int main() {
    // cmd: cmd_name argument1 argument2 ...
    // args = {cmd_name, argument1, argument2, ... }
    // args_count = length of args
    int args_count, END_OF_APP = 0;
    char *args[MAX_CMD_ARGS];
    unsigned char *arena;

    while (!END_OF_APP) {
        // read new command from stdin
        if (!cmd_read(&args_count, args)) {
            break; // no command was found, break loop
        }
        cmd_execute(args_count, args, &arena, &END_OF_APP);  // execute command
        cmd_free(args_count, args); // free memory allocated for args
    }
    return 0;
}

// read a new command from stdin
int cmd_read(int *args_count, char *args[MAX_CMD_ARGS]) {
    char line[MAX_LINE_LEN + 1]; //  buffer for a line

    // try to read a new command (a new line)
    if (!fgets(line, MAX_LINE_LEN, stdin)) {
        return 0; // fail
    }

    // delete extra '\n'
    int len = strlen(line);
    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // split string in tokens ("words")
    const char *delimiters = " ";       // delimiters
    int count = 0;                      // number of tokens
    char *token = strtok(line, delimiters); // get first token

    while (token != NULL) {
        // save current token
        len = strlen(token) + 1; // +1 because I need to store '\0'
        args[count] = (char *) malloc(len * sizeof(char));
        strcpy(args[count], token);

        // get next token
        token = strtok(NULL, delimiters);
        ++count;
    }

    *args_count = count; // save arguments count

    return 1;            // success
}


// free memory allocated for args
void cmd_free(int args_count, char *args[MAX_CMD_ARGS]) {
    for (int i = 0; i < args_count; ++i) {
        free(args[i]);
    }
}

// ONLY FOR DEBUG
void print_args(int args_count, char *args[MAX_CMD_ARGS]) {
    printf("cmd = (%s)\n", args[0]);
    for (int i = 0; i < args_count; ++i) {
        printf("arg[%d] = (%s)\n", i, args[i]);
    }
    printf("\n");
}

// execute a command
void cmd_execute(int args_count, char *args[MAX_CMD_ARGS],
                unsigned char **arena, int *END_OF_APP) {
    char *cmd = args[0];
    static int arena_size; // size of memory area

    // command menu
    if (!strcmp(cmd, "INITIALIZE")) {
        arena_size = ascii_to_int(args[1]);
        initialize(arena, arena_size);
    } else if (!strcmp(cmd, "FINALIZE")) {
        finalize(arena);
        *END_OF_APP = 1; // app finishes after this function
    } else if (!strcmp(cmd, "DUMP")) {
        dump(*arena, arena_size);
    } else if (!strcmp(cmd, "ALLOC")) {
        int size = ascii_to_int(args[1]), err;
        err = alloc(*arena, size, arena_size);  // error message of function
        printf("%d\n", err);
    } else if (!strcmp(cmd, "FREE")) {
        int byte_index = ascii_to_int(args[1]); // gestion block to be freed
        afree(*arena, byte_index);
    } else if (!strcmp(cmd, "FILL")) {
        int byte_index, size, value;

        byte_index = ascii_to_int(args[1]); // starting position
        size = ascii_to_int(args[2]);       // number of bytes to be filled
        value = ascii_to_int(args[3]);      // value of bytes
        fill(*arena, byte_index, size, value);
    } else if (!strcmp(cmd, "SHOW")) {
        if (!strcmp(args[1], "FREE")) {
            show_free(*arena, arena_size);
        } else if (!strcmp(args[1], "USAGE")) {
            show_usage(*arena, arena_size);
        } else if (!strcmp(args[1], "ALLOCATIONS")) {
            show_alloc(*arena, arena_size);
        }
    } else if (!strcmp(cmd, "ALLOCALIGNED")) {
        // TODO: implement ALLOCALIGNED
    } else if (!strcmp(cmd, "REALLOC")) {
        // TODO: implement REALLOC
    } else if (!strcmp(cmd, "SHOW MAP")) {
        // TODO: implement SHOW MAP
    } else {
        fprintf(stderr, "[ERROR] unknown command (%s)\n", cmd);
    }
}
// string to decimal, similar with atoi
int ascii_to_int(char *string_number) {
    int decimal = 0;
    do {
        decimal = decimal * 10 + string_number[0] - '0';
    } while (*(++string_number));
    return decimal;
}

void gestion_modify(unsigned char *arena, int byte_index, int value) {
    arena[byte_index] = value;
}
