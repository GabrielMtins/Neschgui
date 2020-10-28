#include "editor.h"
#include <stdlib.h>
#include <stdio.h>
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
*/

#define SPRITE_SURFACE_WIDTH 128
#define SPRITE_SURFACE_HEIGHT 128
int NUM_COLS = (SPRITE_SURFACE_WIDTH/8);
int NUM_LINES = (SPRITE_SURFACE_HEIGHT/8);

editor* editor_create(){
    editor* self = malloc(sizeof(editor));
    self->offset_tiles = 0;
    self->sprite_surface = SDL_CreateRGBSurface(0, SPRITE_SURFACE_WIDTH, SPRITE_SURFACE_HEIGHT, 32, 0, 0, 0, 0);
    self->main_rom = NULL;
    self->current_color = 0;
    {
        SDL_Color new_color = {0, 0, 0, 255};
        self->palette[0] = new_color;
    }
    {
        SDL_Color new_color = {255, 200, 150, 255};
        self->palette[1] = new_color;
    }
    {
        SDL_Color new_color = {255, 0, 0, 255};
        self->palette[2] = new_color;
    }
    {
        SDL_Color new_color = {150, 120, 0, 255};
        self->palette[3] = new_color;
    }
    self->sheet_widget = widget_create(
        WINDOW_HEIGHT/32, 0, WINDOW_HEIGHT, WINDOW_HEIGHT
    );
    self->draw_widget = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/32, 0, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32
    );
    self->palette_widget = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/32, WINDOW_WIDTH-WINDOW_HEIGHT-WINDOW_HEIGHT/32, WINDOW_HEIGHT/4, WINDOW_HEIGHT/16
    );
    self->slider_sheet = widget_create(
        0, 0, WINDOW_HEIGHT/32, WINDOW_HEIGHT
    );
    self->slider_r = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    self->slider_g = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6+WINDOW_HEIGHT/16,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    self->slider_b = widget_create(
        WINDOW_HEIGHT+WINDOW_HEIGHT/3,
        WINDOW_HEIGHT-WINDOW_HEIGHT/32*6+WINDOW_HEIGHT/8,
        (WINDOW_WIDTH-WINDOW_HEIGHT)/2,
        WINDOW_HEIGHT/32
    );
    return self;
}

static void editor_inputMouse(editor* self, int x, int y){
    if(widget_isMouseInsideWidget(self->sheet_widget, x, y)){
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
        double new_x = 0, new_y = 0;
        widget_getMouseRelativeToWidget(self->draw_widget, x, y, &new_x, &new_y);
        int i = new_x*8;
        int j = new_y*8;
        size_t offset = self->current_sprite_x/8+(self->current_sprite_y/8+self->offset_tiles)*NUM_COLS;
        rom_putPixel(self->main_rom, offset, 7-i, j, self->current_color);
    }
    if(widget_isMouseInsideWidget(self->palette_widget, x, y)){
        double new_x = 0;
        widget_getMouseRelativeToWidget(self->palette_widget, x, y, &new_x, NULL);
        new_x = new_x*4;
        self->current_color = new_x;
    }
    if(widget_isMouseInsideWidget(self->slider_r, x, y)){
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
        int max_offset = self->main_rom->size/NUM_COLS+1;
        new_y *= max_offset;
        new_y = (int)new_y/NUM_COLS;
        self->offset_tiles = (int)(new_y);
    }
}

void editor_input(editor* self, SDL_Event* event){
    if(event == NULL) return;
    if(event->type == SDL_KEYDOWN){
        switch(event->key.keysym.sym){
            case SDLK_DOWN:
            self->offset_tiles+=16;
            break;
            case SDLK_UP:
            if(self->offset_tiles >= 16) self->offset_tiles-=16;
            break;
            case SDLK_s:
            rom_save(self->main_rom);
            SDL_ShowSimpleMessageBox(0, "Save file", "Save file", window);
            break;
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
    if(event->type == SDL_MOUSEWHEEL){
        if(event->wheel.y > 0){
            if(self->offset_tiles >= 3) self->offset_tiles-=3;
        }
        if(event->wheel.y < 0){
            self->offset_tiles+=3;
        }
    }

    if(self->main_rom != NULL && self->offset_tiles >= self->main_rom->size/(NUM_COLS*NUM_COLS)-4){
        self->offset_tiles = self->main_rom->size/(NUM_COLS*NUM_COLS)-4;
    }

    int x, y;
    if(SDL_GetMouseState(&x, &y)&SDL_BUTTON(SDL_BUTTON_LEFT)){
        getMousePositionRelative(&x, &y);
        editor_inputMouse(self, x, y);
    }
}

void editor_loadRom(editor* self, const char* filename){
    if(self == NULL) return;
    if(self->main_rom != NULL){
        editor_freeRom(self);
    }
    self->main_rom = rom_load(filename);
}

void editor_freeRom(editor* self){
    if(self == NULL) return;
    if(self->main_rom == NULL) return;
    rom_destroy(self->main_rom);
    self->main_rom = NULL;
}

void editor_drawTilesetToSurface(editor* self){
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < NUM_COLS+1; i++){
        int tile_size = WINDOW_HEIGHT/NUM_COLS;
        SDL_RenderDrawLine(renderer, i*tile_size+self->sheet_widget->x, 0, i*tile_size+self->sheet_widget->x, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, self->sheet_widget->x, i*tile_size, WINDOW_HEIGHT+self->sheet_widget->x, i*tile_size);
    }
    SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT-1, WINDOW_HEIGHT, WINDOW_HEIGHT-1);
}

static void editor_drawSliders(const editor* self){
    SDL_Color bg = {
        0, 0, 0, 255
    };
    SDL_Color fg = {
        255, 0, 0, 255
    };
    widget_renderSlide(self->slider_r, self->palette[self->current_color].r, bg, fg);
    fg.r = 0; fg.g = 255;
    widget_renderSlide(self->slider_g, self->palette[self->current_color].g, bg, fg);
    fg.g = 0; fg.b = 255;
    widget_renderSlide(self->slider_b, self->palette[self->current_color].b, bg, fg);
    if(self->main_rom != NULL){
        bg.r = 20; bg.g = 20; bg.b = 20;
        fg.r = 255;
        fg.g = 255;
        double percentage = (double)255*self->offset_tiles/self->main_rom->size*NUM_COLS*NUM_COLS;
        widget_renderSlide(self->slider_sheet, (uint8_t)percentage, bg, fg);
    }
}

void editor_render(const editor* self){
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
        SDL_Rect rect_color = {self->palette_widget->x, self->palette_widget->y, WINDOW_HEIGHT/16, WINDOW_HEIGHT/16};
        for(int i = 0; i < 4; i++){
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
