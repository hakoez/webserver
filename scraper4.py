from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium_stealth import stealth
import csv
import time
from datetime import datetime

# **Chrome için Selenium Ayarları**
chrome_options = Options()
chrome_options.add_argument("--headless")
chrome_options.add_argument("--no-sandbox")
chrome_options.add_argument("--disable-dev-shm-usage")
chrome_options.add_argument("--user-data-dir=/tmp/chrome-user-data")
chrome_options.add_argument("start-maximized")
chrome_options.add_argument("disable-infobars")
chrome_options.add_argument("--disable-blink-features=AutomationControlled")

# **ChromeDriver Yolu**
chromedriver_path = "/usr/local/bin/chromedriver"
service = Service(chromedriver_path)
driver = webdriver.Chrome(service=service, options=chrome_options)

# **Stealth Mode (Bot Algılamayı Engelle)**
stealth(driver,
        languages=["en-US", "en"],
        vendor="Google Inc.",
        platform="Win64",
        webgl_vendor="Intel Inc.",
        renderer="Intel Iris OpenGL Engine",
        fix_hairline=True)

# **CSV dosya adı**
csv_filename = "nowa_veri_raw22.csv"

# **Vatangame & Klasgame Siteleri**
sites = [
    {"name": "vatangame.com (Ares - Alış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[1]/div/div/div[3]/span/b"},
    {"name": "vatangame.com (Ares - Satış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[1]/div/div/div[4]/div/span"},
    {"name": "vatangame.com (Tera - Alış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[2]/div/div/div[3]/span/b"},
    {"name": "vatangame.com (Tera - Satış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[2]/div/div/div[4]/div/span"},
    {"name": "vatangame.com (Fenix - Alış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[3]/div/div/div[3]/span/b"},
    {"name": "vatangame.com (Fenix - Satış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[3]/div/div/div[4]/div/span"},
    {"name": "vatangame.com (Aura - Alış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[4]/div/div/div[3]/span/b"},
    {"name": "vatangame.com (Aura - Satış)", "url": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar", "method": "selenium", "xpath": "/html/body/div[4]/div/div[2]/div[2]/div/div[4]/div/div/div[4]/div/span"},

    # **Klasgame Satış & Alış**
    {"name": "klasgame.com (Aura - Satış)", "url": "https://www.klasgame.com/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold", "method": "selenium", "xpath": "//*[@id='urunler']/div[2]/div[1]/div[3]/div[2]/div[2]/div/div[1]/div[1]/span[1]"},
    {"name": "klasgame.com (Fenix - Satış)", "url": "https://www.klasgame.com/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold", "method": "selenium", "xpath": "//*[@id='urunler']/div[3]/div[1]/div[3]/div[2]/div[2]/div/div[1]/div[1]/span[1]"},
    {"name": "klasgame.com (Tera - Satış)", "url": "https://www.klasgame.com/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold", "method": "selenium", "xpath": "//*[@id='urunler']/div[4]/div[1]/div[3]/div[2]/div[2]/div/div[1]/div[1]/span[1]"},
    {"name": "klasgame.com (Ares - Satış)", "url": "https://www.klasgame.com/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold", "method": "selenium", "xpath": "//*[@id='urunler']/div[5]/div[1]/div[3]/div[2]/div[2]/div/div[1]/div[1]/span[1]"},
    {"name": "klasgame.com (Aura - Alış)", "url": "https://www.klasgame.com/satis-yap/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold/nowa-online-aura-1m", "method": "selenium", "xpath": "/html/body/div[13]/div/div/div/div/div[2]/div[2]/div[1]/form/div[6]/div[2]/div[2]/span/span[1]"},
    {"name": "klasgame.com (Fenix - Alış)", "url": "https://www.klasgame.com/satis-yap/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold/nowa-online-fenix-1m", "method": "selenium", "xpath": "/html/body/div[13]/div/div/div/div/div[2]/div[2]/div[1]/form/div[6]/div[2]/div[2]/span/span[1]"},
    {"name": "klasgame.com (Tera - Alış)", "url": "https://www.klasgame.com/satis-yap/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold/nowa-online-tera-1m", "method": "selenium", "xpath": "/html/body/div[13]/div/div/div/div/div[2]/div[2]/div[1]/form/div[6]/div[2]/div[2]/span/span[1]"},
    {"name": "klasgame.com (Ares - Alış)", "url": "https://www.klasgame.com/satis-yap/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold/nowa-online-world-10-m", "method": "selenium", "xpath": "/html/body/div[13]/div/div/div/div/div[2]/div[2]/div[1]/form/div[6]/div[2]/div[2]/span/span[1]"},
  
]

# **Oyuneks Siteleri (JavaScript ile XPath çekiyor)**
oyuneks_url = "https://oyuneks.com/nowa-online-world/nowa-online-world-gold"
oyuneks_xpaths = {
    "Oyuneks (Ares - Alış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[1]/div/div[2]/div[2]/div/div/div[1]/div[2]/span",
    "Oyuneks (Ares - Satış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[1]/div/div[2]/div[2]/div/div/div[2]/div[2]/span",
    "Oyuneks (Tera - Alış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[2]/div/div[2]/div[2]/div/div/div[1]/div[2]/span",
    "Oyuneks (Tera - Satış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[2]/div/div[2]/div[2]/div/div/div[2]/div[2]/span",
    "Oyuneks (Fenix - Alış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[3]/div/div[2]/div[2]/div/div/div[1]/div[2]/span",
    "Oyuneks (Fenix - Satış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[3]/div/div[2]/div[2]/div/div/div[2]/div[2]/span",
    "Oyuneks (Aura - Alış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[4]/div/div[2]/div[2]/div/div/div[1]/div[2]/span",
    "Oyuneks (Aura - Satış)": "/html/body/div[5]/div/div/div[2]/div/div/div/div/div[4]/div/div[2]/div[2]/div/div/div[2]/div[2]/span",
}

# **Vatangame & Klasgame için Veri Çekme**
data = []
for site in sites:
    print(f"📌 {site['name']} verileri çekiliyor...")
    try:
        driver.get(site["url"])
        time.sleep(10)
        element = driver.find_element("xpath", site["xpath"])
        price = element.text.strip()
        print(f"✅ {site['name']} için fiyat bulundu: {price}")
        data.append([site["name"], datetime.now().strftime("%Y-%m-%d %H:%M:%S"), price])
    except Exception as e:
        print(f"⚠ {site['name']} verileri çekilirken hata oluştu: {e}")

# **Oyuneks için Veri Çekme (JavaScript ile)**
driver.get(oyuneks_url)
time.sleep(10)  # Sayfanın yüklenmesini bekle

oyuneks_data = []
for name, xpath in oyuneks_xpaths.items():
    try:
        script = f"""
        return document.evaluate("{xpath}", document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue.innerText;
        """
        price = driver.execute_script(script)
        price = price.strip()
        print(f"✅ {name} için fiyat bulundu: {price}")
        oyuneks_data.append([name, datetime.now().strftime("%Y-%m-%d %H:%M:%S"), price])
    except Exception as e:
        print(f"⚠ {name} verisi alınırken hata oluştu: {e}")


# **CSV'ye kaydetme (Tırnak yok, TL yok, oyuneks.com düzeltildi)**
with open(csv_filename, "w", newline="", encoding="utf-8") as file:
    writer = csv.writer(file)
    writer.writerow(["Site", "Server", "Satis", "Alis"])  # Başlıkları yaz

    site_dict = {}  # Geçici veri saklama için sözlük
    
    for row in data + oyuneks_data:
        site_name = row[0]  # Örnek: "oyuneks.com (Ares - Alış)"
        parts = site_name.split("(")  # Parçalama işlemi
        site = parts[0].strip()  # Site adını al (oyuneks.com)
        server = parts[1].split("-")[0].strip()  # Server adını al (AURA, Fenix vb.)
        transaction_type = parts[1].split("-")[1].replace(")", "").strip()  # Satış mı Alış mı?
        
        # "Oyuneks" olanları "oyuneks.com" yap
        if site.lower() == "oyuneks":
            site = "oyuneks.com"

        # Sözlük yapısında saklama
        key = (site, server)
        if key not in site_dict:
            site_dict[key] = {"satis": "--", "alis": "--"}  # Varsayılan değerler

        # Önce satış sonra alış olacak şekilde düzenleme
        price = row[2].replace(" TL", "").replace(",", ".").strip()  # TL'yi kaldır ve formatı düzelt
        if transaction_type == "Satış":
            site_dict[key]["satis"] = price
        elif transaction_type == "Alış":
            site_dict[key]["alis"] = price

    # Saklanan verileri CSV'ye yaz
    for (site, server), values in site_dict.items():
        writer.writerow([site, server, values["satis"], values["alis"]])

print(f"✅ Tüm fiyatlar '{csv_filename}' dosyasına İSTENİLEN FORMATTA kaydedildi!")


driver.quit()
