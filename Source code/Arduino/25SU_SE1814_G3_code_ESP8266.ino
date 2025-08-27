/*********
  ESP8266 Web Server with Obstacle & CO Detection (3-second buzzer)
*********/

#include <ESP8266WiFi.h>

// WiFi credentials
const char* ssid = "FGApollo";
const char* password = "12345678";

WiFiServer server(80);
String header;

// GPIO pins
const int output5 = 5;
const int buzzerPin = 16;     // Buzzer GPIO16
const int obstaclePin = 4;    // Obstacle sensor GPIO4
const int mq7AnalogPin = A0;  // MQ-7 analog pin
const int coAlertPin = 0;     // D3 = GPIO0
const int pirPin = 12;        // GPIO12 = D6
const int pirLedPin = 13;     // GPIO13 = D7



// States
String output5State = "off";
int mq7Value = 0;
bool danger = false;
bool obstacleActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerPeriod = 3000;  // 3 seconds



// Timing variables
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  pinMode(output5, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(obstaclePin, INPUT_PULLUP);
  digitalWrite(output5, LOW);
  digitalWrite(buzzerPin, LOW);
  pinMode(coAlertPin, OUTPUT);
  digitalWrite(coAlertPin, LOW);  // mặc định không cảnh báo

  pinMode(pirPin, INPUT);
  pinMode(pirLedPin, OUTPUT);
  digitalWrite(pirLedPin, LOW);


  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // Read sensors
  mq7Value = analogRead(mq7AnalogPin);
  bool currentObstacle = (digitalRead(obstaclePin) == LOW);  // LOW = obstacle detected
  bool coDanger = mq7Value > 800;                            // Adjust threshold as needed

  // Danger from obstacle or CO
  if (currentObstacle || coDanger) {
    if (!obstacleActive) {
      obstacleActive = true;
      buzzerStartTime = millis();
      digitalWrite(buzzerPin, HIGH);

      danger = true;

      if (currentObstacle) {
        Serial.println("Obstacle detected - Buzzer ON");
        Serial.println("OBSTACLE:DETECTED");  // ✅ Gửi về Arduino
      }
      if (coDanger) {

        digitalWrite(coAlertPin, coDanger ? HIGH : LOW);
        Serial.println("CO detected - Buzzer ON" + String(mq7Value));
      }
    } else {
      if (millis() - buzzerStartTime >= buzzerPeriod) {
        digitalWrite(buzzerPin, LOW);
        Serial.println("Buzzer OFF after 3 seconds");

        if (digitalRead(obstaclePin) == LOW || analogRead(mq7AnalogPin) > 800) {
          buzzerStartTime = millis();
          digitalWrite(buzzerPin, HIGH);
          Serial.println("Danger still present - Buzzer ON again" + String(mq7Value));
        } else {
          obstacleActive = false;
          danger = false;
        }
      }
    }
  } else {
    if (obstacleActive) {
      obstacleActive = false;
      digitalWrite(buzzerPin, LOW);
      danger = false;
      Serial.println("All clear - Buzzer OFF" + String(mq7Value));
    }
  }

  // PIR chỉ để bật đèn
  bool motionDetected = digitalRead(pirPin) == HIGH;
  digitalWrite(pirLedPin, motionDetected ? HIGH : LOW);

  handleWebClient();
  delay(100);
}


void handleWebClient() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New Client");
  String currentLine = "";
  currentTime = millis();
  previousTime = currentTime;

  while (client.connected() && currentTime - previousTime <= timeoutTime) {
    currentTime = millis();
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      header += c;

      if (c == '\n') {
        if (currentLine.length() == 0) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          if (header.indexOf("GET /5/on") >= 0) {
            output5State = "on";
            digitalWrite(output5, HIGH);
          } else if (header.indexOf("GET /5/off") >= 0) {
            output5State = "off";
            digitalWrite(output5, LOW);
          }

          // HTML page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          // Add meta refresh for 1 second
          client.println("<meta http-equiv=\"refresh\" content=\"1\">");
          client.println("<style>html{font-family:Helvetica; display:inline-block; margin:0 auto; text-align:center;}");
          client.println(".button{background-color:#195B6A;border:none;color:white;padding:16px 40px;");
          client.println("text-decoration:none;font-size:30px;margin:2px;cursor:pointer;}");
          client.println(".button2{background-color:#77878A;}</style></head>");

          client.println("<body style=\"background-image:url('https://marketplace.canva.com/EAFC87jteHk/1/0/900w/canva-blue-minimalist-dreamy-painted-clouds-phone-wallpaper-l5dLrLIsfSs.jpg');\">");
          client.println("<h1>Group 3 ESP8266 Web Server</h1>");

          client.println("<p>Light - State " + output5State + "</p>");
          if (output5State == "off") {
            client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
          } else {
            client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
          }

          client.println("<p>MQ-7 Value: " + String(mq7Value) + "</p>");
          client.println("<p>Obstacle: " + String(digitalRead(obstaclePin) == LOW ? "DETECTED" : "CLEAR") + "</p>");

          if (danger) {
            client.println("<p style='color:red; font-weight:bold;'> DANGER DETECTED!</p>");
            if (digitalRead(obstaclePin) == LOW) {
              client.println("<p style='color:red;'>Obstacle present!</p>");
            }
            if (mq7Value > 400) {
              client.println("<p style='color:red;'>High CO detected!</p>");
            }
          }

          client.println("</body></html>");
          client.println();
          break;
        } else {
          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;
      }
    }
  }

  header = "";
  client.stop();
  Serial.println("Client disconnected");
}