#include <Wire.h>

char a =  '3';
char b = '4';
char c =  '3';
char d = '4';
char e = '4';

void setup() {
  Serial.begin(9600); 
  // Start the I2C Bus as Master
  Wire.begin(); 
}
void loop() {
  
  Wire.beginTransmission(9);
  Wire.write('D');// transmit to device #9
  Wire.write(x);         
//  Wire.write(';');  
  Wire.write(y);  
  Wire.write('F');
  Wire.endTransmission();    // stop transmitting
  delay(1000);
}
