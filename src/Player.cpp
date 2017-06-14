#include "Player.hpp"

Player::Player(std::string deck, CardManager* cardManager) : m_hand(cardManager), m_deck(cardManager), m_graveyard(cardManager), m_invocations(cardManager), m_equipements(cardManager), m_effects(cardManager), m_field(cardManager)
{
	m_hp = HP_MAX;

	std::vector<unsigned int> cards = cardManager->loadDeck(deck);
	m_deck.addCards(cards);

}

void Player::drawCard()
{
	m_deck.moveCardTo(m_deck.lastPosition(), &m_hand, m_hand.lastPosition()+1);
}

bool Player::useCard(unsigned int cardPosition)
{
	return true;
}

bool Player::useCard(unsigned int cardPosition, unsigned int destination, Player* player)
{
	return true;
}

void Player::placeCard(unsigned int cardPosition, unsigned int destination, Player* player) const
{
	switch (m_hand.getCard(cardPosition)->getType())
	{
		case CardTypes::Contre:
			break;
		case CardTypes::Effet:
			break;
		case CardTypes::Equipement:
			m_hand.moveCardTo(cardPosition, &player->getEquipements(), destination);
			break;
		case CardTypes::Invocation:
			m_invocations.moveCardTo(cardPosition, &player->getInvocations(), destination);
			break;
		case CardTypes::Terrain:
			m_field.moveCardTo(cardPosition, &player->getField(), destination);
			break;
	}
}




const unsigned int& Player::getHP() const
{
	return m_hp;
}

void Player::setHP(unsigned int hp)
{
	if (hp <= HP_MAX)
		m_hp = hp;
}

const Player* Player::getOpponent() const
{
	return m_opponent;
}

void Player::setOpponent(Player* opponent)
{
	m_opponent = opponent;
}

const CardHandler& Player::getHand() const
{
	return m_hand;
}

const CardHandler& Player::getDeck() const
{
	return m_deck;
}

CardHandler& Player::getGraveyard()
{
	return m_graveyard;
}

CardHandler& Player::getInvocations()
{
	return m_invocations;
}

CardHandler& Player::getEquipements() 
{
	return m_equipements;
}

CardHandler& Player::getEffects()
{
	return m_effects;
}

CardHandler& Player::getField()
{
	return m_field;
}
