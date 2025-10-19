#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <random>
#include "chess.h"
#include "chess_board.h"

using namespace chss_labels;

std::random_device rd;
std::default_random_engine engine(rd());
std::uniform_int_distribution<int> random_figure(lrook, pawn8);

position parse_input(std::istream& in) {
	std::string input;
	in >> input;
	return position{ ch_to_letter_pos(input[1]), input[0] - '0' };
}

std::string const pos_to_chess_pos(position const& pos) {
	return std::to_string(pos.y + 1) + to_string(letter_pos(pos.x));
}

position pos_to_player_pos(position pos) {
	return (++(pos.y), pos);
}

position auto_select() {
	select(chess_board_high_figure(random_figure(engine)));
	if (!get_hand() || get_hand()->get_selected_cells().size() == 0) 
	{
		auto_select();
	}
	return get_hand()->get_cur_position();
}

position auto_move() {
	std::uniform_int_distribution<size_t> random_move_cell(0, get_hand()->get_selected_cells().size() - 1);
	chess_cell* move_to_cell = get_hand()->get_selected_cells()[random_move_cell(engine)];
	move_to(pos_to_player_pos(move_to_cell->get_pos()));
	return move_to_cell->get_pos();
}

int main(int argc, char* argv[]) {
	initialize_board();
	if (argc > 1) {
		if (strcmp(argv[1], "-a") == 0 || strcmp(argv[1],"--auto") == 0) {
			print_board(std::cout) << "\n";
			while (get_status()) {
				position gen_pos{};
				std::cout << "turn " << (get_turn() ? "black" : "white") << "\n";
				std::cout << pos_to_chess_pos(auto_select()) << "\n";
				print_board(std::cout) << "\n";
				std::cout << pos_to_chess_pos(auto_move()) << "\n";
				print_board(std::cout) << "\n\n";
			}
		} else {
			delete_board();
			std::cerr << "Error: Unknown option \"" << argv[1] << "\"";
			return 1;
		}
	} else {
		print_board(std::cout) << "\n";
		while (get_status()) {
			std::cout << "turn " << (get_turn() ? "black" : "white") << "\n";
			if (select(parse_input(std::cin)))
			{
				print_board(std::cout) << "\n";
				if (move_to(parse_input(std::cin)))
					print_board(std::cout) << "\n\n";
			}
		}
	}

	std::cout << (get_turn() ? "black" : "white") << "s have been defeated";

	delete_board();
	return 0;
}