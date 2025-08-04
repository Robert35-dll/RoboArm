# Mechatronik Projekt: Roboterarm

## Hintergrund
Dies ist ein Repository für mein Mechatronik Projekt im 4. Semester meines Ingeneurinformatik Studiums. Die Grundidee ist, _Kommunikation zwischen Mikrocontrollern_ kennenzulernen, also, zwei Mikrocontroller miteinander (_bestenfalls kabellos_) zu verbinden und damit irgendwelche Aktionen auszuführen.

Konkret für dieses Projekt habe ich entschieden, mit meinem eigenen Arm einen Roboterarm zu steuern. Den genauen Aufbau von einem Roboterarm belasse ich zunächst außer Betracht, weil er ziemlich aufwändig sein soll, um mehr als nur Ellbogen biegen zu können :)

## Gliederung
- [Installation](#installation)
- [Aufbau](#aufbau)
- [Architektur](#architektur)
- [Code](#code)
- [Theorie](#theorie)
  - [I2C-Protokoll](#i2c-protokoll)
  - [Regelungstechnische Grundlagen](#regelungstechnische-grundlagen)
  - [Radiokommunikation](#radiokommunikation)

## Installation
Dieses Projekt wurde im VS Code mithilfe PlatformIO Extension entwickelt. Alternativ kannst du auch in nativem ArduinoIDE arbeiten. Um den Code auf dein PC zu haben tuh bitte folgendes:

1) Klone dieses Repository auf dein PC mit `git clone https://github.com/Robert35-dll/RoboArm.git`
2) Öffne den heruntergeladenen Ordner in deinem IDE
   - _Optional: öffne das `code-workspace` Datei, um in VS Code schneller zwischen Teilprojekten zu wechseln_

An dieser Stelle solltest du die Ordnerstruktur vom Projekt sehen:
```
/Roboarm
  |-/ArmReader
  |   |-/.vscode
  |   |-/include
  |   |-/lib
  |   |-/src
  |       |-< main.cpp (hier liegt der Code von ArmReader)
  |   |-/test
  |   |-< .gitignore
  |   |-< patformio.ini
  |-/ArmWriter
  |   |-/.vscode
  |   |-/include
  |   |-/lib
  |   |-/src
  |       |-< main.cpp (hier liegt der Code von ArmWriter)
  |   |-/test
  |   |-< .gitignore
  |   |-< patformio.ini
  |-< code-workspace.code-workspace
  |-< README.md
```

Wenn dein PlatformIO bzw. ArduinoIDE noch keine Teilprojekte erkannt hat, musst du selber die Pfade von `ArmReader` und `ArmWriter` Ordnern (_nicht von main.cpp Dateien_) öffnen / angeben.
Wenn beide Teilprojekte erkannt werden, installiere die notwendige Bibliotheken:

3) Öffne das `ArmReader` Projekt und installiere `RF24` Bibliothek
4) Öffne das `ArmWriter` Projekt und installiere `RF24` und `Servo` Bibliotheken
5) Anschließend kannst du beide Projekte kompilieren lassen und prüfen, ob alles geklappt hat

## Aufbau
Siehe [BuildTutorial.md](BuildTutorial.md)

## Architektur
Das gesamte Projekt besteht aus zwei Subsystemen:
- `ArmReader` - liest die Lage vom menschlichen Arm und sendet diese an den `ArmWriter`
- `ArmWriter` - übernimmt die Lage des menschlichen Arms und steuert den Roboterarm
 
Diese kommunizieren mittels eines Funkkanals, wobei `ArmReader` als Sender und `ArmWriter` als Empfänger funktionieren. Das einzige zu übertragende Objekt ist ein Array mit gemessenen Winkeln und dem Signal vom Joystick.
Im folgendem PAP sind die grundsätzliche Aktivitäten jeweiliges Systems vereinfacht abgebildet:

<div align="center">
    <img src="Resources\PAP.png" width=70% alt-text="Architektur und Programmablaufplan"/>
</div>

## Code
Die zwei zentrale Codedateien sind die jeweiligen `main.cpp` Dateien in folgenden Ordnern:
- `RoboArm/ArmReader/src/main.cpp`
- `RoboArm/ArmWriter/src/main.cpp`

Beide Dateien sind ähnlich aufgebaut und strukturiert:
```
[*]-[Initialisierung]
[|]-> // Import von Bibliotheken
[|]-> // Definition von Variablen
[|]-> // Deklaration von Prozeduren und Funktionen
 |
[*]-[Hauptteil]
[|]-> // void setup() Prozedur
[|]-> // void loop() Prozedur
 |
[*]-[Nebenteil]
[|]-> // Definition von weiteren notwendigen Prozeduren und Funktionen
 |
[*]-[Andere Prozeduren]
[|]-> Ausgabe Prozeduren zum Testen / Debuggen
```

Die meisten Stellen wurden ziemlich umfangreich kommentiert. Zum tieferen Einblick auf theoretischen Grundlagen siehe den folgenden Abschnitt.

## Theorie
In diesem Abschnitt werden unterschiedliche theoretische Kenntnisse aufgeschrieben, die ich während des Projekts kennengelernt habe.

### I2C-Protokoll

### Regelungstechnische Grundlagen

### Radiokommunikation

