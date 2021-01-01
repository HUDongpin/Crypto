#include "Wallet.h"
#include <iostream>
#include "CSVReader.h"

Wallet::Wallet()
{

}

void Wallet::insertCurrency(std::string type, long double amount)
{
    double balance;
    if (amount < 0)
    {
        throw std::exception{};
    }
    if (currencies.count(type) == 0) // currency not there yet
    {
        balance = 0;
    }
    else
    { // the currency is there
        balance = currencies[type];
    }
    balance += amount; 
    currencies[type] = balance; 
}

bool Wallet::removeCurrency(std::string type, long double amount)
{
    if (amount < 0)
    {
        return false; 
    }
    if (currencies.count(type) == 0) // not there yet
    {
        return false;
    }
    else { // we have enough amount
        if (containsCurrency(type, amount)) // we have enough
        {
            currencies[type] -= amount;
            return true;
        } 
        else // they have the currency but not enough.
            return false; 
    }
}

bool Wallet::containsCurrency(std::string type, long double amount)
{
    if (currencies.count(type) == 0 || currencies[type] <= 0) // not there yet
        return false;
    else 
        return currencies[type] >= amount;
}

std::string Wallet::toString()
{
    std::string s;
    for (std::pair<std::string, long double> pair : currencies)
    {
        std::string currency = pair.first;
        long double amount = pair.second;
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
}

bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    // ask
    if (order.orderType == OrderBookType::ask && order.price > 0 && order.amount > 0)
    {
        long double amount = order.amount;
        std::string currency = currs[0];

        return containsCurrency(currency, amount);
    }
    // bid
    if (order.orderType == OrderBookType::bid && order.price > 0 && order.amount > 0)
    {
        long double amount = order.amount * order.price;
        std::string currency = currs[1];
        return containsCurrency(currency, amount);
    }
    return false; 
}
      

void Wallet::processSale(OrderBookEntry& sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    // ask
    if (sale.orderType == OrderBookType::asksale)
    {
            long double outgoingAmount = sale.amount;
            std::string outgoingCurrency = currs[0];
            long double incomingAmount = sale.amount * sale.price;
            std::string incomingCurrency = currs[1];

            currencies[incomingCurrency] += incomingAmount;
            currencies[outgoingCurrency] -= outgoingAmount;
    }
    // bid
    if (sale.orderType == OrderBookType::bidsale)
    {
            long double incomingAmount = sale.amount;
            std::string incomingCurrency = currs[0];
            long double outgoingAmount = sale.amount * sale.price;
            std::string outgoingCurrency = currs[1];

            currencies[incomingCurrency] += incomingAmount;
            currencies[outgoingCurrency] -= outgoingAmount;
    }
}
std::ostream& operator<<(std::ostream& os,  Wallet& wallet)
{
    os << wallet.toString();
    return os;
}

