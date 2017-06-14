#include "Card.hpp"
#include <string>

vili::DataParser Card::cardFile;
void Card::Init()
{
	cardFile.parseFile("cards/cards.vili");
}

vili::ComplexAttribute& Card::getCard(unsigned int id)
{
	return cardFile.at("Cards", convertIdToViliPath(id));
}

Card::Card(vili::ComplexAttribute& cardObject)
{
	m_texture.loadFromFile("cards/" + cardObject.getID() + ".png");
	m_sprite.setTexture(m_texture);
	m_id = stoi(cardObject.getID());
	m_name = cardObject.at<vili::BaseAttribute>("name").get<std::string>();
	m_quote = cardObject.at<vili::BaseAttribute>("quote").get<std::string>();
	m_description = cardObject.at<vili::BaseAttribute>("description").get<std::string>();
	m_type = convertStringToType(cardObject.at<vili::BaseAttribute>("type"));
	m_collector = cardObject.at<vili::BaseAttribute>("collector");
    m_sprite.setPosition(sf::Vector2f(m_id * 50, 0)); //A suppr
	
}

void Card::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

const unsigned& Card::getId() const
{
	return m_id;
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

const bool& Card::getCollector() const
{
	return m_collector;
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

Card::~Card() {}
