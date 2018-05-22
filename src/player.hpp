#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>

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

}; // class player

#endif // PLAYER_HPP
