# Mechatronik Projekt: Roboterarm

## Hintergrund
Dies ist ein Repository für mein Mechatronik Projekt im 4. Semester meines Ingeneurinformatik Studiums. Die Grundidee ist, _Kommunikation zwischen Mikrocontrollern_ kennenzulernen, also, zwei Mikrocontroller miteinander (_bestenfalls kabellos_) zu verbinden und damit irgendwelche Aktionen auszuführen.

Konkret für dieses Projekt habe ich entschieden, mit meinem eigenen Arm einen Roboterarm zu steuern. Den genauen Aufbau von einem Roboterarm belasse ich zunächst außer Betracht, weil er ziemlich aufwändig sein soll, um mehr als nur Ellbogen biegen zu können :)

## Gliederung
- [Installation](#installation)
- [Aufbau](#aufbau)
- [Architektur](#architektur)

## Installation
Dieses Projekt wurde im VS Code mithilfe PlatformIO Extension entwickelt. Alternativ kannst du auch in nativem ArduinoIDE arbeiten. Um den Code auf dein PC zu haben tuh bitte folgendes:

1) Klone dieses Repository auf dein PC mit `git clone https://github.com/Robert35-dll/RoboArm.git`
2) Öffne den heruntergeladenen Ordner in deinem IDE.
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

Wenn dein PlatformIO noch keine Teilprojekte erkannt hat, musst du ihm selber die Pfade von `ArmReader` und `ArmWriter` Ordnern (_nicht von main.cpp Dateien_) zeigen. Wenn beide Teilprojekte erkannt werden, installiere die notwendige Bibliotheken:

3) Öffne das `ArmReader` Projekt und installiere `RF24` Bibliothek
4) Öffne das `ArmWriter` Projekt und installiere `RF24` und `Servo` Bibliotheken
5) Anschließend kannst du beide Projekte kompilieren lassen und prüfen, ob alles geklappt hat

## Aufbau
### Das brauchst du für deinen eigenen Arm
|Bauteil|Anzahl|
|:-----:|:----:|
| Arduino Nano | x1 |
| PU6050 Gyroskop | x2 |
| nRF24 Radiomodul | x1 |
| Joystick | x1 |
| (Jumper-)Kabeln | viel |

_Hinweis: Jumperkabeln eignen sich gut für einen (ersten) Testaufbau._

### Das brauchst du für deinen Roboterarm
Kommt demnächst :3

## Architektur
Das gesamte Projekt besteht aus zwei Subsystemen: `ArmReader` und `ArmWriter`. Diese kommunizieren mittels eines Radiokanals, wobei `ArmReader` als Sender und `ArmWriter` als Empfänger funktionieren. Das einzige zu übertragende Objekt ist ein Array mit Winkeln und 

## Code


## Theorie
In diesem Abschnitt werden unterschiedliche theoretische Kenntnisse aufgeschrieben, die ich während des Projekts kennengelernt habe.

### Regelungstechnische Grundlagen

### Radiokommunikation

