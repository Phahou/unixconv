
Installation:
1. config.c öffnen und die Namen der IDs eingeben (die ursprünglichen vom MUCEasy)
2. In config.c die Anzahl der aktiven IDs (dh die Gesamtanzahl der unterschiedlichen IDs/Geräte)
   einfügen bzw korregieren Standard ist auf 2 gesetzt
3. Benutzerdefinierte Namen setzen für die Geräte zB: Labor 1 in config.c unter Custom IDs

4. Kompiliere/Erstellen
4.1 Dazu das Terminal aufrufen (Alt+Strg+T auf manchen Systemen), sonst unter Systemwerkzeuge/Zubehör auffindbar
4.2 In das Installationsverzeichnis wechseln (wo diese Datei wahrscheinlich liegt)
4.2.1 Im Terminal kann man mit dem Befehl cd sein Verzeichnis wechseln.

4.3 Im Installationsverzeichnis angekommen den Befehl sudo make eingeben und ausführen
    (Übersetzt die .c Dateien in Maschinencode) und speichert die ausführbare Datei unter /bin/unixconv ab,
    damit man in jedem Verzeichnis mithilfe von dem Befehl "unixconv" Die MUCEasy Dateien konvertieren kann

5. unixconv -h ausführen für Hilfe über das Programm und dessen Verwendung

Achtung: Wenn Sie neue Geräte installiert haben muss das Programm neu kompiliert(übersetzt werden), damit 
         die Änderungen in config.c wirksam werden.

Achtung: Das Programm funktioniert nur mit UNIX-Dateien d.h. wenn die Daten vom FTP gezogen werden den Befehl ascii
         verwenden ODER tr -d '\r' <eingabe.datei >ausgabe.datei benutzen
         oder das Programm dos2unix glaub man kann sich das einfach über die Debian Repos holen
         (Caldera ist Debian-based)
         Glaub zum installieren von Programmen nimmt man auf Debian "apt install <paket>" oder "apt-get install <paket>"

         Benutz Manjaro also von daher ¯\_(ツ)_/¯ Bei mir wärs aber "sudo pacman -S dos2unix"
         Man kann sich das alles eigentlich ersparen, wenn man beim FTP die Dateien per Ascii rübersendet
         (Das Ende der Zeile sollte halt kein ^M sein, weil das wär dos und Unix benutzt halt \n als Zeilenendensymbol)