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
	@param fileName:�������ԭʼ���ݵ��ļ���
	@return 0 ������ļ��ɹ�
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
		vector<string> transaction;          // һ��transaction����������items
		string temp;
		int begin, end;
		while (getline(inFile, temp))        // һ��transaction�����������ӵģ�"A C D F G "
		{
			m_transactionCounter++;
			begin = 0;
			temp.erase(0, temp.find_first_not_of(" \r\t\n"));        // ȥ���ַ����ײ��Ŀո�
			temp.erase(temp.find_last_not_of(" \r\t\n") + 1);        // ȥ���ַ���β���Ŀո�

			while ((end = temp.find(' ', begin)) != string::npos)    // һ��transaction���棬ÿһ��item���Կո������ֵ�
			{
				transaction.emplace_back(temp.substr(begin, end - begin));
				begin = end + 1;
			}
			transaction.emplace_back(temp.substr(begin));            // һ�������е����һ��
			sort(transaction.begin(), transaction.end());            // �����������Ҫ��Ĭ�� operator< ��С��������
			m_transactions.emplace_back(transaction);
			transaction.clear();
		}
		return 0;
	}
}

void Apriori::findFrequentOneItemSets()
{
	map<string, int> transactionOneItemCount;              // ԭʼ���ݼ������itemͳ�Ƽ��� 
	for (size_t i = 0; i != m_transactions.size(); ++i)
	{
		for (size_t j = 0; j != m_transactions[i].size(); ++j)
		{
			transactionOneItemCount[m_transactions[i].at(j)]++;                 // ͳ��ԭʼtransaction�����и���item�ĳ�������
		}
	}

	removeUnfrequentCandidates(transactionOneItemCount);
}

void Apriori::findAllFrequentItemSets()
{
	m_frequentKItemSetCount.clear();
	m_frequentKItemSetCount.emplace_back();  // ����һ���ռ���Ϊ�� k �� vector �� index�±�һһ��Ӧ
	
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
		while (iter2 != m_frequentKItemSetCount[k - 1].end())  // ���� m_frequentKItemSetCount[k-1]��һ��map����������key���ź����
		{                                                   // ������ iter ����������pair<string, string>���������stringҲ���ź���ģ���ߵ�string < �ұߵ�string
			string items1 = iter1->first;                   // item set �� string������� item ��һ��char������Ҳ����string�洢�ġ�����
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
		bool flag = true;           // һ��ʼ������� k-item ������ (k-1)-item �Ӽ�����Ƶ���ģ����� m_frequentKItemSetCount[k - 1] ����
		string item = *iterVec;
		string itemSubset;
		for (size_t i = 0; i < item.size(); ++i)
		{
			itemSubset = item;
			itemSubset.erase(i, 1);

			iterMap = m_frequentKItemSetCount[k - 1].find(itemSubset);
			if (iterMap == m_frequentKItemSetCount[k - 1].end())
			{
				iterVec = candidateKItemSets.erase(iterVec);  // ɾ����k-item ����ֻҪ����һ�� (k-1)-item ���� m_frequentKItemSetCount[k - 1] ���棬��Ҫɾȥ��� k-item
				flag = false;
				break;
			}
		}

		if(flag == true)
			iterVec++;
	}
}

/*
    ��candidate item �����ҵ�Ƶ���� item�������ݱ��������ݳ�Աm_frequentKItemSetCount����

    @param itemCount item -> ͳ�ƴ���
*/
void Apriori::findFrequentItemsFromCandidate(const vector<string>& candidateKItemSets)
{
	map<string, int> candidateKItemSetCount;

	// ���²����� kItems ���м���ͳ�ƣ����� candidateItemCount���ı�������������������ò�����
	for (size_t i = 0; i < candidateKItemSets.size(); ++i)
	{
		string kItem = candidateKItemSets[i];
		int itemCounter = 0;
		for (size_t j = 0; j < m_transactions.size(); ++j)  // kItems[i]�����item���ź���ģ�m_transactions[j]�����itemҲ���ź����
		{
			vector<string> oneTransaction = m_transactions[j];
			size_t transactionIndex = 0;
			size_t itemIndex = 0;
			while (transactionIndex < oneTransaction.size() && itemIndex < kItem.size()) // ������ merge sort ��˼��
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
				itemCounter++;                // ��� kItem ��������ˣ�oneTransaction��û�б�����ɣ�kItem��oneTransaction���棬���� itemCounter ��һ
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
	m_frequentKItemSetCount.emplace_back(frequentItemCount);  // Ƶ�� k-item ����ͳ�ƴ��� ������ m_frequentKItemSetCount[k] ���� 
}


/*
	Figure out whether two items only differ in their last item
	�����һ�� item ����һ�� char��items ����һ�� string
*/
bool Apriori::onlyDifferInLastItem(const string& items1, const string& items2)
{
	size_t length1 = items1.size();
	size_t length2 = items2.size();  // length1, length2 ������ֵ����ȵ�

	if (length1 == 1)
		return true;

	if (items1.substr(0, length1 - 1) == items2.substr(0, length2 - 1))
	{
		if (items1[length1 - 1] != items2[length2 - 1])
		{
			return true;        // ֻ�����һ�ͬ
		}
	}
	return false;
}

