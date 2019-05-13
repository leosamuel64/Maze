#include "SparkFunLSM6DS3.h"
#include "Wire.h"
#include "rgb_lcd.h"
#include <Servo.h>
#include "Ultrasonic.h"

Ultrasonic ultrasonic(4);  // Création de l'objet pour le capteur ultrason

//////////////////////// Variables capteur de chaleur ////////////////////////

int temprefd = 0;
int temprefg = 0;
int tempdroite = 0;
int tempgauche = 0;

//////////////////////// Variables capteur de couleurs ////////////////////////

#define S0 23
#define S1 25
#define S2 27
#define S3 29
#define sensorOut 31
#define pinled 10
int frequencyR = 0;
int frequencyV = 0;
int frequencyB = 0;
int Rb = 0;
int Vb = 0;
int Bb = 0;

int Rn = 0;
int Vn = 0;
int Bn = 0;

int Ra = 0;
int Va = 0;
int Ba = 0;

int Sb = 0;
int Sn = 0;
int Sa = 0;

int SurfaceState = -1;

//////////////////////// Fonctions capteur de couleurs ////////////////////////
  
void color() {        //recuperation des fréquences RVB
  digitalWrite(pinled,HIGH);
       
  digitalWrite(S2,LOW);       // lecture de la densité de rouge
  digitalWrite(S3,LOW);
  frequencyR = pulseIn(sensorOut, LOW);
  delay(20);
  
  digitalWrite(S2,HIGH);        // lecture de la densité de vert
  digitalWrite(S3,HIGH);
  frequencyV = pulseIn(sensorOut, LOW);
  delay(20);
  
  digitalWrite(S2,LOW);       // lecture de la densité de bleu 
  digitalWrite(S3,HIGH);
  frequencyB = pulseIn(sensorOut, LOW);
  delay(20);
  
  digitalWrite(pinled,LOW);
}
 
//////////////////////// Variables gyroscope / accéléro ////////////////////////
  
int anglecoeff = 0;       // Variable d'ajustement de la rotation  
float I = 0;        // integrale du gyro
int Angle = 0;        // angle
int coefrotation = 0;       // coefficient de centrage (depend de la position de l'accelero sur le robot)

//////////////////////// Creation de l'objet servo ////////////////////////

  Servo myservo;
  
//////////////////////// Creation de l'objet pour le gyro ////////////////////////

LSM6DS3 myIMU( I2C_MODE, 0x6A );  //I2C device address 0x6A
rgb_lcd lcd;

//////////////////////// Variables moteur (Gestion pont H) ////////////////////////

int pinLB = 12;     // define pin 12 
int pinLF = 3;     // define pin 3 
int pinRB = 13;    // define pin 13 
int pinRF = 11;    // define pin 11  

//////////////////////// Variables capteurs de distances IR ////////////////////////

#define sens_fl A14 // Sharp IR GP2Y0A41SK0F (4-30cm, analogique)
#define sens_fr A12 // Sharp IR GP2Y0A41SK0F (4-30cm, analogique)
#define sens_bl A15 // Sharp IR GP2Y0A41SK0F (4-30cm, analogique)
#define sens_br A13 // Sharp IR GP2Y0A41SK0F (4-30cm, analogique)

float dis_fl_raw = 0;
float dis_fr_raw = 0;
float dis_bl_raw = 0;
float dis_br_raw = 0;

float dis_fl = 0;
float dis_fr = 0;
float dis_bl = 0;
float dis_br = 0;

//////////////////////// Variables capteur de distances US ////////////////////////

long Distance;        // valeur de la distance réelle entre l'US et le mur en face
long modDistance = 0;       // modulo sur 30 de la valeur Distance



//////////////////////////////////////////////////////////////////////////////////////////////// Void SETUP //////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

//////////////////////// Etalonnage de la température ambiente ////////////////////////

temprefd = analogRead(A11);
temprefg = analogRead(A12);

//////////////////////// Configuration du servomoteur ////////////////////////

myservo.attach(9);

//////////////////////// Configuration des moteurs ////////////////////////

  pinMode(pinLB,OUTPUT); // pin 12
  pinMode(pinLF,OUTPUT); // pin 3 (PWM)
  pinMode(pinRB,OUTPUT); // pin 13
  pinMode(pinRF,OUTPUT); // pin 11 (PWM)
  
//////////////////////// Configuration de l'écran lcd ////////////////////////

  lcd.begin(16, 2);
  lcd.setRGB(255,255,255);
  
//////////////////////// Configuration de l'IMU (gyro) ////////////////////////
  
  if(myIMU.begin() != 0 )
  {
    lcd.setRGB(255,0,0);
	  lcd.print("Device error");
    delay(1000);
    lcd.clear();
  }
  else  
  {
    lcd.write("Hello world!");
    lcd.setRGB(0, 255, 0);
    delay(1000);
    lcd.setRGB(255,255,255);
    lcd.clear();
  }

//////////////////////// Configuration du capteur de couleurs ////////////////////////

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(pinled, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

//////////////////////// Interface Arduino-Raspberry ////////////////////////

  pinMode(45,INPUT);
  pinMode(47,INPUT);
  pinMode(49,INPUT);
  pinMode(51,INPUT);

//////////////////////// Etolonnage du capteur de couleurs ////////////////////////

/*    //Blanc
lcd.print("Blanc ");
delay(800);
lcd.print("3 ");
delay(800);
lcd.print("2 ");
delay(800);
lcd.print("1 ");
delay(800);
color();
Rb = frequencyR;
Vb = frequencyV;
Bb = frequencyB;
Sb = Rb+Vb+Bb;
lcd.clear();

    //Noir
lcd.print("Noir ");
delay(800);
lcd.print("3 ");
delay(800);
lcd.print("2 ");
delay(800);
lcd.print("1 ");
delay(800);
color();
Rn = frequencyR;
Vn = frequencyV;
Bn = frequencyB;
Sn = Rn+Vn+Bn;
lcd.clear();

    //Alu
lcd.print("Alu ");
delay(800);
lcd.print("3 ");
delay(800);
lcd.print("2 ");
delay(800);
lcd.print("1 ");
delay(800);
color();
Ra = frequencyR;
Va = frequencyV;
Ba = frequencyB;
Sa=Ra+Va+Ba;
lcd.clear();*/

} ///////////////////////////////////////////////////////////////////////////////////////////////// Fin SETUP //////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////// Fonction capteur couleurs ////////////////////////

void surface() {
  color();
if (frequencyR+frequencyV+frequencyB < 5*Sb){  
  SurfaceState = 1;       // blanc
}
else {
  SurfaceState = 0;       // noir
}
}

//////////////////////// Fonction éjecteur ////////////////////////

  void larguage(){
    lcd.setRGB(0,0,255);
    lcd.print("LARGUAGE DU KIT");
    int maxserv = 173;
    int minserv = 1;
    int tpsservo =300;
    myservo.write(minserv);
    delay(tpsservo);
    myservo.write(maxserv);
    delay(tpsservo);
    myservo.write(maxserv);
    delay(400); 
    myservo.write(90);
    delay(tpsservo);
    myservo.write(maxserv);
    delay(tpsservo);
    myservo.write(maxserv);
    delay(400);
    myservo.write(90);
    delay(tpsservo);
    myservo.write(maxserv);
    myservo.write(90);
    delay(tpsservo);
    delay(100);
    lcd.setRGB(255,255,255);
  }

//////////////////////// Fonctions moteurs ////////////////////////

void left() { 
    digitalWrite(pinLB,LOW);
    digitalWrite(pinRB,LOW);
    analogWrite(pinLF,255);   
    analogWrite(pinRF,255);
}
    
void stopp() {
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinRB,HIGH);
     analogWrite(pinLF,0);
     analogWrite(pinRF,0); 
}

void back()  {
   digitalWrite(pinLB,LOW);
   digitalWrite(pinRB,HIGH);   
   analogWrite(pinLF, 235);
   analogWrite(pinRF,235);   
}

void advance() {
   digitalWrite(pinLB,HIGH);    
   digitalWrite(pinRB,LOW);  
   analogWrite(pinLF, 225); 
   analogWrite(pinRF,215); 
}
  
void right()  {
     digitalWrite(pinLB,HIGH); 
     digitalWrite(pinRB,HIGH); 
     analogWrite(pinLF,255);  
     analogWrite(pinRF,255);     
}

//////////////////////// Fonction capteurs de distances IR ////////////////////////

void disfl() {        // capteur IR front_left
  
  dis_fl_raw = analogRead(sens_fl)*0.0048828125;
  dis_fl = 13*pow(dis_fl_raw, -1);
}

void disfr() {        // Capteur IR front_right
  
  dis_fr_raw = analogRead(sens_fr)*0.0048828125;
  dis_fr = 13*pow(dis_fr_raw, -1); 
}

void disbl() {        // Capteur IR back_left
  
  dis_bl_raw = analogRead(sens_bl)*0.0048828125;
  dis_bl = 13*pow(dis_bl_raw, -1);   
}

void disbr() {        // Capteur IR back_right
  
  dis_br_raw = analogRead(sens_br)*0.0048828125;
  dis_br = 13*pow(dis_br_raw, -1);  
}

//////////////////////// Fonction IMU (gyro) ////////////////////////

void angle() {
  if (abs(myIMU.readFloatGyroZ()) >= 0.5) {
  I=I+abs(myIMU.readFloatGyroZ());
  coefrotation = 7500;                 // coefrotation est une constante :: A trouver en fonction du robot et du centrage du capteur
  Angle=((I*anglecoeff)/coefrotation);//coefrotation
  delay(50);
}
}

//////////////////////// Fonction capteur distance US ////////////////////////

void distance() { 
    Distance = ultrasonic.MeasureInCentimeters(); // mesures en centimètres de la distance avant
    delay(15); // délai entre deux mesures
}

//////////////////////// Fonctions déplacements ////////////////////////

void dep_right() {        // tourner à droite
  anglecoeff=110;
  I =0;
  Angle = 0;
    while (Angle < 17) {
      right();
      delay(10);
      angle();
   }
   stopp();
  }

void dep_left() {       // tourner à gauche
  anglecoeff = 128;
  I =0;
  Angle = 0;
    while (Angle < 19) {
      left();
      delay(10);
      angle();
   }
   stopp();
  }

void dep_advance() {        // avancer tout droit d'une case à l'autre
  advance();
  distance();
  Distance = Distance+3;
  modDistance = Distance % 30;
 /* surface();*/
  while (modDistance != 10 /*and SurfaceState == 1*/) {
    /*surface();*/
    advance();
    distance();
    modDistance = Distance % 30;
    Distance = Distance + 2;
  }
    /*if (SurfaceState == 0){       // détection passage interdit
      stopp();
      lcd.setRGB(255,150,150);
      lcd.print("zone interdite");
      back();
      delay(1000);
      dep_left();
      delay(100);
      dep_left();
      lcd.clear();
      }*/
    stopp();
}

//////////////////////// Fonction capteurs chaleur IR ////////////////////////

void chauddroite(){       // capteur droit
  tempdroite = analogRead(A11); // fil orange
  if (abs(tempdroite-temprefd) > 100){
     lcd.print("source de chaleur");
     larguage();
     lcd.clear();
  }
}

void chaudgauche(){       // capteur gauche
  tempgauche = analogRead(A12); // fil blanc
  if (abs(tempgauche-temprefg) > 200){
     lcd.print("source de chaleur");
     larguage();
     lcd.clear();
  }
}

//////////////////////// Fonction réception lettre du Raspberry ////////////////////////

void receivelettre(){
int rU = digitalRead(45);       // récup des signaux sur les pins du Raspberry
int rH = digitalRead(47);
int rS = digitalRead(49);
int rX = digitalRead(51);
  while(rU == 0 or rH == 0 or rS == 0 or rX == 0){
      rU = digitalRead(45);
      rH = digitalRead(47);
      rS = digitalRead(49);
      rX = digitalRead(51);
  }
    if(rU == 1){          // détection lettre U
      lcd.setRGB(0,255,0);
      lcd.print("victime indemne (U)");
      lcd.clear();
    }
    else if(rH == 1){         // détection lettre H : larguage de 2 packs
      lcd.setRGB(255,0,0);
      lcd.print("victime blessée (H)");       
      dep_advance();                                       
      disfl();
      disfr();
      disbl();
      disbr();
        if(dis_fl < 20 and dis_bl < 20){    //détection d'un mur à gauche
          right();
          larguage();
          larguage();
      }
        else {    // détection d'un mur à droite
          left();
          larguage();
          larguage();
      back();
      delay(500);       // recule pour ne pas influer sur le prg navigation
      }
      lcd.clear();
    }
    else if(rS == 1){          // détection lettre S : larguage d'1 pack
      lcd.setRGB(250,175,0);
      lcd.print("victime stable (S)");
      dep_advance();
        if(dis_fl < 20 and dis_bl < 20){    //détection d'un mur à gauche
          right();
          larguage();   
       }
       else {    // détection d'un mur à droite
          left();
          larguage();
       }
      back();
      delay(500);
      lcd.clear();
     }
}

//////////////////////// Fonction centrage ////////////////////////

/*void centrage() {
  disfl();
  disfr();
  disbl();
  disbr();  
float r = dis_fl / dis_fr ;       // ratio entre les distances IR front_right et front_right
if (dis_fr < 25 and dis_fl < 25 and dis_br < 25 and dis_bl < 25){       // si il y a des murs de chaque côtés du robot
  while ((r > 0.8 and r < 0.9) || (r > 1.1 and r < 1.2)) {
    disfl();
    disfr();
    disbl();
    disbr(); 
    lcd.print(r)
    lcd.clear();
    if (r > 1.1){
      left();
      delay(5);
    }
    else if (r < 0.9){
      right();
      delay(5);
    }
  }
}
  stopp();
}
*/
//////////////////////// Fonction naviguation ////////////////////////

void navigation(){
disfl();      // récup valeurs capteurs IR
disfr();
disbl();
disbr();
distance();
delay(1000);
 if (dis_fl > 25 /*and dis_bl > 20*/) {    // détection passage à gauche
   dep_left();
   delay(200);
   chauddroite();
   chaudgauche();
   dep_advance();
   delay(1000);
 }
 else if (Distance > 20){    //détection passage avant
   dep_advance();
   chauddroite();
   chaudgauche();
   delay(1000);
 }
 else if (dis_br > 25 /*and dis_br > 20*/) {    // détection passage à droite
   dep_right();
   delay(200);
   chauddroite();
   chaudgauche();
   dep_advance();
   delay(1000);   
 }
 else {    // demi-tour
   dep_left();
   delay(1000);
   chauddroite();
   chaudgauche();
   dep_left();
   delay(200);
   dep_advance();
   delay(1000);
 }
 stopp();
}


void loop() {  //////////////////////////////////////////////////////////////////////////////////// void loop ///////////////////////////////////////////////////////////////////////////////////////////////////

larguage();
delay(700);
 

 /*navigation();


 /*receivelettre();
 delay(500);*/

}

  
