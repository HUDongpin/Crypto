#pragma once

#include <string>
#include <map>
#include "OrderBookEntry.h"
#include <iostream>

class Wallet 
{
    public:
        // Wallet constructor. It neither receives any input nor returns any output.
        Wallet();
    
        // insertCurrency function inserts currency to the wallet. It takes a string value of type, which is the name of the currency and long double value of the currency's amount. It does not return any output. */
        void insertCurrency(std::string type, long double amount);
    
        // removeCurrency function removes a sent amount of the currency from the wallet. It takes a string value of type, which is the name of the currency and long double value of the currency's amount. It returns a boolean value depending on whether a removal was successful.
        bool removeCurrency(std::string type, long double amount);
    
        // containsCurrency function checks if the wallet contains the enough amount of currency to perform a transaction. It takes a string value of type, which is the name of the currency and long double value of the currency's amount. It returns a boolean value depending on whether the wallet has enough currency.
        bool containsCurrency(std::string type, long double amount);
    
        //canFulfillOrder checks if the wallet can cope with this ask or bid. It takes an OrderBookEntry object as an input, returning depending on whether a bid or ask can be made.*/
        bool canFulfillOrder(OrderBookEntry order);

        // processSale function updates the contents of the wallet based on the matched bids and asks. It takes an OrderBookEntry object as an input, while returning nothing.
        void processSale(OrderBookEntry& sale);
    
        // toString function generates a string representation of the wallet. It neither receives any input nore returns any output.*/
        std::string toString();
        friend std::ostream& operator<<(std::ostream& os, Wallet& wallet);
        
    private:
        std::map<std::string,double> currencies;
};



	

