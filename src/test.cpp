#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

int main()
{
	std::string command;
	while (!std::cin.eof())
	{
		std::cin >> command;
		if (command == "ABOUT")
		{
			std::cout << "OK Test Program" << std::endl;
			getline(std::cin, command);
		}
		else if (command == "NEW")
		{
			std::cout << "OK" << std::endl;
			getline(std::cin, command);
		}
		else if (command == "TURN")
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			std::cout << "OK " << (int)(rand() % 15) << ' ' << (int)(rand() % 15) << std::endl;
		}
		else if (command == "MOVE")
		{
			size_t x, y;
			std::cin >> x >> y;
			rand(); rand();
			std::cout << "OK" << std::endl;
		}
		else if (command == "EXIT")
			return 0;
		else
		{
			std::cout << "ERROR command not recognized!" << std::endl;
		}
	}
}
