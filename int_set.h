#ifndef INT_SET_H
#define INT_SET_H

#include <SDL.h>

typedef struct {
	int len;
	int cap;
	int* data;
} IntSet;

/* Allocates an int set with size `capacity`. */
IntSet* IntSet_create(uint capacity);

/* Empties the set and free the memory associated with it. */
void IntSet_free(IntSet* set);

/* Appends an element `val` to `set`. */
void IntSet_add(IntSet* set, int val);

/* Removes the element `val` from the set if it exists. */
void IntSet_remove(IntSet* set, int val);

#endif
