const http = require('http');
const fs = require('fs');
const path = require('path');

const server = http.createServer((req, res) => {
  // Cesta k souboru, který chceš poskytnout
  const filePath = path.join('C:', 'devil', 'dist', 'excel.exe'); // Změněná cesta na nový soubor

  // Ověření, že požadavek je na URL pro stažení souboru
  if (req.url === '/stahnout') {
    // Zjištění typu souboru podle přípony
    const extname = path.extname(filePath).toLowerCase();
    let contentType = 'application/octet-stream'; // Pro binární soubory

    // Nastavení typu pro .exe soubor
    if (extname === '.exe') {
      contentType = 'application/x-msdownload'; // Nastavení pro .exe soubory
    }

    // Odeslání souboru v odpovědi
    fs.readFile(filePath, (err, data) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'text/plain' });
        res.end('Chyba při čtení souboru.');
      } else {
        res.writeHead(200, {
          'Content-Type': contentType,
          'Content-Disposition': 'attachment; filename="excel.exe"' // Změna názvu souboru pro stažení
        });
        res.end(data);
      }
    });
  } else {
    // Pokud není požadována URL pro stažení
    res.writeHead(200, { 'Content-Type': 'text/html' });
    res.end('<html><body><a href="/stahnout">Klikněte zde pro stažení souboru</a></body></html>');
  }
});

const PORT = 3000;
const LOCAL_IP = '10.0.1.35'; // Zde zadej svou lokální IP (např. 192.168.1.100)

server.listen(PORT, LOCAL_IP, () => {
  console.log(`Server běží na http://${LOCAL_IP}:${PORT}`);
});
