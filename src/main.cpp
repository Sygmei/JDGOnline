#include "main.hpp"

int main(int argc, char** argv)
{
	std::cout << argv[0] << std::endl;
	int a;
	std::ifstream useFile;
	useFile.open("cards/listcard.txt");
	std::string currentLine;
	if (useFile.is_open())
	{
		while (getline(useFile, currentLine))
		{
			std::cout << currentLine << std::endl;
		}
	}
	else
	{
		std::cout << "A PAS TROUVEEEE" << std::endl;
	}
	std::cin >> a;
    return 0;
}