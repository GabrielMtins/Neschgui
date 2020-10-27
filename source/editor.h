#ifndef EDITOR_H
#define EDITOR_H

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

#include "global.h"
#include "rom.h"
#include "widget.h"

typedef struct{
    rom* main_rom;
    size_t offset_tiles;
    size_t current_sprite_x, current_sprite_y;
    uint8_t current_color;
    SDL_Color palette[4];
    SDL_Surface* sprite_surface;
    widget* draw_widget;
    widget* sheet_widget;
    widget* slider_r;
    widget* slider_g;
    widget* slider_b;
    widget* slider_sheet;
    widget* palette_widget;
} editor;

editor* editor_create();

void editor_input(editor* self, SDL_Event* event);

void editor_loadRom(editor* self, const char* filename);

void editor_freeRom(editor* self);

void editor_drawTilesetToSurface(editor* self);

void editor_render(const editor* self);

#endif
