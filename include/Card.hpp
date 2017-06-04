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

	Card(unsigned int id);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	const unsigned& getId() const;
	const unsigned& getAtk() const;
	const unsigned& getDef() const;
	const std::string& getName() const;
	const std::string& getQuote() const;
	const std::string& getDescription() const;
	const CardTypes::CardType& getType() const;
	const std::vector<CardFamilies::CardFamily>& getFamilies() const;

private:
	static vili::DataParser cardFile;

	sf::Texture m_texture;
	sf::Sprite m_sprite;
	unsigned int m_id;
	unsigned int m_atk;
	unsigned int m_def;
	std::string m_name;
	std::string m_quote;
	std::string m_description;
	CardTypes::CardType m_type;
	std::vector<CardFamilies::CardFamily> m_families;
};