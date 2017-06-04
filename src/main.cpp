#include "main.hpp"


int main(int argc, char** argv)
{
	Card::Init();
	Deck::Init();
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "JDGO");

	//Card carte = Card(2);
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

		window.clear();
		sf::Transform t1;
		t1.translate(100, 0);
		sf::Transform t2;
		t2.translate(200, 0);
		sf::Transform t3;
		t3.translate(300, 0);

		window.draw(deck.m_deck.at(0), t1);
		window.draw(deck.m_deck.at(1), t2);
		window.draw(deck.m_deck.at(2), t3);
		
		window.display();
	}
    return 0;
}
