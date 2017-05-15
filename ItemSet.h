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
	ItemSet(const set<Item> & itemSet);

	void insert(const Item & item);
	size_t size() const;
	ItemSet subSet(size_t start, size_t end) const;
	Item lastItem() const;

	bool operator==(const ItemSet & itemSet) const;  // ��������� operator== �Ļ���std::sort ���bug
	bool operator!=(const ItemSet & itemSet) const;
	bool operator< (const ItemSet & itemSet) const;
	bool operator> (const ItemSet & itemSet) const;

	const set<Item>& getItemSet() const;

private:
	set<Item> m_itemSet;

};