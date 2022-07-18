unsigned long  duration;
unsigned int distance;
unsigned int distanceStart;
const int SENSOR_MAX_RANGE = 300; // in cm

void setup() {
  Serial.begin(115200);               // Start der seriellen Kommunikation
  pinMode(D7, OUTPUT) ;                // Output für Ultraschall
  pinMode(D8, INPUT) ;                 // Input für Ultraschall
  pinMode(D2, OUTPUT) ;                // Output für Relais und Pumpe
  pinMode(D6, OUTPUT);                // Output Feuchtigkeitssensor
  pinMode(A0, INPUT);

}
void loop() {
  digitalWrite(D2, LOW);
  delay(1000);
  int sensorWert_Feuchtigkeit = analogRead(A0);  // Auslesen des aktuellen Sensorwertes
  Serial.print("Feuchtigkeit: ");
  Serial.println(sensorWert_Feuchtigkeit);       // Ausgabe des Wertes
  if  (sensorWert_Feuchtigkeit<700){ // erstmal 300, müssen wir noch überprüfen
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
      

  while (abs(distance-distanceStart)<3) { // Wert erst zum Testen, noch anpassen
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
   }
   //Abstand messen, um zu checken, ob der Tank leer ist
   // evtl über redNote den Benutzer alarmieren
   
   delay(10000);                        //  Pause in ms
}
