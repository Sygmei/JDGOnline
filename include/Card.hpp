#pragma once
#include <SFML/Graphics.hpp>
#include <vili/Vili.hpp>


namespace CardFamilies
{
	enum CardFamily
	{
		Humain,
		Japon,
		Incarnation,
		RPG,
		Comics,
		Fistiland,
		Monstre,
		Spatial
	};
}
	
namespace CardTypes
{
	enum CardType
	{
		Invocation,
		Effet,
		Equipement,
		Contre,
		Terrain
	};
}

std::string convertIdToViliPath(unsigned int id);
CardFamilies::CardFamily convertStringToFamily(const std::string& family);
CardTypes::CardType convertStringToType(const std::string& type);

class Card : public sf::Drawable
{
public:
	static void Init();
	static vili::ComplexAttribute&  Card::getCard(unsigned int id);

	Card(vili::ComplexAttribute& cardObject);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	const unsigned& getId() const;
	const std::string& getName() const;
	const std::string& getQuote() const;
	const std::string& getDescription() const;
	const CardTypes::CardType& getType() const;
	const bool& getCollector() const;

	virtual ~Card() = 0;
	
private:
	static vili::DataParser cardFile;

	sf::Sprite m_sprite;
    sf::Texture m_texture;
	unsigned int m_id;
	std::string m_name;
	std::string m_quote;
	std::string m_description;
	bool m_collector;
	CardTypes::CardType m_type;
};