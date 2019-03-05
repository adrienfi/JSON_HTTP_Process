#include <Wire.h>

// Variables pour traitement et convertion 
char Char[3];
String str="";

// Variables intermédiaires pour les données 

  float data_0=0; 
  float data_1=0;
  float data_2=0;
  float data_3=0;
  float data_4=0;
  
struct clientData { // Structure fixe des données à extraires@

  char time_stamp[20];
  char lattitude[8];
  char longitude[8];
  char course_over_ground[8];
  char speed_over_ground[8];
  char magnetic_variation[8];
  
  
};
// Indices 
int i=0;


void setup() {
  Serial.begin(9600);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}



void receiveEvent(int bytes,const struct clientData* clientData) {

char c = Wire.read();

while (Wire.available()){
if (1<i<2)
{
char c = Wire.read();
Char[i]+=c;
}

if (c!='F')
{ i++; }

else { 
  i=0;
  }
}

str=Char[0];
data_0=str.toFloat();
strcpy(clientData->lattitude, char(data_0));
str=Char[1];
data_1=str.toFloat();
strcpy(clientData->longitude, char(data_1));
str=Char[2];
data_2=str.toFloat();
strcpy(clientData->course_over_ground, char(data_2));
str=Char[3];
data_3=str.toFloat();
strcpy(clientData->speed_over_ground, char(data_3));
str=Char[4];
data_4=str.toFloat();
strcpy(clientData->magnetic_variation, char(data_4));

Serial.println(clientData->lattitude);
//Serial.println(data_2);

}

void loop() {
 
}
