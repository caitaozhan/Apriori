#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "Item.h"
using namespace std;


/*
    关联规则的经典算法：Apriori算法
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	int  inputTransactions(string fileName);      // 读入数据到m_transactions中
	void countItemInTransactions();               // 对原始的m_transactions数据里的items做统计计数
	// void inputTransactions(Population pop);
	void findAllFrequentItems();                  // 找到所有的频繁项     
	void findAllAssociateRules();                 // 找到所有的关联规则

private:
	void findFrequentItemsFromCandidate(const map<string, int> & candidateItemCount); // A frequent item is a item with sufficient coverage(support)
	void generateCandidates(map<string, int> & candidateItemCount, int k);
	bool onlyDifferInLastItem(const string& items1, const string& items2);

private:
	double m_minSupportRatio;               // 最小支持度
	double m_minConfidence;                 // 最小可信度
	int m_transactionCounter;               // 事务计数器

	// 可以把这个改成 vector<map<string>>
	vector<vector<string>> m_transactions;                // 原始数据集，在vector中顺序存储，顺序和原始数据的顺序一样
	map<string, int> m_transactionItemCount;              // 原始数据集里面的item统计计数    
	vector<map<string, int>> m_frequentKitemCount;        // 记录 K:0~N 的频繁 K-item set，以及对应的统计计数
														  // 现在的item是string，是排好序的，未来这里需要替换成MyPoint。值得注意的是：string的set还是string，MyPoint的set。。。

	vector<vector<string>> m_candidates;    // 候选项集
	vector<vector<string>> m_frequents;     // 频繁项集
	vector<string> m_associationRule;       // 保存最终生成的规则

};