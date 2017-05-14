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
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    ��������ľ����㷨��Apriori�㷨
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	int  inputTransactions(string fileName);     // �������ݵ�m_transactions��
	void findAllFrequentItemSets();              // �ҵ����е�Ƶ����     
	void findStrongestAssociateRules();          // �ҵ����еĹ�������

private:
	void findFrequentOneItemSets();
	void findFrequentItemsFromCandidate(const vector<string>& candidateKItemSets); // A frequent item is a item with sufficient coverage(support)
	void removeUnfrequentCandidates(map<string, int>& candidateKItemSetCount);
	void generateCandidates(vector<string>& candidateItemSets, int k);
	bool onlyDifferInLastItem(const string& items1, const string& items2);
	pair<string, int> findRepresentativeSuperSetCount(const pair<string, int> & representativeKItemSet);
	void findSubSet(string str, int level, int prune, vector<string>& subSet);

private:
	double m_minSupportRatio;                    // ��С֧�ֶ�
	double m_minConfidence;                      // ��С���Ŷ�
	int    m_transactionCounter;                 // ���������

	vector<vector<string>>   m_transactions;                 // ԭʼ���ݼ�����vector��˳��洢��˳���ԭʼ���ݵ�˳��һ��
	vector<map<string, int>> m_frequentKItemSetCount;        // ��¼ K:0~N ��Ƶ�� K-item set���Լ���Ӧ��ͳ�Ƽ���
	map<string, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
															 // ���ڵ�item��string�����ź���ģ�δ��������Ҫ�滻��MyPoint��ֵ��ע����ǣ�string��set����string��MyPoint��set������
	map<pair<string, string>, double> m_associationRule;     // �����������ɵĹ��� (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double

};