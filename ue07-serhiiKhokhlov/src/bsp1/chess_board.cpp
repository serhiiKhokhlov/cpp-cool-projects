#include <cstring>
#include "chess_board.h"

chessman* whites[TEAM_SIZE], * blacks[TEAM_SIZE];
chess_cell cells[BOARD_SIZE][BOARD_SIZE];
chessman* hand{ nullptr };
color turn{ white };

bool game_status = true;

bool get_status() {
	return game_status;
}

color get_turn() {
	return turn;
}

char const* to_string(letter_pos const p) {
	switch (p) {
	case a: return "a";
	case b: return "b";
	case c: return "c";
	case d: return "d";
	case e:	return "e";
	case f: return "f";
	case g: return "g";
	case h: return "h";
	default: return "";
	}
}

letter_pos ch_to_letter_pos(char const& p) {
	switch (p) {
	case 'a': return a;
	case 'b': return b;
	case 'c': return c;
	case 'd': return d;
	case 'e':	return e;
	case 'f': return f;
	case 'g': return g;
	case 'h': return h;
	default: return invalid;
	}
}

void switch_turn() { //TODO
	turn = (color)!(bool)turn;
}

chessman const* get_hand() {
	return hand;
}

void capture(chessman& cm) {
	//takes input with normalized coordinates
		//take chessman from the field
		//check if its an essential figure
		//set game_status

	game_status = !cm.is_essential();
	cm.set_cur_cell(nullptr);
}

bool move_to(position pos) {
	//takes normal player input (rows start with 1)

	if (hand) hand->deselect();

	pos.y--; //adjust to start with 0

	if (!is_valid_pos(pos)) return false; // Out of bounds

	chess_cell* new_cell = &cells[pos.x][pos.y];
	if (hand) {
		if (hand->can_be_moved_to(*new_cell, cells)) {
			if (pawn* p = dynamic_cast<pawn*>(hand)) p->use_first_move();
			if (new_cell->is_used()) {
				if (new_cell->get_chessman()->get_color() != turn)	//friendly fire check
					capture(*(new_cell->get_chessman()));
				else
					return false;
			}
			hand->get_cur_cell()->set_chessman(nullptr);
			new_cell->set_chessman(hand);
			hand = nullptr;
			switch_turn();
		}

		return true;
	}
	else return false;
}

bool select(position pos) {
	//takes normal player input (rows start with 1)
	pos.y--; //adjust to start with 0

	if (!is_valid_pos(pos)) return false; // Out of bounds
	
	hand = cells[pos.x][pos.y].get_chessman();
	if (hand) {
		hand->deselect();
		if (!turn == hand->get_color())
			hand = nullptr;
		else
			
			hand->select(cells);

		return true;
	}
	else return false;
}

bool select(chss_labels::chess_board_high_figure f) {
	if (hand) hand->deselect();
	hand = (turn == white ? whites[f] : blacks[f]);
	return hand->is_on_board() ? (hand->select(cells), true) : (hand = nullptr, false);
}

void initialize_board() {
	color clr = white;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		clr = color(!bool(clr));
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cells[i][j] = { clr, nullptr, position{ i, j } };
			clr = color(!bool(clr));
		}
	}

	//INITIALIZING TEAM WHITE
	cells[a][0].set_chessman(whites[0] = new rook(white, {}));
	cells[h][0].set_chessman(whites[1] = new rook(white, {}));

	cells[b][0].set_chessman(whites[2] = new knight(white, {}));
	cells[g][0].set_chessman(whites[3] = new knight(white, {}));

	cells[c][0].set_chessman(whites[4] = new bishop(white, {}));
	cells[f][0].set_chessman(whites[5] = new bishop(white, {}));

	cells[d][0].set_chessman(whites[6] = new queen(white, {}));
	cells[e][0].set_chessman(whites[7] = new king(white, {}));

	for (int i = 0; i < BOARD_SIZE; ++i) {
		cells[i][1].set_chessman(whites[8 + i] = new pawn(white, {}));
	}

	//INITIALIZING TEAM BLACK
	cells[a][7].set_chessman(blacks[0] = new rook(black, {}));
	cells[h][7].set_chessman(blacks[1] = new rook(black, {}));

	cells[b][7].set_chessman(blacks[2] = new knight(black, {}));
	cells[g][7].set_chessman(blacks[3] = new knight(black, {}));

	cells[c][7].set_chessman(blacks[4] = new bishop(black, {}));
	cells[f][7].set_chessman(blacks[5] = new bishop(black, {}));

	cells[d][7].set_chessman(blacks[6] = new queen(black, {}));
	cells[e][7].set_chessman(blacks[7] = new king(black, {}));

	for (int i = 0; i < BOARD_SIZE; ++i) {
		cells[i][6].set_chessman(blacks[8 + i] = new pawn(black, {}));
	}

}

std::ostream& print_board(std::ostream& out) { //TODO

	//Show possible moves for selected chessman //TODO

	out << "  |";
	for (int i = 0; i < BOARD_SIZE; ++i) {
		out << " " << to_string(letter_pos(i)) << " ";
	}
	out << "|  \n";
	out << "--+";
	for (int i = 0; i < BOARD_SIZE; ++i) {
		out << "---";
	}
	out << "+--\n";
	for (int i = BOARD_SIZE - 1; i >= 0; --i) {
		out << i + 1 << " |";
		for (int j = 0; j < BOARD_SIZE; ++j) {
			out << cells[j][i];
		}
		out << "| " << i + 1 << "\n";
	}
	out << "--+";
	for (int i = 0; i < BOARD_SIZE; ++i) {
		out << "---";
	}
	out << "+--\n";
	out << "  |";
	for (int i = 0; i < BOARD_SIZE; ++i) {
		out << " " << to_string(letter_pos(i)) << " ";
	}
	out << "|  \n";

	return out;
}

void delete_board() {
	for (auto white_cm : whites) {
		delete white_cm;
	}
	for (auto black_cm : blacks) {
		delete black_cm;
	}
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cells[i][j].set_chessman(nullptr);
		}
	}
}