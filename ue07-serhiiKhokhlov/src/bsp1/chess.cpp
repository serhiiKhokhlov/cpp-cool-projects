#include <cctype>
#include <cmath>
#include <string>
#include "chess.h"

//chessman class
color const& chessman::get_color() const {
	return clr;
}

figure const& chessman::get_represents() const {
	return repr;
}

position chessman::get_cur_position() const {
	return cur_cell ? cur_cell->get_pos() : position{};
}

chess_cell * chessman::get_cur_cell() const {
	return cur_cell;
}

std::vector <chess_cell*> const& chessman::get_selected_cells() const {
	return selected_cells;
}

void chessman::set_cur_cell(chess_cell * cl) {
	//cur_cell->set_chessman(nullptr);
	cur_cell = cl;
}

bool const& chessman::is_essential() const {
	return essential;
}

void chessman::select(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
	select_fields(chessboard);
	cur_cell->highlight(true);
}

void chessman::deselect() {
	for (auto cell : selected_cells) {
		cell->deselect();
	}
	//	selected = false;
	selected_cells.clear();
	cur_cell->deselect();
	cur_cell->highlight(false);
}

std::ostream& operator << (std::ostream& out, chessman const& cm) {
	out << (char)((cm.clr == black ? toupper(cm.repr) : cm.repr));
	return out;
}



bool is_valid_pos(position const& pos) {
	return !(pos.x < 0 || pos.x >= BOARD_SIZE || pos.y < 0 || pos.y >= BOARD_SIZE);
}








//chess_cell class
position const& chess_cell::get_pos() const {
	return pos;
}

void chess_cell::set_chessman(chessman* csm) {
	this->cm = csm;
	if(this->cm) this->cm->set_cur_cell(this);
}

void chess_cell::select() {
	selected = true;
}

void chess_cell::deselect() {
	selected = false;
}

void chess_cell::highlight(bool active) {
	highlighted = active;
}

std::ostream& operator << (std::ostream& out, chess_cell const& cll) {
	out << (cll.highlighted ? "(" : (cll.selected ? "|" : " "));
	if (cll.cm)
		out << *(cll.cm);
	else
		out << (cll.clr == white ? "." : "*");
	out << (cll.highlighted ? ")" : (cll.selected ? "|" : " "));
	return out;
}