#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>

#include "game.hpp"
#include "player.hpp"

#include "http_server.hpp"

const size_t BOARD_SIZE = 15;
const size_t BOARD_COND = 5;

game::game<BOARD_SIZE, BOARD_COND> current_game;
std::string PLAYER_BLACK = "N/A",
            PLAYER_WHITE = "N/A";
std::stringstream ss;

player *black, *white;

void init_game_info()
{	
	ss << "BOARD," << BOARD_SIZE << ',' << BOARD_COND;
	http_server::broadcast(ss.str()); ss.str(std::string());
	ss << "PLAYER,BLACK," << PLAYER_BLACK;
	http_server::broadcast(ss.str()); ss.str(std::string());
	ss << "PLAYER,WHITE," << PLAYER_WHITE;
	http_server::broadcast(ss.str()); ss.str(std::string());
    ss << "STATUS," << current_game.query();
    http_server::broadcast(ss.str()); ss.str(std::string());
}

void run_game()
{
	black->new_game(BOARD_SIZE, BOARD_COND, "BLACK");
	white->new_game(BOARD_SIZE, BOARD_COND, "WHITE");
	
	std::cout << "Starting new game between \"" << PLAYER_BLACK << "\" and \"" << PLAYER_WHITE << "\"" << std::endl;
	
	while (current_game.ongoing())
	{
		if (current_game.query() == game::TURN_BLACK)
		{
			game::coord move = black->get_move();
			current_game.move(move);
			if (current_game.query() == game::TURN_BLACK)
			{
				std::cout << "Black gave invalid move!\n";
				break;
			}
			white->notify_move(move);
			ss << "MOVE,BLACK," << move.first << ',' << move.second;
			http_server::broadcast(ss.str()); ss.str(std::string());
			ss << "STATUS," << current_game.query();
			http_server::broadcast(ss.str()); ss.str(std::string());
		}
		else if (current_game.query() == game::TURN_WHITE)
		{
			game::coord move = white->get_move();
			current_game.move(move);
			if (current_game.query() == game::TURN_WHITE)
			{
				std::cout << "White gave invalid move!\n";
				break;
			}
			black->notify_move(move);
			ss << "MOVE,WHITE," << move.first << ',' << move.second;
			http_server::broadcast(ss.str()); ss.str(std::string());
			ss << "STATUS," << current_game.query();
			http_server::broadcast(ss.str()); ss.str(std::string());
		}
	}
	
	black->stop();
	white->stop();
}

int main(int argc, char **argv)
{
	if (argc != 3 && argc != 4)
	{
		std::cerr << "Usage: gomoku-server <player1> <player2> [port]" << std::endl;
		return 0;
	}
	
	// launch players
	black = new player(argv[1]);
    if (black->name != "") PLAYER_BLACK = black->name;
    white = new player(argv[2]);
    if (white->name != "") PLAYER_WHITE = white->name;
	
	init_game_info();
	
	std::thread game_host(run_game);
	game_host.detach();

	// start http and socket servers
	int port = 8000;
	if (argc == 4)
		port = std::stoi(argv[3]);
	try
	{
		http_server::start(port);
	}
	catch (...)
	{
		std::cerr << "Failed to start HTTP Telemetry Server. Is the port used by another process?" << std::endl;
		black->terminate();
		white->terminate();
		return 0;
	}
}
