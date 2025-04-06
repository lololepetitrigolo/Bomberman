#ifndef BUTTON_LIST_H
#define BUTTON_LIST_H

typedef struct ButtonList ButtonList;

#include <stdlib.h>

#include "button.h"

struct ButtonList {
	size_t len;
	size_t cap;
	Button* buttons;
};

ButtonList* button_list_create(size_t capacity);

void button_list_destroy(ButtonList* list);

Button* button_list_new_button(ButtonList* list);

void button_list_add_button(ButtonList* list, Button* button);

void button_list_remove(ButtonList* list, Button* button);

#endif
