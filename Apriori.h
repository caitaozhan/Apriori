#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

/*
    关联规则的经典算法：Apriori算法
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    注意！这个Apriori算法是“string版本”，就是说 transaction 里面的 item 是一个 char，item set 是 std::string。
	此程序中的 char 就是 length=1 的 string，所以不用 char 类型来存储 char，而是用 std::string 类型来存储 char
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	void inputTransactions(const string &fileName);   // 读入数据到m_transactions中
	void findAllFrequentItemSets();                   // 找到所有的频繁项     
	void findStrongestAssociateRules();               // 找到所有的关联规则
	void printRules(const string &fileName);

private:
	void findFrequentOneItemSets();
	void findFrequentItemsFromCandidate(const vector<string>& candidateKItemSets); // A frequent item is a item with sufficient coverage(support)
	void removeUnfrequentCandidates(map<string, int>& candidateKItemSetCount);
	void generateCandidates(vector<string>& candidateItemSets, int k);
	bool onlyDifferInLastItem(const string& items1, const string& items2);
	pair<string, int> findRepresentativeSuperSetCount(const pair<string, int> & representativeKItemSet);
	void findSubSet(string str, unsigned int level, unsigned int prune, vector<string>& subSet);
	string complementSet(const string & fullSet, const string & left);

private:
	double m_minSupportRatio;                    // 最小支持度
	double m_minConfidence;                      // 最小可信度
	int    m_transactionCounter;                 // 事务计数器

	vector<vector<string>>   m_transactions;                 // 原始数据集，在vector中顺序存储，顺序和原始数据的顺序一样
	vector<map<string, int>> m_frequentKItemSetCount;        // 记录 K:0~N 的频繁 K-item set，以及对应的统计计数
	map<string, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
	
	map<pair<string, string>, double> m_associationRule;     // 保存最终生成的规则 (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double
};