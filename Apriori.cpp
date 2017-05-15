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
*/
void Apriori::inputTransactions(const string &fileName)
{
	ifstream inFile(fileName);
	if (!inFile)
	{
		cout << "File to open data file!" << endl;
	}
	else
	{
		vector<ItemSet> transaction;            // 一个transaction里面有若干 item，一个 item 相当于是一个 one-item set 
		string line;
		size_t begin, end;
		while (getline(inFile, line))        // 一个 transaction 是这样子的："(3,3) (1,1) (4,4) (6,6) (7,7) "
		{
			m_transactionCounter++;
			begin = 0;
			line.erase(0, line.find_first_not_of(" \r\t\n"));        // 去除字符串首部的空格
			line.erase(line.find_last_not_of(" \r\t\n") + 1);        // 去除字符串尾部的空格

			while ((end = line.find(' ', begin)) != string::npos)    // 一行transaction里面，每一个item是以空格来区分的
			{
				string itemStr = line.substr(begin, end - begin);
				Item item(itemStr);
				ItemSet itemSet;
				itemSet.insert(item);
				transaction.emplace_back(itemSet);
				begin = end + 1;
			}
			string itemStr = line.substr(begin);
			Item item(itemStr);
			ItemSet itemSet;
			itemSet.insert(item);
			transaction.emplace_back(itemSet);                       // 一个事务中的最后一项
			sort(transaction.begin(), transaction.end());            // 排序，这个很重要，默认 operator< 从小到大排序
			m_transactions.emplace_back(transaction);
			transaction.clear();
		}
	}
	inFile.close();
}


void Apriori::findFrequentOneItemSets()
{
	map<ItemSet, int> oneItemSetCount;                        // transaction 原始数据集里面的 one-item set 统计计数 
	for (size_t i = 0; i != m_transactions.size(); ++i)
	{
		for (size_t j = 0; j != m_transactions[i].size(); ++j)
		{
			oneItemSetCount[m_transactions[i].at(j)]++;       // 统计原始transaction数据中各个item的出现总数，在 Strict weak orderings 吃了大亏
		}
	}
	removeUnfrequentCandidates(oneItemSetCount);
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
		vector<ItemSet> candidateKItemSets;
		generateCandidates(candidateKItemSets, k);
		findFrequentItemsFromCandidate(candidateKItemSets);
	}
}

/*
    Idealy, rules are as shortest as possible one the left-hand-side, and as long as possible on the right-hand-side.
	For example, rule A-->CDF is stronger than AC-->DF. If the former rule is right, then the latter rule must be right,
	because P(AC) <= P(A) and P(ACDF)/P(AC) >= P(ACDF)/P(A) >= minimum accuracy
	Also, rule A-->CDF is stronger than rule A-->CD
*/
void Apriori::findStrongestAssociateRules()
{
	// Find the representative K-item sets and their counts
	for (size_t i = m_frequentKItemSetCount.size() - 2; i > 1; --i)  // [0] 和 [size()-1] 是空元素，[1] 是 one-item set 不能形成rule
	{
		map<ItemSet, int>::const_iterator iter = m_frequentKItemSetCount[i].begin();
		while (iter != m_frequentKItemSetCount[i].end())
		{
			pair<ItemSet, int> superSet = findRepresentativeSuperSetCount(*iter);
			if (superSet.second == 0)                                // 没有找到 super set
			{
				m_representativeItemSetCount.insert(*iter);
			}
			else
			{
				if (iter->second > superSet.second)
				{
					m_representativeItemSetCount.insert(*iter);      // 找到了 super set，而且 support 比 super set 的高
				}
			}
			iter++;
		}
	}
	

	//// Generate rules from the representative item sets，一个规则，左边的item数量越少，则这个规则越“强大”，我们只需要产生那一些强大的规则就行了
	//map<string, int>::const_iterator iterRepre = m_representativeItemSetCount.begin();
	//while (iterRepre != m_representativeItemSetCount.end())
	//{
	//	string leftSet = iterRepre->first;  // 能够出现在左手边的item，初始是所有的items
	//	size_t leftNum = 1;                 // 规则左手边的 item 数量，初始化是左边一个

	//	while (leftNum < leftSet.size())
	//	{
	//		map<pair<string, string>, double> newRules;
	//		vector<string> allLeft;
	//		vector<string> leftInNewRules;
	//		findSubSet(leftSet, leftSet.size(), leftNum, allLeft);
	//		for (size_t i = 0; i < allLeft.size(); ++i)
	//		{
	//			string left = allLeft[i];                                  // left-hand-side is the antecedent
	//			map<string, int>::const_iterator iter = m_frequentKItemSetCount[left.size()].find(left);
	//			int support = iter->second;
	//			double confidence = static_cast<double>(iterRepre->second) / static_cast<double>(support);  // 新rule的置信度，又称准确度accuracy
	//			if (confidence > (m_minConfidence - 1.0e-7))
	//			{
	//				leftInNewRules.emplace_back(left);
	//				string right = complementSet(iterRepre->first, left);  // right-hand-side is the consequent
	//				newRules.emplace(make_pair(left, right), confidence);
	//			}
	//		}
	//		if (newRules.size() > 0)
	//		{
	//			map<pair<string, string>, double>::const_iterator iter = newRules.begin();
	//			while (iter != newRules.end())
	//			{
	//				m_associationRule.insert(*iter);
	//				iter++;
	//			}
	//			for (size_t i = 0; i < leftInNewRules.size(); ++i)
	//			{
	//				string left = leftInNewRules[i];
	//				for (size_t j = 0; j < left.size(); ++j)
	//				{
	//					char ch = left.at(j);
	//					size_t found = leftSet.find(ch);
	//					if (found != string::npos)
	//					{
	//						leftSet.erase(found, 1);
	//					}
	//				}
	//			}
	//		}
	//		leftNum++;
	//	}
	//	iterRepre++;
	//}
}

//void Apriori::printRules(const string &fileName)
//{
//	ofstream outFile(fileName);
//	cout << "There are " << m_associationRule.size() << " rules" << endl;
//	map<pair<string, string>, double>::const_iterator iter = m_associationRule.begin();
//	while (iter != m_associationRule.end())
//	{
//		outFile << iter->first.first << " --> " << iter->first.second << ", condidence: " << iter->second << endl;
//		cout << iter->first.first << " --> " << iter->first.second << ", condidence: " << iter->second << endl;
//		iter++;
//	}
//	outFile.close();
//}

void Apriori::generateCandidates(vector<ItemSet>& candidateKItemSets, int k)
{
	// Find all pairs of (k-1)-item sets in m_frequentKItemSetCount[k-1] that only differ in their last item
	vector<pair<ItemSet, ItemSet>> itemPairs;
	map<ItemSet, int>::const_iterator iter1 = m_frequentKItemSetCount[k - 1].begin();
	map<ItemSet, int>::const_iterator iter2;
	while (iter1 != m_frequentKItemSetCount[k - 1].end())
	{
		iter2 = iter1;
		iter2++;
		while (iter2 != m_frequentKItemSetCount[k - 1].end())  // 由于 m_frequentKItemSetCount[k-1]是一个map，因此里面的key是排好序的
		{                                                      // 这两个 iter 操作下来，pair<string, string>里面的两个string也是排好序的：左边的string < 右边的string
			ItemSet itemSet1 = iter1->first;                   // item set 是 string，里面的 item 是一个char，不过也是用string存储的。。。
			ItemSet itemSet2 = iter2->first;
			if (onlyDifferInLastItem(itemSet1, itemSet2))
			{
				itemPairs.emplace_back(itemSet1, itemSet2);
			}
			iter2++;
		}
		iter1++;
	}
	
	// Create candidate k-item set for each pair by combining the two (k-1)-item sets that are paired
	for (size_t i = 0; i < itemPairs.size(); ++i)
	{
		ItemSet itemSet1(itemPairs[i].first);
		ItemSet itemSet2(itemPairs[i].second);
		itemSet1.insert(itemSet2.lastItem());

		candidateKItemSets.emplace_back(itemSet1);
	}

	// Remove all k-item sets containing any (k-1)-item sets that are not in the m_frequentKItemSetCount[k - 1]
	vector<ItemSet>::const_iterator iterVec = candidateKItemSets.begin();
	map<ItemSet, int>::const_iterator iterMap;
	while (iterVec != candidateKItemSets.end())
	{
		bool flag = true;
		set<Item> itemSet = iterVec->getItemSet();
		set<Item>::iterator iterSet = itemSet.begin();
		while (iterSet != itemSet.end())
		{
			set<Item> itemSubSet(itemSet);
			itemSubSet.erase(*iterSet);
			ItemSet itemSubSetObject(itemSubSet);

			iterMap = m_frequentKItemSetCount[k - 1].find(itemSubSetObject);
			if (iterMap == m_frequentKItemSetCount[k - 1].end())
			{
				iterVec = candidateKItemSets.erase(iterVec);  // 删除：k-item 里面只要存在一个 (k-1)-item 不在 m_frequentKItemSetCount[k - 1] 里面，就要删去这个 k-item
				flag = false;
				break;
			}
			iterSet++;
		}

		if (flag == true)
			iterVec++;
	}
}

/*
    从candidate item 里面找到频繁的 item，将数据保存在数据成员m_frequentKItemSetCount里面
    @param itemCount item -> 统计次数
*/
void Apriori::findFrequentItemsFromCandidate(const vector<ItemSet>& candidateKItemSets)
{
	map<ItemSet, int> candidateKItemSetCount;

	// 对新产生的 kItems 进行计数统计，生成 candidateItemCount（改变这个函数传进来的引用参数）
	for (size_t i = 0; i < candidateKItemSets.size(); ++i)
	{
		ItemSet itemSet = candidateKItemSets[i];
		set<Item> setItem = itemSet.getItemSet();
		int itemCounter = 0;
		for (size_t j = 0; j < m_transactions.size(); ++j)  // kItems[i]里面的item是排好序的，m_transactions[j]里面的item也是排好序的
		{
			vector<ItemSet> oneTransaction = m_transactions[j];
			size_t transactionIndex = 0;
			set<Item>::const_iterator iterSet = setItem.begin();

			while (transactionIndex < oneTransaction.size() && iterSet != setItem.end())  // 类似于 merge sort 的 merge 思想
			{
				ItemSet transactionItem = oneTransaction[transactionIndex];
				ItemSet oneItem;
				oneItem.insert(*iterSet);
				if (oneItem < transactionItem)  // 剪枝，如果candidate item-set当前的item ＜ 当前的transaction里面的item， 后面的更不可能了
				{
					break;
				}
				else if (oneItem == transactionItem)
				{
					transactionIndex++;
					iterSet++;
				}
				else
				{
					transactionIndex++;
				}
			}
			if (iterSet == setItem.end())    // itemSet遍历完成了，说明itemSet里面的元素都找到了
			{
				itemCounter++;
			}
		}
		candidateKItemSetCount.emplace(itemSet, itemCounter);
	}
	removeUnfrequentCandidates(candidateKItemSetCount);
}

void Apriori::removeUnfrequentCandidates(map<ItemSet, int>& candidateKItemSetCount)
{
	map<ItemSet, int> frequentItemCount;
	map<ItemSet, int>::const_iterator iter = candidateKItemSetCount.begin();
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
bool Apriori::onlyDifferInLastItem(const ItemSet& itemSet1, const ItemSet& itemSet2)
{
	size_t length1 = itemSet1.size();
	size_t length2 = itemSet2.size();  // length1, length2 这两个值是相等的

	if (length1 == 1)
		return true;

	if (itemSet1.subSet(0, length1 - 1) == itemSet2.subSet(0, length2 - 1))
	{
		if (itemSet1.lastItem() != itemSet2.lastItem())
		{
			return true;        // 只有最后一项不同
		}
	}
	return false;
}

/*
	Find and return the representative super set of @param representativeKItemSet from m_representativeKItemSetCount
	if not find, return pair("null", 0);
*/
pair<ItemSet, int> Apriori::findRepresentativeSuperSetCount(const pair<ItemSet, int>& representativeKItemSet)
{
	map<ItemSet, int> someRepresentativeSuperSets;
	set<Item> subSet = representativeKItemSet.first.getItemSet();
	map<ItemSet, int>::const_iterator iter = m_representativeItemSetCount.begin();
	
	while (iter != m_representativeItemSetCount.end())
	{
		set<Item> superSet = iter->first.getItemSet();

		set<Item>::const_iterator iterSubSet = subSet.begin();
		set<Item>::const_iterator iterSuperSet = superSet.begin();

		while (iterSubSet != subSet.end() && iterSuperSet != superSet.end())
		{
			if (*iterSubSet < *iterSuperSet)
			{
				break;
			}
			else if (*iterSubSet == *iterSuperSet)
			{
				iterSubSet++;
				iterSuperSet++;
			}
			else
			{
				iterSuperSet++;
			}
		}
		if (iterSubSet == subSet.end())
		{
			someRepresentativeSuperSets.emplace(iter->first, iter->second);   // 可能有多个超集
		}
		iter++;
	}

	// 从多个超集里面选取support最大的一个
	pair<ItemSet, int> superSetCount = pair<ItemSet, int>(ItemSet(), 0);
	int maxSupport = 0;
	iter = someRepresentativeSuperSets.begin();
	while (iter != someRepresentativeSuperSets.end())
	{
		if (iter->second > maxSupport)
		{
			superSetCount = *iter;
			maxSupport = iter->second;
		}
		iter++;
	}
	return superSetCount;
}

///*
//    level 是搜索树的深度，初始化是最开始输入的原始 string 的长度
//    当搜索深度达到 prune（找到了长度为 prune 的 substr）的时候，就剪枝
//    subSet 保存找到的子集的结果
//	比如：
//	vector<string> subset;
//	findSubSet("ACDF", 4, 2, subset);
//	得到的结果：subset={DF, CF, AF, CD, AD, AC}
//*/
//void Apriori::findSubSet(string str, unsigned int level, unsigned int prune, vector<string>& subSet)
//{
//	if (str.length() - level > prune)      // 剩余的节点肯定没有希望，剪枝
//		return;
//
//	if (str.length() == prune)             // 找到目标，剩余的剪枝
//	{
//		subSet.emplace_back(str);
//	}
//	else
//	{
//		findSubSet(str, level - 1, prune, subSet);
//		if (level > 0)
//		{
//			str.erase(level - 1, 1);
//			findSubSet(str, level - 1, prune, subSet);
//		}
//	}
//}
//
///*
//    求一个集合的互补集合,要求集合里面的元素是排好序的，从小到大
//	比如：
//	fullSet = {ACDF}, left = {AC}
//	则返回的 right = {DF}
//*/
//string Apriori::complementSet(const string & fullSet, const string & left)
//{
//	string right;
//	size_t i = 0, j = 0;
//	while(i < fullSet.size() && j < left.size())
//	{
//		if (fullSet[i] == left[j])
//		{
//			i++, j++;
//		}
//		else if (fullSet[i] != left[j])
//		{
//			right.push_back(fullSet[i]);
//			i++;
//		}
//	}
//	if (i < fullSet.size())
//	{
//		right.append(fullSet.substr(i));
//	}
//
//	return right;
//}
