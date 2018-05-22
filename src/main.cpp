#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <thread>

#include "game.hpp"
#include "subprocess.hpp"
#include "player.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "http/server.hpp"

const char HTTP_HOST[] = "192.168.1.102",
           HTTP_PORT[] = "8888",
           HTTP_ROOT[] = "src/web";

game::game<15, 5> current_game;
std::string PLAYER_BLACK = "N/A",
            PLAYER_WHITE = "N/A",
            tmp_string;

std::string get_move(int index)
{
	std::string result;
	game::coord move = current_game.get_move(index);
	if (move == game::NIL) result = "NIL";
	else result = std::to_string(move.first) + "," + std::to_string(move.second);
	return result;
}

std::string get_name_black() { return PLAYER_BLACK; }
std::string get_name_white() { return PLAYER_WHITE; }

std::string get_status()
{
	switch (current_game.query())
	{
		case game::TURN_BLACK: return "TURN_BLACK";
		case game::TURN_WHITE: return "TURN_WHITE";
		case game::OVER_BLACK: return "OVER_BLACK";
		case game::OVER_WHITE: return "OVER_WHITE";
		case game::OVER_TIE: return "OVER_TIE";
	}
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

	// start http server
	http::server::server http_server(HTTP_HOST, HTTP_PORT, HTTP_ROOT);
	std::thread server_thread(&http::server::server::run, &http_server);
	server_thread.detach(); 
    	
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
		}
	}
	
	black.terminate();
	white.terminate();
	
	std::cout << "Press ENTER to continue...\n";
	std::cin.get();
}
