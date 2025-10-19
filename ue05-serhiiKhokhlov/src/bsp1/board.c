//       $Id: board.c 47850 2023-11-22 18:48:28Z p20068 $
//      $URL: https://svn01.fh-hagenberg.at/bin/cepheiden/Inhalt/C/Tetris-Shader/template/src/tetris/src/board.c $
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
#include "./window.h"

#define BOARD_COLOR color_gold

bool ** is_free;
int free_row_blocks [N_ROWS];
int cur_max_used_row = 4;

position_type board_pivot = { 0, 0 };

NormalLineHitbox right_board_edge = NULL;
NormalLineHitbox left_board_edge = NULL;
NormalLineHitbox bottom_board_edge = NULL;
ComplexHitbox used_space_hb = NULL;


// -------------------------------------------------------------------------------------------------

int get_max_row() {
	return cur_max_used_row;
}

void initialize_board () {
	is_free = malloc(sizeof(bool*) * N_COLS);
	assert(is_free);
	for (int i = 0; i < N_COLS; ++i) {
		is_free[i] = malloc(sizeof(bool) * N_ROWS);
		assert(is_free[i]);
	}

	for (int i = 0; i < N_ROWS; ++i) {
		for (int j = 0; j < N_COLS; ++j) {
			is_free[j][i] = true;	
		}
		free_row_blocks[i] = N_COLS;
	}
	used_space_hb = new_complex_hitbox(is_free, N_COLS, N_ROWS, &board_pivot);
	left_board_edge = new_normal_line_hitbox(make_position(-1, -1), make_position(-1, N_ROWS), &board_pivot);
	right_board_edge = new_normal_line_hitbox(make_position(N_COLS, -1), make_position(N_COLS, N_ROWS), &board_pivot);
	bottom_board_edge = new_normal_line_hitbox(make_position(-1, -1), make_position(N_COLS, -1), &board_pivot);
	printf("initialized board\n");
}

bool is_valid_position (position_type const position) {   // ToDo
	return	position.x >= 0 &&
			position.x < N_COLS &&
			position.y >= 0 &&
			position.y < N_ROWS;
}

bool side_collides (EdgeHitbox e_hitbox) {
	return collides_with_nl_hitbox((void*)left_board_edge, relative_pos_to_absolute(e_hitbox->rel_left_edge, *(e_hitbox->pivot_pt)),
														   relative_pos_to_absolute(e_hitbox->rel_right_edge, *(e_hitbox->pivot_pt)))
			||
		   collides_with_nl_hitbox((void*)right_board_edge, relative_pos_to_absolute(e_hitbox->rel_right_edge, *(e_hitbox->pivot_pt)),
															relative_pos_to_absolute(e_hitbox->rel_left_edge, *(e_hitbox->pivot_pt)));
}

bool hit_bottom (position_type const position) {
	return position.y < 0;
}

bool bottom_collides(EdgeHitbox e_hitbox) {
	return collides_with_nl_hitbox((void *)bottom_board_edge, relative_pos_to_absolute(e_hitbox->rel_bottom_edge, *(e_hitbox->pivot_pt)),
															  relative_pos_to_absolute(e_hitbox->rel_top_edge, *(e_hitbox->pivot_pt)));
}

bool collides (position_type const position) {
	return !is_free[position.x][position.y];
}



bool used_space_collides(EdgeHitbox e_hitbox) {
	int i = 0;
	for (; i < e_hitbox->array_size && 
						!collides_with_complex_hitbox((void *)used_space_hb, relative_pos_to_absolute(e_hitbox->rel_pts_array[i], *(e_hitbox->pivot_pt)));
		++i);
	return i < e_hitbox->array_size;
}

bool used_space_bottom_collides(EdgeHitbox e_hitbox) {
	return collides_with_complex_hitbox((void*)used_space_hb, relative_pos_to_absolute(e_hitbox->rel_bottom_edge, *(e_hitbox->pivot_pt)));
}

bool is_free_row(int const rownum) {
//	printf("checking %d row (value %d):", rownum, free_row_blocks[rownum]);
	//if (free_row_blocks[rownum] > 0) printf("free\n"); else printf("full\n");

	return free_row_blocks[rownum] > 0;
}

void take_place (position_type const position) {
	if (!is_valid_position(position)) {
	//	fprintf(stderr, "Invalid position: %d %d\n", position.x, position.y);
		return;
	}
	is_free[position.x][position.y] = false;
//	printf("setting position %d %d false\n", position.x, position.y);
	free_row_blocks[position.y]--;
	cur_max_used_row = cur_max_used_row > position.y ? cur_max_used_row : position.y;
//	printf("taking position: %d %d\n", position.x, position.y);
}

void clear_row (int rownum) {
	//free_row_blocks[rownum] = N_COLS;
	//for (int i = 0; i < N_COLS; ++i) {
	//	is_free[i][rownum] = true;
	//}
	cur_max_used_row--;
	for (int i = rownum; i <= cur_max_used_row; ++i) {
		for (int j = 0; j < N_COLS; ++j) {
			is_free[j][i] = is_free[j][i + 1];
		}
		free_row_blocks[i] = free_row_blocks[i + 1];
	}
	for (int i = 0; i < N_COLS; ++i) {
		is_free[i][cur_max_used_row + 1] = true;
	}
	free_row_blocks[cur_max_used_row + 1] = N_COLS;
	//test_print_board();
}

bool reached_top() {
	return cur_max_used_row == N_ROWS - 1;
}

void render_board () {
	for (int i = 0; i < N_COLS; ++i) {
		for (int j = 0; j <= cur_max_used_row; ++j) {
			if (!is_free[i][j]) {
		//		printf("rendering position: %d %d\n", i, j);
				render_block(make_position(i, j), BOARD_COLOR);
			}
		//	else printf("failed to render %d %d \n", i ,j);
		}
	}
}

void test_row_delete() {
	cur_max_used_row = 3;
	for (int i = 0; i <= cur_max_used_row; ++i) {
		for (int j = 0; j < N_COLS / 2; ++j) {
			is_free[j][i] = false;
		}

		for (int j = N_COLS / 2 + 1; j < N_COLS; ++j) {
			is_free[j][i] = false;
		}
		free_row_blocks[i] = 1;
	}

	is_free[4][1] = true;
	free_row_blocks[1] = 2;
}

void clear_board () {
	free_complex_hitbox(&used_space_hb);
	free_normal_line_hitbox(&left_board_edge);
	free_normal_line_hitbox(&right_board_edge);
	free_normal_line_hitbox(&bottom_board_edge);
}


void test_print_board() {
	for (int i = 0; i < N_ROWS; ++i) {
		for (int j = 0; j < N_COLS; ++j) {
			printf("%d ", (int)is_free[j][i]);
		}
		printf("\n");
	}
	for (int i = 0; i < N_ROWS; ++i) {
		printf("%d\n", free_row_blocks[i]);
	}
}