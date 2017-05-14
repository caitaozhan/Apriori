#pragma once
#include <iostream>
#include <string>
using namespace std;


/*
    OpenFrameworks里面的 class ofVec2f 没有operator<函数
	因此无法比大小，因此无法排序，因此无法放到std::map里面
	所以我自己写一个Item
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