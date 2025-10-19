//       $Id: tetrimino.h 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/tetrimino.h $
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

#define TETRIMINO_BLOCKS 4

typedef enum tetrimino_type_enum { I, J, L, O, S, T, Z, tetrimino_type_size } tetrimino_type_enum;
typedef enum { up, lt, rt, dn, rotations_enum_size } rotations_enum;
// -------------------------------------------------------------------------------------------------

typedef enum {
	// Rainbow order
	color_red = 255U << 0U,                            // Full red
	color_orange = (255U << 0U) | (128U << 8U),           // Strong red + moderate green
	color_yellow = color_red | (255U << 8U),               // Full red + full green
	color_lime = (192U << 0U) | (255U << 8U),           // High green + moderate red
	color_green = 255U << 8U,                            // Full green
	color_teal = (0U << 0U) | (128U << 8U) | (255U << 16U), // Moderate green + full blue
	color_cyan = color_green | (255U << 16U),              // Full green + full blue
	color_skyblue = (135U << 0U) | (206U << 8U) | (235U << 16U), // Soft blue
	color_blue = 255U << 16U,                           // Full blue
	color_purple = (128U << 0U) | (0U << 8U) | (255U << 16U), // Moderate red + full blue
	color_violet = (238U << 0U) | (130U << 8U) | (238U << 16U), // Bright violet
	color_magenta = color_red | color_blue,                // Full red + full blue

	// Additional vibrant colors
	color_pink = (255U << 0U) | (192U << 8U),           // Full red + high green
	color_aqua = (128U << 0U) | (255U << 8U) | (255U << 16U), // Moderate red + full green + blue
	color_gold = (255U << 0U) | (215U << 8U),           // Warm gold
	color_white = color_red | color_green | color_blue,  // Full red + green + blue (white)
	color_black = 0                                      // No color (black)
} color_type;

typedef struct {
   color_type    color;
   position_type position;
} block_type;

typedef block_type* Block;

Block new_block (position_type position, color_type color);
void free_block(Block *b);

typedef struct {
	Block pivot_pt;
	position_type relative_blocks_pos[TETRIMINO_BLOCKS];
	rotations_enum rotation;
	int max_rotations;
	tetrimino_type_enum type;
	EdgeHitbox hitbox;
} tetrimino_type;

typedef tetrimino_type* Tetrimino;

Tetrimino new_tetrimino (Block pivot_pt, tetrimino_type_enum type);
void free_tetrimino(Tetrimino* t);
void render_tetrimino(Tetrimino t);
void rotate_tetrimino(Tetrimino t);
void set_rotation(Tetrimino t, rotations_enum rotation);
void set_color(Tetrimino t, color_type color);
// -------------------------------------------------------------------------------------------------

void test_print_gb_edges(Tetrimino t);

GLfloat normalize_blue  (color_type color);
GLfloat normalize_green (color_type color);
GLfloat normalize_red   (color_type color);
