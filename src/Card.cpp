#include "Card.hpp"

vili::DataParser Card::cardFile;
void Card::Init()
{
	cardFile.parseFile("cards/cards.vili");
}

Card::Card(unsigned int id)
{
	vili::ComplexAttribute& cardObject = cardFile.at("Cards", convertIdToViliPath(id));
	m_texture.loadFromFile("cards/" + convertIdToViliPath(id) + ".png");
	m_sprite.setTexture(m_texture);
	m_id = id;
	m_atk = cardObject.at<vili::BaseAttribute>("atk");
	m_def = cardObject.at<vili::BaseAttribute>("def");
	m_name = cardObject.at<vili::BaseAttribute>("name");
	m_quote = cardObject.at<vili::BaseAttribute>("quote");
	m_description = cardObject.at<vili::BaseAttribute>("description");
	m_type = convertStringToType(cardObject.at<vili::BaseAttribute>("type"));
	for (vili::BaseAttribute* family : cardObject.at<vili::ListAttribute>("families"))
		m_families.push_back(convertStringToFamily(*family));
    m_sprite.setPosition(sf::Vector2f(m_id * 50, 0));
}

void Card::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

const unsigned& Card::getId() const
{
	return m_id;
}

const unsigned& Card::getAtk() const
{
	return m_atk;
}

const unsigned& Card::getDef() const
{
	return m_def;
}

const std::string& Card::getName() const
{
	return m_name;
}

const std::string& Card::getQuote() const
{
	return m_quote;
}

const std::string& Card::getDescription() const
{
	return m_description;
}

const CardTypes::CardType& Card::getType() const
{
	return m_type;
}

const std::vector<CardFamilies::CardFamily>& Card::getFamilies() const
{
	return m_families;
}

std::string convertIdToViliPath(unsigned int id)
{
	return std::string(4 - std::to_string(id).size(), '0') + std::to_string(id);
}

CardFamilies::CardFamily convertStringToFamily(const std::string& family)
{
	if (family == "Humain")
		return CardFamilies::Humain;
	if (family == "Japon")
		return CardFamilies::Japon;
	if (family == "Incarnation")
		return CardFamilies::Incarnation;
	if (family == "RPG")
		return CardFamilies::RPG;
	if (family == "Comics")
		return CardFamilies::Comics;
	if (family == "Fistiland")
		return CardFamilies::Fistiland;
	if (family == "Monstre")
		return CardFamilies::Monstre;
	return CardFamilies::Spatial;
}

CardTypes::CardType convertStringToType(const std::string & type)
{
	if (type == "Invocation")
		return CardTypes::Invocation;
	if (type == "Effet")
		return CardTypes::Effet;
	if (type == "Equipement")
		return CardTypes::Equipement;
	if (type == "Contre")
		return CardTypes::Contre;
	return CardTypes::Terrain;
}
