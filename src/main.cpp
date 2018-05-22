#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>

#include "game.hpp"
#include "subprocess.hpp"
#include "player.hpp"

#include "http_server.hpp"

game::game<15, 5> current_game;
std::string PLAYER_BLACK = "N/A",
            PLAYER_WHITE = "N/A";

std::vector<game::coord> get_all_moves()
{
	std::vector<game::coord> vector;
	game::coord result = {0, 0};
	for (int i = 0; result != game::NIL; ++i)
		vector.push_back(result = current_game.get_move(i));
	vector.pop_back();
	return vector;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: gomoku-server <player1> <player2>\n";
		return 0;
	}
	
	// launch players
	player black(argv[1]);
    if (black.name != "") PLAYER_BLACK = black.name;
    player white(argv[2]);
    if (white.name != "") PLAYER_WHITE = white.name;   

	// start http and socket servers
	http_server::start();
    	
    black.new_game(15, 5, "BLACK");
    white.new_game(15, 5, "WHITE");
	
	while (current_game.ongoing())
	{
		if (current_game.query() == game::TURN_BLACK)
		{
			game::coord move = black.get_move();
			current_game.move(move);
			if (current_game.query() == game::TURN_BLACK)
			{
				std::cout << "Black gave invalid move!\n";
				break;
			}
			white.notify_move(move);
			http_server::broadcast_move(move.first, move.second, "BLACK");
		}
		else if (current_game.query() == game::TURN_WHITE)
		{
			game::coord move = white.get_move();
			current_game.move(move);
			if (current_game.query() == game::TURN_WHITE)
			{
				std::cout << "White gave invalid move!\n";
				break;
			}
			black.notify_move(move);
			http_server::broadcast_move(move.first, move.second, "WHITE");
		}
	}
	
	black.terminate();
	white.terminate();
	
	std::cout << "Press ENTER to continue...\n";
	std::cin.get();
}
