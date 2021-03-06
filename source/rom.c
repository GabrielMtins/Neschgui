#include "rom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

rom* rom_load(const char* filename){
    FILE* file = fopen(filename, "rb");
    if(file == NULL) return NULL;
    rom* self = malloc(sizeof(rom));
    if(!strcmp(filename+strlen(filename)-4, ".nes")){
        self->type = NES_ROM;
        self->bytes_per_sprite = 16;
    }
    if(!strcmp(filename+strlen(filename)-3, ".gb")){
        self->type = GB_ROM;
        self->bytes_per_sprite = 16;
    }
    if(!strcmp(filename+strlen(filename)-4, ".gbc")){
        self->type = GB_ROM;
        self->bytes_per_sprite = 16;
    }
    if(!strcmp(filename+strlen(filename)-4, ".sms")){
        self->type = SMS_ROM;
        self->bytes_per_sprite = 32;
    }
    if(!strcmp(filename+strlen(filename)-3, ".gg")){
        self->type = SMS_ROM;
        self->bytes_per_sprite = 32;
    }
    self->num_of_colors = self->bytes_per_sprite*self->bytes_per_sprite/64;
    strcpy(self->rom_name, filename);
    // get the size of the rom
    fseek(file, 0, SEEK_END);
    self->size = ftell(file);
    fseek(file, 0, SEEK_SET);
    self->rom_buffer = malloc(self->size);
    fread(self->rom_buffer, self->size, 1, file);
    fclose(file);
    return self;
}

rom* rom_loadEmptyRom(){
    rom* self = malloc(sizeof(rom));
    strcpy(self->rom_name, "empty.nes");
    self->num_of_colors = 4;
    self->bytes_per_sprite = 16;
    self->type = NES_ROM;
    self->size = 40*1024;
    self->rom_buffer = malloc(self->size);
    for(size_t i = 0; i < self->size; i++) self->rom_buffer[i] = 0;
    return self;
}

static uint8_t rom_nes_getPixel(const rom* self, size_t offset, size_t x, size_t y){
    /*
        we need to arguments for the color
        for example if we have the numbers:
        1000001 and 11110000, the color of the byte will be:
        32220001
        so we use bitshifting and bitwise ands to do this operation
    */
    if(self == NULL) return 0;
    if(offset*16 > self->size-16) return 0;
    uint8_t j = 1<<x;
    // just to get the right argument for each color
    int color_arg1 = j&self->rom_buffer[0+y+offset*16];
    int color_arg2 = j&self->rom_buffer[8+y+offset*16];
    if(color_arg1&&color_arg2) return 3;
    if(color_arg1) return 2;
    if(color_arg2) return 1;
    return 0;
}

static uint8_t rom_gb_getPixel(const rom* self, size_t offset, size_t x, size_t y){
    if(self == NULL) return 0;
    if(offset*16 > self->size-16) return 0;
    int j = 1<<x;
    int color_arg1 = j&self->rom_buffer[0+y*2+offset*16];
    int color_arg2 = j&self->rom_buffer[1+y*2+offset*16];
    if(color_arg1&&color_arg2) return 3;
    if(color_arg1) return 2;
    if(color_arg2) return 1;
    return 0;
}

static uint8_t rom_sms_getPixel(const rom* self, size_t offset, size_t x, size_t y){
    if(self == NULL) return 0;
    if(offset*32 > self->size-32) return 0;
    int j = 1<<x;
    int color_arg1 = j&self->rom_buffer[0+y*4+offset*32];
    int color_arg2 = j&self->rom_buffer[1+y*4+offset*32];
    int color_arg3 = j&self->rom_buffer[2+y*4+offset*32];
    int color_arg4 = j&self->rom_buffer[3+y*4+offset*32];
    if(color_arg1) color_arg1 = 1;
    if(color_arg2) color_arg2 = 2;
    if(color_arg3) color_arg3 = 4;
    if(color_arg4) color_arg4 = 8;
    int return_color = color_arg1+color_arg2+color_arg3+color_arg4;
    return (uint8_t) return_color;
}

uint8_t rom_getPixel(const rom* self, size_t offset, size_t x, size_t y){
    if(self == NULL) return 0;
    switch(self->type){
        case NES_ROM:
        return rom_nes_getPixel(self, offset, x, y);
        break;
        case GB_ROM:
        return rom_gb_getPixel(self, offset, x, y);
        break;
        case SMS_ROM:
        return rom_sms_getPixel(self, offset, x, y);
        break;
    }
    return 0;
}

static void rom_nes_putPixel(rom* self, size_t offset, size_t x, size_t y, uint8_t color){
    if(self == NULL) return;
    if(offset*16 > self->size-16) return;
    uint8_t j = 1<<x;
    uint8_t* color_arg1 = self->rom_buffer+y+offset*16;
    uint8_t* color_arg2 = self->rom_buffer+y+offset*16+8;
    {
        int new_j = 255-j;
        *color_arg1 = (*color_arg1)&new_j;
        *color_arg2 = (*color_arg2)&new_j;
    }
    if((color&1) == 1) *color_arg2 = (*color_arg2)|j;
    if((color&2) == 2) *color_arg1 = (*color_arg1)|j;
}

static void rom_gb_putPixel(rom* self, size_t offset, size_t x, size_t y, uint8_t color){
    if(self == NULL) return;
    if(offset*16 > self->size-16) return;
    uint8_t j = 1<<x;
    uint8_t* color_arg1 = self->rom_buffer+y*2+offset*16;
    uint8_t* color_arg2 = self->rom_buffer+y*2+offset*16+1;
    {
        int new_j = 255-j;
        *color_arg1 = (*color_arg1)&new_j;
        *color_arg2 = (*color_arg2)&new_j;
    }
    if((color&1) == 1) *color_arg2 = (*color_arg2)|j;
    if((color&2) == 2) *color_arg1 = (*color_arg1)|j;
}

static void rom_sms_putPixel(rom* self, size_t offset, size_t x, size_t y, uint8_t color){
    if(self == NULL) return;
    if(offset*32 > self->size-32) return;
    uint8_t j = 1<<x;
    uint8_t* color_arg1 = self->rom_buffer+y*4+offset*32;
    uint8_t* color_arg2 = self->rom_buffer+y*4+offset*32+1;
    uint8_t* color_arg3 = self->rom_buffer+y*4+offset*32+2;
    uint8_t* color_arg4 = self->rom_buffer+y*4+offset*32+3;
    {
        int new_j = 255-j;
        *color_arg1 = (*color_arg1)&new_j;
        *color_arg2 = (*color_arg2)&new_j;
        *color_arg3 = (*color_arg3)&new_j;
        *color_arg4 = (*color_arg4)&new_j;
    }
    if((color&1) == 1) *color_arg1 = (*color_arg1)|j;
    if((color&2) == 2) *color_arg2 = (*color_arg2)|j;
    if((color&4) == 4) *color_arg3 = (*color_arg3)|j;
    if((color&8) == 8) *color_arg4 = (*color_arg4)|j;
}

void rom_putPixel(rom* self, size_t offset, size_t x, size_t y, uint8_t color){
    if(self == NULL) return;
    switch(self->type){
        case NES_ROM:
        rom_nes_putPixel(self, offset, x, y, color);
        break;
        case GB_ROM:
        rom_gb_putPixel(self, offset, x, y, color);
        break;
        case SMS_ROM:
        rom_sms_putPixel(self, offset, x, y, color);
        break;
    }
}

void rom_save(const rom* self){
    FILE* file = fopen(self->rom_name, "wb");
    fwrite(self->rom_buffer, self->size, 1, file);
    fclose(file);
}

void rom_destroy(rom* self){
    if(self == NULL) return;
    free(self->rom_buffer);
    free(self);
}
