#include "CardInvocation.hpp"

CardInvocation::CardInvocation(vili::ComplexAttribute& cardObject) : Card(cardObject)
{
	m_atk = cardObject.at<vili::BaseAttribute>("atk");
	m_def = cardObject.at<vili::BaseAttribute>("def");
	for (vili::BaseAttribute* family : cardObject.at<vili::ListAttribute>("families"))
		m_families.push_back(convertStringToFamily(*family));
}

const unsigned& CardInvocation::getAtk() const
{
	return m_atk;
}

const unsigned& CardInvocation::getDef() const
{
	return m_def;
}

const std::vector<CardFamilies::CardFamily>& CardInvocation::getFamilies() const
{
	return m_families;
}

void CardInvocation::equip(CardEquipement* equipement)
{
	m_equipement = equipement;
}

const CardEquipement* CardInvocation::getEquipement() const
{
	return m_equipement;
}
