#include "CardEquipement.hpp"

CardEquipement::CardEquipement(vili::ComplexAttribute& cardObject) : Card(cardObject)
{
	m_mod_atk = cardObject.at<vili::BaseAttribute>("mod_atk").get<std::string>();
	m_mod_def = cardObject.at<vili::BaseAttribute>("mod_def").get<std::string>();
}

const std::string& CardEquipement::getModAtk() const
{
	return m_mod_atk;
}

const std::string& CardEquipement::getModDef() const
{
	return m_mod_def;
}
