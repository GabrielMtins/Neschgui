#include "widget.h"

#include <stdio.h>
#include <stdlib.h>

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

widget* widget_create(int x, int y, int w, int h){
    widget* self = malloc(sizeof(widget));
    self->x = x;
    self->y = y;
    self->w = w;
    self->h = h;
    return self;
}

void widget_render(const widget* self, SDL_Texture* texture){
    if(self == NULL || texture == NULL) return;
    SDL_Rect dest = {self->x, self->y, self->w, self->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void getMousePositionRelative(int* x, int* y){
    if(x == NULL || y == NULL) return;
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    int x_mouse = *x;
    int y_mouse = *y;
    if(width*9 != height*16){ // check if the screen is 16:9
        // if its not, we calculate the value of the border
        int border;
        int new_width = height*16/9;
        border = new_width-width;
        x_mouse += border/2;
        width = new_width;
    }
    double new_x = (double)(x_mouse)/width;
    double new_y = (double)(y_mouse)/height;
    new_x*=WINDOW_WIDTH;
    new_y*=WINDOW_HEIGHT;
    *x = (int)new_x;
    *y = (int)new_y;
}

uint8_t widget_isMouseInsideWidget(const widget* self, int x, int y){
    if(self == NULL) return 0;
    {
        if(x < self->x || x >= self->x+self->w) return 0;
        if(y < self->y || y >= self->y+self->h) return 0;
        return 1;
    }
    return 0;
}

void widget_getMouseRelativeToWidget(const widget* self, int mouse_x, int mouse_y, double* x, double* y){
    if(self == NULL) return;
    double new_x = mouse_x-self->x;
    double new_y = mouse_y-self->y;
    new_x = new_x/self->w; // normalize coordinates
    new_y = new_y/self->h;
    if(x != NULL) *x = new_x;
    if(y != NULL) *y = new_y;
}

void widget_renderSlide(const widget* self, uint8_t percentage, SDL_Color bg, SDL_Color fg){
    if(self == NULL) return;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
    SDL_Rect widget_rect = {self->x, self->y, self->w, self->h};
    SDL_RenderFillRect(renderer, &widget_rect);
    if(self->w > self->h){ // multiply 15/16 to subtract the width/height of the slider
        widget_rect.x+=widget_rect.w*15/16*percentage/255;
        widget_rect.w/=16;
    }
    else{
        widget_rect.y+=widget_rect.h*15/16*percentage/255;
        widget_rect.h/=16;
    }
    SDL_SetRenderDrawColor(renderer, fg.r, fg.g, fg.b, 255);
    SDL_RenderFillRect(renderer, &widget_rect);
}

void widget_destroy(widget* self){
    if(self == NULL) return;
    free(self);
}
