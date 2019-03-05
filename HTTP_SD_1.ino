#include "SdFat.h" // Pose problème !!!!
#include "sdios.h"

// Bibliothèques NMEA/Ethernet 
#include <ArduinoJson.h>
#include <Ethernet.h>

// Constantes réseau 
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xA5, 0x7E }; // adresse mac de la carte
EthernetClient client; // Delcaration du client Ethernet 
const char* server = "192.168.1.30"; // Equivalent à l'écriture précédente 
IPAddress ip(192,168,1,31); // Adresse IP du shield 
const char* resource = "/signalk/v1/api/vessels/self/"; // Diretion de la ressource 
const unsigned long HTTP_TIMEOUT = 100;  // Timeout 
const size_t MAX_CONTENT_SIZE = 512;       // Taille maximum de la réponse HTTP (512 initial) 

// Constantes pour convertion char° to float 
 
  float latitude_convertie=0; 
  float longitude_convertie=0;
  float course_convertie=0;
  float speed_convertie=0;
  float magnetic_variation__convertie=0;

// Constantes pour la mesure du temps de réception de la requête 

  double Tstart=0; 
  double Tstart2=0; 
  double Tstop=0; 

//Constantes propres au stockage carte SD 

char name[] = "log.txt"; // Nom du fichier de stockage 
const uint8_t chipSelect = 4; // SS select 
ArduinoOutStream cout(Serial); // SPI stream 

// Constante de débog 

int i =0; 


// Déclaration de la tructure des donénes à extraire de la page 

struct clientData { // Structure fixe des données à extraires@

  char time_stamp[20];
  char lattitude[8];
  char longitude[8];
  char course_over_ground[8];
  char speed_over_ground[8];
  char magnetic_variation[8];
  
  
};


void setup() {
  Serial.begin(9600); // On synchronise le port série sur la vitesse du port NMEA 2000 
  while (!Serial) {
  ;  // On attend que le port série soit initialisé 
 }
  
pinMode(4, OUTPUT);  
pinMode(10, OUTPUT);  
digitalWrite(10, LOW); // enable Ethernet 
digitalWrite(4, HIGH); // Enable SD Card 
  
  Ethernet.begin(mac,ip);
  delay(1000);

SdFat sd; 
sd.begin(chipSelect, SD_SCK_MHZ(50));
delay(1000);



}



// Connection au serveur HTTP 
bool connect(const char* hostName) {

  bool ok = client.connect(hostName, 80);
  return ok;
}

// Envoie de la requête HTTP au serveur 
bool sendRequest(const char* host, const char* resource) {

  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  return true;
}

// Supression de la bannière HTTP 
bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}

 // Optimisation de la taille du buffer graca à l'assistant Arduino + SJON Parsing
bool readReponseContent(struct clientData* clientData) {

// Déclaration dynamique du buffer 

  const size_t bufferSize = 4*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 350;
  
  DynamicJsonBuffer jsonBuffer(bufferSize);

  Serial.println(client);
  
  JsonObject& root = jsonBuffer.parseObject(client);

  if (!root.success()) {
    return false;
  }

  strcpy(clientData->time_stamp, root["navigation"]["position"]["timestamp"]);
  strcpy(clientData->lattitude, root["navigation"]["position"]["latitude"]);
  strcpy(clientData->longitude, root["navigation"]["position"]["longitude"]);
  strcpy(clientData->course_over_ground, root["navigation"]["courseOverGroundTrue"]["value"]);
  strcpy(clientData->speed_over_ground, root["navigation"]["speedOverGround"]["value"]);
  strcpy(clientData->magnetic_variation, root["navigation"]["magneticVariation"]["value"]);

  return true;
}

// Affichage des données extraites du JSON, prend en paramêtres la structure et son pointeur 

void printclientData(const struct clientData* clientData) {
  
//Serial.print("TimeStamp = ");
//Serial.println(clientData->time_stamp);

  Serial.print("latitude = ");
  Serial.println(clientData->lattitude);
  Serial.print("longitude = ");
  Serial.println(clientData->longitude);
  Serial.print("course over ground  = ");
  Serial.println(clientData->course_over_ground);
  Serial.print("speed over ground = ");
  Serial.println(clientData->speed_over_ground);
  Serial.print("Magnetic variation = ");
  Serial.println(clientData->magnetic_variation);
}

// Convertion char* to float et traitement 

void traitement(const struct clientData* clientData) {
  
  latitude_convertie=atof(clientData->lattitude); // atof : converties un char en double float si il est interpretable comme tel 
  longitude_convertie=atof(clientData->longitude);
  course_convertie=atof(clientData->course_over_ground);
  speed_convertie=atof(clientData->speed_over_ground);
  magnetic_variation__convertie=atof(clientData->magnetic_variation);


Serial.print ("latitude : ");
Serial.println (latitude_convertie);
Serial.print ("longitude  : ");
Serial.println (longitude_convertie);
Serial.print ("course : ");
Serial.println (course_convertie);
Serial.print ("vitesse : ");
Serial.println (speed_convertie);
Serial.print ("variation magnétique : ");
Serial.println ( magnetic_variation__convertie);
Serial.println ();
Serial.println ();

//  //Ecriture sur la carte SD 

digitalWrite(10, HIGH); // disable Ethernet 
digitalWrite(4, LOW); // Enable SD Card 


ofstream sdout(name, ios::out | ios::app);

sdout << latitude_convertie;
sdout << ";";
sdout << longitude_convertie;
sdout.close();
  
digitalWrite(10, LOW); // enable Ethernet 
digitalWrite(4, HIGH); // disable SD Card 


  // Calul et traitement des informations ici 

  
 
}


// Fermeture de la connection avec le serveur HTTP 
void disconnect() {
  client.stop();
}

// Temporisation de n secondes 
void wait() {

  delay(1000); // Délais optimal entre deux reqêtes proche de 1 sec 
}



void loop() {
  Tstart=millis(); // 1er timer : on prend en compte le test sur le fait de trouver ou non le serveur 
  if(connect(server)) {
    Tstart2=millis(); // 2em timer : on mesure le temps de réponse du serveur une fois que l'on est sur que la connexion avec ce dernier est établis 
    if(sendRequest(server, resource) && skipResponseHeaders()) { // Si la requête est ien envoyé et que l'on est pas dans le header 
      Serial.println("la");
      clientData clientData;
      Serial.println();
      if(readReponseContent(&clientData)) {
        Serial.println("ici");
        
        //printclientData(&clientData);
        traitement(&clientData);  // Appel de la boucle de traitement ici 
      }
    }
  }
  disconnect();
  Tstop=millis();
//  
//Serial.print(Tstop-Tstart);
//Serial.print(",");
//Serial.print(Tstop-Tstart2);
//Serial.println();
 
  wait();
}
