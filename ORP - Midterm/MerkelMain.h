#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "Bot.hpp"


class MerkelMain
{
    public:
        // MerkelMain constructor : receiving orderBook class, wallet class, and timestamp from main.cpp
        MerkelMain(OrderBook& _orderBook, Wallet& _wallet, std::string& _timestamp);
        // init function is the overarching function which triggers the simulation's functions whenever necessary. It receives no input, and also does not return any output.
        void init();
    private:
        int timestamp_counter = 1;
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void botModeStart();
        void gotoNextTimeframe();
        void WithdrawOrder();
        int getUserOption();
        void processUserOption(int userOption);

        std::string currentTime;
        std::vector<std::string> currentProducts;
        OrderBook& orderBook;
        Wallet& wallet;
        bool loop_stopper = true;
        std::vector<OrderBookEntry> tracker;

};
