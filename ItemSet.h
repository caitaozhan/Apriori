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

private:
	set<Item> m_itemSet;

};