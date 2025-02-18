#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>
#include "server.hpp"
#include <thread>
#include <chrono>


std::vector<Veri> knight_veriler;
std::vector<Veri> rise_veriler;
std::vector<Veri> nowa_veriler;
extern std::atomic<int> daily_visitor_count;

//Running scrapers
void run_scraper(const std::string& script) {
    std::filesystem::current_path("/home/webserver");

    int result;

#ifdef _WIN32
    result = system(("python " + script).c_str());
#elif __linux__
    std::string command = "/home/webserver/scraper_env/bin/python /home/webserver/" + script;
    result = system(command.c_str());
#else
    throw std::runtime_error("Desteklenmeyen işletim sistemi!");
#endif

    if (result != 0) {
        throw std::runtime_error(script + " error");
    }
}

//Rest daily visitors
void reset_daily_counter() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(24)); // 24 saat bekleyin
        visitor_count = 0; // Sayaç sıfırlama
        ip_last_visit.clear(); // IP listesini temizle
        std::cout << "Reseted daily visitor" << std::endl;
    }
}

int main() {
    int scraper_counter = 0;

    // first updating data in main thread
    update_data(knight_veriler, "knight_veri.csv");
    update_data(rise_veriler, "rise_veri.csv");
    update_data(nowa_veriler, "nowa_veri_raw22.csv");

    std::cout << "✅Data (Knight: "
              << knight_veriler.size() << ", Rise: "
              << rise_veriler.size() << ", Nowa: "
              << nowa_veriler.size() << ")" << std::endl;

    std::cout << "nowa data: " << nowa_veriler.size() << std::endl;
    for (const auto& veri : nowa_veriler) {
        std::cout << veri.site << " - " << veri.server << " - "
                  << veri.satis << " - " << veri.alis << std::endl;
    }

    // ✅ start web server
    std::thread server_thread([]() {
        start_web_server(knight_veriler, rise_veriler, nowa_veriler);
    });
    server_thread.detach();  // Sunucuyu arka planda çalıştır

    while (true) {
        try {
            std::cout << "🔄 starting scraper";
            run_scraper("scraper.py");

            std::cout << "📌 Csv updating\n";

            // update data in this thread
            std::thread update_thread([]() {
                update_data(knight_veriler, "knight_veri.csv");
                update_data(rise_veriler, "rise_veri.csv");
                update_data(nowa_veriler, "nowa_veri_raw22.csv");
            });

            update_thread.join();  

        } catch (const std::exception& e) {
            std::cerr << "❌ Error " << e.what() << std::endl;
        }

        std::cout << "⏳ wait 2 mins...\n";
        std::this_thread::sleep_for(std::chrono::minutes(2));

        scraper_counter++;
        if (scraper_counter == 8) {
            std::cout << "🚀 scraper4.py starting " << std::endl;
            run_scraper("scraper4.py");

            // after scraper4 update data
            std::thread scraper_update_thread([]() {
                update_data(knight_veriler, "knight_veri.csv");
                update_data(rise_veriler, "rise_veri.csv");
                update_data(nowa_veriler, "nowa_veri_raw22.csv");
            });

            scraper_update_thread.join(); 

            std::cout << "🔄 updated after scraper4 " << std::endl;

            scraper_counter = 0;
        }
    }

    return 0;
}