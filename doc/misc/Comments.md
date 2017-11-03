# Doxygen

- `\file` ohne Dateinamen - es wird automatisch die aktuelle Datei verwendet

- Für Dateien und Klassen mit `\ingroup` ein Modul auswählen.

- generell
    - alle Todos und Bugs in mit `\todo` und `\bug` dokumentieren, damit sie nicht untergehen

- in H-Dateien
    - nur einzeilige Kommentare von Funktionen
    - Klassen vollständig dokumentieren

- in cc-Dateien
    - Funktionen vollständig dokumentieren
    - kein `\brief`-Teil - das gehört in den Header
    - alle Effekte jenseits der Funktion mit `\post` dokumentieren - insbesondere Memory Allocations
    - alle Annahmen mit `\pre` dokumentieren; dies dann auch als Assertions an den Anfang der Funktion
    - Bei Parametern, die verändert werden können (z.B. Pointer) dies mit `\param[out]` bzw. `\param[in,out]` explizit machen
    - alle Parameter und Rückgabewert mit `\param` und `\return` dokumenieren

# Coding

- Bei Konstruktoren alle Membervariablen in Initializer-List initialisieren.
- Header immer mit absolutem Pfad ab src und in <> einbinden, z.B. `#include<Core/Handlers.h>`.