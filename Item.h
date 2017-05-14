#pragma once
#include <iostream>
#include <string>
using namespace std;


/*
    OpenFrameworks����� class ofVec2f û��operator<����
	����޷��ȴ�С������޷���������޷��ŵ�std::map����
	�������Լ�дһ��Item
*/
class Item
{
public:
	Item();
	Item(string itemStr);
	Item(int x, int y);

	string toString();

	bool operator<(const Item & item) const
	{
		if (m_x < item.m_x)
		{
			return true;
		}
		else if (m_x == item.m_x)
		{
			return m_y < item.m_y;
		}
		return false;
	}

	bool operator==(const Item & item) const
	{
		if (m_x == item.m_x && m_y == item.m_y)
			return true;
		else
			return false;
	}

private:
	int m_x;
	int m_y;
};