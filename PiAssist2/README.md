
# PiAssist2
## Description

A brancher en USB sur le Raspberry Pi de la box domotique DIY DomusBox
Sert à gérer la RF pour le projet de box domotique DIY DomusBox

- Gestion du CC1101
    libs : ELECHOUSE_cc1101->h  (+ libs perso : ChaconDIO.h RF433Sensors.h) </br>
    source :  https://github.com/simonmonk/CC1101_arduino </br>

- Gestion des sondes de température RF433 </br>
    libs : VirtualWire.h </br>
    source : http://www.airspayce.com/mikem/arduino/VirtualWire/VirtualWire-1.27.zip

    - Gestion des sondes de température RF24l01 </br>
    libs : nRF24L01.h RF24.h et RF24_config.h </br>
    source : http://tmrh20.github.io/RF24 </br>

- Gestion des prises RF433 Chacon DIO </br>
    libs : adaptée du fichier radioEmission.cpp dans le zip 'hcc.zip' de idleman : </br>
    source : http://idleman.fr/ressources/PHP/hcc/hcc.zip </br>
    cc1101/chacon/14549858/2/1/ </br>
    cc1101/chacon/14549858/2/0/ </br>

- Gestion du capteur de porte RF433 Chacon DIO
chacon-dio \<sender> \<receptor> \<state> </br>
