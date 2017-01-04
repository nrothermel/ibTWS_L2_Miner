/*
 * File:   mywrapper_header.h
 * Author: Niels Rothermel. Sample code adapted from
 * Jan Boonen's TwsApiC++ test Program.
 *
 * Created on 31. Juli 2014, 15:57
 */

#ifndef MYWRAPPER_HEADER_H
#define	MYWRAPPER_HEADER_H

#include "TwsApiL0.h"
#include "TwsApiDefs.h"
using namespace TwsApi;

// to use the Sleep function
#ifdef WIN32
	#include <windows.h>		// Sleep(), in miliseconds
	#include <process.h>
	#define CurrentThreadId GetCurrentThreadId
#else
	#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )
	#include <pthread.h>
	#define CurrentThreadId pthread_self
#endif

#define PrintProcessId printf("%ld  ", CurrentThreadId() )

#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

int makesqlstatement(int entryID, std::string time, int lastvolume, double lastprice, sql::Connection *con){

    sql::Statement *stmt;
    sql::PreparedStatement *pstmt;

    stmt = con->createStatement();
    stmt->execute("USE gbl_level2");// Name of the Database
    if(lastvolume == -1){
        pstmt = con->prepareStatement("INSERT INTO test(Time, LastPrice, id) VALUES (?, ?, ?)");//change "test" to the name of your table name
        pstmt->setDateTime(1, time);
        pstmt->setDouble(2, lastprice);
        pstmt->setInt(3, entryID);
        pstmt->execute();
    }
    
    if(lastprice == -1){
        pstmt = con->prepareStatement("INSERT INTO test(Time, LastVolume, id) VALUES (?, ?, ?)");//change "test" to the name of your table name
        pstmt->setDateTime(1, time);
        pstmt->setInt(2, lastvolume);
        pstmt->setInt(3, entryID);
        pstmt->execute();
    }
 
    delete stmt;
    delete pstmt;

}

int makesqlstatement_DOM(int entryID, std::string time, int bookposition, std::string side, double price, int size, sql::Connection *con){
    sql::Statement *stmt;
    sql::PreparedStatement *pstmt;
    stmt = con->createStatement();
    stmt->execute("USE gbl_level2");

    pstmt = con->prepareStatement("INSERT INTO test(Time, Bookposition, Side, Price, Size, id) VALUES (?, ?, ?, ?, ?, ?)");
    pstmt->setDateTime(1, time);
    pstmt->setInt(2, bookposition);
    pstmt->setString(3, side);
    pstmt->setDouble(4, price);
    pstmt->setInt(5, size);
    pstmt->setInt(6, entryID);
    pstmt->execute(); 
    
    delete stmt;
    delete pstmt;

}

std::string give_timestamp(){
    char time_stamp[28];
    struct timeval tv;
    time_t clocktime;
    
    time(&clocktime);
    struct tm *_tm = localtime(&clocktime);
    
    gettimeofday(&tv, NULL);//Subsecond fraction of the current second
    
    sprintf(time_stamp,"%04d-%02d-%02d %02d:%02d:%02d.%06ld",_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,_tm->tm_hour,_tm->tm_min,_tm->tm_sec,tv.tv_usec);
    std::string timestring = time_stamp;
    
    return timestring;
}
//----------------------------------------------------------------------------
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:
	MyEWrapper( bool CalledFromThread = true ): EWrapperL0( CalledFromThread ) {}

        sql::Connection *con;
        double last_price = -1;
        int last_volume = -1;
        int entryID = 0;
        
	virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute )
	{
	
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		PrintProcessId,printf
			( "TP: %4ld %02d:%02d:%02d %10s %5.3f\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, price
			);
                //timestamp creation 
            std::string timestamp = give_timestamp();
                if(field == LAST){
                    last_price = price;
                    makesqlstatement(entryID,timestamp,last_volume,price,con);
                    last_price = -1;
                    entryID++;
                }
	}

	virtual void tickSize( TickerId tickerId, TickType field, int size )
	{
	//	time_t		_t	= GetEventTime();		// !! from L1
		time_t		_t; time(&_t);
		struct tm*	_tm	= localtime( &_t );
		PrintProcessId,printf
			( "TS: %4ld %02d:%02d:%02d %10s %5d\n"
			, tickerId
			, _tm->tm_hour, _tm->tm_min, _tm->tm_sec
			, *(TickTypes::ENUMS)field, size
			);
               
            std::string timestamp = give_timestamp();
                if(field == LAST_SIZE){
                    last_volume = size;
                    makesqlstatement(entryID,timestamp,size,last_price,con);
                    last_volume = -1;
                    entryID++;
                }
	}

	virtual void winError( const IBString& str, int lastError )
	{
		PrintProcessId,printf( "WinError: %d = %s\n", lastError, (const char*)str );
	}

	virtual void connectionClosed()
	{
		PrintProcessId,printf( "Connection Closed\n");
	}


	virtual void error( const int id, const int errorCode, const IBString errorString )
	{
		PrintProcessId,printf( "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
	}

   virtual void updateMktDepth(TickerId id, int position, int operation, int side, double price, int size )
   {
       std::string timestamp = give_timestamp();
       std::string bookside;

       if(operation == 1 || operation == 0){ // operation = 1 means "update" 0 means "insert"
           if(side == 0){bookside = "Ask";}
           if(side == 1){bookside = "Bid";}    
           makesqlstatement_DOM(entryID,timestamp,position,bookside,price,size,con);
           entryID++;
           }
   }

   virtual void updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size )
   {
       std::cout << "Level 2 Event" << std::endl;
   }


	virtual void connectionOpened( void )
	{
		PrintProcessId,printf( "Connection Opened\n");
	}

	virtual void checkMessagesStarted( void )
	{
		PrintProcessId,printf( ">>> checkMessagesStarted\n");
	}

	virtual void checkMessagesStopped( void )
	{
		PrintProcessId,printf( "<<< checkMessagesStopped\n");
	}


    virtual void currentTime(long time){
        std::cout << time << std::endl;
        };

};

//----------------------------------------------------------------------------
// TestEnums
//----------------------------------------------------------------------------
void TestEnums( void )
{
	IBString			id = *TickTypes::Bid;
	id = *OrderStatus::PendingCancel;

	IBString			y;
	y = *TriggerMethod::LastPrice;

	OrderStatus::ENUMS	e;
	if( e *= "PendingCancel" )
	{ printf("OK\t"); }
	else
	{ printf("NOK\t"); }
	printf( "%4d %s\n", e, *e );


	if( e *= "not a status" )
	{ printf("OK\t"); }
	else
	{ printf("NOK\t"); }
	printf( "%4d %s\n", e, *e );

	switch( OrderStatus("PendingCancelxxxx") )
	{
	case OrderStatus::PendingCancel:
		{ printf("OK\n"); } break;
	case OrderStatus::_INVALID_:
		{ printf("NOK\n"); } break;
	default:
		{ printf("??\n"); } break;
	}

	// The iterator has a similar interface as the of the std::map
	for( UpdateAccountValueKey::iterator ac = UpdateAccountValueKey::begin(); ac != UpdateAccountValueKey::end(); ++ac )
		printf( "%4d %s\n", ac->first, ac->second );

}




#endif	/* MYWRAPPER_HEADER_H */

