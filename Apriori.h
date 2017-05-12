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
    ��������ľ����㷨��Apriori�㷨
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	int  inputTransactions(string fileName);      // �������ݵ�m_transactions��
	void countItemInTransactions();               // ��ԭʼ��m_transactions�������items��ͳ�Ƽ���
	// void inputTransactions(Population pop);
	void findAllFrequentItems();                  // �ҵ����е�Ƶ����     
	void findAllAssociateRules();                 // �ҵ����еĹ�������

private:
	void findFrequentItemsFromCandidate(const map<string, int> & candidateItemCount); // A frequent item is a item with sufficient coverage(support)
	void generateCandidates(map<string, int> & candidateItemCount, int k);
	bool onlyDifferInLastItem(const string& items1, const string& items2);

private:
	double m_minSupportRatio;               // ��С֧�ֶ�
	double m_minConfidence;                 // ��С���Ŷ�
	int m_transactionCounter;               // ���������

	// ���԰�����ĳ� vector<map<string>>
	vector<vector<string>> m_transactions;                // ԭʼ���ݼ�����vector��˳��洢��˳���ԭʼ���ݵ�˳��һ��
	map<string, int> m_transactionItemCount;              // ԭʼ���ݼ������itemͳ�Ƽ���    
	vector<map<string, int>> m_frequentKitemCount;        // ��¼ K:0~N ��Ƶ�� K-item set���Լ���Ӧ��ͳ�Ƽ���
														  // ���ڵ�item��string�����ź���ģ�δ��������Ҫ�滻��MyPoint��ֵ��ע����ǣ�string��set����string��MyPoint��set������

	vector<vector<string>> m_candidates;    // ��ѡ�
	vector<vector<string>> m_frequents;     // Ƶ���
	vector<string> m_associationRule;       // �����������ɵĹ���

};