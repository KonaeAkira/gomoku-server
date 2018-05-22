#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>

#include "game.hpp"
#include "subprocess.hpp"
#include "player.hpp"

#include "http_server.hpp"

const size_t BOARD_SIZE = 15;
const size_t BOARD_COND = 5;

game::game<BOARD_SIZE, BOARD_COND> current_game;
std::string PLAYER_BLACK = "N/A",
            PLAYER_WHITE = "N/A";
std::stringstream ss;

void init_game_info()
{	
	ss << "BOARD," << BOARD_SIZE << ',' << BOARD_COND;
	http_server::broadcast(ss.str()); ss.str(std::string());
	
	ss << "PLAYER,BLACK," << PLAYER_BLACK;
	http_server::broadcast(ss.str()); ss.str(std::string());
	
	ss << "PLAYER,WHITE," << PLAYER_WHITE;
	http_server::broadcast(ss.str()); ss.str(std::string());
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cout << "Usage: gomoku-server <port> <player1> <player2>\n";
		return 0;
	}
	
	// launch players
	player black(argv[2]);
    if (black.name != "") PLAYER_BLACK = black.name;
    player white(argv[3]);
    if (white.name != "") PLAYER_WHITE = white.name;   

	// start http and socket servers
	http_server::start(std::stoi(argv[1]));
	
	init_game_info();
    	
    black.new_game(BOARD_SIZE, BOARD_COND, "BLACK");
    white.new_game(BOARD_SIZE, BOARD_COND, "WHITE");
	
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
			ss << "MOVE,BLACK," << move.first << ',' << move.second;
			http_server::broadcast(ss.str()); ss.str(std::string());
			ss << "STATUS," << current_game.query();
			http_server::broadcast(ss.str()); ss.str(std::string());
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
			ss << "MOVE,WHITE," << move.first << ',' << move.second;
			http_server::broadcast(ss.str()); ss.str(std::string());
			ss << "STATUS," << current_game.query();
			http_server::broadcast(ss.str()); ss.str(std::string());
		}
	}
	
	black.terminate();
	white.terminate();
	
	std::cout << "Press ENTER to continue...\n";
	std::cin.get();
}
