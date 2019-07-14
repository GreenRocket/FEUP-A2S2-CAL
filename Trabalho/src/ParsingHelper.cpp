#include <iostream>
#include <sstream>
#include <cstring> //strerror
#include "ParsingHelper.h"

using namespace std;

namespace ParsingHelper
{
int iStreamSplitFixedSize(istream &target, vector<string> &container, char delimiter, size_t expectedSize)
{
	string element;
	while (getline(target, element, delimiter)) // Ler até encontrar 'delimiter', enquanto houver coisas para ler
	{
		// Remover espaços no inicio e fim
		element = removeLeadingTraillingWhitespaces(element);
		if (element.size() > 0)
			container.push_back(element);
	}
	if (container.size() != expectedSize)
	{
		while (container.size() < expectedSize)
		{
			container.push_back("ERROR");
		}
		return -1;
	}

	return 0;
}

int iStreamSplitUntilMarker(istream &target, vector<string> &container, char delimiter, size_t expectedSize, string marker)
{
	string element;
	while (getline(target, element, delimiter)) // Ler até encontrar 'delimiter', enquanto houver coisas para ler
	{
		// Parar ao encontrar 'MARKER'
		if (element.compare(marker) == 0 || container.size() == expectedSize)
			break;

		container.push_back(removeLeadingTraillingWhitespaces(element));
	}

	if (container.size() != expectedSize)
	{
		while (container.size() < expectedSize)
		{
			container.push_back("ERROR");
		}
		return -1;
	}

	return 0;
}

int stringSplit(string &target, vector<string> &container, char delimiter, size_t expectedSize)
{
	istringstream sStream(target);
	return iStreamSplitFixedSize(sStream, container, delimiter, expectedSize);
}

int openFileRead(ifstream &inputFile, string fileName)
{
	inputFile.open(fileName, ios::in);

	if (inputFile.fail() || !inputFile.is_open())
		return -1;
	return 0;
}

int openFileWrite(ofstream &outputFile, string fileName)
{
	outputFile.open(fileName, ios::out | ios::trunc);

	if (outputFile.fail() || !outputFile.is_open())
		return -1;
	return 0;
}

string removeLeadingTraillingWhitespaces(string target)
{
	size_t first = target.find_first_not_of(" \t\f\v\n\r");
	size_t last = target.find_last_not_of(" \t\f\v\n\r");
	if (last - first + 1 < first)
		return "";
	string s = target.substr(first, last - first + 1);
	return s;
}

bool safeStoul(unsigned &num, const string &s, unsigned lowerBound, unsigned upperBound)
{
	try
	{
		num = stoul(s);
		return (num >= lowerBound && num <= upperBound);
	}
	catch (invalid_argument)
	{
		cerr << strerror(errno) << endl;
		return false;
	}
	catch (out_of_range)
	{
		cerr << strerror(errno) << endl;
		return false;
	}
}

} // namespace ParsingHelper