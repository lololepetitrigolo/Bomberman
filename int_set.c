#include <assert.h>

#include "int_set.h"

IntSet* IntSet_create(uint capacity) {
	assert(capacity > 0);
	IntSet* set = malloc(sizeof(IntSet));
	set->cap = capacity;
	set->len = 0;
	set->data = (int*)malloc(capacity * sizeof(int));
	return set;
}

void IntSet_free(IntSet* set) {
	free(set->data);
	free(set);
}

void IntSet_add(IntSet* set, int val) {
	if (set->len == set->cap) {
		set->cap *= 2;
		set->data = realloc(set->data, set->cap * sizeof(*set->data));
	}
	set->data[set->len] = val;
	set->len += 1;
}

void IntSet_remove(IntSet* set, int val) {
	for (int i = 0; i < set->len; i++) {
		if (set->data[i] == val) {
			// Exchange the removed element and the last element
			set->data[i] = set->data[set->len - 1];
			set->len--;
			return;
		}
	}
}
