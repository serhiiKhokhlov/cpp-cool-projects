//       $Id: board.h 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/board.h $
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

#include "./primitives.h"

// -------------------------------------------------------------------------------------------------

#define N_COLS 11
#define N_ROWS 22

// -------------------------------------------------------------------------------------------------

bool is_valid_position (position_type position);
bool hit_bottom (position_type position);
bool collides (position_type position);
bool is_free_row(int rownum);
bool reached_top();

bool bottom_collides(EdgeHitbox e_hitbox);
bool used_space_collides(EdgeHitbox e_hitbox);

bool used_space_bottom_collides(EdgeHitbox e_hitbox);

bool side_collides(EdgeHitbox e_hitbox);

void take_place (position_type position);
void clear_row (int rownum);
void initialize_board (); 
void render_board ();

int  get_max_row ();
void clear_board();

//for functional testing
void test_row_delete();
void test_print_board();

