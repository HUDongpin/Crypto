#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Bot.hpp"
#include "Logger.hpp"

MerkelMain::MerkelMain(OrderBook& _orderBook, Wallet& _wallet, std::string& _timestamp):
    orderBook{_orderBook}, wallet{_wallet}, currentTime{_timestamp}
{
}
//============================================================================================================
void MerkelMain::init()
{
    int input;
    while(loop_stopper)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}
//============================================================================================================

void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 Bot mode
    std::cout << "6: Initiate Bot mode " << std::endl;
    // 7 Withdraw Order
    std::cout << "7: Withdraw Order " << std::endl;
    // 8 Continue
    std::cout << "8: continue " << std::endl;

    std::cout << "=============================================== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}
//============================================================================================================
void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}
//============================================================================================================
void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}
//============================================================================================================
void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
//                orderBook.insertOrder(obe, currentTime);
                tracker.push_back(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds. " << std::endl;
                Logger::ErrorLog("Wallet has insufficient funds. " , currentTime);
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
            Logger::ErrorLog("MerkelMain::enterAsk >> Bad input", currentTime);
        }   
    }
}
//============================================================================================================
void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
//                orderBook.insertOrder(obe,currentTime);
                tracker.push_back(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
                Logger::ErrorLog("Wallet has insufficient funds. " , currentTime);
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
            Logger::ErrorLog("MerkelMain::enterAsk >> Bad input", currentTime);
        }   
    }
}
//============================================================================================================
void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
//============================================================================================================
void MerkelMain::gotoNextTimeframe()
{
    timestamp_counter += 1;
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "bot")
            {
                // update the wallet
                wallet.processSale(sale);
            }
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }
    currentTime = orderBook.getNextTime(currentTime);
}
//============================================================================================================
void MerkelMain::WithdrawOrder()
{
    std::cout << "What order do you want to withdraw? Please provide the placement number of the order." <<std::endl;
    std::cout << "If you want to go back to the menu, type 'q'." <<std::endl;
    std::string input;
    std::getline(std::cin, input);
    while(true)
    {
        if(input == "q") // loop breaker.
        {
            break;
        }
        else{
            try
            {
                int input2 = std::stoi(input);
                if(input2 <= tracker.size()) // there are orders in the vector
                {
                    std::cout << "Order has been withdrawn" << std::endl;
                    tracker.erase(tracker.begin() + (input2- 1)); // User will type a number based on 1-index, not 0-index like vector.
                    break;
                }
                else{ // there are no orders in the vector or the user put higher number than the size of the vector.
                    std::cout << "Invalid input. There is not enough orders." <<std::endl;
                    Logger::ErrorLog("MerkelMain::WithdrawOrder >> invalid input.", currentTime);
                    std::getline(std::cin, input);
                }
            }
            catch(std::exception& e) // the user did not type an integer.
            {
                    std::cout << "Invalid input. Please type an integer value." <<std::endl;
                    Logger::ErrorLog("MerkelMain::WithdrawOrder >> invalid input.", currentTime);
                    std::getline(std::cin, input);
            }
        }
    }
    
    for(OrderBookEntry& e : tracker)
    {
        orderBook.insertOrder(e, currentTime);
        Logger::BidAskLog(e.orderType, e.product, e.price, std::to_string(e.amount), currentTime);
    }
    tracker.clear(); // temp vector has to be cleared for the next timestamp
}
//============================================================================================================
void MerkelMain::botModeStart()
{
    loop_stopper = false;
    Bot bot{orderBook, wallet, currentTime};
    bot.init(timestamp_counter);
}
//============================================================================================================
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-7" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // Wrong input
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}
//============================================================================================================
void MerkelMain::processUserOption(int userOption)
{
    switch(userOption) // switch conditional to remove repetitive "if" statements.
    {
        case 0 :
            std::cout << "Invalid choice. Choose 1-6" << std::endl;
            break;
        case 1 :
            printHelp();
            break;
        case 2 :
            printMarketStats();
            break;
        case 3:
            enterAsk();
            break;
        case 4:
            enterBid();
            break;
        case 5:
            printWallet();
            break;
        case 6:
            botModeStart();
            break;
        case 7:
            WithdrawOrder();
            break;
        case 8:
            gotoNextTimeframe();
            break;
    }
}
