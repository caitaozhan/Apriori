#pragma once
#include <iostream>
#include <set>
#include <string>
#include "Item.h"

using namespace std;

class ItemSet
{
public:
	ItemSet();
	
	void insert(const Item & item);

	bool operator==(const ItemSet & ItemSet) const
	{
		return m_itemSet == ItemSet.m_itemSet;
	}

	// TODO: жиди operator<()

private:
	set<Item> m_itemSet;

};