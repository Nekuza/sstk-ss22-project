#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <WiFiManager.h>
WiFiManager wifiManager;
WiFiClient wificlient;
PubSubClient client(client);

unsigned long  duration;
unsigned int distance;
unsigned int distanceStart;
const int distanceLeererTank = 10; // in cm MUSS NOCH ÜBERPRÜFT WERDEN
const int grenzwertTank = 1; //distanceLeererTank - distance = grenzwertTank
const int SENSOR_MAX_RANGE = 300; // in cm

// Möglichkeit zur Festlegung der WiFi-Credentials. Aktuell deaktiviert, weil über WiFiManager verbunden wird
const char* ssid = "";
const char* password = "";

// Setup für MQTT
// mqtt-topics als Konstanten, damit bei Änderung der Code nicht komplett manipuliert werden muss
const char* publish_feuchtigkeit = "pflanzentopf/feuchtigkeit";
const char* publish_verbleibendesWasser = "pflanzentopf/verbleibendesWasser";
const char* publish_benachrichtigung = "pflanzentopf/benachrichtigung";
const char* subscribe_pumpe = "pflanzentopf/pumpe";

const char* mqtt_server = "mqtt.iot.informatik.uni-oldenburg.de";
const int mqtt_port = 2883;
const char* mqtt_user = "sutk";
const char* mqtt_pw = "SoftSkills";
  
void setup_wifi(boolean wifimanager_enabled) {

  if (wifimanager_enabled == true) {
    wifiManager.autoConnect("Pflanzentopf"); 
    delay(2000);
  } else {
      delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    randomSeed(micros());
  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}


//Diese Methode wird aufgerufen, sobald es neue Nachrichten gibt, die die Pumpe betreffen
void callback(char* topic, byte* payload, unsigned int length) {
  char receivedPayload[length];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    receivedPayload[i] = (char) payload[i];
  }
  Serial.println();

  if (strcmp(receivedPayload, "1") == 0) {
    Serial.println("NodeRed: Pumpe an");
    digitalWrite(D2, LOW);
  }
  if (strcmp(receivedPayload, "0") == 0) {
    Serial.println("NodeRed: Pumpe aus");
    digitalWrite(D2, HIGH);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID: Client ID MUSS inviduell sein, da der MQTT Broker nicht mehrere Clients mit derselben ID bedienen kann
    String clientId = "Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe(subscribe_pumpe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_publish(char* topic, char* message) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);       // Ausgabe des Wertes
  client.publish(topic, message);
}

void setup() {
  Serial.begin(115200);               // Start der seriellen Kommunikation
  pinMode(D7, OUTPUT) ;               // Output für Ultraschall
  pinMode(D8, INPUT) ;                // Input für Ultraschall
  pinMode(D2, OUTPUT) ;               // Output für Relais und Pumpe
  pinMode(D6, OUTPUT);                // Output Feuchtigkeitssensor
  pinMode(A0, INPUT);                 // ??
  
  setup_wifi(true);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
void loop() {

  //Verbindung zu Mqtt checken und ggf reparieren
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //Abschalten der Pumpe
  digitalWrite(D2, LOW);
  
  delay(1000);
 
  int sensorWert_Feuchtigkeit = analogRead(A0);  // Auslesen des aktuellen Sensorwertes
  Serial.print("Feuchtigkeit: ");
  Serial.println(sensorWert_Feuchtigkeit);       // Ausgabe des Wertes
  client.publish(publish_feuchtigkeit, (char*) sensorWert_Feuchtigkeit);
  
  if  (sensorWert_Feuchtigkeit<700){ // Wert müssen wir noch überprüfen
    digitalWrite(D7, LOW);
    delayMicroseconds(2);
    digitalWrite(D7, HIGH);
    delayMicroseconds(10);
    duration = pulseIn(D8, HIGH);
    Serial.println(2);
    int distanceStart = duration/58;
    int distance = distanceStart;
    if (distanceStart > SENSOR_MAX_RANGE || distanceStart <= 0){
        Serial.println("Out of sensor range!");
    } else {
        Serial.println("DistanceStart to object: " + String(distanceStart) + " cm");
    }
  digitalWrite(D2,HIGH); // Pumpe über relais anschalten
      

  while (abs(distance-distanceStart) < 3) { // Wert erst zum Testen, noch anpassen
    Serial.println(3);
    digitalWrite(D7, LOW);
    delayMicroseconds(2);
    digitalWrite(D7, HIGH);
    delayMicroseconds(10);
    duration = pulseIn(D8, HIGH);
    distance = duration/58; 
    if (distance > SENSOR_MAX_RANGE || distance <= 0){
        Serial.println("Out of sensor range!");
    } else {
        Serial.println("Distance to object: " + String(distance) + " cm");
    }
    delay(500);
    }
  digitalWrite(D2,LOW) ;// Pumpe über Relais ausschalten
  // TODO Abstand zum Wasser in verbleibendes Wasser umrechnen
  client.publish(publish_verbleibendesWasser, (char*) distance);
   }
   //Abstand messen, um zu checken, ob der Tank leer ist
    if (distanceLeererTank - distance < grenzwertTank) {
    client.publish(publish_benachrichtigung, "Wassertank ist fast leer!");
    Serial.println("Wassertank ist fast leer!");
  }
   
   delay(10000);                        //  Pause in ms
}
