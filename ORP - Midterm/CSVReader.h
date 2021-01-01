#pragma once

#include "OrderBookEntry.h"
#include <vector>
#include <string>
#include <map>


class CSVReader
{
    public:
    // CSVReader constructor. It neither receives any input nor returns any output.
     CSVReader();
    
    //readCSV function reads a CSV file and creates a map which links between a timestamp and vector which contains all the orders within the timestamp. It takes the name of the CSV file as an input, returning a map.
    static std::map<std::string, std::vector<OrderBookEntry>> readCSV(std::string csvFileName);
    
    //tokenise function separates each line in the CSV file by a sent separator, creating a vector of strings. It takes a line from the read CSV file and separator as a input value, returning a vector of strings.
    static std::vector<std::string> tokenise(std::string csvLine, char separator);
    
    //stringsToOBE function creates an OrderBookEntry object with sent parameters. It takes price, amount, timestamp, product and OrderBookType as an input parameter, returning an OrderBookEntry objects.
    static OrderBookEntry stringsToOBE(std::string price,
                                        std::string amount, 
                                        std::string timestamp, 
                                        std::string product, 
                                        OrderBookType OrderBookType);

    private:
     static OrderBookEntry stringsToOBE(std::vector<std::string> strings);
     
};
