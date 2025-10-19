//       $Id: primitives.h 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/primitives.h $
// $Revision: 47849 $
//     $Date: 2023-11-22 19:35:38 +0100 (Mi., 22 Nov 2023) $
//   $Author: p20068 $
//   Creator: Michael Hava
//  Creation: October 24, 2023
// Copyright: (c) 2023 Michael Hava (michael.hava<AT>risc-software.at)
//            (c) 2023 Peter Kulczycki (peter.kulczycki<AT>fh-hagenberg.at)
//   License: This document contains proprietary information belonging to
//            University of Applied Sciences Upper Austria, Campus Hagenberg.
//            It is distributed under the Boost Software License (see
//            https://www.boost.org/users/license.html).

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// -------------------------------------------------------------------------------------------------

#undef  UNUSED
#define UNUSED(v) ((void) (v))

// -------------------------------------------------------------------------------------------------

typedef struct {
   int x;
   int y;
} position_type;

typedef enum {
   action_unknown,
   action_press,
   action_release,
   action_repeat,
} action_type;

typedef enum {
   key_unknown,
   key_down,
   key_left,
   key_right,
   key_up,
   key_r,
   key_esc,
} key_type;

typedef void on_key_type   (key_type, action_type);   // the user's event handler for events from the keyboard
typedef void on_timer_type (double elapsed);          // the user's event handler for events from the timer

// -------------------------------------------------------------------------------------------------

typedef struct {
    position_type rel_top_edge;
    position_type rel_right_edge;
    position_type rel_left_edge;
    position_type rel_bottom_edge;

    position_type * pivot_pt;

    position_type* rel_pts_array;
    int array_size;
} edge_hitbox;

typedef edge_hitbox* EdgeHitbox;

EdgeHitbox new_edge_hitbox(position_type * obj_rel_positions, int arr_size, position_type * pivot_pt);
void free_edge_hitbox(EdgeHitbox* e_hitbox_ptr);

typedef struct {
    position_type pt1   // always smaller x or y coordinate
                , pt2;

    position_type * pivot_pt;
} normal_line_hitbox;

typedef normal_line_hitbox* NormalLineHitbox;

NormalLineHitbox new_normal_line_hitbox(position_type pos1, position_type pos2, position_type * pivot_pt);
void free_normal_line_hitbox(NormalLineHitbox* nl_hitbox_ptr);

typedef struct {
    bool** pt;
    int x_size, y_size;
    position_type * pivot_pt;
} complex_hitbox;

typedef complex_hitbox* ComplexHitbox;

ComplexHitbox new_complex_hitbox(bool ** pt, int x_size, int y_size, position_type * pivot_pt);
void free_complex_hitbox(ComplexHitbox* c_hitbox_ptr);

position_type make_position (int x, int y);
position_type move_position (position_type position, int dx, int dy);
position_type relative_pos_to_absolute(position_type rel_pos, position_type ref_point);
position_type bottom_edge(position_type pos1, position_type pos2);
position_type right_edge(position_type pos1, position_type pos2);
position_type left_edge(position_type pos1, position_type pos2);
position_type top_edge(position_type pos1, position_type pos2);
position_type get_rel_edge(position_type* obj_rel_positions, int arr_size, position_type side_edge_func(position_type pos1, position_type pos2));

bool collides_with_complex_hitbox(void* hitbox, position_type hitbox_pt);
bool collides_with_edge_hitbox(void* hitbox, position_type hitbox_pt);
bool collides_with_nl_hitbox(void* hitbox, position_type hitbox_pt1, position_type hitbox_pt2);

action_type action_type_from_glfw_action (int glfw_action);
key_type    key_type_from_glfw_key       (int glfw_key);

char const * string_from_duration (double duration);
char const * string_from_key      (key_type key, action_type action);

void signal_last_gl_error   (bool force_abort);
void signal_last_glad_error (bool force_abort);
void signal_last_glfw_error (bool force_abort);
void signal_last_ogl_error  (GLuint shader, bool force_abort);
