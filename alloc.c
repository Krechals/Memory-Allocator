#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"

// set size of arena
void initialize(unsigned char **arena, int arena_size) {
    *arena = calloc(arena_size + 1, sizeof(unsigned char));
    // gestion_modify(*arena, 0, 0);
}
// end of application
void finalize(unsigned char **arena) {
    free(*arena);
    *arena = NULL;
}
// set bytes of allocated memory given a value
void fill(unsigned char *arena, int byte_index, int size, int value) {
    for (int i = byte_index; i < byte_index + size; ++i) {
        arena[i] = value;
    }
}
// print the entire memory map (arena)
void dump(unsigned char *arena, int arena_size) {
    for (int current_byte = 0; current_byte < arena_size; ++current_byte) {
        if (current_byte % 8 == 0) {
            printf(" ");
        }
        if (current_byte % 16 == 0) {
            printf("\n%08X\t", current_byte);
        }
        printf("%02X ", arena[current_byte]);
    }
    printf("\n%08X\n", arena_size);
}
/*
    Similar to malloc():
    - allocates the requested memory space
    - returns the byte index of where allocation starts
*/
int alloc(unsigned char *arena, int size, int arena_size) {
    // current_index = byte number of where memory block starts
    // free_space    = number of bytes between 2 memory blocks
    // block_size    = bytes number of the current block memory used
    // next_index    = byte number of where next memory block starts
    // *current      = points where current block starts
    int current_index, free_space, prev_index = 0;
    int block_size, next_index;
    unsigned char *current, *prev, *next;

    current_index = ((int*)arena)[0];
    if (size + 16 >= arena_size) {
        return 0; // NO memory space
    }
    if (current_index == 0) { // first allocation
        ((int*)arena)[0] = 4; // next block allocated (first gestion block)

        ((int*)arena)[1] = 0; // next block allocated (allocated gestion block)
        ((int*)arena)[2] = 0; // previous block allocated
        ((int*)arena)[3] = 12 + size; // total memory size used
        return 16; // byte index of where allocation starts
    } else {
        free_space = current_index - 4;
        block_size = 4;

        // find a free space
        while (free_space < size + 12 && current_index != 0) {
            next_index = 0;
            block_size = 0;
            current = arena + current_index;

            next_index = ((int*)current)[0];
            block_size = ((int*)current)[2];

            if (next_index != 0) {
                free_space = next_index - current_index - block_size;
            } else {
                free_space = arena_size - current_index - block_size;
            }
            prev_index = current_index;
            current_index = next_index;
        }
        if (free_space >= size + 12) { // free space found
            current = arena + prev_index + block_size;
            prev = arena + prev_index;
            next = arena + current_index;

            ((int*)current)[0] = current_index; // next block allocated
            ((int*)current)[1] = prev_index;  // previous block allocated
            ((int*)current)[2] = 12 + size;  // total memory size used

            ((int*)prev)[0] = prev_index + block_size; // update previous block
            if (current_index != 0) {
                ((int*)next)[1] = prev_index + block_size; // update next block
            }
            return prev_index + block_size + 12;
        }
    }
    return 0; // NO memory space
}
/*
    Similar to free():
    - frees the requested memory space
*/
void afree(unsigned char *arena, int byte_index) {
    // *current      = points where current block starts
    unsigned char *current, *next, *prev;
    int prev_index = 0, next_index = 0;

    byte_index -= 12; // move from first usable byte to gestion blocks
    current = arena + byte_index;
    next_index = ((int*)current)[0];
    prev_index = ((int*)current)[1];

    prev = arena + prev_index;
    next = arena + next_index;
    ((int*)prev)[0] = next_index; // update previous memory block
    if (next_index != 0) { // update next memory block
        ((int*)next)[1] = prev_index;
    }
}
// print number of free blocks and bytes
void show_free(unsigned char *arena, int arena_size) {
    // free_blocks      = number of free continuous blocks
    // total_used_space = used bytes in arena
    // free_bytes       = number of free bytes in arena
    int total_used_space = 4, current_index = ((int*)arena)[0];
    int prev_size = 4, prev_index = 0, free_blocks = 0, free_bytes;
    unsigned char *current;

    while (current_index != 0) {
        if (prev_size + prev_index != current_index) { // break between blocks
            free_blocks++;
        }
        current = arena + current_index;
        total_used_space += ((int*)current)[2];
        prev_index = current_index;
        current_index = ((int*)current)[0];
        prev_size = ((int*)current)[2];
    }
    if (prev_size + prev_index < arena_size) {
        free_blocks++;
    }
    free_bytes = arena_size - total_used_space;
    printf("%d blocks (%d bytes) free\n", free_blocks, free_bytes);
}
// print statistic of the memory arena
void show_usage(unsigned char *arena, int arena_size) {
    // used_space       = bytes allocated in arena
    // memory_blocks    = number of memory spaces allocated
    // free_blocks      = number of free continuous blocks
    // total_used_space = used bytes in arena
    int used_space = 0, current_index = ((int*)arena)[0];
    int prev_size = 4, prev_index = 0, free_blocks = 0;
    int memory_blocks = 0, total_used_space = 4;
    unsigned char *current;

    while (current_index != 0) {
        if (prev_size + prev_index != current_index) { // break between blocks
            free_blocks++;
        }
        memory_blocks++;
        current = arena + current_index;
        prev_index = current_index;

        used_space += ((int*)current)[2] - 12;
        total_used_space += ((int*)current)[2];
        current_index = ((int*)current)[0];
        prev_size = ((int*)current)[2];
    }
    if (prev_size + prev_index < arena_size) {
        free_blocks++;
    }

    int eff = 100 * used_space / total_used_space, fragm;
    if (memory_blocks == 0) {
        fragm = 0; // nothing allocated
    } else {
        fragm = 100 * (free_blocks - 1) / memory_blocks;
    }
    // print statistic
    printf("%d blocks (%d bytes) used\n", memory_blocks, used_space);
    printf("%d%% efficiency\n", eff);
    printf("%d%% fragmentation\n", fragm);
}
// print all free/occupied blocks in memory arena
void show_alloc(unsigned char *arena, int arena_size) {
    int total_used_space = 4, current_index = ((int*)arena)[0];
    int prev_size = 4, prev_index = 0, free_blocks = 0;
    unsigned char *current;

    printf("OCCUPIED 4 bytes\n");
    while (current_index != 0) {
        if (prev_size + prev_index != current_index) { // break between blocks
            printf("FREE %d bytes\n", current_index - prev_size - prev_index);
        }
        current = arena + current_index;
        prev_index = current_index;

        total_used_space = ((int*)current)[2];
        current_index = ((int*)current)[0];
        prev_size = ((int*)current)[2];

        printf("OCCUPIED %d bytes\n", total_used_space);
    }
    if (prev_size + prev_index < arena_size) {
        printf("FREE %d bytes\n", arena_size - prev_size - prev_index);
        free_blocks++;
    }
}
