# Apriori
An association algorithm: apriori

This is apriori algorithm written in C++.

There are two versions of apriori algorithm in this repository. 
1. string version (branch master): an item in the transaction is a char, and an item set is a string. Char is treated as a string with length=1 in this program.
2. item version (branch ItemClass): an item is the transaction is a class Item, and an item set is a class ItemSet. An item is sometimes treated as an itemSet with size=1.


The data in file transactionData.txt is from this [Youtube video](https://www.youtube.com/watch?v=TcUlzuQ27iQ)
