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
  - [Accelerometer vs Gyroskop](#accelerometer-vs-gyroskop)
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
Um die gemessene Winkeln zu korrigieren, wurde ein Komplementär Filter implementiert, für den drei verschiedene Techniken eingesetzt wurden:
1) Dynamische Alpha-Korrektur - um Einflüsse von [Accelerometer und Gyroskop](#accelerometer-vs-gyroskop) bei der Winkelberechnung abhängig von der Amplitude dynamisch anzupassen:
```C++
  Alpha = abs(AccMagnitude - 1.0) > 0.1 ? 0.02 : 0.045;
```
2) High-Pass-Filter - um rasante Lageänderungen zu dämpfen:
```C++
  GyroCorrection = (PreviousAngle - AccelerometerAngle) * 0.8;
```
3) Bias-Korrektur (Low-Pass-Filter) - um langfristige Drifts zu vermindern:
```C++
  GyroBias += GyroAngle * 0.0001;
```

Letztendlich sind alle drei Teilberechnungen in der Formel für Komplementär Filter benutzt worden:
```C++
  CorrectedAngle = Alpha * (PreviousAngle + (GyroAngle - GyroBias) * milliseconds)
                 + (1 - Alpha) * (AccelerometerAngle + GyroCorrection);
```

Mehr zum Thema in [meinem Chat mit Copilot](https://copilot.microsoft.com/shares/UibQJuqRkmj2sEJwEYquv).

### Accelerometer vs Gyroskop
Die MPU6050 Module verfügen gleichzeitig über einen Accelerometer und einen Gyroskop die ebenfalls gleichzeitig zur Messung von Winkeln und daher Bestimmung der Lage verwendet werden können.
Der Unterschied zwischen den beiden Mechanismen kann _grob vereinfacht_ so interpretiert werden:
- **Accelerometer** - bestimmt **die Winkeln** mit Referenz zur Erdbeschleunigung
- **Gyroskop** - bestimmt **die Winkelgeschwindigkeiten** mit Referenz zur letzten stabilen Lage

Daher macht es meistens Sinn, beide Geräte gleichzeitig zu verwenden, um die Messgenauigkeit zu erhöhen.

Mehr zum Thema in [diesem Artikel](https://lastminuteengineers.com/mpu6050-accel-gyro-arduino-tutorial/#:~:text=The%20MPU6050%20module%E2%80%99s%20pinout%20is%20as%20follows%3A%20VCC,interface.%20XDA%20is%20the%20external%20I2C%20data%20line.) von Last Minute Engineers.

### Radiokommunikation
Für eine Funkverbindung reichen gut die nRF24L01 Module mit eigener `RF24` Bibliothek aus. Diese werden per SPI Protokoll mit weiteren "Kommunikationsbändern" an den Mikrocontroller angeschlossen:
- **Master In - Slave Out (MISO)** - für die Datenübertragung von Peripheriegeräten (_ggf. nRF24_) zum Controller
- **Master Out - Slave In (MOSI)** - für die Datenübertragung vom Controller zu den Peripheriegeräten (_ggf. nRF24_)
- **Serial Clock (SCL)** - für die Synchronisation dieser Datenübertragung zwischen Sendern und Empfängern (_das gleiche wie bei [I2C Protokoll](#i2c-protokoll)_)
- **Chip Select (CS / CSN)** - für den Auswahl eines Peripheriegeräts (_ggf. nur eines nRF24_) zur nächsten Kommunikation
- **Chip Enable (CE)** - für die Aktivierung eines Peripheriegeräts (_ggf. nRF24_)

Mit der `nRF24` Bibliothek kann ein Funkkanal (_Datenrate, Kanal, Puffergröße, etc._) genau eingestellt werden. Hier sind paar Hinweise zur Anwendung:
- Die Einstellungen sollten einzigartig für den Anwendungsraum sein, um die Störungen durch andere Funkgeräte zu vermeiden.
- Alle Daten (_bis zu 32 Bytes standardmäßig_) werden mit der ´write()´ Methode byteweise ohne implizite Umwandlung geschickt. Die überschüssige Bytes werden beim Senden ignoriert!

Mehr zum nRF24L01 Modul in [diesem Tutorial](https://wolles-elektronikkiste.de/nrf24l01-2-4-ghz-funkmodule) vom Wolles Elektronikkiste.

Mehr zum Thema SPI in [diesem Artikel](https://www.analog.com/en/resources/analog-dialogue/articles/introduction-to-spi-interface.html) von Analog Dialogue.
