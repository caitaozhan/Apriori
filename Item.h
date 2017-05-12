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
	Item(int x, int y);

	string toString();

	bool operator<(const Item & point) const
	{
		if (m_x < point.m_x)
		{
			return true;
		}
		else if (m_x == point.m_x)
		{
			return m_y < point.m_y;
		}
		return false;
	}

private:
	int m_x;
	int m_y;
};