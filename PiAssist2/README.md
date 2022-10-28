
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

- Gestion du capteur de porte RF433 Chacon DIO  </br>
chacon-dio \<sender> \<receptor> \<state> </br>

## Commandes
### Prises Chacon Dio
Pattern
```
cc1101/chacon/IDGROUPE/IDPRISE/1/
```
Exemple : allumer prise 2 du groupe 14549858
```
cc1101/chacon/14549858/2/1/ 
```
### Thermostat
**SET**

Changer consigne
```
nrf24/node/2Nodw/ther/set/cons/19/
```
Changer delta
```
nrf24/node/2Nodw/ther/set/delta/0.7/
```
Changer id Planning
```
nrf24/node/2Nodw/ther/set/plan/1/
```
Activer ou desactiver chaudière 
```
nrf24/node/2Nodw/ther/set/pwr/1/
nrf24/node/2Nodw/ther/set/pwr/0/
```

**GET**

Recupérer infos 
```
nrf24/node/2Nodw/ther/get/info/
```
Recupérer mode
```
nrf24/node/2Nodw/ther/get/mode/1/
```
Recupérer etat power
```
nrf24/node/2Nodw/ther/get/pwr/
```
Recupérer planning day
```
nrf24/node/2Nodw/ther/get/plan/1/
```
Recupérer RTC
```
nrf24/node/2Nodw/ther/get/rtc/
```
**SEL**
Sélectionner mode (de 1 à 4)
```
nrf24/node/2Nodw/ther/sel/mode/1/
```
**PUT**
Modifier un mode (de 1 à 4)
```
nrf24/node/2Nodw/ther/put/mode/4/23.5/0.7/
```
Modifier heure de la RTC
```
nrf24/node/2Nodw/ther/put/rtc/6/2018/02/15/15/34/22/
```
Modifier un jour de planning
```
nrf24/node/2Nodw/ther/put/plan/2/3001/4802/6004/10201/13802/14404/
```
ici '3001' représente minute '300' et mode '1', 13802 représente minute '1380' et mode '2'
