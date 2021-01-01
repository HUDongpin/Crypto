#pragma once
#include "OrderBookEntry.h"
#include <fstream>
#include <string>
class Logger
{
    public:
        // Logger constructor. It neither receives any input nor returns any output.
        Logger();
    
        //AssetLog function logs the assets and how they change over time in "Asset.txt" file. It takes a string value of the asset record from the wallet and timestamp, while returning nothing.
        static void AssetLog(std::string input, std::string timestamp);
    
        //BidAskLog function logs the bids and asks that the bot made in "BidAsk.txt" file. It takes several input parameters related to the bids or asks and current timestamp, while returning nothing.
        static void BidAskLog(OrderBookType orderType, std::string product, long double price, std::string amount, std::string timestamp);
    
        //BidAskLog function logs successful bids and asks that the bot made in "Match.txt" file. It takes several input parameters related to the bids or asks and current timestamp, while returning nothing.
        static void MatchLog(OrderBookType orderType, std::string product, long double price, std::string amount, std::string timestamp);
            
        //ErrorLog function logs error messages generated while the bot was running. It takes a string value of the message and timestamp, while returning nothing.
        static void ErrorLog(std::string message, std::string timestamp);
};
