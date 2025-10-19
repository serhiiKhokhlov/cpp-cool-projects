#pragma once
#include <iostream>
#include <cmath>
#include <vector>

#define BOARD_SIZE 8
#define FIGURE_TYPES 6
#define TEAM_SIZE 16

struct position {
	int x{ 0 }, y{ 0 };

	bool operator== (position const& pos) const {
		return x == pos.x && y == pos.y;
	}

	bool operator!= (position const& pos) const {
		return !(*this == pos);
	}
};

enum color { white, black };

enum figure {
	king_f = 'k',
	queen_f = 'q',
	bishop_f = 'b',
	rook_f = 'r',
	knight_f = 'n',
	pawn_f = 'p'
};

enum letter_pos {
	a, b, c, d, e, f, g, h, invalid
};

bool is_valid_pos(position const& pos);

class chess_cell;

//Abstract Chessman
class chessman {
protected:
	color clr{ white };
	figure repr{ pawn_f };
	bool essential{ false };
	chess_cell* cur_cell{ nullptr };
	std::vector <chess_cell*> selected_cells{};

	//Abstract method
	virtual void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) = 0;

public:
	std::vector <chess_cell*> const& get_selected_cells() const;
	void select(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]);
	color const& get_color() const;
	figure const& get_represents() const;
	position get_cur_position() const;
	chess_cell* get_cur_cell() const;
	void set_cur_cell(chess_cell* cl);
	void deselect();

	friend std::ostream& operator << (std::ostream& out, chessman const& cm);

	bool is_on_board() const{
		return cur_cell != nullptr;
	}

	bool const& is_essential() const;
	//Abstract method
	virtual bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const = 0;

	//Destructor
	virtual ~chessman() {}
};


class chess_cell final {
private:
	color clr{ white };
	chessman* cm{ nullptr };
	position pos{};
	bool selected{ false };
	bool highlighted{ false };

public:

	//Constructor
	chess_cell() = default;

	//Constructor
	chess_cell(color const& clr, chessman* cm, position const& pos)
		: clr{ clr }
		, cm{ cm }
		, pos{ pos }
	{}

	chessman* get_chessman() const {
		return cm;
	}

	void select();
	void deselect();
	void highlight(bool active);

	position const& get_pos() const;

	friend std::ostream& operator << (std::ostream& out, chess_cell const& cll);

	bool is_used() const {
		return cm;
	}

	void set_chessman(chessman* csm);

	//Destructor
	~chess_cell() {}

};

//King
class king final : public chessman {
private:
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();
		for (int x = cur_pos.x - 1; x <= cur_pos.x + 1; ++x) {
			for (int y = cur_pos.y - 1; y <= cur_pos.y + 1; ++y) {
				if (is_valid_pos(position{ x, y }) && (!chessboard[x][y].is_used() || (chessboard[x][y].is_used() && chessboard[x][y].get_chessman()->get_color() != this->clr)))
				{
					chessboard[x][y].select();
					if (position{ x, y } != cur_pos)
						selected_cells.push_back(&(chessboard[x][y]));
				}
			}
		}
	}
public:

	//Constructor
	king(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = king_f;
		essential = true;
		this->cur_cell = cl;
	}

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		chessboard[0][0];	//not used
		position pos = cl.get_pos();
		return cur_cell && ((cl.is_used() && cl.get_chessman()->get_color() != this->clr) || (!cl.is_used())) &&
			cur_cell->get_pos() != pos && abs(cur_cell->get_pos().x - pos.x) <= 1 && abs(cur_cell->get_pos().y - pos.y) <= 1;
	}
};


//Queen
class queen final : public chessman {
private:
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();
		bool used_check = false;
		for (int i = cur_pos.x + 1; i < BOARD_SIZE && !used_check; ++i) {		//horizontal selection
			//if (cur_pos.x != i)
			//	selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			if (!(used_check = chessboard[i][cur_pos.y].is_used()) || (chessboard[i][cur_pos.y].is_used() && chessboard[i][cur_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[i][cur_pos.y].select();
					selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			}
		}

		used_check = false;
		for (int i = cur_pos.x - 1; i >= 0 && !used_check; --i) {		//horizontal selection
			//if (cur_pos.x != i)
			//	selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			if (!(used_check = chessboard[i][cur_pos.y].is_used()) || (chessboard[i][cur_pos.y].is_used() && chessboard[i][cur_pos.y].get_chessman()->get_color() != this->clr)) {
				chessboard[i][cur_pos.y].select();
				selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			}
			
		}


		used_check = false;
		for (int i = cur_pos.y + 1; i < BOARD_SIZE && !used_check; ++i) {		//vertical selection
			//if (cur_pos.y != i)
				//selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			if (!(used_check = chessboard[cur_pos.x][i].is_used()) || (chessboard[cur_pos.x][i].is_used() && chessboard[cur_pos.x][i].get_chessman()->get_color() != this->clr))
			{
				chessboard[cur_pos.x][i].select();
				selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			}
		}

		used_check = false;
		for (int i = cur_pos.y - 1; i >= 0 && !used_check; --i) {		//vertical selection
			//if (cur_pos.y != i)
				//selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			if (!(used_check = chessboard[cur_pos.x][i].is_used()) || (chessboard[cur_pos.x][i].is_used() && chessboard[cur_pos.x][i].get_chessman()->get_color() != this->clr))
			{
				chessboard[cur_pos.x][i].select();
				selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			}
		}

		position lbottom_diagonal{ cur_pos.x - std::min(cur_pos.x, cur_pos.y), cur_pos.y - std::min(cur_pos.x, cur_pos.y) };
		position rtop_diagonal{ cur_pos.x + std::min(BOARD_SIZE - 1 - cur_pos.x, BOARD_SIZE - 1 - cur_pos.y), cur_pos.y + std::min(BOARD_SIZE - 1 - cur_pos.x, BOARD_SIZE - 1 - cur_pos.y) };

		position rbottom_diagonal{ cur_pos.x + std::min(BOARD_SIZE - 1 - cur_pos.x, cur_pos.y), cur_pos.y - std::min(BOARD_SIZE - 1 - cur_pos.x, cur_pos.y) };
		position ltop_diagonal{ cur_pos.x - std::min(cur_pos.x, BOARD_SIZE - 1 - cur_pos.y), cur_pos.y + std::min(cur_pos.x, BOARD_SIZE - 1 - cur_pos.y) };

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x <= rtop_diagonal.x && it_pos.y <= rtop_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x++;
			it_pos.y++;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x >= lbottom_diagonal.x && it_pos.y >= lbottom_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x--;
			it_pos.y--;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x >= ltop_diagonal.x && it_pos.y <= ltop_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x--;
			it_pos.y++;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x <= rbottom_diagonal.x && it_pos.y >= rbottom_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x++;
			it_pos.y--;
		}

	}
public:

	//Constructor 
	queen(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = queen_f;
		this->cur_cell = cl;
	}

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		position pos = cl.get_pos();
		bool res = true;

		if (!cur_cell) return false;

		//if given cell is valid then
			//Cross path check
		if (cur_cell->get_pos().x != pos.x && cur_cell->get_pos().y == pos.y) { // horizontal path

			for (int i = std::min(cur_cell->get_pos().x, pos.x) + 1; i < std::max(cur_cell->get_pos().x, pos.x) && res; ++i) {
				res = !chessboard[i][pos.y].is_used();
			}

			return res;
		}
		else if (cur_cell->get_pos().y != pos.y && cur_cell->get_pos().x == pos.x) { //vertical path

			for (int i = std::min(cur_cell->get_pos().y, pos.y) + 1; i < std::max(cur_cell->get_pos().y, pos.y) && res; ++i) {
				res = !chessboard[pos.x][i].is_used();
			}

			return res;
		}
		else if (abs(cur_cell->get_pos().x - pos.x) == abs(cur_cell->get_pos().y - pos.y)) { //diagonal path

			int x_incr = cur_cell->get_pos().x < pos.x ? 1 : -1;
			int y_incr = cur_cell->get_pos().y < pos.y ? 1 : -1;

			for (position it_pos = { cur_cell->get_pos().x + x_incr, cur_cell->get_pos().y + y_incr }; it_pos.x != pos.x && it_pos.y != pos.y && res;) {
				res = !chessboard[it_pos.x][it_pos.y].is_used();
				it_pos.x += x_incr;
				it_pos.y += y_incr;
			}

			return res;
		}
		else return false;


		//return cur_cell && ((abs(pos.x - cur_cell->get_pos().x) == abs(pos.y - cur_cell->get_pos().y) && cur_cell->get_pos()!= pos)
		//	||
		//	((cur_cell->get_pos().x == pos.x && cur_cell->get_pos().y != pos.y) || (cur_cell->get_pos().y == pos.y && cur_cell->get_pos().x != pos.x)));
	}
};


//Bishop
class bishop final : public chessman {
private:
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();

		position lbottom_diagonal{ cur_pos.x - std::min(cur_pos.x, cur_pos.y), cur_pos.y - std::min(cur_pos.x, cur_pos.y) };
		position rtop_diagonal{ cur_pos.x + std::min(BOARD_SIZE - 1 - cur_pos.x, BOARD_SIZE - 1 - cur_pos.y), cur_pos.y + std::min(BOARD_SIZE - 1 - cur_pos.x, BOARD_SIZE - 1 - cur_pos.y) };

		position rbottom_diagonal{ cur_pos.x + std::min(BOARD_SIZE - 1 - cur_pos.x, cur_pos.y), cur_pos.y - std::min(BOARD_SIZE - 1 - cur_pos.x, cur_pos.y) };
		position ltop_diagonal{ cur_pos.x - std::min(cur_pos.x, BOARD_SIZE - 1 - cur_pos.y), cur_pos.y + std::min(cur_pos.x, BOARD_SIZE - 1 - cur_pos.y) };

		bool used_check = false;
		for (position it_pos = cur_pos; it_pos.x <= rtop_diagonal.x && it_pos.y <= rtop_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x++;
			it_pos.y++;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x >= lbottom_diagonal.x && it_pos.y >= lbottom_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x--;
			it_pos.y--;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x >= ltop_diagonal.x && it_pos.y <= ltop_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x--;
			it_pos.y++;
		}

		used_check = false;
		for (position it_pos = cur_pos; it_pos.x <= rbottom_diagonal.x && it_pos.y >= rbottom_diagonal.y && !used_check;) {
			if (!(used_check = chessboard[it_pos.x][it_pos.y].is_used() && it_pos != cur_pos) || (chessboard[it_pos.x][it_pos.y].is_used() && chessboard[it_pos.x][it_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[it_pos.x][it_pos.y].select();
				if (it_pos != cur_pos)
					selected_cells.push_back(&(chessboard[it_pos.x][it_pos.y]));
			}
			it_pos.x++;
			it_pos.y--;
		}
	}
public:

	//Constructor 
	bishop(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = bishop_f;
		this->cur_cell = cl;
	}

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		bool res = true;
		position pos = cl.get_pos();

		if (!cur_cell) return false;

		if (abs(cur_cell->get_pos().x - pos.x) == abs(cur_cell->get_pos().y - pos.y)) { //diagonal path

			int x_incr = cur_cell->get_pos().x < pos.x ? 1 : -1;
			int y_incr = cur_cell->get_pos().y < pos.y ? 1 : -1;

			for (position it_pos = { cur_cell->get_pos().x + x_incr, cur_cell->get_pos().y + y_incr }; it_pos.x != pos.x && it_pos.y != pos.y && res;) {
				res = !chessboard[it_pos.x][it_pos.y].is_used();
				it_pos.x+= x_incr;
				it_pos.y+= y_incr;
			}

			return res;
		}
		else return false;

		//return cur_cell && (abs(pos.x - cur_cell->get_pos().x) == abs(pos.y - cur_cell->get_pos().y) && cur_cell->get_pos() != pos);
	}

	
};


//Rook
class rook final : public chessman {
private:
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();
		bool used_check = false;
		for (int i = cur_pos.x + 1; i < BOARD_SIZE && !used_check; ++i) {		//horizontal selection
			//if (cur_pos.x != i)
			//	selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			if (!(used_check = chessboard[i][cur_pos.y].is_used()) || (chessboard[i][cur_pos.y].is_used() && chessboard[i][cur_pos.y].get_chessman()->get_color() != this->clr))
			{
				chessboard[i][cur_pos.y].select();
				selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			}
		}

		used_check = false;
		for (int i = cur_pos.x - 1; i >= 0 && !used_check; --i) {		//horizontal selection
			//if (cur_pos.x != i)
			//	selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			if (!(used_check = chessboard[i][cur_pos.y].is_used()) || (chessboard[i][cur_pos.y].is_used() && chessboard[i][cur_pos.y].get_chessman()->get_color() != this->clr)) {
				chessboard[i][cur_pos.y].select();
				selected_cells.push_back(&(chessboard[i][cur_pos.y]));
			}

		}


		used_check = false;
		for (int i = cur_pos.y + 1; i < BOARD_SIZE && !used_check; ++i) {		//vertical selection
			//if (cur_pos.y != i)
				//selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			if (!(used_check = chessboard[cur_pos.x][i].is_used()) || (chessboard[cur_pos.x][i].is_used() && chessboard[cur_pos.x][i].get_chessman()->get_color() != this->clr))
			{
				chessboard[cur_pos.x][i].select();
				selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			}
		}

		used_check = false;
		for (int i = cur_pos.y - 1; i >= 0 && !used_check; --i) {		//vertical selection
			//if (cur_pos.y != i)
				//selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			if (!(used_check = chessboard[cur_pos.x][i].is_used()) || (chessboard[cur_pos.x][i].is_used() && chessboard[cur_pos.x][i].get_chessman()->get_color() != this->clr))
			{
				chessboard[cur_pos.x][i].select();
				selected_cells.push_back(&(chessboard[cur_pos.x][i]));
			}
		}
	}
public:

	//Constructor 
	rook(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = rook_f;
		this->cur_cell = cl;
	}

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		position pos = cl.get_pos();

		bool res = true;

		if (!cur_cell) return false;

		if (cur_cell->get_pos().x != pos.x && cur_cell->get_pos().y == pos.y) { // horizontal path

			for (int i = std::min(cur_cell->get_pos().x, pos.x) + 1; i < std::max(cur_cell->get_pos().x, pos.x) && res; ++i) {
				res = !chessboard[i][pos.y].is_used();
			}

			return res;
		}
		else if (cur_cell->get_pos().y != pos.y && cur_cell->get_pos().x == pos.x) { //vertical path

			for (int i = std::min(cur_cell->get_pos().y, pos.y) + 1; i < std::max(cur_cell->get_pos().y, pos.y) && res; ++i) {
				res = !chessboard[pos.x][i].is_used();
			}

			return res;
		}
		else return false;

		//return cur_cell && ((cur_cell->get_pos().x == pos.x && cur_cell->get_pos().y != pos.y) || (cur_cell->get_pos().y == pos.y && cur_cell->get_pos().x != pos.x));
	}

	
};


//Knight
class knight final : public chessman {
private:
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();

		position pos{};

		for (int i = 0; i <= 1; ++i) {
			for (int j = 0; j <= 1; ++j) {
				for (int k = 0; k <= 1; ++k) {
					int hshift = i ? (k ? 2 : -2) : (j ? 1 : -1);
					int vshift = i ? (j ? 1 : -1) : (k ? 2 : -2);

					pos = { cur_pos.x + hshift, cur_pos.y + vshift };

					if (is_valid_pos(pos)) {
						if (!chessboard[pos.x][pos.y].is_used() || (chessboard[pos.x][pos.y].is_used() && chessboard[pos.x][pos.y].get_chessman()->get_color() != this->clr))
						{
							chessboard[pos.x][pos.y].select();
							selected_cells.push_back(&(chessboard[pos.x][pos.y]));
						}
					}
				}
			}
		}

	}
public:

	//Constructor
	knight(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = knight_f;
		this->cur_cell = cl;
	}

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		position pos = cl.get_pos();
		chessboard[0][0];	//not used
		return cur_cell && ((abs(cur_cell->get_pos().x - pos.x) == 2 && abs(cur_cell->get_pos().y - pos.y) == 1)
			||
			(abs(cur_cell->get_pos().y - pos.y) == 2 && abs(cur_cell->get_pos().x - pos.x) == 1));
	}
};


// Pawn
class pawn final : public chessman {
private:
	bool first_move = true;
	void select_fields(chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) {
		position cur_pos = cur_cell->get_pos();
		position pos1 = { cur_pos.x, cur_pos.y + (clr == white ? 1 : -1) };
		position pos2 = { cur_pos.x, cur_pos.y + (clr == white ? 2 : -2) };
		position pos3 = { cur_pos.x + 1, cur_pos.y + (clr == white ? 1 : -1) };
		position pos4 = { cur_pos.x - 1, cur_pos.y + (clr == white ? 1 : -1) };

		if (is_valid_pos(pos1) && (!chessboard[pos1.x][pos1.y].is_used() || (chessboard[pos1.x][pos1.y].is_used() && chessboard[pos1.x][pos1.y].get_chessman()->get_color() != this->clr))) {
			chessboard[pos1.x][pos1.y].select();
			selected_cells.push_back(&(chessboard[pos1.x][pos1.y]));
		}

		if ((first_move && is_valid_pos(pos2) && (!chessboard[pos2.x][pos2.y].is_used() || (chessboard[pos2.x][pos2.y].is_used() && chessboard[pos2.x][pos2.y].get_chessman()->get_color() != this->clr))) && (is_valid_pos(pos1) && (!chessboard[pos1.x][pos1.y].is_used() || (chessboard[pos1.x][pos1.y].is_used() && chessboard[pos1.x][pos1.y].get_chessman()->get_color() != this->clr)))) {
			chessboard[pos2.x][pos2.y].select();
			selected_cells.push_back(&(chessboard[pos2.x][pos2.y]));
		}

		if (is_valid_pos(pos3) && chessboard[pos3.x][pos3.y].is_used() && chessboard[pos3.x][pos3.y].get_chessman()->get_color() != this->clr) {
			chessboard[pos3.x][pos3.y].select();
			selected_cells.push_back(&(chessboard[pos3.x][pos3.y]));
		}

		if (is_valid_pos(pos4) && chessboard[pos4.x][pos4.y].is_used() && chessboard[pos4.x][pos4.y].get_chessman()->get_color() != this->clr) {
			chessboard[pos4.x][pos4.y].select();
			selected_cells.push_back(&(chessboard[pos4.x][pos4.y]));
		}
	}
public:
	//Constructor
	pawn(color const& clr, chess_cell* cl) {
		this->clr = clr;
		repr = pawn_f;
		this->cur_cell = cl;
	}

	bool is_first_move() { return first_move; };

	void use_first_move() { first_move = false; };

	bool can_be_moved_to(chess_cell const& cl, chess_cell chessboard[BOARD_SIZE][BOARD_SIZE]) const {
		position pos = cl.get_pos();
		chessboard[0][0];	//not used
		return cur_cell && (( !cl.is_used() &&
			(
				pos.y - cur_cell->get_pos().y == (clr == white ? 1 : -1)												//usual forward movement
				|| (pos.y - cur_cell->get_pos().y == (clr == white ? 2 : -2) && first_move)								//first movement
				) && (pos.x == cur_cell->get_pos().x)
			) || (
				cl.is_used() && abs(pos.x - cur_cell->get_pos().x) == 1 && pos.y - cur_cell->get_pos().y == (clr == white ? 1 : -1)		//capturing movement
				));
	}
};