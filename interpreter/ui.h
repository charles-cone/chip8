//
// Created by Charlie Cone on 2/21/21.
//

#ifndef CHIP8_UI_H
#define CHIP8_UI_H

#include "cpu.h"
#include "stdio.h"
#include "stdint.h"
#include "SDL.h"

#define WINDOW_X_RES 640
#define WINDOW_Y_RES 480

#define BLOCK_X_RES (WINDOW_X_RES / 64)
#define BLOCK_Y_RES (WINDOW_Y_RES / 32)

#define WINDOW_NAME "chip-8"

#define KEY_COUNT 16

#define USE_DEFAULT (-1)

struct emu_ui {
    // Rendering
    SDL_Window* emu_window;
    SDL_Renderer* emu_renderer;

    // System Control
    uint32_t is_open;

    // Emulator
    uint32_t* display_buffer;
    uint16_t* buttons;
};

struct emu_ui * initUI(uint32_t* disp, uint16_t* buttons);

void closeUI(struct emu_ui* ui);
void tickUI(struct emu_ui* ui);
void drawDisplayUI(struct emu_ui* ui);

#endif //CHIP8_UI_H
