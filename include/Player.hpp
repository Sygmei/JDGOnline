#pragma once
#include "CardHandler.hpp"
#include "Card.hpp"

#define HP_MAX 60


class Player
{
public:
	Player(std::string deck, CardManager* cardManager);

	void drawCard();
	bool useCard(unsigned int cardPosition);
	bool useCard(unsigned int cardPosition, unsigned int destination, Player* player);
	void placeCard(unsigned int cardPosition, unsigned int destination, Player* player) const;

	const unsigned int& getHP() const;
	void setHP(unsigned int hp);
	const Player* getOpponent() const;
	void setOpponent(Player* opponent);
	const CardHandler& getHand() const;
	const CardHandler& getDeck() const;
	CardHandler& getGraveyard();
	CardHandler& getInvocations();
	CardHandler& getEquipements();
	CardHandler& getEffects();
	CardHandler& getField();
	

private:
	CardHandler m_hand;
	CardHandler m_deck;
	CardHandler m_graveyard;
	CardHandler m_invocations;
	CardHandler m_equipements;
	CardHandler m_effects;
	CardHandler m_field;

	Player* m_opponent;
	
	unsigned int m_hp;
};
