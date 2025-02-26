// Knight Online fetch Data
async function fetchKnightData() {
    try {
        const response = await fetch("/knight-data");
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

        Object.keys(siteMap)
            .sort((a, b) => a.localeCompare(b))
            .forEach(site => {
                let siteUrl = siteLinks[site] || "#";
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

// Nowa Online fetch Data
async function fetchNowaData() {
    try {
        const response = await fetch("/nowa-data");
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

// Rise Online fetch Data
async function fetchRiseData() {
    try {
        const response = await fetch("/rise-data");
        if (!response.ok) throw new Error("Veri alınamadı!");
        const data = await response.json();
        let table = document.getElementById("rise-table");
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
            "bursagb.com": "https://www.bursagb.com/rise-online-world-gold-c-63",
            "bynogame.com": "https://www.bynogame.com",
            "gamesatis.com": "https://www.gamesatis.com",
            "kopazar.com": "https://www.kopazar.com",
            "oyuneks.com": "https://www.oyuneks.com/rise-online-world/rise-online-gold-alis-satis",
            "oyunfor.com": "https://www.oyunfor.com/rise-online-world/rise-online-world-gb",
            "sonteklif.com": "https://www.sonteklif.com/rise-online-world-gold-c-10",
            "klasgame.com": "https://www.klasgame.com/rise-online-world/rise-online-world-gold",
            "vatangame.com": "https://www.vatangame.com"
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

            // 🔥 Max buy price
            if (!maxAlis[serverKey] || alisFiyat > maxAlis[serverKey]) {
                maxAlis[serverKey] = alisFiyat;
            }

            // 💰 Lowest sell price
            if (!minSatis[serverKey] || satisFiyat < minSatis[serverKey]) {
                minSatis[serverKey] = satisFiyat;
            }
        });

        console.log("🔥 En yüksek alış fiyatları:", maxAlis);
        console.log("💰 En düşük satış fiyatları:", minSatis);

        // 📌 Tabloyu Düzenli Şekilde Oluştur
        Object.keys(siteMap).forEach(site => {
            let siteUrl = siteLinks[site] || "#";
            let logoPath = `/logos/${siteLogos[site] || "default.webp"}`;
            let row = `<tr>
                <td class="site-column">
                    <a href="${siteUrl}" target="_blank">
                        <img class="site-logo" src="${logoPath}" alt="${site}">
                    </a>
                </td>`;

            ["GALIA", "MANTIS", "ARVARD"].forEach(server => {
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

function fetchDataKnightAndRise() {
    fetchKnightData(); 
    fetchRiseData();   
}

function fetchDataNowa() {
    fetchNowaData();
}


fetchDataKnightAndRise(); 
fetchDataNowa();         

//Update data in diffrent times
setInterval(fetchDataKnightAndRise, 120000); // 2 min(120000 ms)  
setInterval(fetchDataNowa, 900000);          // 15 min (900000 ms)
