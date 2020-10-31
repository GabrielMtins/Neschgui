#include "util.h"

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


#include <stdio.h>
#include <stdint.h>

void editor_updateTitle(uint8_t saved){
    char new_title[256];
    strcpy(new_title, window_title);
    if(!saved){
        strcat(new_title, "*");
    }
    SDL_SetWindowTitle(window, new_title);
}

void editor_inputHandleCtrlC(editor* self){
    char str_to_copy[65];
    str_to_copy[64] = '\0';
    // we copy the colors of the current tile to a string and set as the clipboard
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            size_t offset = self->current_sprite_x/8+(self->current_sprite_y/8+self->offset_tiles)*NUM_COLS;
            str_to_copy[j*8+i] = '0'+rom_getPixel(self->main_rom, offset, 7-i, j);
        }
    }
    SDL_SetClipboardText(str_to_copy);
}

void editor_inputHandleCtrlV(editor* self){
    char str_to_paste[65] = "";
    strcpy(str_to_paste, SDL_GetClipboardText());
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            size_t offset = self->current_sprite_x/8+(self->current_sprite_y/8+self->offset_tiles)*NUM_COLS;
            if(i == 0 && j == 0){
                undo_stack_push(&self->main_stack, self->main_rom, offset, 7-i, j, UNDO_TYPE_PUT_PIXEL); // save action
            }
            else undo_stack_push(&self->main_stack, self->main_rom, offset, 7-i, j, UNDO_TYPE_PASTE); // save action
            rom_putPixel(self->main_rom, offset, 7-i, j, str_to_paste[j*8+i]-48);
        }
    }
    editor_updateTitle(0);
}

void editor_swapTiles(editor* self, int dir_x, int dir_y){
    if(self->current_sprite_x == 0 && dir_x < 0) return;
    if(self->current_sprite_y == 0 && dir_y < 0) return;
    if(self->current_sprite_x == 7 && dir_x > 0) return;
    if(self->current_sprite_y == 7 && dir_y > 0) return;
    size_t offset = self->current_sprite_x/8+(self->current_sprite_y/8+self->offset_tiles)*NUM_COLS;
    size_t offset_swap = self->current_sprite_x/8+dir_x+(self->current_sprite_y/8+self->offset_tiles+dir_y)*NUM_COLS;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(i == 0 && j == 0){
                undo_stack_push(&self->main_stack, self->main_rom, offset, 7-i, j, UNDO_TYPE_PUT_PIXEL); // save action
                undo_stack_push(&self->main_stack, self->main_rom, offset_swap, 7-i, j, UNDO_TYPE_PASTE); // save action
            }
            else{
                undo_stack_push(&self->main_stack, self->main_rom, offset, 7-i, j, UNDO_TYPE_PASTE); // save action
                undo_stack_push(&self->main_stack, self->main_rom, offset_swap, 7-i, j, UNDO_TYPE_PASTE); // save action
            }
            uint8_t og_pixel = rom_getPixel(self->main_rom, offset, 7-i, j);
            uint8_t swap_pixel = rom_getPixel(self->main_rom, offset_swap, 7-i, j);
            rom_putPixel(self->main_rom, offset, 7-i, j, swap_pixel);
            rom_putPixel(self->main_rom, offset_swap, 7-i, j, og_pixel);
        }
    }
    self->current_sprite_x+=dir_x*8;
    self->current_sprite_y+=dir_y*8;
}
