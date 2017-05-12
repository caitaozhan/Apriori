#include "Item.h"

Item::Item()
{
	m_x = 0;
	m_y = 0;
}

Item::Item(int x, int y)
{
	m_x = x;
	m_y = y;
}

string Item::toString()
{
	string str("()");
	str.insert(1, to_string(m_x));
	str.insert(2, ",");
	str.insert(3, to_string(m_y));
	return str;
}
