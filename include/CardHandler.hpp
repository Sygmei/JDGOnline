#pragma once
#include "CardManager.hpp"

class CardHandler
{
public:
	CardHandler(CardManager* cardManager);
	const std::unique_ptr<Card>& getCard(unsigned int cardPosition) const;
	void addCards(std::vector<unsigned int> cards) const;
	const unsigned int& getMapPosition() const;
	void moveCardTo(unsigned int cardPosition, CardHandler* handlerDestination, unsigned int destination) const;
	const unsigned int lastPosition() const;


private:
	CardManager* m_manager;
	unsigned int m_map_position;
};
