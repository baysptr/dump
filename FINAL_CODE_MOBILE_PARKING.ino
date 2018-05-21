#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <NewPing.h>

#define TRIGGER_PIN 5
#define ECHO_PIN 4
#define MAX_DISTANCE 200

ESP8266WiFiMulti WiFiMulti;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Servo myservo;

int led_satu = 12;
int led_dua = 13;

int LoopTime_Soll = 9;
int LoopTime_Angepasst = LoopTime_Soll;
int LoopTime_Bisher = LoopTime_Soll;
unsigned long LoopTime_Start = 0;

void setup() {
  Serial.begin(115200);

  myservo.attach(2);
  myservo.write(45);
  pinMode(led_satu, OUTPUT);
  pinMode(led_dua, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFiMulti.addAP("syandana A-9", "gede999471");
  while((WiFiMulti.run() != WL_CONNECTED)){
    Serial.println("HTTP is Works!");
    delay(5000);
  }
}

void loop() {
  Serial.println("scan start");

  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      sendReq(WiFi.SSID(i));
      delay(10);
    }
  }
  Serial.println("");

  LoopTime_Bisher = millis() - LoopTime_Start;   
  if(LoopTime_Bisher < LoopTime_Soll){
    delay(LoopTime_Soll - LoopTime_Bisher);
  }
  LoopTime_Angepasst = millis() - LoopTime_Start;
  LoopTime_Start = millis();
}

void sendReq(String nopol){
  HTTPClient http;
        
  http.begin("http://128.199.133.21/mobile_parking/public/api/in_device");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
  int httpCode = http.POST("nopol=" + nopol);

  if(httpCode > 0){
    String payload = http.getString();
    Serial.println("Code: " + payload);
    controllerGate(payload);
  }

  http.end();
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}

void controllerGate(String gate){
  if(gate.toInt() == 0){
    return ;
  }else{
    digitalWrite(led_satu, HIGH);
    myservo.write(125);
  
    int nilai_sensor = sonar.ping_cm();
    Serial.println("Jarak: " + nilai_sensor);
    if(nilai_sensor <= 10){
      digitalWrite(led_satu, LOW);
      digitalWrite(led_dua, HIGH);
      delay(2000);
      kirimBarcode(gate);
      delay(2000);
    }else{
      digitalWrite(led_satu, LOW);
      digitalWrite(led_dua, HIGH);
      myservo.write(45);
    }
  }
}

void ledTutup(){
    digitalWrite(led_satu, HIGH);
    delay(500);
    digitalWrite(led_dua, HIGH);
    delay(500);
    digitalWrite(led_satu, LOW);
    delay(500);
    digitalWrite(led_dua, LOW);
    delay(500);
    digitalWrite(led_satu, HIGH);
    delay(500);
    digitalWrite(led_dua, HIGH);
    delay(500);
    digitalWrite(led_satu, LOW);
    delay(500);
    digitalWrite(led_dua, LOW);
    delay(500);
}

void ledPending(){
    digitalWrite(led_satu, HIGH);
    digitalWrite(led_dua, HIGH);
    delay(300);
    digitalWrite(led_satu, LOW);
    digitalWrite(led_dua, LOW);
    delay(300);
    digitalWrite(led_satu, HIGH);
    digitalWrite(led_dua, HIGH);
    delay(300);
    digitalWrite(led_satu, LOW);
    digitalWrite(led_dua, LOW);
    delay(300);
}

void kirimBarcode(String nopol){
  HTTPClient http;
        
  http.begin("http://128.199.133.21/mobile_parking/public/api/parkir");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
  int httpCode = http.POST("id=" + nopol);

  if(httpCode > 0){
    String payload = http.getString();
    Serial.println("Code: " + payload);
  }

  http.end();
}
