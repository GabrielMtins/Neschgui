#include "undo.h"

/*
    Copyright (C) 2020  Gabriel Martins

    This file is part of Neschgui.

    Neschgui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Neschgui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Neschgui.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>

static uint8_t undo_action_cmp(undo_action a, undo_action b){
    if(a.x != b.x) return 0;
    if(a.y != b.y) return 0;
    if(a.offset != b.offset) return 0;
    return 1;
}

void undo_stack_push(undo_stack* stack, rom* self, size_t offset, uint8_t x, uint8_t y, uint8_t type){
    if(stack->top == MAX_ACTION) stack->top = 0;
    undo_action new_action = {x, y, rom_getPixel(self, offset, x, y), offset, type};
    if(stack->top > 0){
        if(undo_action_cmp(stack->stack[stack->top-1], new_action)) return;
    }
    stack->stack[stack->top] = new_action;
    stack->top++;
}

void undo_stack_pop(undo_stack* stack, rom* self){
    if(stack->top == 0) return;
    undo_action new_action = stack->stack[stack->top-1];
    rom_putPixel(self, new_action.offset, new_action.x, new_action.y, new_action.color);
    if(stack->top > 0) stack->top--;
    if(new_action.type == UNDO_TYPE_PASTE) undo_stack_pop(stack, self);
    // if it was pasted, we just remove all of them that were pasted before
}
