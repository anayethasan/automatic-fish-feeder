#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>

Servo servo;

// Replace with your Wi-Fi credentials
#define WIFI_SSID "Niloy patowary"
#define WIFI_PASSWORD "Leo messi 10"

// Firebase configuration objects
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

FirebaseData timer, feed;
String stimer;
String Str[] = {"00:00", "00:00", "00:00"};
int i, feednow = 0;

void setup() {
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to Wi-Fi: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP Client
  timeClient.begin();

  // Setup Firebase configuration
  firebaseConfig.host = "automatic-fish-feeder-15985-default-rtdb.firebaseio.com";
  firebaseConfig.api_key = "t1kXasvjzHYfpqv7hQy9QpBPqEaRed8oQHlgYA89";

  // (Optional) Setup authentication, if needed
  // firebaseAuth.user.email = "your-email@example.com";
  // firebaseAuth.user.password = "your-password";

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Attach servo to pin D3
  servo.attach(D3);
}
void loop() {
  Firebase.getInt(feed, "feednow");
  feednow = feed.to<int>();
  Serial.println(feednow);
  if (feednow == 1) { // Direct Feeding
      servo.writeMicroseconds(1000); // rotate clockwise
      delay(700); // allow to rotate for n microseconds, you can change this to your need
      servo.writeMicroseconds(1500); // stop rotation
      feednow = 0;
      Firebase.setInt(feed, "/feednow", feednow);
      Serial.println("Fed");
  } else { // Scheduling feed
    for (i = 0; i < 3; i++) {
      String path = "timers/timer" + String(i);
      Firebase.getString(timer, path);
      stimer = timer.to<String>();
      Str[i] = stimer.substring(9, 14);
    }
    timeClient.update();
    String currentTime = String(timeClient.getHours()) + ":" + String(timeClient.getMinutes());
    if (Str[0] == currentTime || Str[1] == currentTime || Str[2] == currentTime) {
      servo.writeMicroseconds(1000); // rotate clockwise
      delay(700); // allow to rotate for n microseconds, you can change this to your need
      servo.writeMicroseconds(1500); // stop rotation
      Serial.println("Success");
      delay(60000);
    }
  }
  Str[0] = "00:00";
  Str[1] = "00:00";
  Str[2] = "00:00";
}

