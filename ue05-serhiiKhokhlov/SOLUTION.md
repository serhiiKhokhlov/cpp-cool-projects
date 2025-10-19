# **SWO3-Übungen - WS2024/25 - Übungszettel 5 - Ausarbeitung**

## **Beispiel 1**

### **Lösungsidee**

#### Überblick
Das Tetris-Projekt ist modular aufgebaut, um die Spiellogik, Darstellung und Benutzerinteraktion klar zu trennen. Die wichtigsten Bestandteile sind:
- Spielfeld (Board): Die Grundlage, auf der das Spiel stattfindet.- Tetriminos: Die beweglichen Spielsteine.
- Kollisionserkennung: Verhindert unzulässige Bewegungen.- Rendering: Zeigt das Spielfeld und die Tetriminos an.
- Ereignissteuerung: Reagiert auf Benutzereingaben und Timer-Ereignisse.
Hier wird das Zusammenspiel dieser Komponenten beschrieben.
---
#### Initialisierung
Beim Start des Spiels werden die zentralen Elemente wie das Spielfeld, die Kanten-Hitboxen und der Timer initialisiert. Die erste Aufgabe des Spiels ist es, einen Tetrimino in der Mitte des Spielfelds zu "spawnen". Diese Aufgabe übernimmt die Funktion spawn, die einen neuen Block anlegt und den zugehörigen Tetrimino (mit Positionen und Hitbox) generiert.
Das Rendering wird ebenfalls eingerichtet, sodass das Spielfeld und die Blöcke dargestellt werden können.

#### Spielfluss und Steuerung
Der Spielfluss wird durch eine Schleife gesteuert, die kontinuierlich Eingaben des Benutzers verarbeitet, das Spielfeld aktualisiert und die grafische Darstellung erneuert.
##### 1. Tastenereignisse
Wenn der Benutzer eine Taste drückt, wird dies durch die Funktion handle_key_event verarbeitet. Sie interpretiert, welche Bewegung oder Aktion gewünscht ist:- Links/Rechts-Bewegung: Der Tetrimino wird seitlich verschoben.
- Rotation: Der Tetrimino wird gedreht.- Abwärtsbewegung: Der Tetrimino fällt schneller.
Bevor eine Bewegung endgültig ausgeführt wird, überprüft die Kollisionserkennung, ob die gewünschte Position gültig ist. Falls nicht, bleibt der Tetrimino an seiner ursprünglichen Position.
##### 2. Automatisches Fallen
Zusätzlich zu den Benutzereingaben wird das automatische Fallen der Tetriminos durch einen Timer gesteuert. Die Funktion on_timer sorgt dafür, dass der aktive Tetrimino periodisch um eine Zeile nach unten bewegt wird. Wenn der Tetrimino eine Kollision mit dem Boden oder anderen Blöcken feststellt, wird er "eingefroren" und in das Spielfeld integriert.

#### Kollisionserkennung
Die Kollisionserkennung spielt eine zentrale Rolle im Spiel. Sie wird durch verschiedene Funktionen umgesetzt:
- Bodenkollision: Überprüft, ob der Tetrimino den unteren Rand des Spielfelds erreicht hat.- Seitliche Kollision: Erkennt, ob der Tetrimino das Spielfeld verlässt oder andere Blöcke seitlich berührt.
- Bereichskollision: Stellt fest, ob der Tetrimino auf bereits platzierte Blöcke fällt.
Diese Prüfungen nutzen die Hitboxen, die die physikalischen Grenzen der Tetriminos und des Spielfelds präzise modellieren.
---
#### Spielfeldverwaltung
Das Spielfeld ist als 2D-Matrix organisiert, die speichert, welche Positionen frei oder belegt sind. Wichtige Aufgaben der Spielfeldverwaltung umfassen:
1. Blockplatzierung: Wenn ein Tetrimino "eingefroren" wird, werden seine Positionen als belegt markiert.2. Zeilenlöschung: Nach der Platzierung wird überprüft, ob eine oder mehrere Reihen vollständig gefüllt sind. Diese Reihen werden gelöscht, und die darüberliegenden Blöcke werden nach unten verschoben.
3. Spielende: Wenn die Blöcke den oberen Rand des Spielfelds erreichen, endet das Spiel.
---
#### Rendering
Das Rendering sorgt dafür, dass der aktuelle Zustand des Spiels visuell dargestellt wird. Es umfasst:
- Tetrimino-Darstellung: Der aktive Tetrimino wird in seiner aktuellen Position und Rotation angezeigt.- Spielfeld-Darstellung: Belegte Felder werden entsprechend ihrer Farben gezeichnet.
- Aktualisierung des Bildschirms: In jeder Spielschleife wird der Bildschirm neu gezeichnet, um Änderungen sichtbar zu machen.
---
#### Zusammenspiel der Komponenten
1. Start: Das Spiel initialisiert das Spielfeld und spawnt den ersten Tetrimino.
2. Eingabe und Timer: Der Benutzer steuert den Tetrimino über die Tastatur, während der Timer das automatische Fallen übernimmt.3. Kollision und Platzierung: Jede Bewegung oder Rotation wird durch die Kollisionserkennung überprüft. Sobald ein Tetrimino den Boden erreicht, wird er fixiert und das Spielfeld wird aktualisiert.
4. Zeilenlöschung: Vollständige Reihen werden entfernt, was die Dynamik des Spiels aufrechterhält.5. Spielende: Das Spiel läuft, bis keine neuen Tetriminos mehr platziert werden können.
---
#### Fazit
Das Zusammenspiel von Spielfeld, Tetriminos, Kollisionserkennung und Rendering erzeugt ein dynamisches und reaktionsschnelles Spielerlebnis. Dank der modularen Struktur ist das Spiel leicht erweiterbar, beispielsweise durch neue Tetrimino-Formen, Schwierigkeitsstufen oder zusätzliche visuelle Effekte.

### **Testfälle**

Gameplay-Test (Siehe in ./doc/)

<video width="40%" controls>
  <source src="./doc/tetris_gameplay_test.mp4" type="video/mp4">
  <source src="./doc/tetris_gameplay_test.mov" type="video/quicktime">
  Your browser does not support the video tag.
</video>