#include "query5.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

// TODO: Include additional headers as needed

int main(int argc, char* argv[]) {
    std::string r_name, start_date, end_date, table_path, result_path;
    int num_threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    if (!parseArgs(argc, argv, r_name, start_date, end_date, num_threads, table_path, result_path)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }
    // std::cout << "region: " << r_name << "\n" << "Data Start: " << start_date << "\n" << "end data : " << end_date << "\n" << "total Threads: " << num_threads << "\n";


    std::vector<std::map<std::string, std::string>> customer_data, orders_data, lineitem_data, supplier_data, nation_data, region_data;

    if (!readTPCHData(table_path, customer_data, orders_data, lineitem_data, supplier_data, nation_data, region_data)) {
        std::cerr << "Failed to read TPCH data." << std::endl;
        return 1;
    }
    // std::cout << "customer " << customer_data.size() << std::endl;
    // std::cout << "customer " << supplier_data.size() << std::endl;
    // std::cout << "customer " << lineitem_data.size() << std::endl;
    // std::cout << "customer " << nation_data.size() << std::endl;
    // std::cout << "customer " << region_data.size() << std::endl;
    // std::cout << "customer " << orders_data.size() << std::endl;

    
    std::map<std::string, double> results;

    if (!executeQuery5(r_name, start_date, end_date, num_threads, customer_data, orders_data, lineitem_data, supplier_data, nation_data, region_data, results)) {
        std::cerr << "Failed to execute TPCH Query 5." << std::endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end_time - start_time;

    if (!outputResults(result_path, results)) {
        std::cerr << "Failed to output results." << std::endl;
        return 1;
    }

    std::cout << "Total Time taken with " << num_threads << " threads is : " << duration.count() << "ms \n";

    std::cout << "TPCH Query 5 implementation completed." << std::endl;
    return 0;
} 
