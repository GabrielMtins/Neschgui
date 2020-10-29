#ifndef UNDO_H
#define UNDO_H

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

// this file will handle the undo action

#include "stdint.h"
#include "rom.h"

#define UNDO_TYPE_PUT_PIXEL 0
#define UNDO_TYPE_PASTE 1

typedef struct{
    uint8_t x, y, color;
    size_t offset;
    uint8_t type;
} undo_action;

#define MAX_ACTION 1024

typedef struct{
    undo_action stack[MAX_ACTION];
    size_t top;
} undo_stack;

void undo_stack_push(undo_stack* stack, rom* self, size_t offset, uint8_t x, uint8_t y, uint8_t type);

void undo_stack_pop(undo_stack* stack, rom* self);


#endif
