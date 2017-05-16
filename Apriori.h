#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

/*
    ��������ľ����㷨��Apriori�㷨
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    ע�⣡���Apriori�㷨�ǡ�string�汾��������˵ transaction ����� item ��һ�� char��item set �� std::string��
	�˳����е� char ���� length=1 �� string�����Բ��� char �������洢 char�������� std::string �������洢 char
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	void inputTransactions(const string &fileName);   // �������ݵ�m_transactions��
	void findAllFrequentItemSets();                   // �ҵ����е�Ƶ����     
	void findStrongestAssociateRules();               // �ҵ����еĹ�������
	void printRules(const string &fileName);          // ��ӡ���еĹ�������

private:
	void findFrequentOneItemSets();                   // ��ԭʼ�� transaction ���棬�ҵ�Ƶ���� one-item set
	void findFrequentItemsFromCandidate(const vector<string>& candidateKItemSets); // A frequent item is a item with sufficient coverage(support)
	void removeUnfrequentCandidates(map<string, int>& candidateKItemSetCount);     // �Ӻ�ѡ�� item set ����,ɾ�� support �����,�Ӷ��õ�Ƶ�� item set
	void generateCandidates(vector<string>& candidateItemSets, int k);             // ������ѡ�� item set
	bool onlyDifferInLastItem(const string& items1, const string& items2);         // �����ź���� item set ֻ�����һ�� item ��ͬ
	pair<string, int> findRepresentativeSuperSetCount(const pair<string, int> & representativeKItemSet); // �� representative item set ����,�ҵ����� frequentKItemSet �ĳ���
	void findSubSet(string str, unsigned int level, unsigned int prune, vector<string>& subSet);         // �Ӵ�СΪ level �� set ���棬�ҵ����д�СΪ prune ���Ӽ�
	string complementSet(const string & fullSet, const string & left);             // �ҵ� left �Ļ�������

private:
	double m_minSupportRatio;                    // ��С֧�ֶ�
	double m_minConfidence;                      // ��С���Ŷ�
	int    m_transactionCounter;                 // ���������

	vector<vector<string>>   m_transactions;                 // ԭʼ���ݼ�����vector��˳��洢��˳���ԭʼ���ݵ�˳��һ��
	vector<map<string, int>> m_frequentKItemSetCount;        // ��¼ K:0~N ��Ƶ�� K-item set���Լ���Ӧ��ͳ�Ƽ���
	map<string, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
	
	map<pair<string, string>, double> m_associationRule;     // �����������ɵĹ��� (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double
};