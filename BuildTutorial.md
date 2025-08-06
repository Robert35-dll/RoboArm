# Aufbaututorial
In dieser Anleitung findest du meine Hinweise dazu, wie man einen Roboterarm aufbauen kann. Ich versichere nicht, dass es keine Widersprüche oder Ungenauigkeiten geben darf! Wenn etwas nicht funktioniert, sei nicht deprimiert und versuche selber herauszufinden, wo der Fehler liegt (_ich habe es selber so gemacht_ :)

Für den Roboterarm sind zunächst keine Zeichnungen vorhanden, weil ich meinen nur mit sehr grober Planung gebastelt habe. Zur Inspiration siehe [Fotos](#fotos) ganz am Ende von diesem Tutorial.

Dieser Tutorial erfordert **nicht**, dass du die [Installationsanleitungen](README.md#installation) bereits gefolgt hast.

## Gliederung
- [Allgemeine Hinweise](#allgemeine-hinweise)
- [ArmReader](#das-brauchst-du-für-deinen-eigenen-arm)
- [ArmWriter](#das-brauchst-du-für-deinen-roboterarm)
- [Fotos](#fotos)

## Allgemeine Hinweise
Bevor du etwas einschaltest, stelle sicher, dass:
1) Keine Kabeln kurz angeschlossen sind (_direkt + und - von einem Bauteil nicht verbinden!_)
2) Alle Kabeln angeschlossen sind
3) Spannungsquellen genau ausreichend sind

Während du etwas baust, stelle sicher, dass:
1) Du nicht verletzt wirst (_Handschuhe nicht vergessen!_)
2) Niemand anderer verletzt wird
3) _Optional: Nichts auf deinem Arbeitsplatz beschädigt wird (inklusiv Arbeitsplatz selbst)_

## Das brauchst du für deinen eigenen Arm
|Bauteil|Anzahl|Hinweis|
|:-----:|:----:|:-----:|
| Spannungsquelle 9V                | x1 | Akkus oder Batterien |
| Adapter für die Spannungsquelle   | x1 | Optional aber sehr empfehlenswert |
| Arduino Nano                      | x1 |
| MPU6050 Gyroskop                  | x2 |
| nRF24 Funkmodul                   | x1 |
| Joystick                          | x1 | Alle funktionieren gleich, benennen nur die Achsen anders ( -_-) |
| (Jumper-)Kabeln                   | viel | Jumperkabeln eignen sich gut für einen (ersten) Testaufbau |

Als Spannungsquelle kannst du eine oder mehrere Batterien verwenden. Ich empfehle aber, Akkus zu nutzen. Diese kannst du entweder direkt an das Arduino anschließen oder (_sehr empfohlen_) durch ein Adapter.

Die o.g. Komponenten verbinde wie auf dem folgenden Plan dargestellt:
<div align="center">
    <img src="Resources\Breadboard_Nano.png" width=70% alt-text="Steckbrett ArmReader"/>
</div>

## Das brauchst du für deinen Roboterarm
|Bauteil|Anzahl|Hinweis|
|:-----:|:----:|:-----:|
| Spannungsquelle 9V                | x2 | Akkus oder Batterien |
| Adapter für die Spannungsquelle   | x1 | Optional aber sehr empfehlenswert |
| Arduino Uno                       | x1 |
| nRF24 Funkmodul                   | x1 |
| Servomotor                        | x5 | Es gibt Servos von unterschiedlichen Stärken. Nimm beliebigen |
| (Jumper-)Kabeln                   | viel | Jumperkabeln eignen sich gut für einen (ersten) Testaufbau |

Als Spannungsquelle kannst du eine oder mehrere Batterien verwenden. Ich empfehle aber, Akkus zu nutzen. Diese kannst du entweder direkt an das Arduino anschließen oder (_sehr empfohlen_) durch ein Adapter.

Aktuell sind Servos nur für die Schulter, Ellbogen und Unterarm gedacht. Optional kannst du sogar überlegen, Griffgelenke mit weiteren Servos einzubauen.

Je nach den ausgewählten Servos wird dein Roboterarm entweder nur seine Hand hochheben, oder sogar irgendwelche Gegenstände halten können.

Die o.g. Komponenten verbinde wie auf dem folgenden Plan dargestellt:
<div align="center">
    <img src="Resources\Breadboard_Uno.png" width=70% alt-text="Steckbrett ArmWriter"/>
</div>

## Fotos
Hier sind die Fotos von meinem Roboterarm mit zwei zusätzlichen Servos für Griffgelenke:

<div align=center>
    <img src="Resources\Build_ArmWriter_Back.png" height=600 width=50% alt-text="Aufgebauter Roboterarm - Hinten"/><img src="Resources\Build_ArmWriter_Front.png" height=600 width=50% alt-text="Aufgebauter Roboterarm - Vorne"/>
</div>