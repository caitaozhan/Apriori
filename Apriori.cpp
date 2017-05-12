#include "Apriori.h"

Apriori::Apriori()
{
	m_minSupportRatio    = 0.5;
	m_minConfidence      = 0.75;
	m_transactionCounter = 0;
}

Apriori::Apriori(double minSupportRatio, double minConfidence)
{
	m_minSupportRatio    = minSupportRatio;
	m_minConfidence      = minConfidence;
	m_transactionCounter = 0;
}

/*
	@param fileName:待输入的原始数据的文件名
	@return 0 则读入文件成功
*/
int Apriori::inputTransactions(string fileName)
{
	ifstream inFile(fileName);
	if (!inFile)
	{
		cout << "File to open data file!" << endl;
		return 1;
	}
	else
	{
		vector<string> transaction;          // 一个transaction里面有若干items
		string temp;
		int begin, end;
		while (getline(inFile, temp))        // 一个transaction可以是这样子的："A C D F G "
		{
			m_transactionCounter++;
			begin = 0;
			temp.erase(0, temp.find_first_not_of(" \r\t\n"));        // 去除字符串首部的空格
			temp.erase(temp.find_last_not_of(" \r\t\n") + 1);        // 去除字符串尾部的空格

			while ((end = temp.find(' ', begin)) != string::npos)    // 一行transaction里面，每一个item是以空格来区分的
			{
				transaction.emplace_back(temp.substr(begin, end - begin));
				begin = end + 1;
			}
			transaction.emplace_back(temp.substr(begin));            // 一个事务中的最后一项
			sort(transaction.begin(), transaction.end());            // 排序，这个很重要，默认 operator< 从小到大排序
			m_transactions.emplace_back(transaction);
			transaction.clear();
		}
		return 0;
	}
}

void Apriori::findFrequentOneItemSets()
{
	map<string, int> transactionOneItemCount;              // 原始数据集里面的item统计计数 
	for (size_t i = 0; i != m_transactions.size(); ++i)
	{
		for (size_t j = 0; j != m_transactions[i].size(); ++j)
		{
			transactionOneItemCount[m_transactions[i].at(j)]++;                 // 统计原始transaction数据中各个item的出现总数
		}
	}

	removeUnfrequentCandidates(transactionOneItemCount);
}

void Apriori::findAllFrequentItemSets()
{
	m_frequentKItemSetCount.clear();
	m_frequentKItemSetCount.emplace_back();  // 插入一个空集，为了 k 和 vector 的 index下标一一对应
	
	int k = 1;
	findFrequentOneItemSets();
	while (m_frequentKItemSetCount[k].size() != 0)  
	{
		k++;
		vector<string> candidateKItemSets;
		generateCandidates(candidateKItemSets, k);
		findFrequentItemsFromCandidate(candidateKItemSets);
	}
}

void Apriori::findAllAssociateRules()
{
}


void Apriori::generateCandidates(vector<string>& candidateKItemSets, int k)
{
	// Find all pairs of (k-1)-item sets in m_frequentKItemSetCount[k-1] that only differ in their last item
	vector<pair<string, string>> itemPairs;
	map<string, int>::const_iterator iter1 = m_frequentKItemSetCount[k - 1].begin();
	map<string, int>::const_iterator iter2;
	while (iter1 != m_frequentKItemSetCount[k - 1].end())
	{
		iter2 = iter1;
		iter2++;
		while (iter2 != m_frequentKItemSetCount[k - 1].end())  // 由于 m_frequentKItemSetCount[k-1]是一个map，因此里面的key是排好序的
		{                                                   // 这两个 iter 操作下来，pair<string, string>里面的两个string也是排好序的：左边的string < 右边的string
			string items1 = iter1->first;                   // item set 是 string，里面的 item 是一个char，不过也是用string存储的。。。
			string items2 = iter2->first;
			if (onlyDifferInLastItem(items1, items2))
			{
				itemPairs.emplace_back(items1, items2);
			}
			iter2++;
		}
		iter1++;
	}
	
	// Create candidate k-item set for each pair by combining the two (k-1)-item sets that are paired
	for (size_t i = 0; i < itemPairs.size(); ++i)
	{
		string item1(itemPairs[i].first);
		string item2(itemPairs[i].second);
		item1.insert(item1.end(), item2[item2.size() - 1]);

		candidateKItemSets.emplace_back(item1);
	}

	// Remove all k-item sets containing any (k-1)-item sets that are not in the m_frequentKItemSetCount[k - 1]
	vector<string>::const_iterator iterVec = candidateKItemSets.begin();
	map<string, int>::const_iterator iterMap;
	while (iterVec != candidateKItemSets.end())
	{
		bool flag = true;           // 一开始假设这个 k-item 的所有 (k-1)-item 子集都是频繁的，即在 m_frequentKItemSetCount[k - 1] 里面
		string item = *iterVec;
		string itemSubset;
		for (size_t i = 0; i < item.size(); ++i)
		{
			itemSubset = item;
			itemSubset.erase(i, 1);

			iterMap = m_frequentKItemSetCount[k - 1].find(itemSubset);
			if (iterMap == m_frequentKItemSetCount[k - 1].end())
			{
				iterVec = candidateKItemSets.erase(iterVec);  // 删除：k-item 里面只要存在一个 (k-1)-item 不在 m_frequentKItemSetCount[k - 1] 里面，就要删去这个 k-item
				flag = false;
				break;
			}
		}

		if(flag == true)
			iterVec++;
	}
}

/*
    从candidate item 里面找到频繁的 item，将数据保存在数据成员m_frequentKItemSetCount里面

    @param itemCount item -> 统计次数
*/
void Apriori::findFrequentItemsFromCandidate(const vector<string>& candidateKItemSets)
{
	map<string, int> candidateKItemSetCount;

	// 对新产生的 kItems 进行计数统计，生成 candidateItemCount（改变这个函数传进来的引用参数）
	for (size_t i = 0; i < candidateKItemSets.size(); ++i)
	{
		string kItem = candidateKItemSets[i];
		int itemCounter = 0;
		for (size_t j = 0; j < m_transactions.size(); ++j)  // kItems[i]里面的item是排好序的，m_transactions[j]里面的item也是排好序的
		{
			vector<string> oneTransaction = m_transactions[j];
			size_t transactionIndex = 0;
			size_t itemIndex = 0;
			while (transactionIndex < oneTransaction.size() && itemIndex < kItem.size()) // 类似于 merge sort 的思想
			{
				string oneItem = kItem.substr(itemIndex, 1);
				string transactionItem = oneTransaction[transactionIndex];
				if (oneItem == transactionItem)
				{
					transactionIndex++;
					itemIndex++;
				}
				else
				{
					transactionIndex++;
				}
			}
			if (transactionIndex < oneTransaction.size())
			{
				itemCounter++;                // 如果 kItem 遍历完成了，oneTransaction还没有遍历完成，kItem在oneTransaction里面，于是 itemCounter 加一
			}
			else if (transactionIndex == oneTransaction.size() && itemIndex == kItem.size())
			{
				itemCounter++;
			}
		}
		candidateKItemSetCount.emplace(kItem, itemCounter);
	}

	removeUnfrequentCandidates(candidateKItemSetCount);
}

void Apriori::removeUnfrequentCandidates(map<string, int>& candidateKItemSetCount)
{
	map<string, int> frequentItemCount;
	map<string, int>::const_iterator iter = candidateKItemSetCount.begin();
	while (iter != candidateKItemSetCount.end())
	{
		if (static_cast<double>(iter->second) / static_cast<double>(m_transactionCounter) > (m_minSupportRatio - 1.0e-8))
		{
			frequentItemCount.emplace(iter->first, iter->second);
		}
		iter++;
	}
	m_frequentKItemSetCount.emplace_back(frequentItemCount);  // 频繁 k-item 及其统计次数 保存在 m_frequentKItemSetCount[k] 里面 
}


/*
	Figure out whether two items only differ in their last item
	这里的一个 item 就是一个 char，items 就是一个 string
*/
bool Apriori::onlyDifferInLastItem(const string& items1, const string& items2)
{
	size_t length1 = items1.size();
	size_t length2 = items2.size();  // length1, length2 这两个值是相等的

	if (length1 == 1)
		return true;

	if (items1.substr(0, length1 - 1) == items2.substr(0, length2 - 1))
	{
		if (items1[length1 - 1] != items2[length2 - 1])
		{
			return true;        // 只有最后一项不同
		}
	}
	return false;
}

