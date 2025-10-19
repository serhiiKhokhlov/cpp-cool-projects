#pragma once
#include <iostream>
#include "chess.h"

namespace chss_labels {
	enum chess_board_high_figure {
		lrook,
		rrook,
		lknight,
		rknight,
		lbishop,
		rbishop,
		queen,
		king,
		pawn1,
		pawn2,
		pawn3,
		pawn4,
		pawn5,
		pawn6,
		pawn7,
		pawn8,
		figures_cnt
	};
}

color get_turn();
bool get_status();
void initialize_board();
char const* to_string(letter_pos const p);
letter_pos ch_to_letter_pos(char const& p);
std::ostream& print_board(std::ostream& out);
void delete_board();

bool move_to(position pos);
bool select(position pos);
bool select(chss_labels::chess_board_high_figure f);
chessman const* get_hand();
