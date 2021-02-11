#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

class OrderBook
{
    public:
        // OrderBook constructor : by using CSVReader::ReadCSV, it stores a map which links a timestamp and a vector of orders.
        OrderBook(std::string filename);
        // getKnownProducts function finds out all known products in the dataset. It does not take any input, and it turns an unordered set of the known products.
        std::unordered_set<std::string> getKnownProducts();
        // getOrders function returns a vector of orders that are matched with certain criteria. The criteria are determined by the input parameters, which are orderBookType, product name, and timestamp.
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);
        // getEarliestTime returns the earliest time in the orderbook. It does not receive any input, and it returns a string value.
        std::string getEarliestTime();
        // getNextTime returns the next timestamp. It takes the current timestamp as a reference and returns a string value of the next timestamp.
        std::string getNextTime(const std::string& timestamp);
        // insertOrder function inserts a newly created order into the main orderbook. It takes an OrderBookEntry object and timestamp, but it does not return any output.
        void insertOrder(OrderBookEntry& order, std::string timestamp);

        // matchAsksToBids function matches asks and bids within the timestamp if certain conditions are met. It takes the name of the product(currency) and timestamp, while returning a vector of the processed orders to update wallet.
        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);
    
        // getHighPrice function returns the highest price within a vector of orders. It takes a vector of orders as an input, returning a double value.
        static double getHighPrice(std::vector<OrderBookEntry>& orders);
    
        // getLowPrice function returns the lowest price within a vector of orders. It takes a vector of orders as an input, returning a double value.
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        std::map<std::string, std::vector<OrderBookEntry>> orders;
        std::unordered_set<std::string> products;

    private:
};
