# **SWE4-Übungen - SS2025 - Übung 2**

## Discrete Event Simulation (src/des)

Implementieren Sie ein C++-Programm, das diskrete Prozesse simulieren kann. Ein
Prozess ist diskret, wenn sich nur zu endlich vielen Zeitpunkten der
Systemzustand ändern kann. Als Methode soll die diskrete Ereignissimulation
eingesetzt werden. Bei dieser Methode wird nicht in Realzeit simuliert, sondern
es werden nur jene Zeitpunkte betrachtet, zu denen Änderungen am System
eintreten. Die Zeit zwischen diesen Ereignissen kann vernachlässigt werden. Das
kann die benötigte Simulationszeit drastisch reduzieren, da nur interessante
Zeitpunkte betrachtet werden. Herzstück eines solchen Simulators ist daher eine
Liste mit noch ausstehenden Ereignissen, die in der Zukunft passieren werden.
Diese Liste muss nach der Zeit aufsteigend sein. Da jederzeit neue Ereignisse
hinzukommen können und die Anzahl der Ereignisse mitunter sehr beachtlich
ausfallen kann, ist die Wahl einer geeigneten Datenstruktur essenziell. Hier
bietet sich die C++-Standardbibliothek mit ihrer großen Anzahl an sehr effizient
implementierten Datenstrukturen an. 

1. **Simulationsbibliothek**
   
    Recherchieren Sie mögliche Designansätze und passende Datenstrukturen für
    diese Aufgabenstellung und implementieren Sie eine möglichst allgemein
    einsetzbare Simulationsbibliothek in Form von C++-Klassen. Folgende
    Funktionalität muss mindestens verfügbar sein:

    * Zukünftige Ereignisse müssen für einen bestimmten Zeitpunkt geplant werden
      können und in einer effizienten Datenstruktur gehalten werden.

    * Ereignisse müssen andere Ereignisse (in derselben Simulation) erzeugen
      können. Überlegen Sie hier, welche API Sie verwenden, um diesen Aspekt, der
      in der Praxis häufig gebraucht wird, möglichst komfortabel gestalten zu
      können.

    * Das System muss verschiedene Arten von Ereignissen unterstützen. Es soll
      einfach möglich sein, neue Ereignistypen hinzuzufügen.

    * Die Simulation muss schrittweise ausgeführt werden können. Es muss also z.
      B. eine Methode `step()` geben, die nur das nächste Ereignis abarbeitet.

    * Die Simulation muss bis zu einem bestimmten Kriterium ausgeführt werden
      können, nachdem sie entweder pausiert oder stoppt. Es muss also z.B. eine
      Methode `run()` geben, die durch Ereignisse in der Simulation beendet
      werden kann.

2. **Anwendungsszenario**

    Demonstrieren Sie die korrekte Funktionsweise Ihrer Bibliothek anhand der
    Simulation eines Call-Centers. Das folgende Szenario soll auf Ihr
    Simulationsmodell abgebildet werden.

    * Das Call-Center verfügt über eine feste Anzahl an Mitarbeitern, die Anrufe
      entgegennehmen können. Ist ein Mitarbeiter verfügbar, wird der Anruf
      direkt zugeteilt, ansonsten in eine Warteschlange eingereiht.

    * Die Zeit zwischen zwei Ereignissen ist exponentialverteilt.Die Anzahl der
      ankommenden Anrufe variiert allerdings über den Tag hinweg und folgt
      einer definierten Tageszeit-basierten Anrufrate (z. B. hoher Andrang am
      Morgen und Mittag, niedriger Andrang am Abend). Der Parameter für die
      Häufigkeit der Anrufe muss daher für jede Stunde eines Tages festgelegt
      werden können (z. B. von 6 \- 7 Uhr rufen durchschnittlich 100 Kunden, von
      7 - 8 Uhr durchschnittlich 150 Kunden etc. an)

    * Die Bearbeitungsdauer eines Anrufs ist zufällig und folgt einer
      Normalverteilung.

    * Ein Anrufer verlässt die Warteschlange (Abbruch), wenn seine Wartezeit
      eine gewisse Grenze überschreitet. Die Wartezeit, bis ein Anrufer aufgibt,
      folgt einer logarithmischen Normalverteilung (Log-Normalverteilung).

    * Sobald ein Mitarbeiter einen Anruf beendet, nimmt er sofort den nächsten
      Anrufer aus der Warteschlange an, falls einer vorhanden ist.

    * Treffen Sie sinnvolle Annahmen für die Parameter der Verteilungen der
      Zufallsgrößen (gestaffelte Anrufhäufigkeit, Bearbeitungszeit, Zeit bis zum
      Abbruch)

    * Simulieren Sie den Betrieb des Call-Centers über einen gesamten Arbeitstag
      (12 Stunden), bestimmen Sie Kennzahlen (Durchschnitt, Minimum, Maximum)
      zur Warteschlangenlänge, Wartezeit und Mitarbeiterauslastung und geben Sie
      diese aus.

3. **Kennzahlen und Vergleichsszenarien** 
  
    Ermitteln Sie in regelmäßigen Abständen die durchschnittliche Länge der
    Warteschlange, die durchschnittliche Wartezeit und die Mitarbeiterauslastung
    für einen bestimmen Zeitbereich. Exportieren Sie diese Werte in eine
    CSV-Datei und stellen Sie diese graphisch (z. B. in Excel) dar.

    * Vergleichen Sie zwei Szenarien:

      + _Szenario 1:_ Über den gesamten Arbeitstag hinweg steht eine fixe Anzahl
        an Mitarbeitern zur Verfügung.

      + _Szenario 2:_ Die Mitarbeiteranzahl kann für jede Stunde des Tages
        angepasst werden.

        Entwickeln Sie eine Kennzahl, die sowohl die Kundenzufriedenheit (kurze
        Wartezeiten) als auch die Mitarbeiterauslastung berücksichtigt. Die
        Kennzahl sollte eine vernünftige Balance zwischen diesen beiden
        Parametern herstellen. Bestimmen in jedem Simulationslauf diese
        Kennzahl.

        Ermitteln Sie durch Variation der Mitarbeiteranzahl einen möglichst
        guten Wert für diese Kennzahl. Es ist ausreichend, wenn Sie durch
        Experimentieren zu einem guten Wert für diese Kennzahl kommen.
        Alternativ können Sie Ihr Programm um einen einfachen
        Optimierungsalgorithmus erweitern, der durch systematisches Probieren
        eine möglichst gute Lösung berechnet.
   
_Hinweise:_

* Ereignisse, die unabhängig von anderen Ereignissen sind, können gleich zu
  Beginn der Simulation in die Ereigniswarteschlange eingefügt werden. Dazu
  zählen beispielsweise eingehende Anrufe oder Zeitpunkte, an denen statistische
  Auswertungen erfolgen müssen. Voraussetzung dafür ist, das man von vorneweg
  weiß, wie lange die Simulation laufen wird. Man kann aber auch so vorgehen,
  dass man bei der Abarbeitung eines derartigen Ereignisses das nächste zur
  Ereigniswarteschlange hinzufügt.

* Bei bestimmten Ereignissen ist bekannt, dass sie nach einer gewissen Zeit
  weitere Ereignisse auslösen. Wird beispielsweise ein Anrufer einem Mitarbeiter
  zugewiesen, kann die Gesprächsdauer (auf Basis einer Normalverteilung)
  bestimmt und das Ereignis "Anruf beendet" in die Ereigniswarteschlange
  eingetragen werden.

* Manche Ereignisse sind subtiler auf das Simulationsmodell abzubilden. Um den
  Abbruch eines Gesprächs zu modellieren, könnten Sie folgendermaßen vorgehen:
  Wenn beim Ereignis "Anruf triff ein" kein Mitarbeiter verfügbar ist, gibt man
  den Anruf in die Anruf-Warteschlange, berechnet die Zeit, nach welcher der
  Anrufer auflegen würde und fügt das Ereignis "Anruf abgebrochen" in die
  Ereigniswarteschlange ein. Stellt man bei der Abarbeitung dieses Ereignisses
  fest, dass sich der Anruf nicht mehr in der Anruf-Warteschlange befindet, kann
  man das Ereignis verwerfen.  

* Bei der Abarbeitung von Ereignissen müssen Sie die anwendungsbezogenen
  Datenstrukturen aktualisieren. Sie müssen beispielsweise den Status der
  Mitarbeiter (verfügbar, belegt) sowie die Warteschlange der nicht
  zugeteilten Anrufer in passenden Datenstrukturen verwalten. Ebenso
  benötigen Sie Datenstrukturen zum Speichern von statistischen Daten und
  der Kennzahlen.  

### Technologische Anforderungen:

* Achten Sie auf ein sauberes objektorientiertes Design.

* Verwenden Sie möglichst viele Klassen und Konzepte der C++-BehälterBibliothek
  (Behälterklassen, Algorithmen, Funktionsobjekte). Setzen Sie zur Realisierung
  der Ereigniswarteschlange die Datenstruktur `std::priority_queue<>` ein.
  Verwenden Sie die Zufallszahlengeneratoren der C++-Standardbibliothek. Diese
  bietet auch Möglichkeiten, Zufallszahlen, die eine bestimmte Verteilung
  aufweisen, zu generieren.

* Achten Sie darauf, dass Ihr System mit neuen Event-Typen einfach erweiterbar
  ist. Das können Sie am einfachsten gewährleisten, indem Sie eine abstrakte
  Basisklasse für Events anbieten, von der konkrete Event-Typen abgeleitet
  werden können.

* Verwenden Sie zur Verwaltung von dynamisch erzeugten Objekten konsequent
  Smart-Pointer-Klassen aus der STL. Es sollte an keiner Stelle in Ihrem
  Programm notwendig sein, die Schlüsselwörter `new` oder `delete` einzusetzen. 

* Vermeiden Sie globale Variablen.