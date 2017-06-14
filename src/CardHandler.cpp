#include "CardHandler.hpp"

CardHandler::CardHandler(CardManager* cardManager)
{
	m_manager = cardManager;
	m_map_position = m_manager->attributeMap();
}

const std::unique_ptr<Card>& CardHandler::getCard(unsigned int cardPosition) const
{
	return m_manager->getCard(m_map_position, cardPosition);
}

void CardHandler::addCards(std::vector<unsigned int> cards) const
{
	for (int i = 0; i < cards.size(); i++)
	{
		m_manager->setCard(m_map_position, i, cards.at(i));
	};
}

const unsigned int& CardHandler::getMapPosition() const
{
	return m_map_position;
}

void CardHandler::moveCardTo(unsigned int cardPosition, CardHandler* handlerDestination, unsigned int destination) const
{
	const std::unique_ptr<Card>& card = m_manager->getCard(m_map_position, cardPosition);
	unsigned int cardId = card->getId();
	m_manager->eraseCard(m_map_position, cardPosition);
	m_manager->setCard(handlerDestination->getMapPosition(), destination, cardId);
}

const unsigned int CardHandler::lastPosition() const
{
	return m_manager->getSize(m_map_position);
}
