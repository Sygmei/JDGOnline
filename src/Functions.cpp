#include "Functions.hpp"

namespace Functions
{
	std::unique_ptr<Card> buildCard(unsigned int id)
	{
		vili::ComplexAttribute& cardObject = Card::getCard(id);
		CardTypes::CardType type = convertStringToType(cardObject.at<vili::BaseAttribute>("type"));
		switch (type)
		{
		case CardTypes::Contre:
			return std::make_unique<CardContre>(cardObject);
		case CardTypes::Effet:
			return std::make_unique<CardEffet>(cardObject);
		case CardTypes::Equipement:
			return std::make_unique<CardEquipement>(cardObject);
		case CardTypes::Invocation:
			return std::make_unique<CardInvocation>(cardObject);
		case CardTypes::Terrain:
			return std::make_unique<CardTerrain>(cardObject);
		}
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
}