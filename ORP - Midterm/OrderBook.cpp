#include "OrderBook.h"
#include "CSVReader.h"
#include <algorithm>
#include <iostream>

// construct, reading a csv data file 
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

std::unordered_set<std::string> OrderBook::getKnownProducts()
{
    if(products.empty()) // If this function has run before, then it shouldn't run again. Just use cached data.
    {
        for(const auto& e: orders)
        {
            for(const auto& v: e.second)
            {
                products.emplace(v.product);
            }
        }
    }
    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                        std::string product,
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    
    for(OrderBookEntry& e : orders[timestamp])
    {
        if (e.orderType == type && e.product == product)
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return orders.begin()->first;
}

std::string OrderBook::getNextTime(const std::string& timestamp)
{
    auto Next = orders.upper_bound(timestamp); // Get the upper bound of the current timestamp

    if (Next == orders.end())
    {
        return "end"; // the loop stopper keyword
    }
    return Next->first;
}

void OrderBook::insertOrder(OrderBookEntry& order, std::string timestamp)
{
    orders[timestamp].push_back(order);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);
    std::vector<OrderBookEntry> sales; 

    if (asks.size() == 0 || bids.size() == 0)
    {
        return sales; // no bids or asks 
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    
    for (OrderBookEntry& ask : asks)
    {
        for (OrderBookEntry& bid : bids)
        {
            // if bid.price >= ask.price # we have a match 
            if (bid.price >= ask.price)
            {
            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (bid.username == "bot")
                {
                    sale.username = "bot";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
                if (ask.username == "bot")
                {
                    sale.username = "bot";
                    sale.orderType =  OrderBookType::asksale;
                }
            
                // if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break;
                }
                if (bid.amount > ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount =  bid.amount - ask.amount;
                    break;
                }
                // if bid.amount < ask.amount # bid is completely gone, slice the ask 
                if (bid.amount < ask.amount && 
                   bid.amount > 0 && ask.amount >0)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount = ask.amount - bid.amount;
                    bid.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;             
}
