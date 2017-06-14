#pragma once

#include "Card.hpp"
#include "Functions.hpp"

class CardManager
{
public:
	CardManager();
	
	const std::unique_ptr<Card>& getCard(unsigned int mapPosition, unsigned int cardPosition) const;
	void setCard(unsigned int mapPosition, unsigned int cardPosition, unsigned int cardId);
	void eraseCard(unsigned int mapPosition, unsigned int cardPosition);
	std::vector<unsigned int> loadDeck(std::string deck);
	unsigned int getSize(unsigned int mapPosition);
	unsigned int attributeMap();

private:
	static vili::DataParser deckFile;

	std::vector<std::unique_ptr<Card>> m_cards;
	std::vector<std::map<unsigned int, unsigned int>> m_cards_pos;


};
