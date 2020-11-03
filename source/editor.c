#include "editor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

#ifndef __MINGW32__
static void editor_loadDefaultConfig(editor* self){
    char filename[256] = "";
    strcpy(filename, getenv("HOME"));
    strcat(filename, "/.neschguicfg");
    FILE* file = fopen(filename, "r");
    if(file == NULL) return;
    char current_table[16] = "";
    while(fscanf(file, "%s", current_table) == 1){
        if(!strcmp(current_table, "default")){
            break;
        }
        if(!strcmp(current_table, "palette:")){
            for(int i = 0; i < 4; i++){
                int r, g, b;
                fscanf(file, "%i %i %i", &r, &g, &b);
                if(r < 256 && g < 256 && b < 256){
                    SDL_Color new_color = {r, g, b, 255};
                    self->palette[i] = new_color;
                }
            }
        }
    }
    fclose(file);
}
#endif

editor* editor_create(){
    editor* self = malloc(sizeof(editor));
    self->offset_tiles = 0;
    self->sprite_surface = SDL_CreateRGBSurface(0, SPRITE_SURFACE_WIDTH, SPRITE_SURFACE_HEIGHT, 32, 0, 0, 0, 0);
    self->main_rom = NULL;
    self->current_color = 0;
    {
        int index = 0;
        for(int r = 0; r < 2; r++){
            for(int g = 0; g < 2; g++){
                for(int b = 0; b < 2; b++){
                    SDL_Color new_color = {(r)*255, (g)*255, (b)*255, 255};
                    self->palette[index] = new_color;
                    index++;
                }
            }
        }
    }
    for(int i = 0; i < 8; i++){
        self->palette[i+8].r = self->palette[i].r/2;
        self->palette[i+8].g = self->palette[i].g/2;
        self->palette[i+8].b = self->palette[i].b/2;
    }
    self->palette[8].r = 100;
    self->palette[8].g = 100;
    self->palette[8].b = 100;
    #ifndef __MINGW32__
        editor_loadDefaultConfig(self);
    #endif
    self->sheet_widget = widget_create(
        WINDOW_HEIGHT/32, 0, WINDOW_HEIGHT, WINDOW_HEIGHT
    );
    self->draw_widget = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/32, 0, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32
    );
    self->palette_widget = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/32, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32, WINDOW_HEIGHT/16
    );
    self->slider_sheet = widget_create(
        0, 0, WINDOW_HEIGHT/32, WINDOW_HEIGHT
    );
    self->slider_r = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6+10,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    self->slider_g = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6+WINDOW_HEIGHT/16+10,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    self->slider_b = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6+WINDOW_HEIGHT/8+10,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    self->main_stack.top = 0;
    self->main_rom = NULL;
    return self;
}

static void editor_inputMouse(editor* self, int x, int y){
    if(widget_isMouseInsideWidget(self->sheet_widget, x, y)){
        if(self->main_rom == NULL) return;
        /*
            the sheet is composed by NUM_COLS*NUM_LINES of sprites
            to get the pixel of the current sprite, we multiply the
            relative position of the widget by NUM_COLS and by NUM_LINES
            And then we get the current srpite
            to get the size in pixels, we multiply it by 8 (each sprite is 8x8)
        */
        double new_x = 0, new_y = 0;
        widget_getMouseRelativeToWidget(self->sheet_widget, x, y, &new_x, &new_y);
        new_x*=NUM_COLS;
        new_y*=NUM_LINES;
        self->current_sprite_x = new_x;
        self->current_sprite_y = new_y;
        self->current_sprite_x *= 8;
        self->current_sprite_y *= 8;
    }
    if(widget_isMouseInsideWidget(self->draw_widget, x, y)){
        if(self->main_rom == NULL) return;
        double new_x = 0, new_y = 0;
        widget_getMouseRelativeToWidget(self->draw_widget, x, y, &new_x, &new_y);
        // we get the relative mouse position to the widget and then
        // multiply by 8 to get the current pixel of the draw_widget
        int i = new_x*8;
        int j = new_y*8;
        size_t offset = self->current_sprite_x/8+(self->current_sprite_y/8+self->offset_tiles)*NUM_COLS;
        undo_stack_push(&self->main_stack, self->main_rom, offset, 7-i, j, UNDO_TYPE_PUT_PIXEL); // save action
        rom_putPixel(self->main_rom, offset, 7-i, j, self->current_color);
        // we changed the rom, so we put an * at the end of the title
        editor_updateTitle(0);

    }
    if(widget_isMouseInsideWidget(self->palette_widget, x, y)){
        double new_x = 0;
        widget_getMouseRelativeToWidget(self->palette_widget, x, y, &new_x, NULL);
        // multiply by 4 to get one of the 4 colors available
        new_x = new_x*(self->main_rom->num_of_colors);
        self->current_color = new_x;
    }
    if(widget_isMouseInsideWidget(self->slider_r, x, y)){
        // on all sliders, we just multiply the color by the relative position
        // of the x value
        double new_x = 0;
        widget_getMouseRelativeToWidget(self->slider_r, x, y, &new_x, NULL);
        self->palette[self->current_color].r = new_x*255;
    }
    if(widget_isMouseInsideWidget(self->slider_g, x, y)){
        double new_x = 0;
        widget_getMouseRelativeToWidget(self->slider_g, x, y, &new_x, NULL);
        self->palette[self->current_color].g = new_x*255;
    }
    if(widget_isMouseInsideWidget(self->slider_b, x, y)){
        double new_x = 0;
        widget_getMouseRelativeToWidget(self->slider_b, x, y, &new_x, NULL);
        self->palette[self->current_color].b = new_x*255;
    }
    if(widget_isMouseInsideWidget(self->slider_sheet, x, y)){
        double new_y = 0;
        widget_getMouseRelativeToWidget(self->slider_sheet, x, y, NULL, &new_y);
        int max_offset = self->main_rom->size/self->main_rom->bytes_per_sprite/NUM_LINES; // each sprite takes 16 bytes
        new_y = max_offset*new_y;
        new_y = (int)new_y;
        // to create a new offset, we get the original size and define a max offset
        // after that we multiply the relative position of mouse_y and get the correct
        // offset
        self->offset_tiles = (int)(new_y);
    }
}

static void editor_arrowKeysInput(editor* self, SDL_Event* event){
    IF_ERROR(self);
    IF_ERROR(event);
    switch(event->key.keysym.sym){
        case SDLK_LEFT:
        if(SDL_GetModState()&KMOD_SHIFT) editor_swapTiles(self, -1, 0);
        break;
        case SDLK_RIGHT:
        if(SDL_GetModState()&KMOD_SHIFT) editor_swapTiles(self, 1, 0);
        break;
        case SDLK_DOWN:
        if(SDL_GetModState()&KMOD_SHIFT) editor_swapTiles(self, 0, 1);
        else self->offset_tiles+=16;
        break;
        case SDLK_UP:
        if(SDL_GetModState()&KMOD_SHIFT) editor_swapTiles(self, 0, -1);
        else if(self->offset_tiles >= 16) self->offset_tiles-=16;
        break;
    }
}

static void editor_keysInput(editor* self, SDL_Event* event){
    IF_ERROR(self);
    IF_ERROR(event);
    switch(event->key.keysym.sym){
        case SDLK_s:
        if(SDL_GetModState()&KMOD_CTRL){
            rom_save(self->main_rom);
            editor_updateTitle(1);
        }
        break;
        case SDLK_c:
        if(SDL_GetModState()&KMOD_CTRL) editor_inputHandleCtrlC(self);
        break;
        case SDLK_v:
        if(SDL_GetModState()&KMOD_CTRL) editor_inputHandleCtrlV(self);
        break;
        case SDLK_z: // handle ctrl + z
        if(SDL_GetModState()&KMOD_CTRL){
            if(self->main_stack.top > 0) editor_updateTitle(0);
            undo_stack_pop(&self->main_stack, self->main_rom);
        }
        break;
        case SDLK_r: // handle ctrl + r
        if(SDL_GetModState()&KMOD_CTRL){
            editor_rotateTile(self);
        }
        break;
        case SDLK_i: // handle ctrl + i
        if(SDL_GetModState()&KMOD_CTRL){
            editor_invertTile(self);
        }
        break;
    }
}

static void editor_colorInput(editor* self, SDL_Event* event){
    IF_ERROR(self);
    IF_ERROR(event);
    switch(event->key.keysym.sym){
        case SDLK_1:
        self->current_color = 0;
        break;
        case SDLK_2:
        self->current_color = 1;
        break;
        case SDLK_3:
        self->current_color = 2;
        break;
        case SDLK_4:
        self->current_color = 3;
        break;
    }
}

void editor_input(editor* self, SDL_Event* event){
    IF_ERROR(self);
    IF_ERROR(event);
    if(event->type == SDL_KEYDOWN){
        editor_arrowKeysInput(self, event);
        editor_keysInput(self, event);
        editor_colorInput(self, event);
    }
    if(event->type == SDL_MOUSEWHEEL){
        if(event->wheel.y > 0){
            if(self->offset_tiles >= 3) self->offset_tiles-=3;
        }
        if(event->wheel.y < 0){
            self->offset_tiles+=3;
        }
    }
    if(self->main_rom != NULL && self->offset_tiles >= self->main_rom->size/(self->main_rom->bytes_per_sprite*NUM_COLS)-4){
        // test if we are outside the bounds of the file
        self->offset_tiles = self->main_rom->size/(self->main_rom->bytes_per_sprite*NUM_COLS)-4;
    }
    int x, y;
    if(SDL_GetMouseState(&x, &y)&SDL_BUTTON(SDL_BUTTON_LEFT)){
        getMousePositionRelative(&x, &y);
        editor_inputMouse(self, x, y);
    }
    editor_handleDraggedFile(self, event);
}

void editor_loadRom(editor* self, const char* filename){
    IF_ERROR(self);
    if(self->main_rom != NULL){
        editor_freeRom(self);
    }
    if(filename == NULL) self->main_rom = rom_loadEmptyRom();
    else self->main_rom = rom_load(filename);
}

void editor_freeRom(editor* self){
    IF_ERROR(self);
    IF_ERROR(self->main_rom);
    rom_destroy(self->main_rom);
    self->main_rom = NULL;
}

void editor_drawTilesetToSurface(editor* self){
    IF_ERROR(self);
    IF_ERROR(self->main_rom);
    for(int i = 0; i < SPRITE_SURFACE_HEIGHT; i++){
        for(int j = 0; j < SPRITE_SURFACE_WIDTH; j++){
            size_t offset = i/8+(j/8+self->offset_tiles)*NUM_COLS;
            int color = rom_getPixel(self->main_rom, offset, 7-i%8, j%8);
            uint8_t r = 0, g = 0, b = 0;
            r = self->palette[color].r;
            g = self->palette[color].g;
            b = self->palette[color].b;
            uint32_t* pixels = self->sprite_surface->pixels;
            pixels[i+j*SPRITE_SURFACE_WIDTH] = SDL_MapRGB(self->sprite_surface->format, r, g, b);
        }
    }
}

static void editor_drawLines(const editor* self){
    IF_ERROR(self);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < NUM_COLS+1; i++){ // these are the lines that divide the sprite sheet
        int tile_size = WINDOW_HEIGHT/NUM_COLS;
        SDL_RenderDrawLine(renderer, i*tile_size+self->sheet_widget->x, 0, i*tile_size+self->sheet_widget->x, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, self->sheet_widget->x, i*tile_size, WINDOW_HEIGHT+self->sheet_widget->x, i*tile_size);
    }
    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    {
        double x = WINDOW_HEIGHT+WINDOW_HEIGHT/32;
        double y = 0;
        double offset_pixel = (double)(WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32)/8;
        double width = (WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32);
        for(int i = 0; i < 9; i++){
            SDL_RenderDrawLine(renderer, x, 0, x, width);
            SDL_RenderDrawLine(renderer, WINDOW_HEIGHT+WINDOW_HEIGHT/32, y, WINDOW_WIDTH, y);
            x+=offset_pixel;
            y+=offset_pixel;
        }
    }
    SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT-1, WINDOW_HEIGHT, WINDOW_HEIGHT-1);
}

static void editor_drawSliders(const editor* self){
    IF_ERROR(self);
    SDL_Color bg = {
        0, 0, 0, 255
    };
    SDL_Color fg = {
        255, 0, 0, 255
    }; // start color is red
    widget_renderSlide(self->slider_r, self->palette[self->current_color].r, bg, fg);
    fg.r = 0; fg.g = 255; // change color to green
    widget_renderSlide(self->slider_g, self->palette[self->current_color].g, bg, fg);
    fg.g = 0; fg.b = 255; // change color to blue
    widget_renderSlide(self->slider_b, self->palette[self->current_color].b, bg, fg);
    if(self->main_rom != NULL){
        bg.r = 20; bg.g = 20; bg.b = 20;
        fg.r = 255;
        fg.g = 255; // change color to white
        double percentage = (double)255*self->offset_tiles/self->main_rom->size*self->main_rom->bytes_per_sprite*NUM_COLS;
        widget_renderSlide(self->slider_sheet, (uint8_t)percentage, bg, fg);
    }
}

void editor_render(const editor* self){
    IF_ERROR(self);
    SDL_Texture* sheet_texture = SDL_CreateTextureFromSurface(renderer, self->sprite_surface);
    { // draw sheet
        widget_render(self->sheet_widget, sheet_texture);
    }
    { // draw current sprite
        SDL_Rect dst = {self->draw_widget->x, self->draw_widget->y,
            self->draw_widget->w, self->draw_widget->h};
        SDL_Rect src = {self->current_sprite_x, self->current_sprite_y, 8, 8};
        SDL_RenderCopy(renderer, sheet_texture, &src, &dst);
    }
    { // draw color palette
        int number_of_colors = self->main_rom->num_of_colors;
        SDL_Rect rect_color = {self->palette_widget->x, self->palette_widget->y, self->palette_widget->w/number_of_colors, self->palette_widget->h};
        for(int i = 0; i < number_of_colors; i++){
            SDL_SetRenderDrawColor(renderer, self->palette[i].r, self->palette[i].g, self->palette[i].b, 255);
            SDL_RenderFillRect(renderer, &rect_color);
            rect_color.x += rect_color.w;
        }
    }
    editor_drawLines(self);
    editor_drawSliders(self);
    SDL_DestroyTexture(sheet_texture);
}

void editor_destroy(editor* self){
    IF_ERROR(self);
    widget_destroy(self->draw_widget);
    widget_destroy(self->sheet_widget);
    widget_destroy(self->slider_r);
    widget_destroy(self->slider_g);
    widget_destroy(self->slider_b);
    widget_destroy(self->slider_sheet);
    widget_destroy(self->palette_widget);
    SDL_FreeSurface(self->sprite_surface);
    editor_freeRom(self);
    free(self);
}
