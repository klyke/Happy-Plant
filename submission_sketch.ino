//5v Moisture 0: dry, 650-750: Saturated
//3.3V Moisture 0: dry, 300-350: Saturated

//Light: 505 max

#include <DHT.h>

#define SSID <ssid>   // SSID
#define PASS <password>     // Network Password
#define HOST "http://www.happyplant.heliohost.org"

#define MOISTURE_PIN A0
#define DHT_PIN 7
#define DHT_TYPE DHT11
#define LIGHT_PIN A5

DHT dht(DHT_PIN, DHT_TYPE);

long delay_time = 600000; //10 minutes

void setup()
{
  Serial.begin(115200);
  
  // Test ESP8266 module.
  Serial.println("AT+RST");
  delay(2000);

  int incomingByte;
  
  // send data only when you receive data:
  if (Serial.available() > 0) 
  {
    // read the incoming byte:
    incomingByte = Serial.read();
    Serial.println(incomingByte, DEC);
    if (incomingByte == 65)
    {
      connectWiFi();
    }
  }
}

boolean connectWiFi()
{
  Serial.println("AT+GMR");
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
    Serial.println("Connection");
    return true;
  }
  else{
    Serial.println("No Connection");
    return false;
  }
}

String makeDataString()
{
  int moisture = analogRead(MOISTURE_PIN);
  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  int light = analogRead(LIGHT_PIN)/2.0;

  temp = temp*1.8 + 32.0;//convert to fahrenhiet
  float light_f = 100.0 * light / 505.0;
  float moisture_f = 100.0*moisture/700.0;

  String data = "?moisture=";
  data += moist;
  data += "&temperature=";
  data += temp;
  data += "&humidity=";
  data += humid;
  data += "&light=";
  data += light;

  return data;
}

void sendData()
{
    String phpAddress = <phpAddress>;
    String data = makeDataString();
    
    //Set up TCP connection.
    String tcp = "AT+CIPSTART=\"TCP\",\"";
    tcp += HOST;
    tcp += "\",80";
    Serial.println(tcp);
    
    delay(5000);
    
    while(Serial.available())
    {
      String tmpResp = Serial.readString();
      Serial.println(tmpResp);
    }
    
    // Send data.
    String postRequest = "POST " +  phpAddress + " HTTP/1.0\r\n";
    postRequest += "Host: ";
    postRequest += HOST;
    postRequest += "\r\n";
    postRequest += "Accept: *";
    postRequest += "/*\r\n";
    postRequest += "Content-Length: ";
    postRequest += data.length();
    postRequest += "\r\n";
    postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
    postRequest += "\r\n" + data;
    
    Serial.print("AT+CIPSEND=");
    Serial.println(postRequest.length());
    delay(5000);
    while(Serial.available())
    {
      String tmpResp = Serial.readString();
      Serial.println(tmpResp);
    }
    Serial.println(postRequest);
    delay(9000);
    while(Serial.available())
    {
      String tmpResp = Serial.readString();
      Serial.print(tmpResp);
    }
    Serial.println("AT+CIPCLOSE");
}

void loop()
{
  sendData();
  delay(delay_time);
}

