
------------------------------------------------------------------------------------------------------------------------
Programme d'analyse d'image via python et skimage pour detection des lettres U,H et S.
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------

Instalation sur le RaspBerry Pi :

$ sudo apt-get update\
$ sudo apt-get install python-picamera python3-picamera\
$ sudo apt-get install python3-skimage\
$ sudo apt install python3-gpiozero

------------------------------------------------------------------------------------------------------------------------

Demarrage du programme :

Telecharger le Repository avec : $ sudo git clone https://www.gitgub.com/leosamuel64/Maze \
Acceder au dossier Letter_Detection et lancer le programme start.py avec : $ sudo python3 start.py

------------------------------------------------------------------------------------------------------------------------

Connections electroniques :

Les signaux des lettres sont sur les GPIO :\
S -> 14\
U -> 18\
H -> 15\
Rien -> 24

