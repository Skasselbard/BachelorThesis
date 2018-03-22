# Motivation
- Was: LoLA
- Warum: parallel ist langsam
- evtl peek aufs ergebnis

# Background
- Petri Netze
- Wie sah LoLA vorher aus
- DFS in LoLa
- Benchmarking
- Environment

# Compare And Swap
- bottleneck wurde bei der synchronisation vermutet
- => wechsel der synchronisation
- bisher pthread
- austausch durch CAS
- CAS ist eine atomare operation
- atomare operationen können nur komplett oder garnicht ausgeführt werden
- implementation durch spinnlock (**Bild**)

# CAS Auswertung
- 1000 philosophen
    - lasten den rechner gut aus
    - passen in den speicher
    - kann parallel abgearbeitet werden
- komplette aufdeckung des zustandsraums
    - zur besseren vergleichbarkeit der ergebnisse
- macro benchmark deutet darauf hin das es keine verbesserung gibt
- genaue suche des bottlenecks ist erforderlich

## Manuelle Instrumentierung
- manuelles einfügen von code zum messen der suchzeit
- berechnen von start und endzeiten
- fokus auf synchronisationsstellen
- versuch die ganze suche zu überdecken

## Ergebnisse
- Zeiten sind inkonsistent
- gesamtzeit in threads is eine größenordnung größer als "work" und "work" (sollten die suche überdecken)
- **Aber:** die meiste zeit in searchAndInsert
- Erkenntnis das die aktuelle suche im speicher nicht thread safe ist
- in zukunft nur suche mit hashingWrapper (**Bild**)
    - verwaltet mehrere speicher die nur von einem thread betreten werden dürfen
    - bucket wird durch hash des gesuchten zustands bestimmt

# Profiling Einführung
- alshkdlkah
# Profiling Ergebnisse
# Mara
# Profiling nach Mara Integration
# Macro Benchmarks
# Zusammenfassung
# Auswertung