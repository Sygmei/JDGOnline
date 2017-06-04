#include "main.hpp"


int main(int argc, char** argv)
{
	Card::Init();
	Deck::Init();
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "JDGO");

	Deck deck = Deck("firstdeck");
	deck.loadDeck();
	deck.shuffle();

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

        for (Card& card : deck.getCards())
		    window.draw(card);
		
		window.display();
	}
    return 0;
}
