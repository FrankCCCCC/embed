{\rtf1\ansi\ansicpg950\cocoartf1347\cocoasubrtf570
{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
\paperw11900\paperh16840\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural

\f0\fs24 \cf0 #include <SoftwareSerial.h>\
#include <Wire.h>\
SoftwareSerial I2CBT(10,11); // TX:10, RX:11\
byte serialA;\
\
void setup()\{\'a0\
\'a0 Serial.begin(9600);\
\'a0 I2CBT.begin(9600);\
\}\
\'a0\
void loop ()\{\
\'a0 \'a0 byte Data[6];\
\'a0 \'a0 byte cmmd[20];\
\'a0 \'a0 int insize;\
\'a0 \'a0 int x=analogRead(A0);//read sensor value\
\'a0 \'a0 int y=analogRead(A1);//read sensor value\
\'a0 \'a0 serialA=I2CBT.read();\
\'a0 \'a0 Data[0]='a';\
\'a0 \'a0 Data[1]=x/256;\
\'a0 \'a0 Data[2]=x%256;\
\'a0 \'a0 Data[3]='b';\
\'a0 \'a0 Data[4]=y/256;\
\'a0 \'a0 Data[5]=y%256;\
\'a0 \'a0 \'a0Serial.print(x);\
\'a0 \'a0 \'a0Serial.print(" , ");\
\'a0 \'a0 \'a0Serial.println(y);\
\'a0\
\'a0 \'a0 \'a0if (serialA == 49)\{\
\'a0 \'a0 \'a0 \'a0 \'a0for(int j=0;j<5;j++)\
\'a0 \'a0 \'a0 \'a0 \'a0I2CBT.write(Data[j]);\
\'a0 \'a0 \'a0 \'a0 \'a0serialA=0;\
\'a0 \'a0 \'a0 \}\
\'a0 delay(100);\
\}}