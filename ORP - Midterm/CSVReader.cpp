#include "CSVReader.h"
#include <cstdio>
#include <iostream>
#include <fstream>


CSVReader::CSVReader()
{
}

std::map<std::string, std::vector<OrderBookEntry>> CSVReader::readCSV(std::string csvFileName)
{
    std::map<std::string, std::vector<OrderBookEntry>> entries;
    std::pair<std::map<std::string,std::vector<OrderBookEntry>>::iterator,bool> ret;
    std::ifstream csvFile{csvFileName};
    std::string line;
    if (csvFile.is_open())
    {
        while(std::getline(csvFile, line))
        {
            try {
                std::vector<std::string> tokenized = tokenise(line, ',');
                OrderBookEntry obe = stringsToOBE(tokenized);
                std::vector<OrderBookEntry> obe_vector;
                obe_vector.push_back(obe);
                ret = entries.emplace(tokenized[0], obe_vector);
                if(ret.second==false)
                {
                    entries[tokenized[0]].push_back(obe);
                }
            }catch(const std::exception& e)
            {
                std::cout << "CSVReader::readCSV bad data"  << std::endl;
            }
            
        }// end of while
    }
    return entries; 
}

std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator)
{
   std::vector<std::string> tokens;
   signed int start, end;
   std::string token;
    start = csvLine.find_first_not_of(separator, 0);
    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break;
        if (end >= 0) token = csvLine.substr(start, end - start);
        else token = csvLine.substr(start, csvLine.length() - start);
        tokens.push_back(token);
    start = end + 1;
    }while(end > 0);

   return tokens; 
}

OrderBookEntry CSVReader::stringsToOBE(std::vector<std::string> tokens)
{
    double price, amount;

    if (tokens.size() != 5) // bad
    {
        std::cout << "Bad line " << std::endl;
        throw std::exception{};
    }
    // we have 5 tokens
    try {
         price = std::stod(tokens[3]);
         amount = std::stod(tokens[4]);
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[3]<< std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[4]<< std::endl; 
        throw;        
    }

    OrderBookEntry obe{price, 
                        amount, 
                        tokens[0],
                        tokens[1], 
                        OrderBookEntry::stringToOrderBookType(tokens[2])};

    return obe; 
}


OrderBookEntry CSVReader::stringsToOBE(std::string priceString, 
                                    std::string amountString, 
                                    std::string timestamp, 
                                    std::string product, 
                                    OrderBookType orderType)
{
    double price, amount;
    try {
         price = std::stod(priceString);
         amount = std::stod(amountString);
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << priceString<< std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << amountString<< std::endl; 
        throw;        
    }
    OrderBookEntry obe{price, 
                    amount, 
                    timestamp,
                    product, 
                    orderType};
                
    return obe;
}
     
