#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>
#include "server.hpp"
#include <atomic>
#include <mutex>
#include <condition_variable>

std::vector<Veri> knight_veriler;
std::vector<Veri> rise_veriler;
std::vector<Veri> nowa_veriler;
std::mutex data_mutex;
std::condition_variable data_cv; // Condition variable for data update
std::atomic<bool> scraper_running(false); // To check if the scraper is running

// Scraper Runner
void run_scraper(const std::string& script) {
    std::filesystem::current_path("/home/webserver");
    std::string command = "/home/webserver/scraper_env/bin/python /home/webserver/" + script;
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "❌ Scraper error: " << script << std::endl;
    }
}

// Load updated CSV data
void update_csv_data() {
    while (true) {
        std::unique_lock<std::mutex> lock(data_mutex);
        data_cv.wait(lock, [] { return !scraper_running.load(); }); // Proceed if scraper is not running

        update_data(knight_veriler, "knight_veri.csv");
        update_data(rise_veriler, "rise_veri.csv");
        update_data(nowa_veriler, "nowa_veri_raw22.csv");
        std::cout << "📌 CSV data updated." << std::endl;

        lock.unlock();
        std::this_thread::sleep_for(std::chrono::minutes(2)); // Wait for 2 minutes
    }
}

// Scraper Execution Loop
void scraper_loop() {
    int scraper_counter = 0;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(data_mutex);
            scraper_running.store(true); // Scraper is running
        }

        std::cout << "🔄 Starting scraper" << std::endl;
        run_scraper("scraper.py");
        scraper_counter++;

        if (scraper_counter == 8) {
            std::cout << "🚀 scraper4.py starting" << std::endl;
            run_scraper("scraper4.py");
            scraper_counter = 0;
        }

        {
            std::lock_guard<std::mutex> lock(data_mutex);
            scraper_running.store(false); // Scraper completed
            data_cv.notify_all(); // Wake up CSV update thread
        }

        std::this_thread::sleep_for(std::chrono::minutes(2)); // Wait for 2 minutes
    }
}

// Daily visitor counter reset
void reset_daily_counter() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(24));
        daily_visitor_count.store(0);
        ip_last_visit.clear();
        std::cout << "🔄 Daily visitor count reset." << std::endl;
    }
}

int main() {
    
    // Initial data update
    update_data(knight_veriler, "knight_veri.csv");
    update_data(rise_veriler, "rise_veri.csv");
    update_data(nowa_veriler, "nowa_veri_raw22.csv");
    std::cout << "✅ Initial data loaded." << std::endl;

    // Start the web server
    std::thread server_thread([]() {
        start_web_server(knight_veriler, rise_veriler, nowa_veriler);
    });

    // Scraper Execution Thread
    std::thread scraper_thread(scraper_loop);

    // Daily Visitor Reset Thread
    std::thread reset_thread(reset_daily_counter);

    // Wait for threads (without join, main thread remains in infinite wait)
    server_thread.join();
    scraper_thread.join();
    reset_thread.join();

    return 0;
}
