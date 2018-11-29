# Capteur intelligent de gaz monté sur une Arduino Uno
Dans le cadre du cours "Microcontrolleurs et open-source hardware" de 5ème année ISS (Innovative Smart System) à l'INSA de Toulouse, nous avons réalisé un shield Arduino incluant une carte Lora afin de faire transiter les informations issues de divers capteurs sur ledit réseau bas débit. Le projet se décompose en plusieurs parties :
* Création du shield Arduino Uno sous KiCad incluant :
  - la puce Lora RN2483 et son antenne associée
  - le capteur de gaz MQ2 avec connectique GROVE
  - un capteur de gaz "maison" et son étage d'amplification
  - deux LEDs
* Développement d'un programme sous l'IDE Arduino permettant de récupérer les informations issues de divers capteurs branchés sur l'Arduino et les transmettre sur le réseau basse consommation Lora nommé TheThingsNetwork.
* Création d'une interface web affichant les informations issues du capteur de gaz sous la forme d'un graphe, grâce à node-red.

# Prérequis
* Software
	- Arduino IDE avec les bibliothéques SoftwareSerial et TheThingsNetwork
	- Un compte TheThingsNetwork
	- Node-red
* Hardware
	- Un Arduino Uno
	- Un capteur de gaz MQ2 avec connecteur GROVE
	- Deux LEDs
	- Une puce Lora RN2483

# Création d'un shield pour Arduino Uno
Vous pouvez trouver les sources KiCad dans le dossier [ShieldArduinoKiCad](/ShieldArduinoKicad/).
Vous trouverez ci-dessous une vue d'ensemble de la schématique, du routage et du rendu 3D de notre shield.
![Schématique du shield Arduino](https://i.imgur.com/loexOIT.png)
![Routage du shield Arduino - Face 1](https://i.imgur.com/1Du8ad6.png)
![Routage du shield Arduino - Face 2](https://i.imgur.com/NUFL8C6.png)
![Rendu 3D du shield Arduino](https://i.imgur.com/h4zXJdw.png)

# Développement du software sur Arduino Uno
Le code est distribué dans le dossier [arduino](/arduino/).
Concrétement, il implémente :
* La connexion de la carte Lora au réseau TheThingsNetwork
* La mise en place d'un mode uplink (communication Arduino --> theThingsNetwork via Lora afin d'envoyer les informations issues des capteurs)
* La mise en place d'un mode downlink (communication theThingsNetwork --> Arduino via Lora permettant de commander des LEDs via une interface web). Il est intéressant de noter que la réception d'un message sur l'arduino (message downlink) se fait uniquement après l'envoi d'un message uplink, car la puce RN2483 est un device Lora de classe A. C'est pourquoi il est obligatoire d'envoyer périodiquement une trame (manuellement à l'aide d'un *sendBytes* ou automatiquement à l'aide de la fonction *ttn.poll()*)
* La mise en place de deux LEDs, l'une indiquant un niveau de gaz trop élevé dans la pièce, l'autre permettant de tester le mode downlink

# Développement d'une interface graphique de gestion de données grâce à node-red
Node-red est un outil graphique permettant de lier plusieurs services et APIs ensemble de manière assez facile. Ainsi, nous avons créé une interface permettant d'afficher les informations issues du capteur de gaz sous forme de gauge de danger. De plus, nous avons la possibilité d'allumer, d'éteindre ou de faire clignoter la LED sur notre Arduino, à distance grâce à node-red ou l'interface theThingsNetwork. Vous trouverez ci-dessous l'interface node-red créé pour le projet ainsi que le rendu graphique final.
![Interface de programmation de node-red](https://i.imgur.com/brm5BxF.png)
![Rendu graphique final](https://i.imgur.com/hErL446.png)

# FAQ software
## La connexion à TheThingsNetwork ne fonctionne pas
* Réinitialiser l'Arduino à l'aide du bouton RESET, ainsi que la carte Lora en branchant la pin RST du RN2483 sur la masse de l'arduino pendant quelques secondes
* Vérifier qu'une gateway Lora est bien présente aux alentours et que le signal est suffisamment fort pour permettre la connexion

## La connexion de node-red à TheThingsNetwork est impossible
* Vérifier que votre routeur internet ne bloque pas la connexion

## Le mode uplink ne fonctionne pas
* Vérifier que la connexion avec TheThingsNetwork est bien réalisée à l'aide de la console série Arduino
* Cliquer sur "RESET FRAME COUNTERS" sur la page de votre device sur TheThingsNetwork

## Le mode downlink ne fonctionne pas
* Vérifier que la connexion avec TheThingsNetwork est bien réalisée à l'aide de la console série Arduino
* Vérifier que votre code envoie périodiquement des trames Lora afin de permettre les messages downlink
