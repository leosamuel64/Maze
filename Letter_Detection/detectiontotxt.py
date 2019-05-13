#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Oct 28 22:09:43 2018
@author: leo
"""
import skimage  #  sudo apt-get install python3-skimage
import math
import os
#import picamera # sudo apt-get install python3-picamera, spécifique raspberry
from skimage import data, io
 # on fait une autre image qui sera modifiée

a=os.system('raspistill -vf -hf -o img.jpg' )
u=os.system('convert img.jpg -threshold 35%% conv.bmp')
imageSource = io.imread('conv.bmp')
formatImage=imageSource.shape
#print(formatImage) # format contient hauteur / largeur / et ...
xmax=formatImage[1]
ymax=formatImage[0]
xmax=xmax-5
ymax=ymax-5
print("xmax = "+str(xmax))
print("ymax = "+str(ymax))
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
point=imageSource[y,x]
while y<ymax and (point[0]==255) and (point[1]==255) and (point[2]==255) : ##Test si il y a des pixels noirs sur la bordure gauche
    point=imageSource[y,x]
    y=y+1
if (point[0]==0) and (point[1]==0) and (point[2]==0):
    out = 1
print("hors du cadre ? : "+str(out)) 
x=xmax
y=1
point=imageSource[y,x]
while y<ymax and (point[0]==255) and (point[1]==255) and (point[2]==255) :
    point=imageSource[y,x]
    y=y+1
if (point[0]==0) and (point[1]==0) and (point[2]==0): ##Test si il y a des pixels noirs sur la bordure droite
    out = 1

if out ==1:
    print("la lettre est hors")
print("hors du cadre ? : "+str(out))    

##Scan du haut
y=1
x=1
point=imageSource[y,x]
while (point[0]==255) and (point[1]==255) and (point[2]==255)  and x < xmax and y < ymax and out==0:
    point=imageSource[y,x]
    if x == xmax-1:
        x=1
        y=y+1
    x=x+1
if (point[0]==0) and (point[1]==0) and (point[2]==0)  and x < xmax and y < ymax and out==0:
    xhaut = x
    yhaut = y
    print("xhaut : "+str(xhaut))
    print("yhaut : "+str(yhaut))

##Scan du bas
x=1
y=ymax-1
point=imageSource[y,x]
while (point[0]==255) and (point[1]==255) and (point[2]==255)  and x < xmax and y > 1 and out==0:
    point=imageSource[y,x]
    if x == xmax-1:
        x=1
        y=y-1
    x=x+1
if (point[0]==0) and (point[1]==0) and (point[2]==0)  and x < xmax and y > 1 and out==0:
    xbas = x
    ybas = y
    print("xbas : "+str(xbas))
    print("ybas : "+str(ybas))
else :
    print("Il n'y a pas de lettre")
    presence=0

## calcul de difference
deltax = xbas - xhaut
print("delta x : "+str(deltax))
if deltax < 0:
    deltax=-deltax
print("delta x : "+str(deltax))

#calcul avec une marge
pourcentage= (deltax/xmax)*100
print("pourcentage : "+str(pourcentage))
if (pourcentage < 5 or pourcentage > 25)  and presence == 1 and out==0:
    print("S ou H")
    y=yhaut
    deltay = yhaut-ybas
    deltay=abs(deltay)
    while y != ybas:
        point=imageSource[y,x]
        if (point[0]==0) and (point[1]==0) and (point[2]==0):
            quantity = quantity + 1
        y=y+1
    pourcentage2=(quantity/deltay)*100
    print("pourcentage2 : "+str(pourcentage2))
    if pourcentage2 < 50:
        print("c'est un S")
    else:
        print("c'est un H")
elif pourcentage < 10 and presence ==0 and out==0:
    print("il n'y a pas de lettre")
elif pourcentage > 6 and presence ==1 and out==0:
    print("C'est un U") 
