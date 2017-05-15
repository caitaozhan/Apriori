#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "ItemSet.h"
using namespace std;

/*
    关联规则的经典算法：Apriori算法
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    注意！这个Apriori算法是“ItemSet版本”，就是说 transaction 里面的 item 和 itemSet 都是自定义的类
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	void inputTransactions(const string &fileName);     // 读入数据到m_transactions中
	void findAllFrequentItemSets();              // 找到所有的频繁项     
	void findStrongestAssociateRules();          // 找到所有的关联规则
	//void printRules(const string &fileName);

private:
	void findFrequentOneItemSets();
	void generateCandidates(vector<ItemSet>& candidateItemSets, int k);
	void findFrequentItemsFromCandidate(const vector<ItemSet>& candidateKItemSets); // A frequent item is a item with sufficient coverage(support)
	void removeUnfrequentCandidates(map<ItemSet, int>& candidateKItemSetCount);
	bool onlyDifferInLastItem(const ItemSet& items1, const ItemSet& items2);
	pair<ItemSet, int> findRepresentativeSuperSetCount(const pair<ItemSet, int> & representativeKItemSet);
	//void findSubSet(string str, unsigned int level, unsigned int prune, vector<string>& subSet);
	//string complementSet(const string & fullSet, const string & left);

private:
	double m_minSupportRatio;                    // 最小支持度
	double m_minConfidence;                      // 最小可信度
	int    m_transactionCounter;                 // 事务计数器

	vector<vector<ItemSet>>   m_transactions;                 // 原始数据集，在vector中顺序存储，顺序和原始数据的顺序一样
	vector<map<ItemSet, int>> m_frequentKItemSetCount;        // 记录 K:0~N 的频繁 K-item set，以及对应的统计计数
	map<ItemSet, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
															  // 现在的item是string，是排好序的，未来这里需要替换成MyPoint。值得注意的是：string的set还是string，MyPoint的set。。。
	map<pair<ItemSet, ItemSet>, double> m_associationRule;    // 保存最终生成的规则 (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double
};