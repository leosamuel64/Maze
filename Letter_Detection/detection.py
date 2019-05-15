#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Created on Oct 2018
@author: Leo SAMUEL
"""
##Biblio

from picamera import PiCamera # Controle de la camera raspi
from gpiozero import LED # Communication avec l'arduino via les ports GPIO
import skimage  # Analyse d'image
from skimage import data, io
import math
import os

#Setup

camera = PiCamera() # création de la camera
camera.rotation = 180 # met l'image dans le bon sens car caméra monté à l'envers
# On initialise les GPIO
S=LED(14)
H=LED(15)
U=LED(18)
X=LED(24)
# On met les GPIO au niveau bas : 0
S.off()
H.off()
U.off()
X.off()


# recuperation des infos de bases

camera.capture('img.jpg') # On prend une photo
u=os.system('convert img.jpg -resize 150x150 -threshold 40%% conv.bmp') # on redimensionne en 150x150 et on applique un threshold de 40%
imageSource = io.imread('conv.bmp') # on charge l'image dans une variable
formatImage=imageSource.shape # on recupere dans une liste les dimensions de l'image...
xmax=formatImage[1] # en 1, la dimension en x (axe horizontal)...
ymax=formatImage[0] # en 0, la dimension en y (axe vertical)
xmax=xmax-5 # on reduit pour éviter de sortir du cadre
ymax=ymax-5 # Idem

# Definition des variables à l'état 0
xhaut=0
yhaut=0
xbas=0
ybas=0
presence=1
quantity=0
out=0

#anti sortie du cadre par la gauche et la droite
x=1
y=1
point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y) dans une liste
##Test si il y a des pixels noirs sur la bordure gauche avec un balayage vertical
while y<ymax and (point[0]==255) and (point[1]==255) and (point[2]==255) : # Pixel blanc
    point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
    y=y+1 # incrémentation de 1
if (point[0]==0) and (point[1]==0) and (point[2]==0): # pixel noir
    out = 1 #la lettre est en dehors du cadre
 
x=xmax
y=1
point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
while y<ymax and (point[0]==255) and (point[1]==255) and (point[2]==255) : # pixel noir
    point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
    y=y+1
if (point[0]==0) and (point[1]==0) and (point[2]==0): ##Test si il y a des pixels noirs sur la bordure droite
    out = 1 # la lettre est en dehors du cadre

if out ==1: # la lettre est en dehors du cadre
    X.on() # communication arduino
   

##Scan du haut
y=1
x=1
point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
while (point[0]==255) and (point[1]==255) and (point[2]==255)  and x < xmax and y < ymax and out==0: # on balaye jusqu'à trouver un pixel noir
    point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
    if x == xmax-1: # Si la fin de la ligne est atteint...
        x=1 #on revient au debut de la ligne
        y=y+1 # et on passe à la suivante
    x=x+1 # balayage horizontal
    
# on est hors de la boucle while, reste a trouver pourquoi on est en dehors
    
if (point[0]==0) and (point[1]==0) and (point[2]==0)  and x < xmax and y < ymax and out==0: # Si le pixel est noir
    xhaut = x # on stock les coordonnées du haut de la lettre
    yhaut = y # Idem

   
# Scan du bas
x=1
y=ymax-1
point=imageSource[y,x] # on recupere la valeur en RGB du pixel de coordonées (x;y)
while (point[0]==255) and (point[1]==255) and (point[2]==255)  and x < xmax and y > 1 and out==0:
    point=imageSource[y,x]
    if x == xmax-1:
        x=1
        y=y-1                # Comme pour le haut mais on remonte les lignes
    x=x+1
if (point[0]==0) and (point[1]==0) and (point[2]==0)  and x < xmax and y > 1 and out==0: # pixel noir
    xbas = x # on stock les coordonnées du haut de la lettre
    ybas = y
else :   # Sinon on communique à l'arduino qu'il ny a pas de lettre
    X.on()
    presence=0

    
## calcul de difference pour corrigé l'inclinaison de la lettre
deltax = xbas - xhaut  #Calcul de la difference de coordonées en x entre le haut et le bas
if deltax < 0: #Si negatif, passage en positif 
    deltax=-deltax

  
pourcentage= (deltax/xmax)*100  # calcul de l'écart relatif entre xbas et xhaut
if (pourcentage < 5 or pourcentage > 25)  and presence == 1 and out==0: # Si le resultat est petit ou tres petit (S ou H sur Branche 1 et 1 ou H sur branche 1 et 2
 # S ou H
    y=yhaut
    deltay = yhaut-ybas # calcul de la difference en y)
    deltay=abs(deltay) # valeur absolue
    while y != ybas:    #On compte le nombre de pixel noir sur la lettre
        point=imageSource[y,x]
        if (point[0]==0) and (point[1]==0) and (point[2]==0):
            quantity = quantity + 1
        y=y+1
    pourcentage2=(quantity/deltay)*100 # calcul de la quantité relative de pixel noir
    if pourcentage2 < 50: # si le taux de remplissage est petit, c'est un S
        S.on()
    else:  # sinon (si il est grand), c'est un H
        H.on()
elif pourcentage < 10 and presence == 0 and out == 0: # Si il n'y a pas de lettre, il n'y a pas de lettre
    X.on()
elif pourcentage > 6 and presence ==1 and out==0: # Si le decalage en x est superieur a 6% (mais inferieur à 25%), c'est un U
    U.on()
