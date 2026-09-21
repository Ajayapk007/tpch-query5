#include "query5.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

// Function to parse command line arguments
bool parseArgs(int argc, char *argv[], std::string &r_name, std::string &start_date, std::string &end_date, int &num_threads, std::string &table_path, std::string &result_path)
{
    // TODO: Implement command line argument parsing
    // Example: --r_name ASIA --start_date 1994-01-01 --end_date 1995-01-01 --threads 4 --table_path /path/to/tables --result_path /path/to/results

    num_threads = 0;

    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 >= argc)
            return false;

        std::string flag = argv[i];
        std::string value = argv[i + 1];

        if (flag == "--r_name")
            r_name = value;
        else if (flag == "--start_date")
            start_date = value;
        else if (flag == "--end_date")
            end_date = value;
        else if (flag == "--threads")
            num_threads = std::stoi(value);
        else if (flag == "--table_path")
            table_path = value;
        else if (flag == "--result_path")
            result_path = value;
        else
            return false; // when wrong data
    }
    return !r_name.empty() && !start_date.empty() && !end_date.empty() &&
           !table_path.empty() && !result_path.empty() && num_threads >= 1;
}

bool loadTable(const std::string &table_path,
               const std::vector<std::string> &attributes,
               const std::vector<int> &indices,
               std::vector<std::map<std::string, std::string>> &currMap)
{
    std::ifstream file(table_path);
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << table_path << "\n";
        return false;
    }

    std::string data;
    while (std::getline(file, data))
    {
        if (data.empty())
            continue;

        int index = 0;
        int curr = 0;
        std::map<std::string, std::string> row;
        std::string currStr = "";

        for (int i = 0; i < data.size(); i++)
        {
            if (data[i] == '|')
            {
                if (index < indices.size() && curr == indices[index])
                {
                    row[attributes[index]] = currStr;
                    index++;
                    if (index == indices.size())
                    {
                        break;
                    }
                }
                currStr = "";
                curr++;
            }
            else
            {
                currStr += data[i];
            }
        }
        currMap.push_back(row);
    }
    // check
    //  for(const auto& item : currMap){
    //      for(const auto& p : item){
    //          std::cout << p.first << " -> " << p.second << "\n";
    //      }
    //  }
    return true;
}

// Function to read TPCH data from the specified paths
bool readTPCHData(const std::string &table_path,
                  std::vector<std::map<std::string, std::string>> &customer_data,
                  std::vector<std::map<std::string, std::string>> &orders_data,
                  std::vector<std::map<std::string, std::string>> &lineitem_data,
                  std::vector<std::map<std::string, std::string>> &supplier_data,
                  std::vector<std::map<std::string, std::string>> &nation_data,
                  std::vector<std::map<std::string, std::string>> &region_data)
{
    // TODO: Implement reading TPCH data from files
    // for nation kye => n + _ + key = n_Key
    return loadTable(table_path + "/region.tbl", {"r_key", "r_name"}, {0, 1}, region_data) &&
           loadTable(table_path + "/nation.tbl", {"n_key", "n_name", "n_r_key"}, {0, 1, 2}, nation_data) &&
           loadTable(table_path + "/customer.tbl", {"c_key", "c_n_key"}, {0, 3}, customer_data) &&
           loadTable(table_path + "/supplier.tbl", {"s_key", "s_n_key"}, {0, 3}, supplier_data) &&
           loadTable(table_path + "/orders.tbl", {"o_key", "o_c_key", "o_orderdate"}, {0, 1, 4}, orders_data) &&
           loadTable(table_path + "/lineitem.tbl", {"l_key", "l_s_key", "l_price", "l_discount"}, {0, 2, 5, 6}, lineitem_data);
}

// Function to execute TPCH Query 5 using multithreading
bool executeQuery5(const std::string &r_name,
                   const std::string &start_date,
                   const std::string &end_date,
                   int num_threads,
                   const std::vector<std::map<std::string, std::string>> &customer_data,
                   const std::vector<std::map<std::string, std::string>> &orders_data,
                   const std::vector<std::map<std::string, std::string>> &lineitem_data,
                   const std::vector<std::map<std::string, std::string>> &supplier_data,
                   const std::vector<std::map<std::string, std::string>> &nation_data,
                   const std::vector<std::map<std::string, std::string>> &region_data,
                   std::map<std::string, double> &results)
{
    // TODO: Implement TPCH Query 5 using multithreading

    // 1 work region table
    std::string query_r_key = ""; // vector<string> if need data from multiple region < our task is only Aisa>

    //| Index | `r_key` | `r_name` |
    //| **0** | 0       | AFRICA
    //| **1** | 1       | AMERICA
    //| **2** | 2       | ASIA
    //| **3** | 3       | EUROPEfinal
    //| **4** | 4       | MIDDLE EAST
    // o -> order
    // n -> nation
    // s-> supplier
    // r-> region
    // c-> customer
    // l->lineitem

    for (const auto &row : region_data)
    {
        auto it = row.find("r_name");
        if (it != row.end() && it->second == r_name)
        {
            query_r_key = row.at("r_key");
            break;
        }
    }

    // 2 nation table

    // Index,    "n_key""",  """n_name""",   """n_r_key"""
    // row [0],  """1""",    """ARGENTINA""",    """1"""
    // row [1],  """2""",    """BRAZIL""",       """1"""
    // row [2],  """3""",    """CANADA""",       """1"""
    // row [3],  """4""",    """EGYPT""",        """4"""

    std::unordered_map<std::string, std::string> n_key_to_name;
    for (const auto &row : nation_data)
    {
        auto n_r_key_it = row.find("n_r_key");
        auto n_key_it = row.find("n_key");
        auto name_it = row.find("n_name");

        if (n_r_key_it != row.end() && n_key_it != row.end() && name_it != row.end() && n_r_key_it->second == query_r_key)
        {
            n_key_to_name[n_key_it->second] = name_it->second;
        }
    }
    // std::cout << "how many nations : " << n_key_to_name.size() << "\n";

    //| n_key | name|
    //| `"1"` | `"China"` |
    //| `"2"` | `"India"` |
    //| `"3"` | `"Russia"` |

    // 3rd supplier

    // "s_key", "s_n_key"

    std::unordered_map<std::string, std::string> s_key_to_n;

    for (const auto &row : supplier_data)
    {
        auto s_n_key_it = row.find("s_n_key");
        auto s_key_it = row.find("s_key");

        if (s_n_key_it != row.end() && s_key_it != row.end())
        {
            auto n_it = n_key_to_name.find(s_n_key_it->second);

            if (n_it != n_key_to_name.end())
            {
                s_key_to_n[s_key_it->second] = s_n_key_it->second;
            }
        }
    } 

    // 4th supplier 
    std::unordered_map<std::string, std::string> c_key_to_n;

    for (const auto &row : customer_data)
    {
        auto c_n_key_it = row.find("c_n_key");
        auto c_key_it = row.find("c_key");

        if (c_n_key_it != row.end() && c_key_it != row.end())
        {
            auto n_it = n_key_to_name.find(c_n_key_it->second);

            if (n_it != n_key_to_name.end())
            {
                c_key_to_n[c_key_it->second] = c_n_key_it->second;
            }
        }
    }

    //  std::cout << "how many cust : " << c_key_to_n.size() << "\n";

    // 5
    std::unordered_map<std::string, std::string> o_key_to_c;
    for (const auto &row : orders_data)
    {
        auto date_it = row.find("o_orderdate");
        auto cust_it = row.find("o_c_key");
        auto order_it = row.find("o_key");

        if (date_it != row.end() && cust_it != row.end() && order_it != row.end())
        {
            const std::string &o_date = date_it->second;
            if (o_date >= start_date && o_date < end_date)
            {
                if (c_key_to_n.find(cust_it->second) != c_key_to_n.end())
                {
                    o_key_to_c[order_it->second] = cust_it->second;
                }
            }
        }
    }
    // std::cout << "how many nations : " << o_key_to_c.size() << "\n";

    // 6 "l_key", "l_s_key", "l_price", "l_discount"}
    std::vector<std::thread> threads;
    std::vector<std::map<std::string, double>> thread_results(num_threads);
    int total_lines = lineitem_data.size();
    int chunk_size = (total_lines + num_threads - 1) / num_threads;

    auto worker = [&](int thread_id, int start, int end)
    {
        for (int i = start; i < end; ++i)
        {
            const auto &row = lineitem_data[i];

            auto order_key_it = row.find("l_key");
            if (order_key_it == row.end())
                continue;

            auto it_order = o_key_to_c.find(order_key_it->second);
            if (it_order == o_key_to_c.end())
                continue;
            const std::string &c_custkey = it_order->second;

            // Supplier Lookup
            auto supp_key_it = row.find("l_s_key");
            if (supp_key_it == row.end())
                continue;

            auto it_supp = s_key_to_n.find(supp_key_it->second);
            if (it_supp == s_key_to_n.end())
                continue;
            const std::string &s_n_key = it_supp->second;

            auto c_n_it = c_key_to_n.find(c_custkey);
            if (c_n_it == c_key_to_n.end())
                continue;
            const std::string &c_n_key = c_n_it->second;

            if (c_n_key == s_n_key)
            {
                auto price_it = row.find("l_price");
                auto disc_it = row.find("l_discount");
                if (price_it == row.end() || disc_it == row.end())
                    continue;

                double revenue = std::stod(price_it->second) * (1.0 - std::stod(disc_it->second));

                auto name_it = n_key_to_name.find(c_n_key);
                if (name_it != n_key_to_name.end())
                {
                    thread_results[thread_id][name_it->second] += revenue;
                }
            }
        }
    };
    for (int i = 0; i < num_threads; ++i)
    {
        int start = i * chunk_size;
        int end = std::min(start + chunk_size, total_lines);
        threads.push_back(std::thread(worker, i, start, end));
    }

    for (int i = 0; i < threads.size(); ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }

    // sum every thread result
    for (const auto &result : thread_results)
    {
        for (const auto &p : result)
        {
            results[p.first] += p.second;
        }
    }

    return true;
}

// Function to output results to the specified path
bool outputResults(const std::string &result_path,
                   const std::map<std::string, double> &results)
{
    //  TODO: Implement outputting results to a file

    std::ofstream out(result_path);

    if (!out.is_open())
    {
        return false;
    }

    std::vector<std::pair<std::string, double>> sorted_results(
        results.begin(),
        results.end());

    std::sort(
        sorted_results.begin(),
        sorted_results.end(),
        [](const std::pair<std::string, double> &a,
           const std::pair<std::string, double> &b)
        {
            return a.second > b.second;
        });

    out << std::fixed << std::setprecision(2);
    std::cout << std::fixed << std::setprecision(2);

    for (const auto &p : sorted_results)
    {
        out << p.first << "|" << p.second << "\n";
        std::cout << p.first << "|" << p.second << "\n";
    }

    return true;
}
