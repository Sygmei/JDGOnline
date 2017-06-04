#include "Deck.hpp"
#include "tinydir.h"

#define CARDLIMIT 40

vili::DataParser Deck::deckFile;
void Deck::Init()
{
	for (std::string& file : listFileInDir("decks"))
	{
		if (file.rfind(".vili") == (file.size() - 5))
		{
			deckFile.parseFile("decks/"+file);
		}
	}
}

Deck::Deck(std::string name)
{
	m_name = name;
    m_deck.reserve(40);
}

void Deck::shuffle()
{
	std::random_shuffle(m_deck.begin(), m_deck.end());
}

Card Deck::drawCard()
{
	Card topCard = m_deck.back();
	//m_deck.pop_back();
	return topCard;
	//return Card(1);
}

void Deck::loadDeck()
{
	vili::ComplexAttribute& deckObject = deckFile.at("Decks", m_name);
	for (vili::BaseAttribute* card : deckObject.at<vili::ListAttribute>("cards"))
        m_deck.emplace_back(*card);
}

std::vector<Card>& Deck::getCards()
{
    return m_deck;
}

std::vector<std::string> listFileInDir(const std::string& path)
{
	tinydir_dir dir;
	tinydir_open(&dir, path.c_str());

	std::vector<std::string> fileList;
	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir) { fileList.push_back(std::string(file.name)); }
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return fileList;
}
