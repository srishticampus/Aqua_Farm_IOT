#include <LiquidCrystal_I2C.h>
#include <Arduino.h> // Include for digital pin functionalities
 ///#include <Wire.h> 
 


#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials

// #define WIFI_SSID "Galaxy A14 0C7B"
// #define WIFI_PASSWORD "cnt4zfz2j3rnndn"
#define WIFI_SSID "realme narzo 20 Pro"
#define WIFI_PASSWORD "akshaisunil"
// #define WIFI_SSID "Galaxy F62"
// #define WIFI_PASSWORD "vbii6922"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDDnLUVH73nws93FbskwYRKKLWyYIcW-bE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://aquafarmuit-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// Define LCD connection details
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address might be different (check LCD datasheet)

// Define sensor pin connections
const int trigPin = 5;    // Digital output pin connected to Trig of HC-SR04
const int echoPin = 18;   // Digital input pin connected to Echo of HC-SR04
const int ldrPin = 35;    // Analog pin connected to LDR sensor
const int phPin = 34;     // Analog pin connected to pH sensor
const int tempPin = 4;    // Analog pin connected to temperature sensor (replace with actual pin)
#define LED_PIN 13     // GPIO22 pin connected to Ambiant light LED
#define LED_PINmOtor 12     // GPIO22 pin connected to Motor LED

// Variables for sensors
long duration; // Duration of the echo pulse
float distance; // Calculated distance in centimeters
int ldrValue; // Raw analog value from the LDR sensor
float phValue; // Calculated pH value
float temperature; // Calculated temperature value
float phSensorVoltage=0;
enum SensorType { DISTANCE, LDR, PH, TEMPERATURE }; // Enum for sensor types with temperature added
SensorType currentSensor = DISTANCE; // Current sensor to be displayed
bool signupOK = false;
String sValue, sValue2;
int checkamLight=0;
int checkmoLight=0;
void setup() {
  Serial.begin(115200);
  // Initialize LCD (number of columns, rows)
  lcd.init();
  // Turn on backlight and clear display
  lcd.backlight();
  lcd.clear();

  // Set sensor pins as digital or analog
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(phPin, INPUT);
  pinMode(tempPin, INPUT); // Set temperature sensor pin as analog
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PINmOtor, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  Serial.println("in loop");
  // if(checkamLight==1){
  //   digitalWrite(LED_PIN,HIGH);
  // }
  // else{
  //   digitalWrite(LED_PIN,LOW);
  // }
  // if(checkmoLight==1){
  //   digitalWrite(LED_PINmOtor,HIGH);
  // }
  // else{
  //   digitalWrite(LED_PINmOtor,LOW);
  // }

  if (Firebase.ready() && signupOK ) {
    // LDR sensor reading (existing code)
   amlightcontrl();
  WaterLevelContrl();
 
      digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Send a 10 microsecond pulse
  digitalWrite(trigPin, LOW);
 
  // Measure echo pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance (adjust formula based on sensor datasheet if needed)
  distance = duration * 0.034 / 2; // Speed of sound in cm/us

      // Clear LCD and print "Distance:"
      lcd.clear();
      lcd.print("Water Level: ");
      if (distance>30){
        lcd.setCursor(0, 1); // Move cursor to second line
        // Print calculated distance with one decimal place
      lcd.print("Very Low");
     if (Firebase.RTDB.setString(&fbdo, "/Sensor/WaterLevel","Very Low")){
          Serial.println("water level PASSED"); 
               
          }
      }
      else{
        lcd.setCursor(0, 1); // Move cursor to second line
        lcd.print("Normal");
     if (Firebase.RTDB.setString(&fbdo, "/Sensor/WaterLevel","Normal")){
          Serial.println("water level PASSED"); 
               
          }

      }
      
 lcd.clear();
    // case LDR:
      // LDR sensor reading (existing code)
    // amlightcontrl();
      ldrValue = analogRead(ldrPin); // Read analog value from LDR

      // Clear LCD and print "LDR:"
      lcd.clear();
      lcd.print("Ambient Light: ");
      
      if(ldrValue>2000){
        lcd.setCursor(0, 1); // Move cursor to second line
        lcd.print("Too Bright");
     if (Firebase.RTDB.setString(&fbdo, "/Sensor/AmbiantLight","Too Bright")){
          Serial.println("AmbiantLight PASSED"); 
               
          }
       }
        else if(ldrValue<10){
          lcd.setCursor(0, 1); // Move cursor to second line
        lcd.print("Too Low");
     if (Firebase.RTDB.setString(&fbdo, "/Sensor/AmbiantLight","Too Low")){
          Serial.println("AmbiantLight PASSED"); 
               
          }
       }
      else{
        lcd.setCursor(0, 1); // Move cursor to second line
        // Print calculated distance with one decimal place
      lcd.print("Normal");
     if (Firebase.RTDB.setString(&fbdo, "/Sensor/AmbiantLight","Normal")){
          Serial.println("AmbiantLight PASSED"); 
               
          }
      }

 lcd.clear();

      // Update current sensor and delay for 3 seconds
      // currentSensor = PH;
      // //delay(300);
      // break;

    // case PH:
      // Read voltage from pH sensor (existing code)
    // amlightcontrl();
      phSensorVoltage = analogRead(phPin) * (5.0 / 4095.0); // Assuming 5V reference voltage

      // Calculate pH value using your sensor's calibration formula
      phValue = (3.5 * phSensorVoltage) - 2.75; // Replace with the formula from your sensor datasheet


      // Clear LCD and print "pH:"
      lcd.clear();
      lcd.print("pH: ");
      lcd.setCursor(0, 1); // Move cursor to second line
      // Print calculated pH value with one decimal place
      lcd.print(phValue, 1);
      if (Firebase.RTDB.setString(&fbdo, "/Sensor/PHValue",phValue)){
          Serial.println("PHValue PASSED"); 
               
          }
      // Update current sensor and delay for 3 seconds
    //   currentSensor = TEMPERATURE;
    //  // delay(300);
    //   break;
 lcd.clear();
 lcd.print("Oxygen: ");
      lcd.setCursor(0, 1); // Move cursor to second line
      // Print calculated Oxygen value with one decimal place
      lcd.print("Normal");
     
if (Firebase.RTDB.setString(&fbdo, "/Sensor/diss_oxy","normal")){
          Serial.println("diss oxy PASSED"); 
               
          }
lcd.clear();
    // case TEMPERATURE:
 //  amlightcontrl();
      // Read voltage from temperature sensor
      float tempVoltage = analogRead(tempPin) * (5.0 / 4095.0); // Assuming 5V reference voltage

      // Replace this with the conversion formula for your specific temperature sensor
      // You'll need to consult the datasheet for the sensor you're using
      temperature = ((tempVoltage - 2.5) * 100)+280; // Example conversion (might need adjustment)

      // Clear LCD and print "Temp:"
      lcd.clear();
      lcd.print("Temp: ");
      lcd.setCursor(0, 1); // Move cursor to second line
      // Print calculated temperature value with one decimal place
      lcd.print(temperature, 1);
      lcd.print(" C"); // Adjust unit (C or F) based on your sensor
      if (Firebase.RTDB.setString(&fbdo, "/Sensor/Temperature",temperature)){
          Serial.println("Temperature PASSED"); 
               
          }
    
  }
}
void amlightcontrl(){
 ldrValue = analogRead(ldrPin); // Read analog value from LDR
int a,light;
        // Serial.println(Firebase.RTDB.getString(&fbdo, "/Automatic"));
        // Serial.println(Firebase.RTDB.getString(&fbdo, "/Light"));

         if (Firebase.RTDB.getString(&fbdo, "/Switch/Automatic")) {
            if (fbdo.dataType() == "string") {
               sValue = fbdo.stringData();
               a = sValue.toInt();
                Serial.println(a);
        
        
            }
          }
         if (Firebase.RTDB.getString(&fbdo, "/Switch/Light")) {
              if (fbdo.dataType() == "string") {
                 sValue = fbdo.stringData();
                  light = sValue.toInt();
                 Serial.println(light);
        
        
               }
          }

          if ((a==1)&&(ldrValue<10)) {

              digitalWrite(LED_PIN,HIGH);
                    
            }
          else if ((a==0)&&(ldrValue<10)) {

              digitalWrite(LED_PIN,LOW);
                    
            }
          else if ((a==1)&&(ldrValue>10)) {

              digitalWrite(LED_PIN,LOW);
                    
            }
          else if (light==1) {
    
              digitalWrite(LED_PIN,HIGH);
          }
          else if (light==0) {
    
              digitalWrite(LED_PIN,LOW);
          }
        
      }
     
void WaterLevelContrl(){
 digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Send a 10 microsecond pulse
  digitalWrite(trigPin, LOW);
 
  // Measure echo pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance (adjust formula based on sensor datasheet if needed)
  distance = duration * 0.034 / 2; // Speed of sound in cm/us
  Serial.println(distance);
  int ai, water;
        // Serial.println(Firebase.RTDB.getString(&fbdo, "/Automatic"));
        // Serial.println(Firebase.RTDB.getString(&fbdo, "/Light"));

      if (Firebase.RTDB.getString(&fbdo, "/Switch/Automatic")) {
        if (fbdo.dataType() == "string") {
          sValue = fbdo.stringData();
          ai = sValue.toInt();
          Serial.println(ai);
          Serial.println("Auto");
          Serial.println("Auto:"+ai);
        
        
        }
      }
      if (Firebase.RTDB.getString(&fbdo, "/Switch/WaterPumb")) {
        if (fbdo.dataType() == "string") {
          sValue = fbdo.stringData();
          water = sValue.toInt();
          Serial.println("Manual");
          Serial.println("water:"+water);
        
        
        }
      }

      if ((ai==1)&&(distance>30)) {

            digitalWrite(LED_PINmOtor,HIGH);
                    
      }
      else if ((ai==0)&&(distance>30)) {

            digitalWrite(LED_PINmOtor,LOW);
                    
      }
      else if ((ai==1)&&(distance<30)) {

            digitalWrite(LED_PINmOtor,LOW);
                    
      }

      else if (water==1) {
    
            digitalWrite(LED_PINmOtor,HIGH);
      }
      else if (water==0) {
    
            digitalWrite(LED_PINmOtor,LOW);
      }
        
}



