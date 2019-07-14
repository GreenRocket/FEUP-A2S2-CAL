/*
 * Change.cpp
 */

#include "Change.h"
#include <iostream>
#include <iomanip>

using namespace std;

#define PRINTHEADER() 									\
	for(int k = 0; k <= m; k++) cout << setw (4) << k;	\
	cout << setw (4) << "|";							\
	for(int k = 0; k <= m; k++) cout << setw (4) << k;	\
	cout << endl << endl;

#define PRINTLINE()												\
	for(int k = 0; k <= m; k++) cout << setw (4) << minCoins[k];\
	cout << setw (4) << "|";									\
	for(int k = 0; k <= m; k++) cout << setw (4) << lastCoin[k];\
	cout << endl;												\

string calcChange(int m, int numCoins, int *coinValues)
{
	PRINTHEADER();

	int biggestCoin;
	int remainderMinCoins;
	int epsilon = m + 1;
	int minCoins[m + 1];
	int lastCoin[m + 1];

	for(int i = 0; i <= numCoins; i++)							// Sets of available coins in increasing order.
	{
		biggestCoin = (i == 0 ? 0 : coinValues[i - 1]);			// Biggest coin available from this iteration's set.
		for(int k = 0; k <= m; k++)								// Amount of change in increasing order.
		{
			if (i == 0)											// Fill arrays with base case, i.e., no coins available.
			{
				minCoins[k] = (k == 0 ? 0 : epsilon);			// minCoins is always 0 for change=0; epsilon(m+1) is used to define an impossible case.
				lastCoin[k] = 0;								// lastCoin can simply use 0 for impossible cases.
			}

			else if (k >= biggestCoin)							// If the current change is higher than the biggest coin available in this iteration's set, we can use that coin.
			{
				remainderMinCoins = minCoins[k - biggestCoin];  // Number of coins needed for the remainder change, i.e., the amount left after subtracting the biggest coin
				if (1 + remainderMinCoins < minCoins[k])		// If that number of coins plus one is less than the number of coins from the previous iteration:
				{
					minCoins[k] = 1 + remainderMinCoins;		// Update the minimum number of coins for this amount of change.
					lastCoin[k] = biggestCoin;					// Update the last coin used to the biggest coin from this iteration's set.
				}
			}
		}
		PRINTLINE();
	}
	cout << endl << endl;

	ostringstream oss;
	if (minCoins[m] == epsilon)
		oss << '-';
	else
	{
		for(int i = m; i > 0; i -= lastCoin[i])
			oss << lastCoin[i] << ";";
	}
    return oss.str();
}
