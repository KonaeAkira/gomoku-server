#pragma once

#include <iostream>
#include <mutex>

#include "subprocess.hpp"

class player
{

private:

	spawn *process;
	std::mutex mutex;

public:

	std::string name;
	
	player() {}

	player(std::string exec_file)
	{
		std::lock_guard<std::mutex> lock(mutex);
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
			std::clog << "Spawned " << name << " (PID " << process->child_pid << ")" << std::endl;
		}
		else
		{
			std::getline(process->stdout, reply);
			std::cerr << "[" << name << "] " << reply << std::endl;
		}
	}
	
	~player() { terminate(); }
	
	void new_game(size_t size, size_t cond, &std::string side)
	{
		mutex.lock();
		process->stdin << "NEW " << size << ' ' << cond << ' ' << side << std::endl;
		mutex.unlock();
		
		std::string reply;
		process->stdout >> reply;
		if (reply != "OK")
		{
			std::getline(process->stdout, reply);
			std::cerr << "[" << name << "] " << reply << std::endl;
		}
	}
	
	game::coord get_move()
	{
		mutex.lock();
		process->stdin << "TURN" << std::endl;
		mutex.unlock();
		
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
			std::cerr << "[" << name << "] " << reply << std::endl;
			return {-1, -1};
		}
	}
	
	void notify_move(game::coord move)
	{
		mutex.lock();
		process->stdin << "MOVE " << move.first << ' ' << move.second << std::endl;
		mutex.unlock();
		
		std::string reply;
		process->stdout >> reply;
		if (reply != "OK")
		{
			std::getline(process->stdout, reply);
			std::cerr << "[" << name << "] " << reply << std::endl;
		}
	}
	
	void terminate()
	{
		std::lock_guard<std::mutex> lock(mutex);
		kill(process->child_pid, 15);
		std::clog << "Terminated " << name << " (PID " << process->child_pid << ")" << std::endl;
		delete process;
	}
	
	void stop()
	{
		std::lock_guard<std::mutex> lock(mutex);
		process->stdin << "EXIT" << std::endl;
	}

}; // class player
