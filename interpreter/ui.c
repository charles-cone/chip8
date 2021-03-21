//
// Created by Charlie Cone on 2/21/21.
//

#include "ui.h"

struct emu_ui* initUI(uint32_t* disp, uint16_t* buttons) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[ERROR] unable to init sdl2\n");
        return NULL;
    }

    struct emu_ui* ui = (struct emu_ui*)malloc(sizeof(struct emu_ui));

    ui->is_open = 1;
    ui->display_buffer = disp;
    ui->buttons = buttons;

    ui->emu_window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      WINDOW_X_RES, WINDOW_Y_RES, SDL_WINDOW_SHOWN);

    ui->emu_renderer = SDL_CreateRenderer(ui->emu_window, USE_DEFAULT,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return ui;
}

void tickUI(struct emu_ui* ui) {
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                ui->is_open = 0;
                break;
        }
    }
    static const int key_bindings[KEY_COUNT] = {
            SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
            SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
            SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
            SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_X, SDL_SCANCODE_C
    };

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    uint16_t keypad = 0;
    for(int i = 0; i < KEY_COUNT; i++) {
        if(state[key_bindings[i]]) {
            keypad |= 1 << i;
        }
    }
    *ui->buttons = keypad;
}

void drawDisplayUI(struct emu_ui* ui) {
    SDL_SetRenderDrawColor(ui->emu_renderer, 0, 0, 0, 0);
    SDL_RenderClear(ui->emu_renderer);
    SDL_SetRenderDrawColor(ui->emu_renderer, 255, 255, 255, 255);

    for(int i = 0; i < DISP_Y; i++) {
        for(int j = 0; j < DISP_X; j++) {
            uint32_t screen_line = ui->display_buffer[i*2 + (j / BITS_PER_LINE)];
            if(screen_line & 1 << (j % BITS_PER_LINE)) {
                SDL_Rect r = {BLOCK_X_RES*j, BLOCK_Y_RES*i, BLOCK_X_RES, BLOCK_Y_RES};
                SDL_RenderFillRect(ui->emu_renderer, &r);
            }
        }
    }

    SDL_RenderPresent(ui->emu_renderer);
}

void closeUI(struct emu_ui* ui) {
    SDL_DestroyWindow(ui->emu_window);
    SDL_DestroyRenderer(ui->emu_renderer);
    SDL_Quit();
    free(ui);
}
