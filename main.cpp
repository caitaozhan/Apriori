#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "Apriori.h"

using namespace std;

int main()
{
	Apriori apriori(0.5, 0.75);
	apriori.inputTransactions("transactionData.txt");
	apriori.findAllFrequentItemSets();
	apriori.findStrongestAssociateRules();
	apriori.printRules("associateRule.txt");

	return 0;
}
