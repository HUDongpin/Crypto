#include "Logger.hpp"


Logger::Logger()
{
}

void Logger::AssetLog(std::string input, std::string timestamp)
{
    std::fstream Asset;
    Asset.open("Asset.txt", std::fstream::app | std::fstream::out);
    Asset << timestamp << "[INFO]" << "\n" << input <<"\n";
    Asset.close();
}

void Logger::BidAskLog(OrderBookType orderType, std::string product, long double price, std::string amount, std::string timestamp)
{
    std::fstream BidAsk;
    BidAsk.open("BidAsk.txt", std::fstream::app | std::fstream::out);
    if (orderType == OrderBookType::bid){
        BidAsk << timestamp << "[INFO] Bid made. Product : " << product << "  Price : " << price << "  Amount : " << amount <<"\n";
    }
    else{
        BidAsk << timestamp << "[INFO] Ask made. Product : " << product << "  Price : " << price << "  Amount : " << amount << "\n";
    }
    BidAsk.close();
}

void Logger::MatchLog(OrderBookType orderType, std::string product, long double price, std::string amount, std::string timestamp)
{
    std::fstream Match;
    
    Match.open("Match.txt", std::fstream::app | std::fstream::out);
    if (orderType == OrderBookType::bidsale){
        Match << timestamp << "[INFO] Bid is successfully made. Product : " << product << "  Price : " << price << "  Amount : " << amount <<"\n";
    }
    else{
        Match << timestamp << "[INFO] Ask is successfully made. Product : " << product << "  Price : " << price << "  Amount : " << amount << "\n";
    }
    Match.close();
}

void Logger::ErrorLog(std::string message, std::string timestamp)
{
    std::fstream Error;
    
    Error.open("Error.txt", std::fstream::app | std::fstream::out);
    Error << timestamp << "[Error] " << message << "\n" ;
    Error.close();
}
