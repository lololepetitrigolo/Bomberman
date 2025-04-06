#include <assert.h>

#include "button_list.h"

ButtonList* button_list_create(size_t capacity) {
	ButtonList* list = malloc(sizeof(*list));
	list->cap = capacity;
	list->len = 0;
	list->buttons = malloc(capacity * sizeof(*list->buttons));
	return list;
}

void button_list_destroy(ButtonList* list) {
	free(list->buttons);
	free(list);
}

Button* button_list_new_button(ButtonList* list) {
	if (list->len == list->cap) {
		list->cap *= 2;
		list->buttons =
			realloc(list->buttons, list->cap * sizeof(*list->buttons));
	}
	list->len++;
	return &list->buttons[list->len - 1];
}

void button_list_remove(ButtonList* list, Button* button) {
	assert(list->len > 0);
	int index = (button - list->buttons) / sizeof(Button);
	list->len--;
	list->buttons[index] = list->buttons[list->len];
}

void button_list_add_button(ButtonList* list, Button* button) {
	if (list->len == list->cap) {
		list->cap *= 2;
		list->buttons =
			realloc(list->buttons, list->cap * sizeof(*list->buttons));
	}
	list->len++;
	list->buttons[list->len - 1] = *button;
}
