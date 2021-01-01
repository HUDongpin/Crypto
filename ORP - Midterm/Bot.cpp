#include "Bot.hpp"
#include <chrono>

Bot::Bot(OrderBook& _orderBook, Wallet& _wallet, std::string& _timestamp):
    orderBook(_orderBook), wallet(_wallet), CurrentTime(_timestamp) 
{
}
//============================================================================================================
void Bot::init()
{
    Logger::AssetLog(wallet.toString(), CurrentTime);
    
    std::cout<< "Do you want the bot to automatically withdraw unfeasible orders? [y/n]" <<std::endl;
    std::string flag;
    std::getline(std::cin, flag);
    while(true){
        if(flag == "y") // Automatic withdrawl activated 
        {
            auto start = std::chrono::steady_clock::now(); // to calculate execution time 
            std::cout << "The trading is initiated." << std::endl;
            while(Loop_stopper){
                for (std::string p : orderBook.getKnownProducts())
                {
                    makeBid(p);
                    makeAsk(p);
                };
                AutomaticWithdrawOrder();
                GoToNextTimeframe();
            }
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> execution_seconds = end - start;
            std::cout << "Execution time: " << execution_seconds.count() << " seconds" << "\n" <<std::endl;
            break;
        }
        if(flag == "n") // No withdrawl activated 
        {
            auto start = std::chrono::steady_clock::now(); // to calculate execution time 
            std::cout << "The trading is initiated." << std::endl;
            while(Loop_stopper){
                for (std::string p : orderBook.getKnownProducts())
                {
                    makeBid(p);
                    makeAsk(p);
                };
                NoWithdrawOrder();
                GoToNextTimeframe();
            }
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> execution_seconds = end - start;
            std::cout << "Execution time: " << execution_seconds.count() << " seconds" << "\n" <<std::endl;
            break;
        }
        else
        {
            std::cout << "Bad input. Please type either 'y' or 'n'." <<std::endl;
            std::getline(std::cin, flag);
            continue;
        }
    }
}

//============================================================================================================
// Closing_price is assumed as the average of all the prices of either bid or ask at a specific timestamp 
long double Bot::ClosingPrice(std::string product, OrderBookType orderType)
{
    long double sum_total = 0.0;
    long double counter = 0.0;
    std::map<int, long double> price_store;
    std::multimap<std::string, std::map<int, long double>>::iterator it;
    std::pair<std::map<int, long double>::iterator, bool> ret;
    
    for (const OrderBookEntry& o : orderBook.orders[CurrentTime])
    {
        if(o.product == product &&
           o.orderType == orderType)
        {
            sum_total += o.price;
            counter += 1;
        }
    }
    price_store[timestamp_counter] = sum_total/counter; // storing the average at a given timestamp 
    
    if (orderType == OrderBookType::bid)
    {
        if(timestamp_counter == 1) // For the very first timestamp, simply store vector
        {
            Closing_Bid_Price_store.insert(std::pair<std::string, std::map<int, long double>> (product, price_store));
        }
        else
        {
            ret = Closing_Bid_Price_store.find(product)->second.emplace(timestamp_counter, sum_total/counter); // iterator mapped between vector and boolean
            
            if(ret.second == true) // if data could be stored in the map, this means that there is no duplicate with the same timestamp within the map. 
            {
                Closing_Bid_Price_store.insert(std::pair<std::string, std::map<int, long double>> (product, price_store));
            }
        }
    }
    else{
        if(timestamp_counter == 1)
        {
            Closing_Ask_Price_store.insert(std::pair<std::string, std::map<int, long double>> (product, price_store));
        }
        else
        {
            ret = Closing_Ask_Price_store.find(product)->second.emplace(timestamp_counter, sum_total/counter);
            if(ret.second == true)
            {
                Closing_Ask_Price_store.insert(std::pair<std::string, std::map<int, long double>> (product, price_store));
            }
        }
    }
    return price_store[timestamp_counter]; // If either data with the same timestamp is already stored, then simply return the average. Data must be not stored again
}

//============================================================================================================
/* The price gets predicted in two steps:
 1) For each OrderBookType, the average of the closing prices for the recent 10 timestamps is calculated.
 2) For Bid, the predicted price = Ask closing price if the bid average of the past 10 timestamps is lower than the closing price of Ask at the current timestamp.
 3) For Ask, the predicted price = Bid closing price if the ask average of the past 10 timestamps is high than the closing price of Bid at the current timestamp */
long double Bot::PredictPrice(std::string product, OrderBookType orderType)
{
    long double Ask_closing_price = ClosingPrice(product, OrderBookType::ask);
    long double Bid_closing_price = ClosingPrice(product, OrderBookType::bid);
    
    long double price_sum = 0.0;
    if(timestamp_counter > 10)
    {
        if (orderType == OrderBookType::bid)
        {
            for(int i = timestamp_counter - 10; i < timestamp_counter; i++)
            {
                std::multimap<std::string, std::map<int, long double>>::iterator it = Closing_Bid_Price_store.find(product);
                price_sum += it->second[i]; //as high as the minimum of the Ask price
            }
            if((price_sum / 10) < Ask_closing_price)
            {
                return Ask_closing_price;
            }
            
        }
        else
        {
            for(int i = timestamp_counter - 10; i < timestamp_counter; i++)
            {
                std::multimap<std::string, std::map<int, long double>>::iterator it = Closing_Ask_Price_store.find(product);
                price_sum += it->second[i]; //closer to the maximum of the bid price
            }
            if((price_sum / 10) > Bid_closing_price)
            {
                return Bid_closing_price;
            }
        }
    }
    else
    {
        return ClosingPrice(product, orderType);
    }
    return price_sum/10;
}

//============================================================================================================
// EMA = Closing_price(t) * k + EMA of previous period * (1−k) when k = 2 / (# of period + 1) 
long double Bot::LongTermEMA(std::string product, OrderBookType orderType)
{
    std::map<int, long double> Bid;
    std::map<int, long double> Ask;
    std::pair<std::map<int, long double>::iterator, bool> ret;
    
    double EMA_stamp = timestamp_counter; //the number of timestamps in EMA
    double k_value = 2.0 / (EMA_stamp + 1.0);
    
    if(orderType == OrderBookType::bid)
    {
        long double closingPrice = ClosingPrice(product, OrderBookType::bid);
        if(timestamp_counter == 1)
        {
            Bid[timestamp_counter] = closingPrice;
            Bid_EMA_store.insert(std::pair<std::string, std::map<int, long double>> (product, Bid));
            // Bid_EMA_store is a multimap which maps between product and its Bid EMA history
            return closingPrice;
        }
        else
        {
            std::multimap<std::string, std::map<int, long double>>::iterator it = Bid_EMA_store.find(product);
            long double previousEMA = it->second[timestamp_counter-1];
            ret = Bid_EMA_store.find(product)->second.emplace(timestamp_counter, ((closingPrice - previousEMA)* k_value)+ previousEMA);
            if(ret.second == true) // There was nothing stored in the map within the multimap.
            {
                Bid[timestamp_counter] = ((closingPrice - previousEMA)* k_value)+ previousEMA;
                Bid_EMA_store.insert(std::pair<std::string, std::map<int, long double>> (product, Bid));
            }
            return Bid[timestamp_counter];
        }
    }
    else
    {
        long double closingPrice = ClosingPrice(product, OrderBookType::ask);
        if(timestamp_counter == 1)
        {
            Ask[timestamp_counter] = closingPrice;
            Ask_EMA_store.insert(std::pair<std::string, std::map<int, long double>> (product, Ask));
            // Ask_EMA_store is a multimap which maps between product and its Ask EMA history
            return closingPrice;
        }
        else
        {
            std::multimap<std::string, std::map<int, long double>>::iterator it = Ask_EMA_store.find(product);
            long double previousEMA = it->second[timestamp_counter-1];
            ret = Ask_EMA_store.find(product)->second.emplace(timestamp_counter, ((closingPrice - previousEMA)* k_value)+ previousEMA);
            if(ret.second == true)// There was nothing stored in the map within the multimap.
            {
                Ask[timestamp_counter] = ((closingPrice - previousEMA)* k_value)+ previousEMA;
                Ask_EMA_store.insert(std::pair<std::string, std::map<int, long double>> (product, Ask));
            }
            return Ask[timestamp_counter];
        }
    }
}

//============================================================================================================
// the average of EMA for the 10 recent timestamp
long double Bot::ShortTermEMA(std::string product, OrderBookType orderType)
{
    long double EMA_sum = 0.0;
    if(timestamp_counter > 10)
    {
        if (orderType == OrderBookType::bid)
        {
            for(int i = timestamp_counter - 10; i <timestamp_counter; ++i )
            {
                std::multimap<std::string, std::map<int, long double>>::iterator it = Bid_EMA_store.find(product);
                EMA_sum += it->second[i];
            }
        }
        else
        {
            for(int i = timestamp_counter - 10; i <timestamp_counter; ++i )
            {
                std::multimap<std::string, std::map<int, long double>>::iterator it = Ask_EMA_store.find(product);
                EMA_sum += it->second[i];
            }
        }
    }
    else
    {
        return LongTermEMA(product, orderType); // just rolling EMA is enough for the first 10 timestamps. 
    }
    return EMA_sum / 10;
}

//============================================================================================================
// The bot will make a bid order with the predicted price if the short term EMA is higher than the long term/rolling EMA) 
void Bot::makeBid(std::string product)
{
    long double Bid_Rolling_EMA = LongTermEMA(product, OrderBookType::bid);
    long double Bid_short_term_EMA = ShortTermEMA(product, OrderBookType::bid);
    long double Predicted_price = PredictPrice(product, OrderBookType::bid);

    if (Bid_short_term_EMA > Bid_Rolling_EMA) //short term MA > long term MA  : buy!
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                std::to_string(Predicted_price),
                std::to_string(0.5),
                CurrentTime,
                product,
                OrderBookType::bid);
            obe.username = "bot";

            if (wallet.canFulfillOrder(obe))
            {
                tracker.push_back(obe);
            }
            else
            {
                Logger::ErrorLog("Bot::wallet >> Wallet has insufficient funds.", CurrentTime);
            }
        }
        catch (const std::exception& e)
        {
            Logger::ErrorLog("Bot::makeBid >> Bad input.", CurrentTime);
        }
    }
}

//============================================================================================================
// The bot will make an ask order with the predicted price if the long-term EMA is higher than the short-term EMA. 
void Bot::makeAsk(std::string product)
{
    long double Ask_Rolling_EMA = LongTermEMA(product, OrderBookType::ask);
    long double Ask_short_term_EMA = ShortTermEMA(product, OrderBookType::ask);
    long double Predicted_price = PredictPrice(product, OrderBookType::ask);

    if (Ask_short_term_EMA < Ask_Rolling_EMA) //short term MA < long term MA  : Sell!
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                std::to_string(Predicted_price),
                std::to_string(0.5),
                CurrentTime,
                product,
                OrderBookType::ask
            );
            obe.username = "bot";

            if (wallet.canFulfillOrder(obe))
            {
                tracker.push_back(obe); // Instead of directly inserting the order, storing the order in a temp vector in case of withdrawl.
            }
            else {
                Logger::ErrorLog("Bot::wallet >> Wallet has insufficient funds.", CurrentTime);
            }
        }catch (const std::exception& e)
        {
            Logger::ErrorLog("Bot::makeAsk >> Bad input.", CurrentTime);
        }
    }
}

//============================================================================================================
// Orders in the temp vector will be inserted into the main orderbook without any withdrawl.
void Bot::NoWithdrawOrder()
{
    for(int i = 0; i < tracker.size(); i++)
    {
        Logger::BidAskLog(tracker[i].orderType, tracker[i].product, tracker[i].price, std::to_string(tracker[i].amount) , CurrentTime);
        orderBook.insertOrder(tracker[i], CurrentTime);
    }
    tracker.clear();// temp vector has to be cleared for the next timestamp
}

//============================================================================================================
/* The bot will cancel order automatically if
 1) If a bit order's price is lower than the minimum price of ask orders at the current timestamp.
 2) If an ask order's price is higher than the maximum price of bid orders at the current timestamp. */
void Bot::AutomaticWithdrawOrder()
{
    std::vector<OrderBookEntry> bid;
    std::vector<OrderBookEntry> ask;
    for(OrderBookEntry& e : orderBook.orders[CurrentTime]) // Dividing bid and ask orders
    {
        if(e.orderType == OrderBookType::bid)
        {
            bid.push_back(e);
        }
        else
        {
            ask.push_back(e);
        }
    }
    long double minAsk = OrderBook::getLowPrice(ask);
    long double maxBid = OrderBook::getHighPrice(bid);
    
    for(int i = 0; i < tracker.size(); i++)
    {
        //bit order's price is lower than the minimum price of ask orders
        if(tracker[i].orderType == OrderBookType::bid && tracker[i].price < minAsk)
        {
            tracker.erase(tracker.begin() + i);
        }
        // ask order's price is higher than the maximum price of bid orders 
        if(tracker[i].orderType == OrderBookType::ask && tracker[i].price > maxBid)
        {
            tracker.erase(tracker.begin() + i);
        }
        Logger::BidAskLog(tracker[i].orderType, tracker[i].product, tracker[i].price, std::to_string(tracker[i].amount) , CurrentTime);
        orderBook.insertOrder(tracker[i], CurrentTime);
    }
    tracker.clear(); // temp vector has to be cleared for the next timestamp
}

//============================================================================================================
void Bot::GoToNextTimeframe()
{
    timestamp_counter += 1;
    
    for (std::string p : orderBook.getKnownProducts())
    {
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, CurrentTime);
        for (OrderBookEntry& sale : sales)
        {
            if (sale.username == "bot" || sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
                Logger::MatchLog(sale.orderType, sale.product, sale.price, std::to_string(sale.amount), CurrentTime);
                Logger::AssetLog(wallet.toString(), CurrentTime);
            }
        }
    }

    CurrentTime = orderBook.getNextTime(CurrentTime);
    if(CurrentTime == "end") // stopping while loop in the init function.
    {
        Loop_stopper = false;
        std::cout << "\n" << "Bot has finished the trading. The program will be terminated."<< "\n" <<std::endl;
        std::cout << "==============================================================" << "\n" << std::endl;
        std::cout << "The final overview of the asset" << "\n" << std::endl;
        std::cout << wallet.toString() << std::endl;
        std::cout << "==============================================================" << "\n" <<std::endl;
    }
}
