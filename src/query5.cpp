#include "query5.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>

// Function to parse command line arguments
bool parseArgs(int argc, char* argv[], std::string& r_name, std::string& start_date, std::string& end_date, int& num_threads, std::string& table_path, std::string& result_path) {
    // TODO: Implement command line argument parsing
    // Example: --r_name ASIA --start_date 1994-01-01 --end_date 1995-01-01 --threads 4 --table_path /path/to/tables --result_path /path/to/results

    num_threads = 0;

    for(int i = 1; i < argc; i+= 2){
        if(i + 1 >= argc) return false;

        std::string flag = argv[i];
        std::string value = argv[i+1];

        if (flag == "--r_name")            r_name = value;
        else if (flag == "--start_date")   start_date = value;
        else if (flag == "--end_date")     end_date = value;
        else if (flag == "--threads")      num_threads = std::stoi(value);
        else if (flag == "--table_path")   table_path = value;
        else if (flag == "--result_path")  result_path = value;
        else return false;   // when wrong data 

    }
    return !r_name.empty() && !start_date.empty() && !end_date.empty() &&
           !table_path.empty() && !result_path.empty() && num_threads >= 1;
}

bool loadTable(const std::string& table_path,
      const std::vector<std::string>& attributes,
      const std::vector<int>& indices, 
      std::vector<std::map<std::string, std::string>>& currMap){
        std::ifstream file(table_path);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << table_path << "\n";
        return false;
    }

    std::string data; 
    while (std::getline(file, data)) {
        if (data.empty()) continue;

        int index = 0;
        int curr = 0; 
        std::map<std::string, std::string> row;
        std::string currStr = "";

        for (int i = 0; i < data.size(); i++) {
            if (data[i] == '|') { 
                if (index < indices.size() && curr == indices[index]) {
                    row[attributes[index]] = currStr;
                    index++;
                    if (index == indices.size()) {
                        break; 
                    }
                }
                currStr = ""; 
                curr++;        
            } else {
                currStr += data[i]; 
            }
        }
        currMap.push_back(row);
    }
        //check
        // for(const auto& item : currMap){
        //     for(const auto& p : item){
        //         std::cout << p.first << " -> " << p.second << "\n";
        //     }
        // }
        return true; 
}

// Function to read TPCH data from the specified paths
bool readTPCHData(const std::string& table_path, 
    std::vector<std::map<std::string, std::string>>& customer_data, 
    std::vector<std::map<std::string, std::string>>& orders_data, 
    std::vector<std::map<std::string, std::string>>& lineitem_data, 
    std::vector<std::map<std::string, std::string>>& supplier_data, 
    std::vector<std::map<std::string, std::string>>& nation_data, 
    std::vector<std::map<std::string, std::string>>& region_data) {
    // TODO: Implement reading TPCH data from files

    return loadTable(table_path +  "/region.tbl", {"r_regionkey", "r_name"}, {0, 1}, region_data) &&
    loadTable(table_path + "/nation.tbl", {"n_nationkey", "n_name", "n_regionkey"}, {0, 1, 2}, nation_data) &&
    loadTable(table_path + "/customer.tbl", {"c_custkey", "c_nationkey"}, {0, 3}, customer_data) &&
    loadTable(table_path + "/supplier.tbl", {"s_suppkey", "s_nationkey"}, {0, 3}, supplier_data) &&
    loadTable(table_path + "/orders.tbl", {"o_orderkey", "o_custkey", "o_orderdate"}, {0, 1, 4}, orders_data) &&
    loadTable(table_path + "/lineitem.tbl", {"l_orderkey", "l_suppkey", "l_extendedprice", "l_discount"}, {0, 2, 5, 6}, lineitem_data);
    
}

// Function to execute TPCH Query 5 using multithreading
bool executeQuery5(const std::string& r_name, const std::string& start_date, const std::string& end_date, int num_threads, const std::vector<std::map<std::string, std::string>>& customer_data, const std::vector<std::map<std::string, std::string>>& orders_data, const std::vector<std::map<std::string, std::string>>& lineitem_data, const std::vector<std::map<std::string, std::string>>& supplier_data, const std::vector<std::map<std::string, std::string>>& nation_data, const std::vector<std::map<std::string, std::string>>& region_data, std::map<std::string, double>& results) {
    // TODO: Implement TPCH Query 5 using multithreading
    return false;
}

// Function to output results to the specified path
bool outputResults(const std::string& result_path, const std::map<std::string, double>& results) {
    // TODO: Implement outputting results to a file
    return false;
} 