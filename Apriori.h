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
    ��������ľ����㷨��Apriori�㷨
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    ע�⣡���Apriori�㷨�ǡ�ItemSet�汾��������˵ transaction ����� item �� itemSet �����Զ������
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	void inputTransactions(const string &fileName);     // �������ݵ�m_transactions��
	void findAllFrequentItemSets();              // �ҵ����е�Ƶ����     
	void findStrongestAssociateRules();          // �ҵ����еĹ�������
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
	double m_minSupportRatio;                    // ��С֧�ֶ�
	double m_minConfidence;                      // ��С���Ŷ�
	int    m_transactionCounter;                 // ���������

	vector<vector<ItemSet>>   m_transactions;                 // ԭʼ���ݼ�����vector��˳��洢��˳���ԭʼ���ݵ�˳��һ��
	vector<map<ItemSet, int>> m_frequentKItemSetCount;        // ��¼ K:0~N ��Ƶ�� K-item set���Լ���Ӧ��ͳ�Ƽ���
	map<ItemSet, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
															  // ���ڵ�item��string�����ź���ģ�δ��������Ҫ�滻��MyPoint��ֵ��ע����ǣ�string��set����string��MyPoint��set������
	map<pair<ItemSet, ItemSet>, double> m_associationRule;    // �����������ɵĹ��� (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double
};