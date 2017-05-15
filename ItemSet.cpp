#include "ItemSet.h"

ItemSet::ItemSet()
{
}

ItemSet::ItemSet(const set<Item>& itemSet)
{
	m_itemSet = itemSet;
}

void ItemSet::insert(const Item & item)
{
	m_itemSet.insert(item);
}

size_t ItemSet::size() const
{
	return m_itemSet.size();
}

/*
    获取 m_itemSet 的子集，参数 start 和 end 是区间范围，左闭右开 [start, end)
*/
ItemSet ItemSet::subSet(size_t start, size_t end) const
{
	ItemSet subSet;
	size_t counter = 0;
	set<Item>::const_iterator iter = m_itemSet.begin();
	while (iter != m_itemSet.end())
	{
		if (counter >= start && counter < end)
		{
			subSet.insert(*iter);
		}
		counter++, iter++;

		if (counter >= end)
			break;
	}

	return subSet;
}

Item ItemSet::lastItem() const
{
	return *m_itemSet.rbegin();
}

bool ItemSet::operator==(const ItemSet & itemSet) const
{
	if (m_itemSet.size() != itemSet.size())   // 如果大小不相等，两个 set 一定不相等
		return false;

	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis != *iterParam)
			return false;

		iterThis++, iterParam++;
	}
	return true;
}

bool ItemSet::operator!=(const ItemSet & itemSet) const
{
	if (*this == itemSet)
		return false;

	return true;
}

bool ItemSet::operator<(const ItemSet & itemSet) const
{
	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis < *iterParam)
		{
			return true;
		}
		else if(*iterThis == *iterParam)
		{
			iterThis++, iterParam++;
			continue;
		}
		else
		{
			return false;
		}
	}
	if (iterThis == m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ItemSet::operator>(const ItemSet & itemSet) const
{
	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis > *iterParam)
		{
			return true;
		}
		else if (*iterThis == *iterParam)
		{
			iterThis++, iterParam++;
			continue;
		}
		else
		{
			return false;
		}
	}
	if (iterThis == m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

const set<Item>& ItemSet::getItemSet() const
{
	return m_itemSet;
}
