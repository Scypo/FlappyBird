#include"Game.h"
#include<iostream>

int main()
{
	bool quit = false;
	while (!quit)
	{
		int choice = 0;
		system("cls");
		std::cout << "Select Mode:\n 1)Play Flappy Bird\n 2)Train NeuralNet\n 3)Quit" << std::endl;
		std::cin >> choice;
		system("cls");
		switch (choice)
		{
		case 1:
		{
			PlayableGame game;
			game.Run();
			break;
		}
		case 2:
		{
			TrainingGame game;
			game.Run();
			break;
		}
		case 3:
			quit = true;
			break;
		default:
			break;
		}
	}

	return 0;
}