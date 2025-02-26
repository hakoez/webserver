async function fetchData() {
    try {
        const response = await fetch("/nowa-data"); //Get Data from API
        if (!response.ok) throw new Error("Veri alınamadı!");

        const data = await response.json();
        let table = document.getElementById("nowa-table");
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
            "oyuneks.com": "https://www.oyuneks.com/nowa-online-world/nowa-online-world-gold",
            "klasgame.com": "https://www.klasgame.com/mmorpg-oyunlar/nowa-online-world/nowa-online-world-gold",
            "vatangame.com": "https://www.vatangame.com/oyun-parasi/nowa-online-world-goldbar"
        };
        let siteMap = {};
        let maxAlis = {};
        let minSatis = {};

        // 📌 JSON data
        data.forEach(veri => {
            let serverKey = veri.server.trim().toUpperCase();
            let satisFiyat = parseFloat(veri.satis);
            let alisFiyat = parseFloat(veri.alis);

            if (!siteMap[veri.site]) {
                siteMap[veri.site] = {};
            }
            siteMap[veri.site][serverKey] = { satis: satisFiyat, alis: alisFiyat };

            
            if (!maxAlis[serverKey] || alisFiyat > maxAlis[serverKey]) {
                maxAlis[serverKey] = alisFiyat;
            }

  
            if (!minSatis[serverKey] || satisFiyat < minSatis[serverKey]) {
                minSatis[serverKey] = satisFiyat;
            }
        });

        console.log("🔥 En yüksek alış fiyatları:", maxAlis);
        console.log("💰 En düşük satış fiyatları:", minSatis);

        Object.keys(siteMap).forEach(site => {
            let siteUrl = siteLinks[site] || "#";
            let logoPath = `/logos/${siteLogos[site] || "default.webp"}`;
            let row = `<tr>
                <td class="site-column">
                    <a href="${siteUrl}" target="_blank">
                        <img class="site-logo" src="${logoPath}" alt="${site}">
                    </a>
                </td>`;

            ["ARES", "FENIX", "TERA", "AURA"].forEach(server => {
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

