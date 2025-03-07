---
coverY: 0
---

# Module 3 : Programmation

Le **code** se trouve sur : [https://github.com/CedricCourson/LittObs](https://github.com/CedricCourson/LittObs)﻿﻿\


> **Etapes de construction :**
>
> 1. Installer le driver CH340/CH341
> 2. Installer Arduino IDE
> 3. Installer la carte ESP32 Firebeetle
> 4. Installer les librairies

**1. Installer le driver CH340 ou CH341**

* Si vous utilisez un Mac ou un PC sous windows, suivez le lien suivant et suivez les instructions : ﻿https://docs.wemos.cc/en/latest/ch340\_driver.html
* Si vous uti﻿lisez Linux, le module est déjà installé (si vous rencontrez des problèmes, vérifiez avec dmesg qu'il n'y a pas un autre driver qui cause des incompatibilités comme brltty)

﻿\


**2. Installer Arduino**

* Télécharger le logiciel Arduino : [﻿https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)﻿
* Suivre les instructions ﻿d'installation [https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing)﻿

﻿\


**3. Installer la carte ESP32 Firebeetle**

* dans le menu Files > Preferences, ajouter ﻿l'URL suivant dans "Additional board manager" et cliquer sur OK : [http://download.dfrobot.top/FireBeetle/package\_esp32\_index.json](http://download.dfrobot.top/FireBeetle/package_esp32_index.json)﻿

﻿\
![](https://wikifactory.com/files/RmlsZTo5OTc2NDE=)﻿\


* Dans le menu Tools > Board > Boards manager : chercher ESP32
* Installer "esp32 by Espressif Systems" library

﻿\


**4. Installer les librairies**

* Télécharger le zip du github SensOcean où se trouve un dossier "Librairies" contenant toutes les librairies necéssaire : [https://github.com/astrolabe-expeditions/SensOcean](https://github.com/astrolabe-expeditions/SensOcean)﻿
* Placer les fichier sur votre ordinaeur, dans Document, créer un dossier Arduino et déposez y les librairies
* Vérifier que tout marche bien :
* brancher votre carte ESP32 à l'ordinateur
* sélectionner ﻿FireBeetle-ESP32 pour le port de communication (COM...) sur lequel vous l'avez branché
* ![](https://wikifactory.com/files/RmlsZTo5OTc2NDQ=)
* Coller le code suivant dans la fenêtre de travail et téléversez le sur l'ESP32 :

> int led = 2;﻿\
> void setup() {// put your setup code here, to run once:Serial.begin(115200);Serial.println("Coucou ! ");pinMode(led, OUTPUT);digitalWrite(led, LOW);}﻿\
> void loop() {Serial.println("Start boucle");﻿\
> for (int i=0; i<15; i++){
>
> ```
> digitalWrite\(led, HIGH\);
> ```
>
> ```
> ﻿
> ```
>
> ```
> delay\(200\);
> ```
>
> ```
> ﻿
> ```
>
> ```
> digitalWrite\(led, LOW\);
> ```
>
> ```
> ﻿
> ```
>
> ```
> delay\(200\);
> ```
>
> }﻿\
> delay(1000);// put your main code here, to run repeatedly:﻿\
> }

﻿\
\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_

## Test

### 1. Hardware

**Vérifier les soudures**

* Vérifier à l'aide d'un multimètre que les soudures ne se touchent pas (notamment les transistors qui ont des pattes très rapprochées

**Vérifier le bon fonctionnement du module EZO (petit module vert sur lequel on vient connecter la sonde de conductimétrie)**

* Mettre la carte sous tension, et vérifier que la petit LED du module EZO (module auquel on relie la sonde) clignote﻿. Si ce n'est pas le cas,
  * Vérifier qu'il y a bien une tension de 5V entre la borne VCC et la borne GND du module violet. Si ce n'est pas le cas, c'est probablement le voltage booster qui n'est pas bon (le petit module munie d'une bobine se trouvant sous la carte ESP). Une solution de secours consiste à soit alimenter directement le module violet avec la sortie 3.3V de la carte ESP, soit à faire un court circuit entre la première et la troisième patte du module voltage booster.

**VÉRIFIER QUE LE MODULE DE CONDUCTIMÉTRIE EZO EST BIEN CONFIGURÉ**

* Par défaut, le capteur communique en UART. Notre programme utilise le protocole I2c pour communiquer avec. Vous savez qu'il est en UART si le module vert clignote en vert puis bleu clair. Il clignote seulement en bleu foncé s'il est en I2c.
* Pour procéder en changement, munissez vous d'une alimentation stabilisée 5V (une Arduino ou une alimentation de laboratoire), et de 3 fils de connection.
* Détachez le module vert du module violet et branchez-le de la sorte :

﻿\
![](https://wikifactory.com/files/RmlsZToxMDc3OTYw)﻿\


* Le LED s'allume en bleu, c'est bon !
* Remettez le module vert sur le module violet (dans le bon sens !)

Lien de la doc : [https://files.atlas-scientific.com/EC\_EZO\_Datasheet.pdf](https://files.atlas-scientific.com/EC_EZO_Datasheet.pdf)

﻿

### 2. Software



**1. Télécharger le PROJET**

* Télécharger le dossier du projet sur github : [https://github.com/CedricCourson/LittObs](https://github.com/CedricCourson/LittObs)﻿
* Dézipper le dossier

﻿\


**2. Configurer la carte SD**

Si la sonde n'a pas encore de carte SD

* Prendre une carte microSD ﻿de 8Go
* L'ouvrir sur un PC
* La formater en FAT32

﻿\


**3. Configurer la sonde**

* Copier coller le fichier ﻿`config_MODELE.txt` sur la carte SD
* Renomer le fichier en `config.txt`
* Les première lignes expliquent chaque paramètre. Une fois que vous en avez pris connaissance, supprimer ces lignes explicatives. Il ne reste plus que :

```
id_logger=OSOCTD022;
```

```
delay_batch=5;
```

```
number_measures=3;
```

```
debug_mode=1;
```

```
led_mode=1;
```

* Changer le ﻿paramètre OSOCTD022 pour qu'il corresponde au nom de la sonde (par exemple : AELO007)
* Laisser les autres paramètres tels quel﻿s

﻿\


**4. Vérifier que l'ESP communique**

* Ouvrir le fichier Module\_CTD.ino avec l'IDE Arduino
* Connecter le ESP32 en USB et sélectionner la carte FireBeetle ESP32 pour le bon port COM
* Téléverser le code sur la carte
* L'ESP est censé renvoyer quelquechose comme :

```
﻿ets Jun 8 2016 00:22:57
```

```
rst:0x1 (POWERON_RESET),boot:0x1b (SPI_FAST_FLASH_BOOT)
```

```
flash read err, 1000
```

```
ets_main.c 371
```

```
ets Jun 8 2016 00:22:57
```

```
rst:0x10 (RTCWDT_RTC_RESET),boot:0x1b (SPI_FAST_FLASH_BOOT)
```

```
configsip: 0, SPIWP:0xee
```

```
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
```

```
mode:DIO, clock div:1
```

```
load:0x3fff0018,len:4
```

```
load:0x3fff001c,len:1044
```

```
load:0x40078000,len:8896
```

```
load:0x40080400,len:5816
```

```
entry 0x400806ac
```

```
Error: Unable to communicate with BQ27441.
```

```
Check wiring and try again.
```

```
(Battery must be plugged into Battery Babysitter!)
```

```
Init failed!
```

```
Are SDA/SCL connected correctly?
```

```
Blue Robotics Bar30: White=SDA, Green=SCL
```

**5. Vérifier que le module de conductimétrie EZO est bien configuré**

* Par défaut, le capteur communique en UART. Notre programme utilise le protocole I2c pour communiquer avec. Vous savez qu'il est en UART si le module vert clignote en vert puis bleu clair. Il clignote seulement en bleu foncé s'il est en I2c.
* Pour procéder en changement, munissez vous d'une alimentation stabilisée 5V (une Arduino ou une alimentation de laboratoire), et de 3 fils de connection.
* Détachez le module vert du module violet et branchez-le de la sorte :

![](https://wikifactory.com/files/RmlsZToxMDc3ODA4)

* Le LED s'allume en bleu, c'est bon !
* Remettez le module vert sur le module violet (dans le bon sens !)

Lien de la doc : [https://files.atlas-scientific.com/EC\_EZO\_Datasheet.pdf](https://files.atlas-scientific.com/EC_EZO_Datasheet.pdf)﻿﻿\


**6. Vérifier que les capteurs communiquent avec l'ESP**

...﻿\


**7. Vérifier que l'alimentation sur batterie fonctionne**

...﻿\


**8. Tester le WIFI**

* Mettre les fichiers du dossier Server\_Files sur la carte SD﻿ (attention, bien les mettre à la racine, pas dans des dossiers)
* Ouvrir et téléverser le fichier ﻿`Module_CTD_Wifi.ino` avec l'IDE Arduino

#### Interface de gestion de l'instrument

L'instrument est doté d’une interface web permettant de lire et récupérer le fichier de données et le fichiers de configurations. Pour y accéder il suffit de placer un aimant devant l’interrupteur ILS au moment de l’allumage.﻿\
Dans ce cas l’ESP32 démarre en mode serveur Web, et peut y accéder simplement via wifi depuis n’importe quel appareil (ordi, smartphone, tablette…).﻿\
L’ESP32 démarre un serveur wifi au SSID (nom) : _LittObs-Instrument_﻿\
Ensuite il suffit d’ouvrir un navigateur web et d’entrer l’adresse : 192.168.1.4﻿\
On accéder alors à une page web permettant de gérer l’instrument.﻿\
**Menu principal**﻿\
Page web de l’interface (download data file et config file et accès au menu)![](https://wikifactory.com/files/RmlsZTo5MDMwODE=)﻿\
﻿\
﻿\
**Données**﻿\
Page de gestion des données permettant actuellement de lire directement les données, télécharger, ou tout effacer.![](https://wikifactory.com/files/RmlsZTo5MDMwODI=)﻿\
﻿\
﻿\
﻿\
**Configuration de l’instrument**﻿\
Permet actuellement de synchroniser l’horloge RTC avec l’ordinateur (l’objectif : étalonner les capteurs et réglage instrument).![](https://wikifactory.com/files/RmlsZTo5MDMwODM=)

#### Visualisation et téléchargement des données
