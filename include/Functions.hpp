#pragma once
#include "Card.hpp"
#include "CardContre.hpp"
#include "CardEffet.hpp"
#include "CardEquipement.hpp"
#include "CardInvocation.hpp"
#include "CardTerrain.hpp"
#include <tinydir/tinydir.h>


namespace Functions
{
	std::unique_ptr<Card> buildCard(unsigned int id);
	std::vector<std::string> listFileInDir(const std::string& path);
}