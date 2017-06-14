#include "CardManager.hpp"

vili::DataParser CardManager::deckFile;

CardManager::CardManager()
{
	for (std::string& file : Functions::listFileInDir("decks"))
	{
		if (file.rfind(".vili") == (file.size() - 5))
		{
			deckFile.parseFile("decks/" + file);
		}
	}
}

const std::unique_ptr<Card>& CardManager::getCard(unsigned int mapPosition, unsigned int cardPosition) const
{
	unsigned int cardId = m_cards_pos.at(mapPosition).at(cardPosition);
	return m_cards.at(cardId);

}

void CardManager::setCard(unsigned int mapPosition, unsigned int cardPosition, unsigned int cardId)
{
	m_cards_pos.at(mapPosition).insert(std::pair<unsigned int,unsigned int>(cardPosition, cardId));
}

void CardManager::eraseCard(unsigned int mapPosition, unsigned int cardPosition)
{
	m_cards_pos.at(mapPosition).erase(cardPosition);
}

std::vector<unsigned int> CardManager::loadDeck(std::string deckName)
{
	std::vector<unsigned int> cardsIds;
	bool stop;
	vili::ComplexAttribute& deckObject = deckFile.at("Decks", deckName);
	for (vili::BaseAttribute* id : deckObject.at<vili::ListAttribute>("cards")) {
		stop = false;
		for (int i = 0; i < m_cards.size(); i++)
		{
			if (m_cards.at(i)->getId() == int(*id))
			{
				cardsIds.push_back(i);
				stop = true;
				break;
			}
		}
		if (!stop) {
			cardsIds.push_back(m_cards.size());
			m_cards.push_back(Functions::buildCard(*id));
		}
	}
	return cardsIds;
}

unsigned int CardManager::getSize(unsigned int mapPosition)
{
	return m_cards_pos.at(mapPosition).size();
}

unsigned int CardManager::attributeMap()
{
	m_cards_pos.push_back(std::map<unsigned int, unsigned int>());
	return m_cards_pos.size()-1;
}

