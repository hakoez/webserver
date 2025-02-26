#define CPPHTTPLIB_OPENSSL_SUPPORT
//// delosgb.com /////////
#include "server.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <asio/ssl.hpp>
#include "httplib.h"
#include <atomic>
#include <unordered_map>
#include <ctime>
#include <shared_mutex>

using json = nlohmann::json;

const std::string CERT_FILE = "/etc/letsencrypt/live/delosgb.com/fullchain.pem";
const std::string KEY_FILE = "/etc/letsencrypt/live/delosgb.com/privkey.pem";
std::atomic<bool> updating = false;
std::shared_mutex cache_mutex; // Thread safety is enhanced using std::shared_mutex
std::atomic<int> daily_visitor_count(0); // Daily visitor counter
std::unordered_map<std::string, std::time_t> ip_last_visit; // Stores IP and last visit time
std::mutex ip_mutex;  // Mutex for IP access
int visitor_count = 0;

// Cached file contents
std::unordered_map<std::string, std::string> file_cache;

// Cached data (for JSON APIs)
std::unordered_map<std::string, std::string> data_cache;

// Data structures for IP request counting and blocking mechanism
std::unordered_map<std::string, int> ip_request_count; // IP request counter
std::unordered_map<std::string, std::time_t> ip_last_request_time; // IP last request time
std::unordered_map<std::string, std::time_t> blocked_ips; // Blocked IPs
std::mutex ip_block_mutex; // Mutex for accessing blocked IP data

// Function to check and unblock IPs
void check_and_unblock_ips() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Check every 10 seconds

        std::lock_guard<std::mutex> lock(ip_block_mutex);
        std::time_t now = std::time(nullptr);

        // Remove IPs whose blocking duration has expired
        for (auto it = blocked_ips.begin(); it != blocked_ips.end();) {
            if (std::difftime(now, it->second) >= 60) { // 60 seconds = 1 minute
                it = blocked_ips.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// Function to check request count and block IPs if necessary
bool should_block_ip(const std::string& ip) {
    std::lock_guard<std::mutex> lock(ip_block_mutex);

    std::time_t now = std::time(nullptr);

    // If the IP is blocked, return true
    if (blocked_ips.find(ip) != blocked_ips.end()) {
        return true;
    }

    // Reset request counter if the last request was more than 60 seconds ago
    if (std::difftime(now, ip_last_request_time[ip]) >= 60) {
        ip_request_count[ip] = 0;
        ip_last_request_time[ip] = now;
    }

    // Increase request count
    ip_request_count[ip]++;
    ip_last_request_time[ip] = now;

    // If more than 100 requests are made within 60 seconds, block the IP
    if (ip_request_count[ip] > 100) {
        blocked_ips[ip] = now;
        ip_request_count[ip] = 0; // Reset the counter
        return true;
    }

    return false;
}

//-----------------------------------------------------------------

// Visitor function
void increment_visitor_count(const std::string& ip) {
    std::lock_guard<std::mutex> lock(ip_mutex); // Lock IP access

    std::time_t now = std::time(nullptr); // Current time
    std::time_t last_visit = ip_last_visit[ip]; // Last visit time for this IP

    // 🔥 Add a 5-minute cooldown (300 seconds)
    if (std::difftime(now, last_visit) >= 3600) { // 300 seconds = 5 minutes
        daily_visitor_count++;
        ip_last_visit[ip] = now; // Update last visit time
        std::cout << "✅ New visitor counted: " << ip << std::endl;
    } else {
        std::cout << "⏳ Cooldown active, visitor not counted: " << ip << std::endl;
    }
}
// **Read CSV File** (Cached Version)
std::vector<Veri> read_csv(const std::string& csv_path) {
    std::vector<Veri> veri_listesi;
    std::ifstream file(csv_path);

    if (!file.is_open()) {
        std::cerr << "❌ Failed to open CSV file: " << csv_path << std::endl;
        return veri_listesi;
    }

    std::string line;
    bool first_line = true;

    while (std::getline(file, line)) {
        if (first_line) {
            first_line = false;
            continue;
        }

        std::stringstream ss(line);
        std::string site, server, satis, alis;

        std::getline(ss, site, ',');
        std::getline(ss, server, ',');
        std::getline(ss, satis, ',');
        std::getline(ss, alis, ',');

        // 📌 **If `alis` value is "--", replace it with "0.00"**
        if (alis == "--") {
            alis = "0.00";
        }

        veri_listesi.push_back({site, server, satis, alis});
    }

    file.close();
    return veri_listesi;
}
// 🔹 CSV Data Update Function (Optimized Version)
void update_data(std::vector<Veri>& veri_listesi, const std::string& csv_path) {
    // Read new data into a temporary vector
    auto new_data = read_csv(csv_path);

    std::cout << "🔍 Number of data rows in " << csv_path << ": " << new_data.size() << std::endl;

    // Minimize lock duration while updating data
    {
        std::unique_lock<std::shared_mutex> lock(cache_mutex);  // Write lock
        veri_listesi = std::move(new_data);  // Quickly assign new data
    }

    std::cout << "✅ Data update completed: " << csv_path << std::endl;
}

void check_and_update_data(std::vector<Veri>& veri_listesi, const std::string& csv_path, const std::string& cache_key) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(2)); // ✅ Check every 2 minutes

        // 🔹 Read new CSV data
        auto new_data = read_csv(csv_path);

        // 🔹 Read previous JSON cache
        std::shared_lock<std::shared_mutex> lock(cache_mutex);
        std::string current_cache = data_cache[cache_key];
        lock.unlock();

        // 🔹 Prepare new JSON data
        json new_json_data = json::array();
        for (const auto& veri : new_data) {
            new_json_data.push_back({
                {"site", veri.site},
                {"server", veri.server},
                {"satis", veri.satis},
                {"alis", veri.alis}
            });
        }
        std::string new_cache = new_json_data.dump();

        // 🔥 If new data is the same as the previous one, do not update
        if (new_cache == current_cache) {
            std::cout << "⏳ No changes: " << cache_key << std::endl;
            continue;
        }

        // 🔄 If data has changed, update
        std::unique_lock<std::shared_mutex> write_lock(cache_mutex);
        veri_listesi = std::move(new_data);
        data_cache[cache_key] = new_cache; // ✅ Store new data in cache
        std::cout << "🔄 Updated: " << cache_key << std::endl;
    }
}

// **Start Web Server**
void start_web_server(std::vector<Veri>& knight_veriler, std::vector<Veri>& rise_veriler, std::vector<Veri>& nowa_veriler) {
    httplib::SSLServer svr(CERT_FILE.c_str(), KEY_FILE.c_str());
    httplib::Server http_svr; // HTTP server (port 80)
    if (!svr.is_valid()) {
        std::cerr << "❌ HTTPS Server could not start! Check the certificates.\n";
        return;
    }

    // **Serve Index.html** (Cached version)
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/index.html";
        if (file_cache.find(file_path) == file_cache.end()) {
            std::ifstream file(file_path);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file_cache[file_path] = buffer.str();
            } else {
                res.status = 404;
                res.set_content("<h1>404 Not Found</h1>", "text/html");
                return;
            }
        }
        res.set_content(file_cache[file_path], "text/html");
    });

    // **Serve Knight Online Page** (Cached version)
    svr.Get("/knight", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/knight.html";
        if (file_cache.find(file_path) == file_cache.end()) {
            std::ifstream file(file_path);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file_cache[file_path] = buffer.str();
            } else {
                res.status = 404;
                res.set_content("<h1>404 Not Found</h1>", "text/html");
                return;
            }
        }
        res.set_content(file_cache[file_path], "text/html");
    });

    // **Serve Rise Online Page** (Cached version)
    svr.Get("/rise", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/rise.html";
        if (file_cache.find(file_path) == file_cache.end()) {
            std::ifstream file(file_path);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file_cache[file_path] = buffer.str();
            } else {
                res.status = 404;
                res.set_content("<h1>404 Not Found</h1>", "text/html");
                return;
            }
        }
        res.set_content(file_cache[file_path], "text/html");
    });

    // **Serve Nowa Online Page** (Cached version)
    svr.Get("/nowa", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/nowa.html";
        if (file_cache.find(file_path) == file_cache.end()) {
            std::ifstream file(file_path);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file_cache[file_path] = buffer.str();
            } else {
                res.status = 404;
                res.set_content("<h1>404 Not Found</h1>", "text/html");
                return;
            }
        }
        res.set_content(file_cache[file_path], "text/html");
    });

    // 🔹 API requests retrieve data from cache
    svr.Get("/knight-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::string client_ip = req.get_header_value("X-Forwarded-For"); // Get client IP

        // If IP is blocked, return 429 Too Many Requests
        if (should_block_ip(client_ip)) {
            res.status = 429;
            res.set_content("Too Many Requests", "text/plain");
            return;
        }

        std::shared_lock<std::shared_mutex> lock(cache_mutex);
        res.set_content(data_cache["/knight-data"], "application/json");
    });

    svr.Get("/rise-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::string client_ip = req.get_header_value("X-Forwarded-For"); // Get client IP

        // If IP is blocked, return 429 Too Many Requests
        if (should_block_ip(client_ip)) {
            res.status = 429;
            res.set_content("Too Many Requests", "text/plain");
            return;
        }

        std::shared_lock<std::shared_mutex> lock(cache_mutex);
        res.set_content(data_cache["/rise-data"], "application/json");
    });

    svr.Get("/nowa-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::string client_ip = req.get_header_value("X-Forwarded-For"); // Get client IP

        // If IP is blocked, return 429 Too Many Requests
        if (should_block_ip(client_ip)) {
            res.status = 429;
            res.set_content("Too Many Requests", "text/plain");
            return;
        }

        std::shared_lock<std::shared_mutex> lock(cache_mutex);
        res.set_content(data_cache["/nowa-data"], "application/json");
    });

    // Visitor count
    svr.Get("/visitor-count", [](const httplib::Request& req, httplib::Response& res) {
        std::string ip = req.get_header_value("X-Forwarded-For"); 
        if (ip.empty()) {
            ip = req.remote_addr;
        }

        increment_visitor_count(ip);

        int visitor_count = daily_visitor_count.load();

        json response = {
            {"visitorCount", visitor_count}
        };

        res.set_content(response.dump(), "application/json");
    });

    // **Serve Logos** (Cached version)
    svr.Get("/logos/(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/logos/" + req.matches[1].str();
        if (file_cache.find(file_path) == file_cache.end()) {
            std::ifstream file(file_path, std::ios::binary);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                file_cache[file_path] = buffer.str();
            } else {
                res.status = 404;
                res.set_content("<h1>404 Not Found</h1>", "text/html");
                return;
            }
        }

        // 🔥 Cache Headers
        res.set_header("Cache-Control", "public, max-age=31536000"); // 1-year cache
        res.set_header("Expires", "Wed, 21 Oct 2025 07:28:00 GMT"); // Fixed date, can be updated

        // **Set MIME Type**
        std::string mime_type = "image/png"; // Default PNG
        if (file_path.find(".jpg") != std::string::npos || file_path.find(".jpeg") != std::string::npos)
            mime_type = "image/jpeg";
        else if (file_path.find(".gif") != std::string::npos)
            mime_type = "image/gif";
        else if (file_path.find(".webp") != std::string::npos)
            mime_type = "image/webp";

        res.set_content(file_cache[file_path], mime_type);
    });
    // Server settings
svr.set_keep_alive_max_count(500); // Allow up to 500 requests per connection
svr.set_keep_alive_timeout(10);    // Keep the connection open for 10 seconds (optimized)
svr.set_base_dir("/home/webserver/public");

std::cout << "🚀 HTTPS server is running on port 443...\n";

// **Start HTTP Server and Redirect to HTTPS**
http_svr.Get(".*", [](const httplib::Request& req, httplib::Response& res) {
    std::string new_url = "https://" + req.get_header_value("Host") + req.path;
    res.set_redirect(new_url);
});

// **Run HTTP Server on Port 80 (For Redirection)**
std::thread http_thread([&]() {
    std::cout << "🌍 HTTP server is running on port 80... (All requests are redirected to HTTPS)\n";
    http_svr.listen("0.0.0.0", 80);
});

// Check and update data every 2 minutes
std::thread knight_updater(check_and_update_data, std::ref(knight_veriler), "knight_veri.csv", "/knight-data");
std::thread rise_updater(check_and_update_data, std::ref(rise_veriler), "rise_veri.csv", "/rise-data");
std::thread nowa_updater(check_and_update_data, std::ref(nowa_veriler), "nowa_veri_raw22.csv", "/nowa-data");

knight_updater.detach();
rise_updater.detach();
nowa_updater.detach();

// Start the thread that checks and unblocks blocked IPs
std::thread unblock_thread(check_and_unblock_ips);
unblock_thread.detach();

// Start the server
if (!svr.listen("0.0.0.0", 443)) {
    std::cerr << "❌ Server could not start! Port 443 may be in use or certificates might be incorrect.\n";
}
