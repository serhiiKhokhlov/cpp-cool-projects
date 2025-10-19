[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/dil86f2I)
# **SWO3-Übungen - WS2024/25 - Übungszettel 7**

## **Beispiel 1: Schach**

Implementieren Sie in C++ unter Anwendung des objektorientierten
Programmierparadigmas eine einfache Version des Brettspiels "Schach". Beachten
Sie dabei die folgenden Anforderungen und Hinweise.

Für alle Spielfiguren gibt es eine gemeinsame, abstrakte Basisklasse `chessman`,
die eine Schnittstelle für mindestens die folgenden Funktionalitäten bietet:

1. sie liefert die Farbe einer Spielfigur,

1. sie liefert eine symbolische Darstellung einer Spielfigur in Form eines
   ASCII-Zeichens,

1. sie beantwortet die Frage, ob eine Spielfigur "essentiell" ist (ein Verlust
   einer essenziellen Spielfigur bedeutet das Ende des Spiels) und

1. sie beantwortet die Frage, ob sich eine Spielfigur auf einem Schachbrett von
   Position $(z_0,s_0)$ auf Position $(z_1,s_1)$ bewegen kann.

Alle konkreten Spielfiguren (wie z.B. Bauer und Springer) sollen dann,
insbesondere für die letzte Funktionalität, eigene Implementierungen besitzen.
Erstellen Sie dazu von `chessman` abgeleitete Klassen für die typischen
Schachfiguren.

1. Der König ("king" - K) kann sich in alle Richtungen um ein Feld bewegen. Das
   Zielfeld darf aber nicht durch eine Spielfigur der eigenen Farbe besetzt
   sein. Der König ist eine essenzielle Spielfigur.

1. Die Dame ("queen" - Q) kann sich beliebig weit diagonal, horizontal oder
   vertikal auf dem Spielbett bewegen, solange keine andere Spielfigur im Weg
   ist.

1. Der Läufer ("bishop" - B) kann sich beliebig weit diagonal auf dem Spielbett
   bewegen, solange keine andere Spielfigur im Weg ist.

1. Der Turm ("rook" - R) kann sich beliebig weit horizontal oder vertikal auf
   dem Spielbett bewegen, solange keine andere Spielfigur im Weg ist.

1. Der Springer ("knight" - N) kann sich um jeweils zwei Felder in eine Richtung
   (horizontal oder vertikal) und gleichzeitig um ein Feld in die andere
   Richtung (vertikal oder horizontal) bewegen. Dabei kann er beliebig andere
   Spielfiguren überspringen.

1. Der Bauer ("pawn" - P) kann sich um ein Feld nach vor bewegen. Bei seinem
   ersten Zug können dies auch zwei Felder vorwärts sein. Der Bauer kann nur
   seitlich vorwärts schlagen.

Alle Spielfiguren können auf einer feindlichen Spielfigur landen, was diese aus
dem Spiel befördert und zum eigenen Sieg beitragen kann. Die Spielfiguren
befinden sich auf einem Spielbrett was z.B. mit einer Klasse `chessboard`
implementiert werden kann. Dieses Spielfeld muss mindestens die folgenden
Funktionalitäten aufweisen:

1. Es muss erkannt werden, wann ein Spiel beendet ist (nämlich dann, wenn einer
   der Spieler eine essenzielle Figur verloren hat).

1. Für jede Position muss die darauf befindliche Figur ermittelt werden können.

1. Der aktuellen Spieler sowie die Größe des Spielfeldes müssen ebenfalls
   abgefragt werden können.

1. Die folgenden Fragen müssen für jedes Feld des Spielbretts beantwortet werden
   können:
   1. Kann es "befahren" werden bzw. ist es frei?
   1. Kann eine Spielfigur mit einer bestimmten Farbe geschlagen werden?
   1. Kann eine Spielfigur einer bestimmten Farbe darauf landen? Ist es also
      frei oder kann dort eine andere Figur geschlagen werden?
   1. Kann man auf diesem Feld eine Figur momentan "in die Hand nehmen"? Also
      ist dort eine Figur und wenn ja, von der aktuell am Zug befindlichen
      Farbe?
   1. Kann man die aktuell aktive ("in der Hand befindliche") Figur an diesem
      Feld abstellen? Darf die Figur also von ihrem aktuellen Startpunkt aus
      dort hinfahren und kann auch dortbleiben?

1. Kann man von einem Feld $(z_0,s_0)$ auf ein Feld $(z_1,s_1)$ fahren?

1. Folgende Aktionen müssen außerdem möglich sein:
   1. Die Spielfigur an einer bestimmten Stelle aktivieren, also "in die Hand
      nehmen".
   1. Die aktive Spielfigur auf einem bestimmten Feld wieder abstellen (sofern
      das erlaubt ist).

1. Der aktuelle Spielstand (das Spielbrett mit den Spielfiguren) muss, genauso
   wie unten gezeigt, als ASCII-Grafik auf der Konsole ausgegeben werden.

1. Weitere Spielregeln, wie "Der König darf nicht 'Selbstmord' begehen" etc.

Erledigen Sie nun die folgenden Aufgabenstellungen:

1. Implementieren Sie alle klassischen Schachfiguren und testen Sie deren
   Bewegungsmuster ausführlich. Gehen Sie in der Lösungsidee auch darauf ein,
   wieviel Speicher pro Spielfigur benötigt wird.

1. Schreiben Sie ein Hauptprogramm, mit dem zwei Spieler interaktiv Schach
   spielen können. Dabei müssen alle Zugmöglichkeiten der jeweils aktiven
   Spielfigur angezeigt werden. Zwei Beispiele: Im linken Beispiel ist der weiße
   Springer (N) in Zeile 1, Spalte b aktiv. Im rechten Beispiel ist die die
   weiße Dame (Q) in Zeile 1, Spalte d, die einen gegnerischen schwarzen Bauern
   [p] in Zeile 4, Spalte d schlagen könnte, aktiv.

   ```txt
     | a  b  c  d  e  f  g  h |              | a  b  c  d  e  f  g  h |
   --+------------------------+--          --+------------------------+--
   8 | r  n  b  q  k  b  n  r | 8          8 | r  n  b  q  k  b  n  r | 8
   7 | p  p  p  p  p  p  p  p | 7          7 | p  p  p  p  *  p  p  p | 7
   6 | .  *  .  *  .  *  .  * | 6          6 | .  *  .  *  .  *  .  * | 6
   5 | *  .  *  .  *  .  *  . | 5          5 | *  .  *  .  *  .  * [.]| 5
   4 | .  *  .  *  .  *  .  * | 4          4 |[.] *  . [p] .  * [.] * | 4
   3 |[*] . [*] .  *  .  *  . | 3          3 | * [.] P [.] P [.] *  . | 3
   2 | P  P  P  P  P  P  P  P | 2          2 | P  P [.][*][.] P  P  P | 2
   1 | R (N) B  Q  K  B  N  R | 1          1 | R  N  B (Q) K  B  N  R | 1
   --+------------------------+--          --+------------------------+--
     | a  b  c  d  e  f  g  h |              | a  b  c  d  e  f  g  h |
   ```

   Abbildung: Die unterschiedlichen Farben der Spielfiguren sind als Groß- bzw.
   Kleinbuchstaben, die unterschiedlichen Farben der Felder durch Punkt bzw.
   Stern dargestellt. Die aktive Spielfigur ist in runde Klammern gesetzt. Die
   Felder, die die aktive Spielfigur betreten kann, sind in eckige Klammern
   gesetzt.

1. Schreiben Sie ein alternatives Hauptprogramm (oder eine über die
   Kommandozeile auswählbare Variante), das bzw. die es erlaubt, einen
   zufälligen, und natürlich nur aus gültigen Zügen bestehenden, Spielablauf zu
   generieren.
