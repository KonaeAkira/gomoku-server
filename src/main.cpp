#include <iostream>
#include <string>
#include <cassert>

#include "game.hpp"
#include "subprocess.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "http/server.hpp"

const char HTTP_HOST[] = "localhost",
           HTTP_PORT[] = "8888",
           HTTP_ROOT[] = "src/web";
const size_t HTTP_THREADS = 4;

game::game<15, 5> current_game;
std::string PLAYER_BLACK = "\033[41mOffline\033[0m",
            PLAYER_WHITE = "\033[41mOffline\033[0m",
            tmp_string;
std::ofstream live("src/web/live.log");
            
class player
{

private:

	spawn *process;

public:

	std::string name;

	player(std::string exec_file)
	{
		const char* const argv[] = {exec_file.c_str(), (const char*)0};
		process = new spawn(argv);
		std::string reply;
		process->stdin << "ABOUT" << std::endl;
		process->stdout >> reply;
		if (reply == "OK")
		{
			std::getline(process->stdout, name);
			while (!name.empty() && name[0] == ' ')
				name = name.substr(1, name.size() - 1);
		}
		else
		{
			std::getline(process->stdout, reply);
			std::cerr << reply << '\n';
		}
	}
	
	~player()
	{
		process->send_eof();
		process->wait();
	}
	
	void new_game(size_t size, size_t cond, std::string side)
	{
		process->stdin << "NEW " << size << ' ' << cond << ' ' << side << std::endl;
		std::string reply;
		process->stdout >> reply;
		if (reply != "OK")
		{
			std::getline(process->stdout, reply);
			std::cerr << reply << '\n';
		}
	}
	
	game::coord get_move()
	{
		process->stdin << "TURN" << std::endl;
		std::string reply;
		process->stdout >> reply;
		if (reply == "OK")
		{
			game::coord move;
			process->stdout >> move.first >> move.second;
			return move;
		}
		else
		{
			std::getline(process->stdout, reply);
			std::cerr << reply << '\n';
			return {-1, -1};
		}
	}
	
	void notify_move(game::coord move)
	{
		process->stdin << "MOVE " << move.first << ' ' << move.second << std::endl;
		std::string reply;
		process->stdout >> reply;
		if (reply != "OK")
		{
			std::getline(process->stdout, reply);
			std::cerr << reply << '\n';
		}
	}
	
	void terminate()
	{
		process->stdin << "EXIT" << std::endl;
	}

};

int main()
{
	// start http server
	http::server3::server http_server(HTTP_HOST, HTTP_PORT, HTTP_ROOT, HTTP_THREADS);
	http_server.run();

    player black("./test.out");
    if (black.name != "")
    	PLAYER_BLACK = black.name;
    
    player white("./test.out");
    if (white.name != "")
    	PLAYER_WHITE = white.name;    
    	
    black.new_game(15, 5, "BLACK");
    white.new_game(15, 5, "WHITE");
	
	current_game.print();
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
			else white.notify_move(move);
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
			else black.notify_move(move);
		}
		current_game.print();
	}
	
	black.terminate();
	white.terminate();
}
