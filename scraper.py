from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
import csv
import time
import os

# **Chrome seçeneklerini ayarla**
chrome_options = Options()
chrome_options.add_argument("--headless")  # Arka planda çalıştır (UI olmadan)
chrome_options.add_argument("--no-sandbox")  # Google Cloud için gerekli
chrome_options.add_argument("--disable-dev-shm-usage")  # Bellek kullanımını azalt
chrome_options.add_argument("--user-data-dir=/tmp/chrome-user-data")  # Yeni kullanıcı dizini

# **ChromeDriver servis ayarı**
service = Service("/usr/local/bin/chromedriver")

# **Tarayıcıyı başlat**
driver = webdriver.Chrome(service=service, options=chrome_options)

# **Çıkartılacak Siteler (Hem Knight hem de Rise için)**
excluded_sites = ["kabasakal.com", "tezgamepin.com"]

# **1️⃣ Knight Online GB Verilerini Çek**
print("📌 Knight Online GB verileri çekiliyor...")
driver.get("https://www.enucuzgb.com/knight-online/kogbdata")
time.sleep(5)  # Cloudflare korumasını beklemek için

try:
    tablo = driver.find_element("tag name", "table")
    satirlar = tablo.find_elements("tag name", "tr")

    knight_veri_listesi = []
    for satir in satirlar:
        hucreler = [hucre.text.strip() for hucre in satir.find_elements("tag name", "td") + satir.find_elements("tag name", "th")]
        if hucreler:
            knight_veri_listesi.append(hucreler)
    
    if len(knight_veri_listesi) > 2:
        knight_veri_listesi = knight_veri_listesi[2:]

    knight_site_isimleri = [
        "sonteklif.com", "bynogame.com", "kopazar.com", "oyuneks.com", "gamesatis.com",
        "bursagb.com", "oyunfor.com", "klasgame.com", "kabasakal.com", "tezgamepin.com"
    ]
    knight_server_isimleri = ["ZERO", "FELIS", "AGARTHA", "PANDORA", "DRYADS", "DESTAN", "MINARK", "OREADS"]

    knight_guncel_veri_listesi = []
    for i, satir in enumerate(knight_veri_listesi):
        site_adi = knight_site_isimleri[i] if i < len(knight_site_isimleri) else f"Unknown_Site_{i}"
        if satir and satir[0] == "":
            satir = satir[1:]

        for j in range(8):
            if j * 2 < len(satir):
                satis_fiyat = satir[j * 2]
                alis_fiyat = satir[j * 2 + 1]
                knight_guncel_veri_listesi.append([site_adi, knight_server_isimleri[j], satis_fiyat, alis_fiyat])

    # **Knight İçin Filtreleme**
    knight_guncel_veri_listesi = [veri for veri in knight_guncel_veri_listesi if veri[0] not in excluded_sites]

    knight_csv_dosya_adi = "knight_veri.csv"
    with open(knight_csv_dosya_adi, mode="w", newline="", encoding="utf-8") as dosya:
        csv_writer = csv.writer(dosya)
        csv_writer.writerow(["Site", "Server", "Satis", "Alis"])
        csv_writer.writerows(knight_guncel_veri_listesi)

    print(f"✅ Knight GB verileri kaydedildi: {knight_csv_dosya_adi}")

except Exception as e:
    print(f"❌ Hata oluştu (Knight Online): {e}")

# **2️⃣ Rise Online GB Verilerini Çek**
print("📌 Rise Online GB verileri çekiliyor...")
try:
    driver.get("https://www.enucuzgb.com/rise-online/risegbdata")
    time.sleep(5)  # Sayfanın yüklenmesini bekle

    tablo = driver.find_element("tag name", "table")
    satirlar = tablo.find_elements("tag name", "tr")

    rise_veri_listesi = []
    for satir in satirlar:
        hucreler = [hucre.text.strip() for hucre in satir.find_elements("tag name", "td") + satir.find_elements("tag name", "th")]
        if hucreler:
            rise_veri_listesi.append(hucreler)
    
    if len(rise_veri_listesi) > 2:
        rise_veri_listesi = rise_veri_listesi[2:]

    rise_site_isimleri = [
        "sonteklif.com", "klasgame.com", "oyuneks.com", "bursagb.com", "kabasakal.com", "oyunfor.com"
    ]
    rise_server_isimleri = ["GALIA", "MANTIS", "ARVARD"]

    rise_guncel_veri_listesi = []
    for i, satir in enumerate(rise_veri_listesi):
        site_adi = rise_site_isimleri[i] if i < len(rise_site_isimleri) else f"Unknown_Site_{i}"
        if satir and satir[0] == "":
            satir = satir[1:]

        for j in range(3):  # 3 Server için
            if j * 2 < len(satir):
                satis_fiyat = satir[j * 2]
                alis_fiyat = satir[j * 2 + 1]
                rise_guncel_veri_listesi.append([site_adi, rise_server_isimleri[j], satis_fiyat, alis_fiyat])

    # **Rise İçin Filtreleme**
    rise_guncel_veri_listesi = [veri for veri in rise_guncel_veri_listesi if veri[0] not in excluded_sites]

    rise_csv_dosya_adi = "rise_veri.csv"
    with open(rise_csv_dosya_adi, mode="w", newline="", encoding="utf-8") as dosya:
        csv_writer = csv.writer(dosya)
        csv_writer.writerow(["Site", "Server", "Satis", "Alis"])
        csv_writer.writerows(rise_guncel_veri_listesi)

    print(f"✅ Rise GB verileri kaydedildi: {rise_csv_dosya_adi}")

except Exception as e:
    print(f"❌ Hata oluştu (Rise Online): {e}")

driver.quit()  # **Tarayıcıyı kapat**
