#define CPPHTTPLIB_OPENSSL_SUPPORT

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

using json = nlohmann::json;

const std::string CERT_FILE = "/etc/letsencrypt/live/delosgb.com/fullchain.pem";
const std::string KEY_FILE = "/etc/letsencrypt/live/delosgb.com/privkey.pem";
std::atomic<bool> updating = false;
std::mutex cache_mutex;
std::atomic<int> daily_visitor_count(0); 
std::unordered_map<std::string, std::time_t> ip_last_visit; 
std::mutex ip_mutex; 
int visitor_count=0;
//-----------------------------------------------------------------

//visitor fonc
void increment_visitor_count(const std::string& ip) {
    std::lock_guard<std::mutex> lock(ip_mutex);

    std::time_t now = std::time(nullptr); 
    std::time_t last_visit = ip_last_visit[ip]; 

   
    if (std::difftime(now, last_visit) >= 3600) { 
        daily_visitor_count++;
        ip_last_visit[ip] = now; 
    
    } else {
        std::cout << "Not counted" << ip << std::endl;
    }
}

// Read csv
std::vector<Veri> read_csv(const std::string& csv_path) {
    std::vector<Veri> veri_listesi;
    std::ifstream file(csv_path);

    if (!file.is_open()) {
        std::cerr << "❌couldnt open " << csv_path << std::endl;
        return veri_listesi;
    }

    std::string satir;
    bool first_line = true;

    while (std::getline(file, satir)) {
        if (first_line) {
            first_line = false;
            continue;
        }

        std::stringstream ss(satir);
        std::string site, server, satis, alis;

        std::getline(ss, site, ',');
        std::getline(ss, server, ',');
        std::getline(ss, satis, ',');
        std::getline(ss, alis, ',');

       
        if (alis == "--") {
            alis = "0.00";
        }

        veri_listesi.push_back({site, server, satis, alis});
    }

    file.close();
    return veri_listesi;
}
//Update data
void update_data(std::vector<Veri>& veri_listesi, const std::string& csv_path) {

    auto new_data = read_csv(csv_path);

    std::cout << "🔍 " << csv_path << " Data inside: " << new_data.size() << std::endl;

 
    {
        std::lock_guard<std::mutex> lock(cache_mutex);  
        veri_listesi = std::move(new_data);  
    }

    std::cout << "Data update completed " << csv_path << std::endl;
}

//Start web server
void start_web_server(std::vector<Veri>& knight_veriler, std::vector<Veri>& rise_veriler, std::vector<Veri>& nowa_veriler) {
    httplib::SSLServer svr(CERT_FILE.c_str(), KEY_FILE.c_str());
    httplib::Server http_svr; // HTTP için (80 portu)
    if (!svr.is_valid()) {
        std::cerr << "Couldnt run server \n";
        return;
    }

    // **Index.html*
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("/home/webserver/public/index.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found</h1>", "text/html");
        }
    });

    // **Knight**
    svr.Get("/knight", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("/home/webserver/public/knight.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found</h1>", "text/html");
        }
    });

    // **Rise Online**
    svr.Get("/rise", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("/home/webserver/public/rise.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found</h1>", "text/html");
        }
    });

    // **Nowa Online**
    svr.Get("/nowa", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("/home/webserver/public/nowa.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found</h1>", "text/html");
        }
    });
    
    //Visitor count
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
    //Css and Js data
    svr.Get(R"(/(css|js)/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/" + req.path;
        std::ifstream file(file_path, std::ios::binary);
    
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();
    
          
            res.set_header("Cache-Control", "public, max-age=2592000"); // 30 gün (1 ay)
            res.set_header("Expires", "Wed, 21 Oct 2025 07:28:00 GMT"); // Sabit tarih
    
           
            std::string mime_type = "text/plain";
            if (file_path.find(".css") != std::string::npos)
                mime_type = "text/css";
            else if (file_path.find(".js") != std::string::npos)
                mime_type = "application/javascript";
    
            res.set_content(content, mime_type);
        } else {
            res.status = 404;
            res.set_content("404 Not Found", "text/plain");
        }
    });
    // **Knight Online Data**
    svr.Get("/knight-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(cache_mutex);  // Verilere erişimi kilitle
        json json_data = json::array();
        for (const auto& veri : knight_veriler) {
            json_data.push_back({
                {"site", veri.site},
                {"server", veri.server},
                {"satis", veri.satis},
                {"alis", veri.alis}
            });
        }
        res.set_content(json_data.dump(), "application/json");
    });

    // **Rise Online Data**
    svr.Get("/rise-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(cache_mutex);  // Verilere erişimi kilitle
        json json_data = json::array();
        for (const auto& veri : rise_veriler) {
            json_data.push_back({
                {"site", veri.site},
                {"server", veri.server},
                {"satis", veri.satis},
                {"alis", veri.alis}
            });
        }
        res.set_content(json_data.dump(), "application/json");
    });

    // **Nowa Online Data**
    svr.Get("/nowa-data", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(cache_mutex);  // Verilere erişimi kilitle
        json json_data = json::array();
        for (const auto& veri : nowa_veriler) {
            json_data.push_back({
                {"site", veri.site},
                {"server", veri.server},
                {"satis", veri.satis},
                {"alis", veri.alis}
            });
        }
        res.set_content(json_data.dump(), "application/json");
    });


    // **Static logos**
    svr.Get("/logos/(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::string file_path = "/home/webserver/public/logos/" + req.matches[1].str();
        std::ifstream file(file_path, std::ios::binary);

        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();
        // 🔥 Önbellekleme Başlıkları
        res.set_header("Cache-Control", "public, max-age=31536000"); // 1 yıl önbellek
        res.set_header("Expires", "Wed, 21 Oct 2025 07:28:00 GMT"); // Sabit tarih, güncellenebilir

        // **MIME Türünü Ayarla**
        std::string mime_type = "image/png"; // Varsayılan PNG
        if (file_path.find(".jpg") != std::string::npos || file_path.find(".jpeg") != std::string::npos)
            mime_type = "image/jpeg";
        else if (file_path.find(".gif") != std::string::npos)
            mime_type = "image/gif";
        else if (file_path.find(".webp") != std::string::npos)
            mime_type = "image/webp";

        res.set_content(content, mime_type);
    } else {
        res.status = 404;
        res.set_content("<h1>404 Not Found</h1>", "text/html");
    }
});
    //Server confg
    svr.set_keep_alive_max_count(500); // Aynı bağlantıda 500 istek işlenebilir
    svr.set_keep_alive_timeout(30);    // 30 saniye boyunca bağlantıyı açık tut
    svr.set_base_dir("/home/webserver/public");

    std::cout << "HTTPS on 433\n";

    // **HTTP to HTTPS
    http_svr.Get(".*", [](const httplib::Request& req, httplib::Response& res) {
        std::string new_url = "https://" + req.get_header_value("Host") + req.path;
        res.set_redirect(new_url);
    });



    // start server
    if (!svr.listen("0.0.0.0", 443)) {
        std::cerr << "❌ Error server\n";
    }
}