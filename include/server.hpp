#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>
#include <future>

//global
extern std::mutex cv_mutex;
extern std::condition_variable cv;
extern std::atomic<int> daily_visitor_count; // Counter variable
extern int visitor_count; 
extern std::unordered_map<std::string, std::time_t> ip_last_visit;

//struct
struct Veri {
    std::string site;
    std::string server;
    std::string satis;
    std::string alis;
};



// Reading CSV
std::vector<Veri> read_csv(const std::string& csv_path);

//Start web server with data
void start_web_server(std::vector<Veri>& knight_veriler, std::vector<Veri>& rise_veriler,std::vector<Veri>& nowa_veriler);

void update_data(std::vector<Veri>& veri_listesi, const std::string& csv_path);

#endif
