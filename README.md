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
I2C steht für **Inter-Integrated Circuit** und wird hauptsächlich benutzt, um mehrere Peripheriegeräte mit wenigen Kabeln an einen Mikrocontroller anzuschließen. Dies wird durch zwei zentrale "Kommunikationsbände" ermöglicht:
- **Serial Data (_SDA_)** - für die tatsächliche Datenübertragung
- **Serial Clock (_SCL_)** - für die Synchronisation dieser Datenübertragung zwischen Sendern und Empfängern

sowie die Angabe von **einzigartigen IDs** für jedes Gerät.

_Wichtige Bemerkung: Falls zwei Geräten das gleiche ID von Herstellern gegeben wurden, muss ein davon vor der Initialisierung der Kommunikation geändert werden! Mehr dazu im Abschnitt von [Wire Bibliothek](#wire-bibliothek)._

Die entsprechende Pins gibt es auf allen Arduino Boards. Arduino GIGA hat sogar mehrere SDA / SCL Pins.

Mehr zum Thema in der [Arduino I2C Dokumentation](https://docs.arduino.cc/learn/communication/wire/#what-is-i2c).

#### Wire Bibliothek
Arduino hat die **Wire Bibliothek** vorbereitet, die manche technische grundlegende Operationen übernimmt. Von gängigen Methoden sind:
- `begin() / end()` - um einen I2C Bus zu initialisieren / auflösen
- `begin- / endTransmission()` - um einzelne Datenübertragung zu starten / beenden
- `write()` - um Daten auf den Bus zu schreiben oder reserviert Bytes für die nächste Übertragung
- `read()` - um den nächsten Byte von Daten aus dem Bus zu lesen

Die originale [Dokumentation von Arduino](https://docs.arduino.cc/language-reference/en/functions/communication/wire/?_gl=1*ss306l*_up*MQ..*_ga*MjA4NzcxNjI3Ni4xNzU0MTQ4ODMy*_ga_NEXN8H46L5*czE3NTQxNDg4MzEkbzEkZzAkdDE3NTQxNDg4MzEkajYwJGwwJGg0MDg1ODI0MTk.#functions) wird einem Anfänger nicht viel bei, deswegen empfehle ich sehr, selbständig Beispiele zu finden und daraus zu lernen :)

### Regelungstechnische Grundlagen

### Radiokommunikation

