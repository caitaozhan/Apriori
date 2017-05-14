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
	/*
	Apriori apriori(0.5, 0.75);
	apriori.inputTransactions("transactionData.txt");
	apriori.findAllFrequentItemSets();
	apriori.findStrongestAssociateRules();
	apriori.printRules("associateRule.txt");
	*/

	vector<Item> vec;
	vec.emplace_back(Item(1, 1));
	vec.emplace_back(Item(1, 2));
	vec.emplace_back(Item(-1, 3));
	vec.emplace_back(Item(1, 0));
	vec.emplace_back(Item(2, -1));
	sort(vec.begin(), vec.end());

	map<Item, int> myMap;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		myMap[vec[i]] = i;
	}

	Item item1(1, 1);
	Item item2(1, 1);
	if (item1 == item2)
	{
		cout << "equal" << endl;
	}

	Item item3(string("(10,20)"));
	
	ItemSet itemSet1;
	itemSet1.insert(Item(1, 1));
	itemSet1.insert(Item(1, 2));
	itemSet1.insert(Item(-1, 3));
	itemSet1.insert(Item(1, 0));
	itemSet1.insert(Item(2, -1));

	ItemSet itemSet2;
	itemSet2.insert(Item(1, 1));
	itemSet2.insert(Item(1, 2));
	itemSet2.insert(Item(-1, 3));
	itemSet2.insert(Item(1, 0));
	itemSet2.insert(Item(2, -1));

	if (itemSet1 == itemSet2)
	{
		cout << "equal" << endl;
	}
	
	return 0;
}
