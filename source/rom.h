#ifndef ROM_H
#define ROM_H

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
    Contact: gabrielmartinsthe@gmail.com
*/

#define NES_ROM 0
#define GB_ROM 1
#define SMS_ROM 2

#include <stdint.h>
#include <stddef.h>

typedef struct{
    uint8_t* rom_buffer;
    char rom_name[64];
    size_t size;
    uint8_t type;
    uint8_t bytes_per_sprite;
    uint8_t num_of_colors;
} rom;

rom* rom_load(const char* filename);

rom* rom_loadEmptyRom();

uint8_t rom_getPixel(const rom* self, size_t offset, size_t x, size_t y);

void rom_putPixel(rom* self, size_t offset, size_t x, size_t y, uint8_t color);

void rom_save(const rom* self);

void rom_destroy(rom* self);

#endif
