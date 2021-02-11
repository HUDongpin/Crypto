#include "Wallet.h"
#include "MerkelMain.h"
#include "OrderBook.h"
#include "Bot.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <chrono>

int main()
{
    Wallet wallet{};
    wallet.insertCurrency("BTC", 10000);
    wallet.insertCurrency("ETH", 10000);
    wallet.insertCurrency("USDT", 10000);
    wallet.insertCurrency("DOGE", 10000);
        
    std::cout << "How do you want to trade? Press 'm' for manual mode or 'b' for bot mode" << std::endl;
    std::string flag;
    std::getline(std::cin, flag);

    while(true)
    {
        if(flag=="m") // manual mode
        {
            std::cout << "Manual mode is selected." << std::endl;
            std::cout<< "Data are being loaded. Please wait." << std::endl;
            OrderBook orderBook{"20200601.csv"}; // Read the CSV file and convert data into a map.
            std::string currentTime = orderBook.getEarliestTime();
            MerkelMain app{orderBook, wallet, currentTime};
            app.init();
            break;
        }
        if(flag=="b") // bot mode
        {
            std::cout << "Bot mode is selected." << std::endl;
            std::cout<< "Data are being loaded. Please wait." << std::endl;
            OrderBook orderBook{"20200601.csv"}; // Read the CSV file and convert data into a map.
            std::string currentTime = orderBook.getEarliestTime();
            Bot bot{orderBook, wallet, currentTime};
            bot.init();
            break;
        }
        else
        {
            std::cout << "Bad Input. Please type either 'm' or 'b'" << std::endl;
            std::getline(std::cin, flag);
            continue;
        }
        
    }

    return 0;
}
