#pragma once

#include <string>

enum class OrderBookType{bid, ask, unknown, asksale, bidsale};

class OrderBookEntry
{
    public:
        // OrderBookEntry constructor. It receives price, amount, timestamp, product, orderType and username as an input, storing them into the variables which have the same names.
        OrderBookEntry( double _price, 
                        double _amount, 
                        std::string _timestamp, 
                        std::string _product, 
                        OrderBookType _orderType, 
                        std::string username = "dataset");
    
        // stringToOrderBookType function converts the string input parameter into an OrderBookType enum value.
        static OrderBookType stringToOrderBookType(std::string s);

        // compareByPriceAsc function compares the price values, which will be used to sort orderBookEntry objects. It receives two OrderBookEntry objects and returns the boolean value of 'true' if the second input parameter's price is bigger.
        static bool compareByPriceAsc(OrderBookEntry& e1, OrderBookEntry& e2)
        {
            return e1.price < e2.price;
        }
        // compareByPriceDesc function compares the price values, which will be used to sort orderBookEntry objects. It receives two OrderBookEntry objects and returns the boolean value of 'true' if the first input parameter's price is bigger.
         static bool compareByPriceDesc(OrderBookEntry& e1, OrderBookEntry& e2)
        {
            return e1.price > e2.price;
        }

        double price;
        double amount;
        std::string timestamp;
        std::string product;
        OrderBookType orderType;
        std::string username;
};
