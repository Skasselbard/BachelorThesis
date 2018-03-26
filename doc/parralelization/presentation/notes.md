# Motivation
- Kann der LowLevelAnalyzer effizient parralel ausgeführt werden?
- Vorheriger versuch von Gregor Behnke
    - Hat zustandsraumsuche von lola erweitert
    - loadbalancing für verschiedene threads in tiefensuche
    - ist leider unabhängig von anzahl der threads in etwa so schnell wie der sequentielle algorithmus

# Compare And Swap
- bottleneck wurde bei der synchronisation vermutet
    - andere programmteile sind unverändert
- => wechsel der synchronisation
- bisher pthread
- austausch durch CAS
- CAS ist eine atomare operation
- atomare operationen können nur komplett oder garnicht ausgeführt werden
    - erwarteter wert wird geändert
    - oder nichts wird gemacht
- CAS in schleife funktioniert wie ein mutex
    - jeglicher overhead der pthread semaphore wird umgangen
    - Aber es gibt keine convenience funktionen (wie deadlock Erkennung)

# Testnetz
- 1000 philosophen
    - lasten den rechner gut aus
    - passen in den speicher
    - kann parallel abgearbeitet werden
- komplette aufdeckung des zustandsraums
    - zur besseren vergleichbarkeit der ergebnisse

# CAS Auswertung
- macro benchmark deutet darauf hin das es keine verbesserung gibt
- genaue suche des bottlenecks ist erforderlich

# Profiling Einführung
- Nach gescheitertem manuellem versuch doch profiling
- Entscheidung zu nutzung des Perf Profiling tools
- Sampled Programzustände
    - in welcher funktion befinde ich mich (in diesem sample)
    - durch welche funktionsaufrufe habe ich diese funktion erreicht
    - Sample Häufigkeit korreliert mit ausführungszeit
- so können die zeitintensievsten funktionen ermittelt werden und der weg wie diese erreicht werden

# Profiling Ergebnisse
- call graph weist auf großen zeitaufwand innerhalb von libcalloc hin
- vermutlich durch aufrufe von malloc, calloc und new
- innerhalb der suche im speicher -> so wie schon durch manuellen ansatz vermutet wurde
- häufige libcalloc funktionen deuten auf threadsynchronisation
- systemallocator scheint stark am bottleneck beteiligt zu sein
- neue allocations strategie könnte das problem lösen

# Mara
- eigener allocator wurde bereits in vorherigem project entwickelt
- speziell auf Lola zugeschnitten
    - holen von großen mengen Heapspace
    - allozierter speicher braucht nicht freigegeben zu werden
    - netzzustände werden sowiso bis zum schluss gebraucht
    - allokator kann alte bereiche vergessen wenn neuer systemspeicher genutzt wird
    - beim programmende wird speicher vom betriebssystem freigegeben
- interne speicheranordnung wie stack
- im wesentlichen drei opereationen
    - merken des returns
    - inkrement des belegt pointer um angeforderte größe
    - rückgabe des gemerkt werts
    - -> sehr schnell
- ist aktueller bereich voll wird neuer geholt und der aktuelle vergessen

# Mara + Hashing wrapper
- eine mara instanz pro bucket
    - jede mit eigenem speicherbereich
    - dadurch brauch mara nicht thread safe sein
- ersetzen von malloc calloc und new innerhalb des buckets mit maras new aufruf

# Profiling nach Mara Integration
- erneutes profiling zeigt das libcalloc samples unter significanz grenze sind
- search and insert beitrag hat sich halbiert
- ABER: profiling gibt nur relative angeben und keine aussagen zur zeit

# Macro Benchmarks
- Macrobenchmark zum zeitvergleich

## SingleThreadPerformance
- CAS und Pthread implementationen sind etwa gleich schnell
- Mara Implementationen sind merkbar schneller
- Auch in zukunft: pthread ist eher schneller als cas, daher nur noch pthread

## Multithread 100 Buckets
- Vorherige Implementation immer gleich schnell
    - Bug  verursachte immer gleichen funktionsaufruf
- Singlethread performance ist bei original implementation deutlich besser
    - HashingWrapperStore hatte deutlich höhere hardcodierte bucket anzahl
    - multithreading zugriff war auskommentiert -> höherer synchronisations aufwand
- ab 2 threads rund lineares scaling mit den threads
- zwischen 1 und 2 threads nicht weil der unterliegende algorithmus gewechselt wird
- in höheren thread bereichen verschwindet das lineare scaling
    - mehr konflikte beim eintritt in buckets
    - initialisierung und aufräumen wurde mit gemessen

## Multithread 100 und 1000 Buckets
- höhere bucket anzahl reduziert die zeit deutlich
    - weniger konflickte
    - kleinere suchräume (buckets halten weniger zustände in denen gesucht wird)

# Zusammenfassung
- LoLA kann von Parallelisierung profitieren
    - zumindest für 1000 Philosophen 
- Unterschied zwischen CAS und Pthread ist zu vernachlässigen
- Speziallisierte Heapverwaltung z.B. durch Mara ist Vorraussetzung für Parallelisierung von LoLA
    - Bietet außerdem potential generell Zeit und Speicher zu sparen