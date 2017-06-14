#pragma once
#include "Card.hpp"

class CardEquipement : public Card
{
public:
	CardEquipement(vili::ComplexAttribute& cardObject);
	const std::string& CardEquipement::getModAtk() const;
	const std::string& CardEquipement::getModDef() const;

private:
	std::string m_mod_atk;
	std::string m_mod_def;
};
