async function fetchData() {
    try {
        const response = await fetch("/knight-data"); // API'den veri çek
        if (!response.ok) throw new Error("Veri alınamadı!");

        const data = await response.json();
        let table = document.getElementById("knight-table");
        table.innerHTML = "";

        let siteLogos = {
            "bursagb.com": "bursagb.webp",
            "bynogame.com": "bynogame.webp",
            "gamesatis.com": "gamesatis.webp",
            "kopazar.com": "kopazar.webp",
            "oyuneks.com": "oyuneks.webp",
            "oyunfor.com": "oyunfor.webp",
            "sonteklif.com": "sonteklif.webp",
            "klasgame.com": "klasgame.webp",
            "vatangame.com": "vatangame.webp"
        };
        let siteLinks = {
            "bursagb.com": "https://www.bursagb.com/knight-online-gb-c-4",
            "bynogame.com": "https://www.bynogame.comtr/oyunlar/knight-online/gold-bar",
            "gamesatis.com": "https://www.gamesatis.com/knight-online-goldbar",
            "kopazar.com": "https://www.kopazar.com/knight-online-gold-bar",
            "oyuneks.com": "https://www.oyuneks.com/knight-online-world/knight-online-goldbar-alis-satis",
            "oyunfor.com": "https://www.oyunfor.com/knight-online/gb-gold-bar",
            "sonteklif.com": "https://www.sonteklif.com/knight-online-gb-c-2",
            "klasgame.com": "https://www.klasgame.com/knightonline/knight-online-gb-goldbar-premium-cash",
            "vatangame.com": "https://www.vatangame.com/oyun-parasi/knight-online-gold-bar"
        };

        let siteMap = {};
        let maxAlis = {};
        let minSatis = {};

        // 📌 JSON Verisini Site ve Sunucu Bazında Grupla
        data.forEach(veri => {
            let serverKey = veri.server.trim().toUpperCase();
            let satisFiyat = parseFloat(veri.satis);
            let alisFiyat = parseFloat(veri.alis);

            if (!siteMap[veri.site]) {
                siteMap[veri.site] = {};
            }
            siteMap[veri.site][serverKey] = { satis: satisFiyat, alis: alisFiyat };

            // 🔥 En yüksek alış fiyatını bul
            if (!maxAlis[serverKey] || alisFiyat > maxAlis[serverKey]) {
                maxAlis[serverKey] = alisFiyat;
            }

            // 💰 En düşük satış fiyatını bul
            if (!minSatis[serverKey] || satisFiyat < minSatis[serverKey]) {
                minSatis[serverKey] = satisFiyat;
            }
        });

        console.log("🔥 En yüksek alış fiyatları:", maxAlis);
        console.log("💰 En düşük satış fiyatları:", minSatis);

        // 📌 Tabloyu Düzenli Şekilde Oluştur
        Object.keys(siteMap)
            .sort((a, b) => a.localeCompare(b)) // 🟢 Site isimlerini alfabetik sıraya sokuyoruz
            .forEach(site => {
                let siteUrl = siteLinks[site] || "#"; // 🔥 Manuel site linkini kullanıyoruz
                let logoPath = `/logos/${siteLogos[site] || "default.webp"}`;
                let row = `<tr>
                            <td class="site-column">
                                <a href="${siteUrl}" target="_blank">
                                    <img class="site-logo" src="${logoPath}" alt="${site}">
                                </a>
                            </td>`;

                
                ["ZERO", "FELIS", "AGARTHA", "PANDORA", "DRYADS", "DESTAN", "MINARK", "OREADS"].forEach(server => {
                    let serverKey = server.trim().toUpperCase();
                    let satisFiyat = siteMap[site][serverKey]?.satis || "-";
                    let alisFiyat = siteMap[site][serverKey]?.alis || "-";

                    let satisClass = satisFiyat === minSatis[serverKey] ? "best-sell" : "";
                    let alisClass = alisFiyat === maxAlis[serverKey] ? "best-buy" : "";

                    row += `<td class="${satisClass}">${satisFiyat}</td><td class="${alisClass}">${alisFiyat}</td>`;
                });

                row += "</tr>";
                table.innerHTML += row;
            });

    } catch (error) {
        console.error("Hata:", error);
    }
}

fetchData();
setInterval(fetchData, 900000);

