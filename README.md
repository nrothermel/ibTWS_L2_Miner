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
adjusted in the source code.
