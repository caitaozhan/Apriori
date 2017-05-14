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
		vector<string> transaction;          // һ��transaction����������items
		string temp;
		size_t begin, end;
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
	}
	inFile.close();
}

void Apriori::findFrequentOneItemSets()
{
	map<string, int> transactionOneItemCount;                        // ԭʼ���ݼ������itemͳ�Ƽ��� 
	for (size_t i = 0; i != m_transactions.size(); ++i)
	{
		for (size_t j = 0; j != m_transactions[i].size(); ++j)
		{
			transactionOneItemCount[m_transactions[i].at(j)]++;      // ͳ��ԭʼtransaction�����и���item�ĳ�������
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

/*
    Idealy, rules are as shortest as possible one the left-hand-side, and as long as possible on the right-hand-side.
	For example, rule A-->CDF is stronger than AC-->DF. If the former rule is right, then the latter rule must be right,
	because P(AC) <= P(A) and P(ACDF)/P(AC) >= P(ACDF)/P(A) >= minimum accuracy
	Also, rule A-->CDF is stronger than rule A-->CD
*/
void Apriori::findStrongestAssociateRules()
{
	// Find the representative K-item sets and their counts
	for (size_t i = m_frequentKItemSetCount.size() - 2; i > 1; --i)  // [0] �� [size()-1] �ǿ�Ԫ�أ�[1] �� one-item set �����γ�rule
	{
		map<string, int>::const_iterator iter = m_frequentKItemSetCount[i].begin();
		while (iter != m_frequentKItemSetCount[i].end())
		{
			pair<string, int> superSet = findRepresentativeSuperSetCount(*iter);
			if (superSet.second == 0)                                // û���ҵ� super set
			{
				m_representativeItemSetCount.insert(*iter);
			}
			else
			{
				if (iter->second > superSet.second)
				{
					m_representativeItemSetCount.insert(*iter);      // �ҵ��� super set������ support �� super set �ĸ�
				}
			}
			iter++;
		}
	}
	
	// Generate rules from the representative item sets��һ��������ߵ�item����Խ�٣����������Խ��ǿ�󡱣�����ֻ��Ҫ������һЩǿ��Ĺ��������
	map<string, int>::const_iterator iterRepre = m_representativeItemSetCount.begin();
	while (iterRepre != m_representativeItemSetCount.end())
	{
		string leftSet = iterRepre->first;  // �ܹ����������ֱߵ�item����ʼ�����е�items
		size_t leftNum = 1;                 // �������ֱߵ� item ��������ʼ�������һ��

		while (leftNum < leftSet.size())
		{
			map<pair<string, string>, double> newRules;
			vector<string> allLeft;
			vector<string> leftInNewRules;
			findSubSet(leftSet, leftSet.size(), leftNum, allLeft);
			for (size_t i = 0; i < allLeft.size(); ++i)
			{
				string left = allLeft[i];                                  // left-hand-side is the antecedent
				map<string, int>::const_iterator iter = m_frequentKItemSetCount[left.size()].find(left);
				int support = iter->second;
				double confidence = static_cast<double>(iterRepre->second) / static_cast<double>(support);  // ��rule�����Ŷȣ��ֳ�׼ȷ��accuracy
				if (confidence > (m_minConfidence - 1.0e-7))
				{
					leftInNewRules.emplace_back(left);
					string right = complementSet(iterRepre->first, left);  // right-hand-side is the consequent
					newRules.emplace(make_pair(left, right), confidence);
				}
			}
			if (newRules.size() > 0)
			{
				map<pair<string, string>, double>::const_iterator iter = newRules.begin();
				while (iter != newRules.end())
				{
					m_associationRule.insert(*iter);
					iter++;
				}
				for (size_t i = 0; i < leftInNewRules.size(); ++i)
				{
					string left = leftInNewRules[i];
					for (size_t j = 0; j < left.size(); ++j)
					{
						string item = left.substr(j, 1);
						size_t found = leftSet.find(item);
						if (found != string::npos)
						{
							leftSet.erase(found, 1);
						}
					}
				}
			}
			leftNum++;
		}
		iterRepre++;
	}
}

void Apriori::printRules(const string &fileName)
{
	ofstream outFile(fileName);
	cout << "There are " << m_associationRule.size() << " rules" << endl;
	map<pair<string, string>, double>::const_iterator iter = m_associationRule.begin();
	while (iter != m_associationRule.end())
	{
		outFile << iter->first.first << " --> " << iter->first.second << ", condidence: " << iter->second << endl;
		cout << iter->first.first << " --> " << iter->first.second << ", condidence: " << iter->second << endl;
		iter++;
	}
	outFile.close();
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
		{                                                      // ������ iter ����������pair<string, string>���������stringҲ���ź���ģ���ߵ�string < �ұߵ�string
			string items1 = iter1->first;                      // item set �� string������� item ��һ�� char��Ҳ����length=1��string����˲���char���洢char��������string
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
			if (itemIndex == kItem.size())  // kItem��������ˣ�˵��kItem�����Ԫ�ض��ҵ���
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
	�����һ�� item ����һ�� char��length=1��string����items ����һ�� string
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

/*
	find and return the representative super set of @param representativeKItemSet from m_representativeKItemSetCount
	if not find, return pair("null", 0);
*/
pair<string, int> Apriori::findRepresentativeSuperSetCount(const pair<string, int>& representativeKItemSet)
{
	map<string, int> someRepresentativeSuperSets;

	map<string, int>::const_iterator iter = m_representativeItemSetCount.begin();
	while (iter != m_representativeItemSetCount.end())
	{
		string subSet = representativeKItemSet.first;
		string superSet = iter->first;

		size_t subIndex = 0, superIndex = 0;
		while (subIndex < subSet.size() && superIndex < superSet.size())
		{
			if (subSet.substr(subIndex, 1) == superSet.substr(superIndex, 1))
			{
				subIndex++;
				superIndex++;
			}
			else
			{
				superIndex++;
			}
		}
		if (subIndex == subSet.size())
		{
			someRepresentativeSuperSets.emplace(iter->first, iter->second);   // �����ж������
		}
		iter++;
	}

	// �Ӷ����������ѡȡsupport����һ��
	pair<string, int> superSetCount = pair<string, int>("null", 0);
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

/*
    level ������������ȣ���ʼ�����ʼ�����ԭʼ string �ĳ���
    ��������ȴﵽ prune���ҵ��˳���Ϊ prune �� substr����ʱ�򣬾ͼ�֦
    subSet �����ҵ����Ӽ��Ľ��
	���磺
	vector<string> subset;
	findSubSet("ACDF", 4, 2, subset);
	�õ��Ľ����subset={DF, CF, AF, CD, AD, AC}
*/
void Apriori::findSubSet(string str, unsigned int level, unsigned int prune, vector<string>& subSet)
{
	if (str.length() - level > prune)      // ʣ��Ľڵ�϶�û��ϣ������֦
		return;

	if (str.length() == prune)             // �ҵ�Ŀ�꣬ʣ��ļ�֦
	{
		subSet.emplace_back(str);
	}
	else
	{
		findSubSet(str, level - 1, prune, subSet);
		if (level > 0)
		{
			str.erase(level - 1, 1);
			findSubSet(str, level - 1, prune, subSet);
		}
	}
}

/*
    ��һ�����ϵĻ�������,Ҫ�󼯺������Ԫ�����ź���ģ���С����
	���磺
	fullSet = {ACDF}, left = {AC}
	�򷵻ص� right = {DF}
*/
string Apriori::complementSet(const string & fullSet, const string & left)
{
	string right;
	size_t i = 0, j = 0;
	while(i < fullSet.size() && j < left.size())
	{
		if (fullSet[i] == left[j])
		{
			i++, j++;
		}
		else if (fullSet[i] != left[j])
		{
			right.push_back(fullSet[i]);
			i++;
		}
	}
	if (i < fullSet.size())
	{
		right.append(fullSet.substr(i));
	}

	return right;
}
