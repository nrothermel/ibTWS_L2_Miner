/* 
 * File:   main.cpp
 * Author: Niels Rothermel
 *
 * Created on January 3, 2017, 11:08 AM
 */

#include <cstdlib>
#include <iostream>
#include "mywrapper_header.h"


int main(int argc, char** argv) {
//Defining Contract to observe
    Contract GBL;
    GBL.symbol = "GBL";
    GBL.secType = *SecType::FUT;
    GBL.exchange = *Exchange::DTB;
    GBL.currency = "EUR";
    GBL.expiry = "201703";
    
    sql::Driver *driver;
    sql::Connection *con;
    
    driver = get_driver_instance();
    
    try{
        con = driver->connect("tcp://127.0.0.1:3306","####","####");//MySQL username and password
    }
    catch(...){
        std::cout << "Exception: Couldnt' connect to MySQL server. Check if it is running" << std::endl;
        return 0;
    }
//    MyEWrapper Obj inherited from EWrapperL0
    MyEWrapper MW;
    MW.con = con;
//    EClientL0 Obj pointer is initilized with MyEWrapper Obj
    EClientL0*	EC = EClientL0::New( &MW );
    EC->eConnect("127.0.0.1",7496,101);

    EC->reqMktData(112,GBL,"233",false);
    EC->reqMktDepth(112,GBL,20);
    std::cin.get();
   
    EC->eDisconnect();
    
    driver->threadEnd();
    
    delete con;
    
    return 0;
}
