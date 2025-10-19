//       $Id: tetrimino.c 47849 2023-11-22 18:35:38Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/tetrimino.c $
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

#include "./window.h"

// -------------------------------------------------------------------------------------------------

position_type i_rotations[2][TETRIMINO_BLOCKS] = 
{ {{1, 0}, {1, 1}, {1, 2}, {1, 3}},
  {{0 ,2}, {1, 2}, {2, 2}, {3, 2}} };
position_type j_rotations[4][TETRIMINO_BLOCKS] =
{   { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
	{ {0, 2}, {0, 1}, {1, 1}, {2, 1 } },
	{ {2, 2}, {1, 2}, {1, 1}, {1, 0} },
	{ {0, 1}, {1, 1}, {2, 1}, {2, 0} } };
position_type l_rotations[4][TETRIMINO_BLOCKS] =
{   {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
	{ {0, 0}, {0, 1}, {1, 1}, {2, 1} },
	{ {1, 0}, {1, 1}, {1, 2}, {0, 2} },
	{ {0, 1}, {1, 1}, {2, 1}, {2, 2} }
};
position_type o_rotations[1][TETRIMINO_BLOCKS] =
{ {{0, 0}, {1, 0}, {0, 1}, {1, 1}} 
};
position_type s_rotations[4][TETRIMINO_BLOCKS] = {
	{{0, 1}, {1, 1}, {1, 2}, {2, 2}},
	{ {1, 2}, {1, 1}, {2, 1}, {2, 0} },
	{ {0, 0}, {1, 0}, {1, 1}, {2, 1} },
	{ {1, 0}, {1, 1}, {0, 1}, {0, 2} }
};
position_type t_rotations[4][TETRIMINO_BLOCKS] = {
	{{1, 0}, {0, 1}, {1, 1}, {2, 1}},
	{ {1, 0}, {1, 1}, {1, 2}, {2, 1} },
	{ {1, 2}, {0, 1}, {1, 1}, {2, 1} },
	{ {1, 0}, {1, 1}, {1, 2}, {0, 1} }
};
position_type z_rotations[4][TETRIMINO_BLOCKS] = {
	{ {0, 2}, {1, 2}, {1, 1}, {2, 1} },
	{ {1, 0}, {1, 1}, {2, 1}, {2, 2} },
	{ {1, 0}, {2, 0}, {0, 1}, {1, 1} },
	{ {0, 0}, {0, 1}, {1, 1}, {1, 2} }
};

position_type(*rotatable_tetriminos_pos_array[tetrimino_type_size])[TETRIMINO_BLOCKS] = {
	i_rotations,
	j_rotations,
	l_rotations,
	o_rotations,
	s_rotations,
	t_rotations,
	z_rotations
};

GLfloat normalize_blue (color_type const color) {
   return (GLfloat) (color & color_blue) / color_blue;
}

GLfloat normalize_green (color_type const color) {
   return (GLfloat) (color & color_green) / color_green;
}

GLfloat normalize_red (color_type const color) {
   return (GLfloat) (color & color_red) / color_red;
}

Block new_block (position_type position, color_type color) {
	Block b = malloc(sizeof(block_type));
	if (b) {
		b->position = position;
		b->color = color;
	}
	return b;
}

void free_block (Block *b) {
	if (*b) {
		free(*b);
		*b = NULL;
	}
}

Tetrimino new_tetrimino(Block pivot_pt, tetrimino_type_enum type) {
	Tetrimino t = malloc(sizeof(tetrimino_type));
	if (t) {
		t->pivot_pt = pivot_pt;
		t->type = type;
		t->rotation = up; // default
		switch (t->type) {
		case I:
			t->max_rotations = 2;
			break;
		case O:
			t->max_rotations = 1;
			break;
		default:
			t->max_rotations = 4;
		}
		for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
			t->relative_blocks_pos[i] = rotatable_tetriminos_pos_array[type][up][i];
		}
		t->hitbox = new_edge_hitbox(rotatable_tetriminos_pos_array[type][up], TETRIMINO_BLOCKS, &(pivot_pt->position));
	}
	return t;
}

void rotate_tetrimino(Tetrimino t) {
	++t->rotation;
	set_rotation(t, t->rotation);
}

void set_rotation(Tetrimino t, rotations_enum rotation) {
	t->rotation = rotation % t->max_rotations;
	for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
		t->relative_blocks_pos[i] = rotatable_tetriminos_pos_array[t->type][t->rotation][i];
	}
	free_edge_hitbox(&(t->hitbox));
	t->hitbox = new_edge_hitbox(rotatable_tetriminos_pos_array[t->type][t->rotation], TETRIMINO_BLOCKS, &(t->pivot_pt->position));
}

void free_tetrimino(Tetrimino* t) {
	if (*t) {
		free_block(&((*t)->pivot_pt));
		free_edge_hitbox(&((*t)->hitbox));
		free(*t);
		*t = NULL;
	}
}

void set_color(Tetrimino t, color_type color) {
	t->pivot_pt->color = color;
}

void render_tetrimino(Tetrimino t) {
	for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
		position_type pos = relative_pos_to_absolute(t->relative_blocks_pos[i], t->pivot_pt->position);
		render_block(pos, t->pivot_pt->color);
	}
}

void test_print_gb_edges(Tetrimino t) {
	printf("top edge %d %d\n", t->hitbox->rel_top_edge.x, t->hitbox->rel_top_edge.y);
	printf("bottom edge %d %d\n", t->hitbox->rel_bottom_edge.x, t->hitbox->rel_bottom_edge.y);
	printf("left edge %d %d\n", t->hitbox->rel_left_edge.x, t->hitbox->rel_left_edge.y);
	printf("right edge %d %d\n", t->hitbox->rel_right_edge.x, t->hitbox->rel_right_edge.y);
}

position_type get_bottom_edge(Tetrimino t) {
	position_type bottom_pos = {0, 0};
	for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
		bottom_pos = bottom_pos.y < t->relative_blocks_pos[i].y ? bottom_pos : t->relative_blocks_pos[i];
 	}
	return bottom_pos;
}

position_type get_right_edge(Tetrimino t) {
	position_type bottom_pos = { 0, 0 };
	for (int i = 0; i < TETRIMINO_BLOCKS; ++i) {
		bottom_pos = bottom_pos.y < t->relative_blocks_pos[i].y ? bottom_pos : t->relative_blocks_pos[i];
	}
	return bottom_pos;
}