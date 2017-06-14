#include "main.hpp"


int main(int argc, char** argv)
{
	std::srand(unsigned(std::time(0)));
	Card::Init();
	CardManager cardManager = CardManager();
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "JDGO");

	
	
	Player player = Player("firstdeck", &cardManager);
	Player opponent = Player("firstdeck", &cardManager);
	player.setOpponent(&opponent);
	opponent.setOpponent(&player);
	int a;

	while(window.isOpen())
	{
		sf::Event event;

		while(window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		for (int i = 0; i < opponent.getOpponent()->getDeck().lastPosition(); i++)
		{
			window.draw(*opponent.getOpponent()->getDeck().getCard(i));
			std::cout << "name : " << opponent.getOpponent()->getDeck().getCard(i)->getName() << std::endl;
			if (opponent.getOpponent()->getDeck().getCard(i)->getType() == CardTypes::Invocation)
				std::cout << "atk : " << static_cast<CardInvocation*>(opponent.getOpponent()->getDeck().getCard(i).get())->getAtk() << std::endl;
			std::cin >> a;
		}

		window.display();
	}
    return 0;
}