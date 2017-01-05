# ibTWS_L2_Miner

The ibTWS_L2_Miner is a small command line tool to feed a MySQL database with orderbook data which is received
from the Interactive Brokers Trader Workstation. For communication we make use of Jan Boonen's TwsApiC++ library
<href>https://github.com/JanBoonen/TwsApiCpp</href> . We also make use of some the sample code he provides for the implementation 
of the EWrapper functions.

----------------------------
Prerequisites
----------------------------
1. MySQL Connector/C++
2. TwsApiC++
3. Standalone version of the Interactive Brokers TWS.
4. Level 2 market data subscription for the Contracts you want to collect data of.

----------------------------
Setup
----------------------------
At present the database and table names as well as well as the MySQL username and password are harcoded, so they have to be 
adjusted in the source code. To run the standard scenario it is easiest to set up the database as following
```
mysql> CREATE DATABASE gbl_level2;
mysql> USE gbl_level2;
mysql> CREATE TABLE test (id INT(11), Time DATETIME(6), Price DOUBLE, Size INT(11), Bookposition INT(11), Side CHAR(3), LastVolume INT(11), LastPrice DOUBLE);
```
There is no check if there is existing data in a table, so its advisable to define a new table for each run when the tool is 
used prodictively. The contract under observation can be adjusted in main.cpp and is currently set to the 10 year german 
treasury bond at Eurex.

I you look for something to visualize the data and play around with it, please follow my other project: https://github.com/nrothermel/DOMA
