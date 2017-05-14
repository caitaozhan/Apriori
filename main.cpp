#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "Item.h"
#include "Apriori.h"

using namespace std;

int main()
{
	Apriori apriori(0.5, 0.75);
	apriori.inputTransactions("transactionData.txt");
	apriori.findAllFrequentItemSets();
	apriori.findStrongestAssociateRules();



	/*
	vector<Item> vec;
	vec.emplace_back(Item(1, 1));
	vec.emplace_back(Item(1, 1.5));
	vec.emplace_back(Item(0.5, 1));
	vec.emplace_back(Item(1, 0.8));
	vec.emplace_back(Item(2, -1));

	sort(vec.begin(), vec.end());

	cout << vec.size() << endl;

	map<Item, int> myMap;
	for (int i = 0; i < vec.size(); ++i)
	{
		myMap[vec[i]] = i;
	}

	cout << vec.size() << endl;
	*/

	return 0;
}
