Installation (windows):
1. gcc runterladen (mingw) Link: https://sourceforge.net/projects/mingw-w64/
2. mingw installieren
3. in den installationsordner wechseln (momentan "C:\Program Files\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0")
4. Dort mingw-w64 ausführen
5. Per cd in diesen Ordner wieder wechseln und dort den befehl
"gcc io.c -o unixconv.exe -lshlwapi"
ausführen um das Programm zu erstellen. (-o sorgt für den Dateinamen | -lshlwapi bindet die shlwapi.h Bibliothek von Windows ein)
Hinweis: Wenn der Ordner sich auf dem Desktop befindet kann man folgenden Befehl nutzen um
auf den Desktop zu gelangen
"cd \Users\%USERNAME%\Desktop"
6. Das Programm kann nun ausgeführt werden in diesem Ordner mithilfe von "unixconv.exe"
auf der Kommandozeile
7. Ein Beispiel wäre um sich damit vertraut zu machen
"unixconv.exe -h"

Zusatz: Wenn man einen anderen Namen für die Geräte haben möchte oder diese umändern muss,
dann kann man unter config.c dies in den #define Direktiven einstellen. Ein Beispiel ist dort
vorhanden.
___________________________________________________________________________________________

Installation (unix):
1. config.c öffnen und die Namen der IDs eingeben (die ursprünglichen vom MUCEasy)
2. In config.c die Anzahl der aktiven IDs (dh die Gesamtanzahl der unterschiedlichen IDs/Geräte)
   einfügen bzw korregieren Standard ist auf 2 gesetzt
3. Benutzerdefinierte Namen setzen für die Geräte zB: Labor 1 in config.c unter Custom IDs

4. Kompiliere/Erstellen
4.1 Dazu das Terminal aufrufen (Alt+Strg+T auf manchen Systemen), sonst unter Systemwerkzeuge/Zubehör auffindbar
4.2 In das Installationsverzeichnis wechseln (wo diese Datei wahrscheinlich liegt)
4.2.1 Im Terminal kann man mit dem Befehl cd sein Verzeichnis wechseln.
4.3 Im Installationsverzeichnis angekommen den Befehl sudo make eingeben und ausführen
    (Übersetzt die .c Dateien in Maschinencode) und speichert die ausführbare Datei unter /usr/bin/unixconv ab,
    damit man in jedem Verzeichnis mithilfe von dem Befehl "unixconv" Die MUCEasy Dateien konvertieren kann unter dem
    aktuellen Benutzer

5. unixconv -h ausführen für Hilfe über das Programm und dessen Verwendung

Achtung: Wenn Sie neue Geräte installiert haben muss das Programm neu kompiliert(übersetzt werden), damit 
         die Änderungen in config.c wirksam werden.