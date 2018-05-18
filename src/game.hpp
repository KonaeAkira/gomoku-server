#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstring>

extern std::string PLAYER_BLACK, PLAYER_WHITE;

namespace game
{

	namespace
	{
		char dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
		char dy[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	}

	typedef std::pair<size_t, size_t> coord;
	enum status
	{
		TURN_BLACK, // black's turn
		TURN_WHITE, // white's turn
		OVER_BLACK, // black won
		OVER_WHITE, // white won
		OVER_TIE // tie
	}; // enum status

	template<size_t SIZE>
	class board
	{

	private:

		char __cell[SIZE][SIZE];
		
	public:
		
		board()
		{
			// set all cells to '0' - empty
			memset(__cell, 0x00, sizeof(__cell));
		}
		
		// returns a reference to a new instance of this board
		board &clone() { return *(new board(*this)); }
		
		// flip the board (switch sides) and return the reference to this board
		board &flip()
		{
			for (size_t i = 0; i < SIZE; ++i)
				for (size_t j = 0; j < SIZE; ++j)
					__cell[i][j] = -__cell[i][j];
			return *this;
		}
		
		// check if provided coordinate lie within the board
		bool in_range(size_t x, size_t y) const { return x < SIZE && y < SIZE; }
		bool in_range(coord k) const { return in_range(k.first, k.second); }
		
		// check if a move at the provided coordinate is valid
		bool valid(size_t x, size_t y) const { return in_range(x, y) && __cell[x][y] == 0; }
		bool valid(coord k) const { return valid(k.first, k.second); }
		
		// return reference to the board cell corresponding to provided coordinate
		char &cell(size_t x, size_t y) { assert(in_range(x, y)); return __cell[x][y]; }
		char &cell(coord k) { return cell(k.first, k.second); }

	}; // class board

	template<size_t SIZE, size_t COND>
	class game
	{
	
	private:
	
		std::vector<coord> move_stack;
		status game_status;
		board<SIZE> game_board;
		
		void print_status()
		{
			std::cout << "  Game status: ";
			switch (game_status)
			{
				case TURN_BLACK: std::cout << "TURN_BLACK"; break;
				case TURN_WHITE: std::cout << "TURN_WHITE"; break;
				case OVER_BLACK: std::cout << "OVER_BLACK"; break;
				case OVER_WHITE: std::cout << "OVER_WHITE"; break;
				case OVER_TIE: std::cout << "OVER_TIE";
			}
		}
		
		void print_last_move()
		{
			std::cout << "  Last move: ";
			if (move_stack.empty()) std::cout << "\033[37mN/A\033[0m";
			else std::cout << static_cast<char>(move_stack.back().first + 'A') << move_stack.back().second;
		}
		
	public:
	
		game() { game_status = TURN_BLACK; }
		
		// return the game's status
		status query() { return game_status; }
		
		// print game information
		void print()
		{
			for (size_t i = 0; i < 80; ++i)
				std::cout << '=';
			std::cout << '\n';
			
			for (size_t i = SIZE - 1; i < SIZE; --i)
			{
				if (i + 1 < 10) std::cout << ' ' << i + 1;
				else std::cout << i + 1;
				for (size_t j = 0; j < SIZE; ++j)
				{
					char format = ' ';
					if (!move_stack.empty())
						if (move_stack.back().first == i && move_stack.back().second == j)
							format = '[';
						else if (move_stack.back().first == i && move_stack.back().second + 1 == j)
							format = ']';
					switch (game_board.cell(i, j))
					{
						case  1: std::cout << format << "X"; break;
						case -1: std::cout << format << "O"; break;
						case  0: std::cout << format << "\033[90m.\033[0m";
					}
				}
				
				switch (SIZE - i)
				{
					case 1: std::cout << "  GOMOKU SERVER"; break;
					case 2: std::cout << "  \033[37mhttps://github.com/KonaeAkira/gomoku-server\033[0m"; break;
					case 4: print_status(); break;
					case 5: print_last_move(); break;
					case 7: std::cout << "  Black (X): " << PLAYER_BLACK; break;
					case 8: std::cout << "  White (O): " << PLAYER_WHITE; break;
				}
				
				std::cout << '\n';
			}
			
			std::cout << "   ";
			for (size_t j = 0; j < SIZE; ++j)
				std::cout << static_cast<char>(j + 'A') << ' ';
			std::cout << '\n';
			
			for (size_t i = 0; i < 80; ++i)
				std::cout << '=';
			std::cout << '\n';
		}
		
		// check if game is ongoing
		bool ongoing() const
		{
			switch (game_status)
			{
				case TURN_BLACK:
				case TURN_WHITE:
					return true;
				default: return false;
			}
		}
		
		// check if game is over
		bool over() const
		{
			switch (game_status)
			{
				case TURN_BLACK:
				case TURN_WHITE:
					return false;
				default: return true;
			}
		}
		
		// play a move
		void move(size_t x, size_t y)
		{
			if (game_board.valid(x, y))
			{
				char own_piece;
				switch (game_status)
				{
					case TURN_BLACK: own_piece = 1; break;
					case TURN_WHITE: own_piece = -1; break;
					default:
						std::cerr << "Error: Cannot make move (game already over)\n";
						return;
				}
				// set the piece on the board
				game_board.cell(x, y) = own_piece;
				// push the move onto the stack
				move_stack.push_back(std::make_pair(x, y));
				// check for `COND` consecutive pieces
				char cnt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
				for (size_t d = 0; d < 8; ++d)
				{
					size_t i = x + dx[d],
					       j = y + dy[d];
					while (game_board.in_range(i, j) && game_board.cell(i, j) == own_piece)
					{
						++cnt[d];
						i += dx[d];
						j += dy[d];
					}
				}
				for (size_t d = 0; d < 4; ++d)
					// found exactly `COND` consecutive pieces
					if (cnt[d] + cnt[d + 4] + 1 == COND)
					{
						game_status = (game_status == TURN_BLACK ? OVER_BLACK : OVER_WHITE);
						return;
					}
				// no player has won, check for tie
				if (move_stack.size() == SIZE * SIZE)
					game_status = OVER_TIE;
				else if (game_status == TURN_BLACK)
					game_status = TURN_WHITE;
				else game_status = TURN_BLACK;
			}
			else std::cerr << "Error: Cannot make move (" << static_cast<char>('A' + y) << x + 1 << " is not a valid move)\n";
		}
		void move(coord k) { move(k.first, k.second); }

	private:

		void undo_once()
		{
			coord k = move_stack.back();
			move_stack.pop_back();
			switch (game_board.cell(k))
			{
				case -1: game_status = TURN_WHITE; break;
				case  1: game_status = TURN_BLACK; break;
			}
			game_board.cell(k) = 0;
		}
		
	public:

		void undo(size_t count = 1)
		{
			while (count--)
				if (move_stack.empty())
				{
					std::cerr << "Error: Cannot undo (no more moves to undo)\n";
					return;
				}
				else undo_once();
		}
	
	}; // class game
	
} // namespace game

#endif // GAME_HPP
