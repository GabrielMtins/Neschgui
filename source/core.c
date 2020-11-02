#include "core.h"
#include "global.h"
#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

editor* main_editor = NULL;

void core_init(int argc, char** argv){
    if(argc == 1){
        printf("No arguments!\n");
        exit(-1);
    }
    SDL_Init(SDL_INIT_VIDEO);
    main_editor = editor_create();
    editor_loadRom(main_editor, argv[1]);
    if(main_editor->main_rom == NULL){
        printf("Failed to read rom\n");
        editor_destroy(main_editor);
        SDL_Quit();
        exit(-1);
    }
    strcat(window_title, argv[1]);
    window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_ShowSimpleMessageBox(0, "About", "Neschgui  Copyright (C) 2020  Gabriel Martins\n \
    This program comes with ABSOLUTELY NO WARRANTY\n \
    This is free software, and you are welcome to redistribute it\n \
    under certain conditions. read COPYING for details",
    window);
}

void core_render(){
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    editor_drawTilesetToSurface(main_editor);
    editor_render(main_editor);
    SDL_RenderPresent(renderer);
}

void core_loop(){
    uint8_t quit = 0;
    SDL_Event event;
    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) quit = 1;
            else editor_input(main_editor, &event);
        }
        core_render();
    }
}

void core_quit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    editor_destroy(main_editor);
    SDL_Quit();
}

void core_run(int argc, char** argv){
    core_init(argc, argv);
    core_loop();
    core_quit();
}
