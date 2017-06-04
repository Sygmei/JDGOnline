#pragma once
#include "Card.hpp"


std::vector<std::string> listFileInDir(const std::string& path);

class Deck
{
public:
	static void Init();

	Deck(std::string name);
	void shuffle();
	Card drawCard();
	void loadDeck();

	std::vector<Card> m_deck;


private:
	static vili::DataParser deckFile;

	//std::vector<Card> m_deck;
	std::string m_name;
};
