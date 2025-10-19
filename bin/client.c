#include "client.h"
#include "properties.h"
#include "raylib.h"
#include <fcntl.h>
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "images.h"
#include "game_state.h"
#include "button.h"
#include "server.h"
#include "types.h"
#include "vec2.h"

void restart_on_pressed(button_t *btn) {
    game_state_t *state = (game_state_t *)btn->param;

    client_message_t restart_msg = {
        .type = CLIENT_MSG_RESTART,
    };

    if (sr_send_message_to_server(&state->client, &restart_msg) != 0) {
        fprintf(stderr, "error: Failed to send restart request\n");
    }
}

void button_on_pressed(button_t *btn) {
    game_state_t *state = (game_state_t *)btn->param;

    float width =
        BOARD_SIDE * BOARD_BUTTON_SIZE + (BOARD_SIDE - 1) * BOARD_SPACING;
    float start_x = (float)SCREEN_WIDTH / 2 - width / 2;
    float start_y = (float)SCREEN_HEIGHT / 2 - width / 2;

    float relative_x = btn->pos.x - start_x;
    float relative_y = btn->pos.y - start_y;

    int x = (int)(relative_x / (BOARD_BUTTON_SIZE + BOARD_SPACING));
    int y = (int)(relative_y / (BOARD_BUTTON_SIZE + BOARD_SPACING));

    state->waiting_for_response = 0;

    if (x >= 0 && x < BOARD_SIDE && y >= 0 && y < BOARD_SIDE
        && state->cells[x][y] == CELL_EMPTY && !state->waiting_for_response) {
        client_message_t set_mark_msg = {
            .type = CLIENT_MSG_SET_MARK,
            .data.position = (vec2){ x, y },
        };

        if (sr_send_message_to_server(&state->client, &set_mark_msg) == 0) {
            state->pending_x = x;
            state->pending_y = y;
            state->waiting_for_response = 1;
        }
    }
}

void receive_server_message(game_state_t *state) {
    client_t        *client = &state->client;
    server_message_t msg;

    if (sr_receive_server_message(client, &msg) < 0) {
        return;
    }

    switch (msg.type) {
        case SERVER_MSG_RESTART: {
            state_init(state);
            break;
        }
        case SERVER_MSG_ALLOW: {
            if (state->waiting_for_response) {
                state->cells[state->pending_x][state->pending_y] = CELL_PLAYER;
                state->waiting_for_response = 0;
            }
            break;
        }
        case SERVER_MSG_MARK_SET: {
            int x = (int)msg.data.position.x;
            int y = (int)msg.data.position.y;
            state->cells[x][y] = CELL_ENEMY;
            break;
        }
        case SERVER_MSG_GAME_END: {
            state->is_finished = 1;
            break;
        }
        default: {
            fprintf(stderr, "error: Wrong message server type\n");
            break;
        }
    }
}

void init_buttons(button_t *btns, game_state_t *state, Texture2D *texture) {
    float width =
        BOARD_SIDE * BOARD_BUTTON_SIZE + (BOARD_SIDE - 1) * BOARD_SPACING;

    float start_x = (float)SCREEN_WIDTH / 2 - width / 2;
    float start_y = (float)SCREEN_HEIGHT / 2 - width / 2;

    for (int i = 0; i < BOARD_SIDE; i++) {
        for (int j = 0; j < BOARD_SIDE; j++) {
            button_t *btn = &btns[i + BOARD_SIDE * j];
            vec2      pos = { start_x + i * (BOARD_BUTTON_SIZE + BOARD_SPACING),
                              start_y + j * (BOARD_BUTTON_SIZE + BOARD_SPACING) };
            vec2      size = { BOARD_BUTTON_SIZE, BOARD_BUTTON_SIZE };

            button_init(
                btn,
                pos,
                size,
                INITIAL_COLOR,
                texture,
                (void *)state,
                button_on_pressed
            );
        }
    }
}

void update_buttons(button_t *btns, game_state_t *state) {
    for (int i = 0; i < BOARD_SIDE; i++) {
        for (int j = 0; j < BOARD_SIDE; j++) {
            button_t *btn = &btns[i + BOARD_SIDE * j];

            switch (state->cells[i][j]) {
                case CELL_EMPTY: {
                    btn->color = INITIAL_COLOR;
                    btn->texture = state->empty_texture;
                    break;
                }
                case CELL_PLAYER: {
                    btn->color = PLAYER_COLOR;
                    btn->texture = state->circle_texture;
                    break;
                }
                case CELL_ENEMY: {
                    btn->color = ENEMY_COLOR;
                    btn->texture = state->cross_texture;
                    break;
                }
            }
            button_draw(btn);
        }
    }
}

int main(void) {
    game_state_t state;

    state_init(&state);

    sr_client_connect(&state.client, SERVER_IP, SERVER_PORT);

#ifndef DEBUGGING
    SetTraceLogLevel(LOG_WARNING);
#endif

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TicTacToe");
    SetTargetFPS(FPS);

    Image circle_img = { .data = CIRCLE_IMAGE_DATA,
                         .width = CIRCLE_IMAGE_WIDTH,
                         .height = CIRCLE_IMAGE_HEIGHT,
                         .mipmaps = 1,
                         .format = CIRCLE_IMAGE_FORMAT };

    Image cross_img = { .data = CROSS_IMAGE_DATA,
                        .width = CROSS_IMAGE_WIDTH,
                        .height = CROSS_IMAGE_HEIGHT,
                        .mipmaps = 1,
                        .format = CROSS_IMAGE_FORMAT };

    Image empty_img = { .data = EMPTY_IMAGE_DATA,
                        .width = EMPTY_IMAGE_WIDTH,
                        .height = EMPTY_IMAGE_HEIGHT,
                        .mipmaps = 1,
                        .format = EMPTY_IMAGE_FORMAT };

    Texture2D circle_texture = LoadTextureFromImage(circle_img);
    Texture2D cross_texture = LoadTextureFromImage(cross_img);
    Texture2D empty_texture = LoadTextureFromImage(empty_img);

    SetTextureFilter(circle_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(cross_texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(empty_texture, TEXTURE_FILTER_POINT);

    state.circle_texture = &circle_texture;
    state.cross_texture = &cross_texture;
    state.empty_texture = &empty_texture;

    button_t *btns = malloc(BOARD_SIDE * BOARD_SIDE * sizeof(button_t));

    init_buttons(btns, &state, NULL);

    button_t *restart_btn = malloc(sizeof(button_t));

    button_init(
        restart_btn,
        (vec2){ (float)SCREEN_WIDTH / 2 - (float)RESTART_BUTTON_WIDTH / 2,
                (float)SCREEN_HEIGHT / 2 + RESTART_BUTTON_OFFSET_Y },
        (vec2){ RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT },
        WHITE,
        NULL,
        &state,
        restart_on_pressed
    );

    while (!WindowShouldClose()) {
        receive_server_message(&state);

        if (!state.is_finished) {
            for (int i = 0; i < BOARD_SIDE * BOARD_SIDE; i++) {
                button_check(&btns[i]);
            }
        }

        if (state.is_finished) {
            button_check(restart_btn);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (!state.is_finished) {
            update_buttons(btns, &state);
        }

        if (state.is_finished) {
            const char *text = "Game Finished!";
            int         text_width = MeasureText(text, FONT_SIZE);
            DrawText(
                text,
                SCREEN_WIDTH / 2 - text_width / 2,
                SCREEN_HEIGHT / 2,
                FONT_SIZE,
                WHITE
            );

            button_draw(restart_btn);

            const char *restart_text = "Restart";
            int         restart_text_width =
                MeasureText(restart_text, RESTART_BUTTON_TEXT_SIZE);
            DrawText(
                restart_text,
                SCREEN_WIDTH / 2 - restart_text_width / 2,
                SCREEN_HEIGHT / 2 + RESTART_BUTTON_OFFSET_Y
                    + (RESTART_BUTTON_HEIGHT - RESTART_BUTTON_TEXT_SIZE) / 2,
                RESTART_BUTTON_TEXT_SIZE,
                BLACK
            );
        }

        EndDrawing();
    }

    UnloadTexture(cross_texture);
    UnloadTexture(circle_texture);
    UnloadTexture(empty_texture);
    free(btns);
    free(restart_btn);
    CloseWindow();
    return 0;
}
