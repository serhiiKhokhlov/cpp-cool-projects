//       $Id: tetris.c 47850 2023-11-22 18:48:28Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/tetris.c $
// $Revision: 47850 $
//     $Date: 2023-11-22 19:48:28 +0100 (Mi., 22 Nov 2023) $
//   $Author: p20068 $
//   Creator: Michael Hava
//  Creation: October 24, 2023
// Copyright: (c) 2023 Michael Hava (michael.hava<AT>risc-software.at)
//            (c) 2023 Peter Kulczycki (peter.kulczycki<AT>fh-hagenberg.at)
//   License: This document contains proprietary information belonging to
//            University of Applied Sciences Upper Austria, Campus Hagenberg.
//            It is distributed under the Boost Software License (see
//            https://www.boost.org/users/license.html).

#include "./board.h"
#include "./timer.h"
#include "./window.h"
#include <math.h>
#include <crtdbg.h>

// -------------------------------------------------------------------------------------------------

static Block g_current_block = NULL;
static Tetrimino g_current_tetrimino = NULL;
static bool       g_game_over     = false;
static double level = 0;
const int scaling = 10;
int rainbow_pallete_iter = 0;
color_type cur_color;

#define RAINBOW_SIZE 22
const color_type rainbow_array[RAINBOW_SIZE] = { color_red,
                                       color_orange,
                                       color_yellow,
                                       color_lime,
                                       color_green,
                                       color_teal,
                                       color_cyan,
                                       color_skyblue,
                                       color_blue,
                                       color_purple,
                                       color_violet,
                                       color_magenta,
                                       color_violet,
                                       color_purple,
                                       color_blue,
                                       color_skyblue,
                                       color_cyan,
                                       color_teal,
                                       color_green,
                                       color_lime,
                                       color_yellow,
                                       color_orange };

// -------------------------------------------------------------------------------------------------

static void handle_key_event (key_type key, action_type action);

// -------------------------------------------------------------------------------------------------

int rand_for_range(int min, int max) {
    srand((unsigned int)time(NULL));
    return  rand() % (max - min + 1) + min;
}

void spawn(color_type color, tetrimino_type_enum type) {
    
    g_current_block = new_block(make_position(N_COLS / 2, N_ROWS - 1), color);
    g_current_tetrimino = new_tetrimino(g_current_block, type);
}

void on_key (key_type const key, action_type const action) {   // ToDo
    if (g_game_over)
        return;

    printf("key event: %s\n", string_from_key(key, action));
    handle_key_event(key, action);
}

void on_paint (void) {   // ToDo
    if (g_game_over) 
        return;
    render_tetrimino(g_current_tetrimino);
    render_board();
}

void on_timer (double const elapsed) {   // ToDo
    if (g_game_over)
        return;

    handle_key_event(key_down, action_press);
    printf("paint event: repaint\n");
    printf("timer event: %s elapsed\n", string_from_duration(elapsed));
}

void tetrimino_take_place(Tetrimino t) {
    for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
        position_type pos = relative_pos_to_absolute(t->relative_blocks_pos[i], t->pivot_pt->position);
        if (is_valid_position(pos))
            take_place(pos);
    }
}

void decrease_interval() {
    set_interval(*get_interval() - (double)scaling * log(++level));
}

void check_clear_row() {
    int i = 0;
    for (; i <= get_max_row() && is_free_row(i); ++i) {}
    if (i <= get_max_row()) {
        clear_row(i);
        check_clear_row();
        decrease_interval();
    }
}

void freeze() {
    tetrimino_take_place(g_current_tetrimino);
    check_clear_row();
    free_tetrimino(&g_current_tetrimino);
    g_current_block = NULL;
    spawn(cur_color, rand_for_range(I, Z));
}

color_type next_rainbow_color() {
    return (rainbow_array[(rainbow_pallete_iter++ % RAINBOW_SIZE)]);
}

// -------------------------------------------------------------------------------------------------

static void handle_key_event (key_type const key, action_type const action) {   // ToDo
    if (key == key_esc)
        g_game_over = true;
    
    if (!g_game_over && (key != key_unknown) && (action == action_press || action == action_repeat)) {
        int dx = 0;
        int dy = 0;
        bool rotated = false;

        switch (key) {
            case key_down: dy = -1; break;
            case key_left: dx = -1; break;
            case key_right: dx = 1; break;
            case key_r: rotated = true; break;
            //case key_up: dy = 1; break;
        }

        position_type position = move_position(g_current_block->position, dx, dy);
        position_type prev_position = g_current_block->position;

        rotations_enum prev_rotation = g_current_tetrimino->rotation;

        if (rotated) {
            rotate_tetrimino(g_current_tetrimino);

            if (bottom_collides(g_current_tetrimino->hitbox) ||
                side_collides(g_current_tetrimino->hitbox) ||
                used_space_collides(g_current_tetrimino->hitbox))
                set_rotation(g_current_tetrimino, prev_rotation);
        }

        g_current_block->position = position;

        if (side_collides(g_current_tetrimino->hitbox) ||
            (used_space_collides(g_current_tetrimino->hitbox) && (position.x != prev_position.x && position.y == position.y)))
            g_current_block->position.x = prev_position.x;

        if (bottom_collides(g_current_tetrimino->hitbox) ||
            (used_space_collides(g_current_tetrimino->hitbox) && (position.y != prev_position.y))) {
            g_current_block->position.y = prev_position.y;
            freeze();
        }
    }
}

// -------------------------------------------------------------------------------------------------

int main (void) {
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   initialize_renderer (N_ROWS, N_COLS, on_key);
   initialize_timer (on_timer);
   initialize_board ();

   cur_color = next_rainbow_color();

   spawn(cur_color, rand_for_range(I, Z));

   do {   // the message pump
      drive_timer ();

      begin_frame ();
      on_paint ();
      cur_color = next_rainbow_color();
      set_color(g_current_tetrimino, cur_color);    
      end_frame ();

      if (!renderer_is_open () || reached_top())
         g_game_over = true;
   } while (!g_game_over);

   clear_board();
   printf ("game over\n"); shut_down_renderer();
}
