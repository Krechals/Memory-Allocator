#ifndef ALLOC_H
#define ALLOC_H

void initialize(unsigned char **arena, int arena_size);
void finalize(unsigned char **arena);
void dump(unsigned char *arnea, int area_size);
void fill(unsigned char *arena, int byte_index, int size, int value);
int alloc(unsigned char *arena, int size, int arena_size);
void afree(unsigned char *arena, int byte_index);
void show_free(unsigned char *arena, int arena_size);
void show_usage(unsigned char *arena, int arena_size);
void show_alloc(unsigned char *arena, int arena_size);

#endif
