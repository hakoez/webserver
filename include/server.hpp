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
extern std::atomic<int> daily_visitor_count; // Sayaç değişkeni
extern int visitor_count; 
extern std::unordered_map<std::string, std::time_t> ip_last_visit;

//struct
struct Veri {
    std::string site;
    std::string server;
    std::string satis;
    std::string alis;
};

//class for thread
class ThreadPool {
    public:
        ThreadPool(size_t num_threads) {
            for (size_t i = 0; i < num_threads; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(queue_mutex);
                            condition.wait(lock, [this] { return !tasks.empty() || stop; });
    
                            if (stop && tasks.empty()) return;
    
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                });
            }
        }
    
        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true;
            }
            condition.notify_all();
            for (std::thread &worker : workers)
                worker.join();
        }
    
        void enqueue(std::function<void()> task) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.push(std::move(task));
            }
            condition.notify_one();
        }
    
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;
    };

// Reading CSV
std::vector<Veri> read_csv(const std::string& csv_path);

//Start web server with data
void start_web_server(std::vector<Veri>& knight_veriler, std::vector<Veri>& rise_veriler,std::vector<Veri>& nowa_veriler);

void update_data(std::vector<Veri>& veri_listesi, const std::string& csv_path);

#endif
