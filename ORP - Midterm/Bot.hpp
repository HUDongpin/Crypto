#pragma once
#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "CSVReader.h"
#include "Logger.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>

class Bot
{
    public:
        // Bot constructor : receiving orderBook class, wallet class, and timestamp from main.cpp
        Bot(OrderBook& _orderBook, Wallet& _wallet, std::string& _timestamp);
        // init function is the overarching function which triggers bot's functions whenever necessary. It receives no input, and also does not return any output.
        void init();
    
    private:
    
        bool Loop_stopper = true;
        int timestamp_counter = 1;
        std::string CurrentTime;
        std::multimap<std::string, std::map<int, long double>> Closing_Bid_Price_store;
        std::multimap<std::string, std::map<int, long double>> Closing_Ask_Price_store;
        std::multimap<std::string, std::map<int, long double>> Bid_EMA_store;
        std::multimap<std::string, std::map<int, long double>> Ask_EMA_store;
        std::vector<OrderBookEntry> tracker;
        OrderBook& orderBook;
        Wallet& wallet;
    
        long double LongTermEMA(std::string product, OrderBookType orderType);
        // Rolling_EMA function continuously calculates the EMA of either ask or bid closing prices while the bot is running. It receives the name of the product(currency) and orderbooktype and returns a long double value.
        long double ShortTermEMA(std::string product, OrderBookType orderType);
        // Short_term_EMA function calculates the average of the EMAs of the 10 most recent timestamp. the name of the product(currency) and orderbooktype and returns a long double value.
        void makeBid(std::string product);
        // makeBid function allows the bot to create a bid OrderBookEntry object if a short-term EMA is higher than a long-term EMA. Then, it will be stored in a temporary vector before being inserted into the main orderbook. It receives the name of the product(currency), but it will not return any output.
        void makeAsk(std::string product);
        // makeAsk function allows the bot to create an Ask OrderBookEntry object if a long-term EMA is higher than a short-term EMA. Then, it will be stored in a temporary vector before being inserted into the main orderbook. It receives the name of the product(currency), but it will not return any output.
        long double ClosingPrice(std::string product,OrderBookType orderType);
        // ClosingPrice function will calculate the average of either bid or ask orders of the current timestamp. Since no closing price is given in the dataset, this average will be assumed as the closing price of either bid or ask orders for a product. It receives the name of the product(currency) and orderType and returns a long double value.
        long double PredictPrice(std::string product,OrderBookType orderType);
        // PredictPrice function will return a predicted price by calculating the average of the 10 most recent closing price. It receives the name of the product(currency) and orderType and returns a long double value.
        void GoToNextTimeframe();
        // GoToNextTimeframe function increases the timestamp counter by one and move onto the next timeframe. Also, it matches the ask and bids. It neither receives any input nor return any output.
        void NoWithdrawOrder();
        // NoWithdrawOrder function is to bypass the automatic order withdrawl feature. It neither receives any input nor return any output.
        void AutomaticWithdrawOrder();
        // AutomaticWithdrawOrder function allows the bot to automatically remove unfeasible orders if certain conditions are met. It neither receives any input nor return any output.

};

