#ifndef BUTTON_H
#define BUTTON_H

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

#include <stdint.h>
#include "global.h"

typedef struct{
    int x, y, w, h;
} widget;

widget* widget_create();

void getMousePositionRelative(int* x, int* y);

void widget_render(const widget* self, SDL_Texture* texture);

uint8_t widget_isMouseInsideWidget(const widget* self, int x, int y);

void widget_getMouseRelativeToWidget(const widget* self, int mouse_x, int mouse_y, double* x, double* y);

void widget_renderSlide(const widget* self, uint8_t percentage, SDL_Color bg, SDL_Color fg);

void widget_destroy(widget* self);

#endif
