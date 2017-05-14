#include "ItemSet.h"

ItemSet::ItemSet()
{
}

void ItemSet::insert(const Item & item)
{
	m_itemSet.insert(item);
}
