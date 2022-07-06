void setup() {
  Serial.begin(115200);               // Start der seriellen Kommunikation
  pinMode(13, OUTPUT) ;                // Output für Ultraschall
  pinMode(12, INPUT) ;                 // Input für Ultraschall
  pinMode(12, OUTPUT) ;                // Output für Relais und Pumpe


}
void loop() {
  int sensorWert_Feuchtigkeit = analogRead(A0);  // Auslesen des aktuellen Sensorwertes
  Serial.println(sensorWert_Feuchtigkeit);       // Ausgabe des Wertes
 if  (sensorWert_Feuchtigkeit<300){ // erstmal 300, müssen wir noch überprüfen
  
  digitalWrite(13, LOW);
  delay(2);
  digitalWrite(13, HIGH);
  delayMicroseconds(10);
  int duration = pulseIn(12, HIGH);
  int distance = duration/58;
  int distance1 = distance;
  digitalWrite(6,HIGH); // Pumpe über relais anschalten
while (distance-distance1==3) // Wert erst zum Testen, noch anpassen
{ digitalWrite(13, LOW);
  delay(2);
  digitalWrite(13, HIGH);
  delayMicroseconds(10);
  int duration = pulseIn(12, HIGH);
  int distance1 = duration/58; 
}
digitalWrite(6,LOW) ;// Pumpe über Relais ausschalten
 }
 delay(2592000);                        //  Pause von 12 Stunden
}
