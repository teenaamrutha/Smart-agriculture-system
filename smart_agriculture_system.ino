String myAPIkey = "ASLN9ORFZ951V5IC";  

#include <SoftwareSerial.h>
SoftwareSerial ESP8266(12, 13); // Rx,  Tx
long writingTimer = 17; 
long startTime = 0;
long waitTime = 0;



unsigned char check_connection=0;
unsigned char times_check=0;
boolean error;

int a,b,c,d,e,f; 
#include <Wire.h>//i2c communication library 
#include <LiquidCrystal_I2C.h>//i2c lcd library
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);//address,columns,rows
#include <dht.h>
#define dht_apin 2 // Analog Pin sensor is connected to
dht DHT; 
void setup()
{
//   lcd.begin();
  lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("SMART AGRICULTURE");
    lcd.setCursor(0,1);
    lcd.print("USING IOT      ");
 pinMode(3, INPUT);
 pinMode(4, INPUT);
 pinMode(6, OUTPUT);//led
 pinMode(7, OUTPUT);//motor
 pinMode(8, OUTPUT);//buzzer
 digitalWrite(6,LOW);
 digitalWrite(7,LOW);
 digitalWrite(8,LOW);
  Serial.begin(9600); 
  ESP8266.begin(9600); 
  startTime = millis(); 
  ESP8266.println("AT+RST");
 // ESP8266.println("AT+CWMODE=1");
  delay(2000);
 
 // Serial.println("Connecting to Wifi");
   while(check_connection==0)
  {
   // Serial.print(".");
  ESP8266.print("AT+CWJAP=\"project12\",\"project123456\"\r\n");
  ESP8266.setTimeout(5000);
 if(ESP8266.find("WIFI CONNECTED\r\n")==1)
 {
 Serial.println("WIFI CONNECTED");
 break;
 }
 times_check++;
 if(times_check>3) 
 {
  times_check=0;
   Serial.println("Trying to Reconnect..");
  }
  }
  lcd.clear();
}

void loop()
{
  waitTime = millis()-startTime;   
  if (waitTime > (writingTimer*1000)) 
  {
     DHT.read11(dht_apin);
    a=DHT.temperature;
     b=DHT.humidity;
   
   int z=analogRead(A0);
    c=z/46;

   d=digitalRead(3);
   e=digitalRead(4);
    
    String s2="IR:";
    if(a>33 || b>80)
    {
     
      digitalWrite(6,HIGH);
    }
    else
    {
      digitalWrite(6,LOW);
     
    }
     if(e==0)
    {
     s2=s2+"YES ";
      digitalWrite(8,HIGH);
    }
    else
    {
      s2=s2+"NO  ";
      digitalWrite(8,LOW);
     
    }
    if(d==1)
    {
     s2=s2+"NO WATER     ";
      digitalWrite(7,HIGH);
    }
    else
    {
      s2=s2+"WATER OK      ";
      digitalWrite(7,LOW);
     
    }
   
    lcd.clear();
    String s1="T:"+String(a)+" H:"+String(b)+" PH:"+String(c)+" ";
     lcd.setCursor(0,0);
    lcd.print(s1);
    lcd.setCursor(0,1);
    lcd.print(s2);
    writeThingSpeak();
    startTime = millis();   
  }
}




void writeThingSpeak(void)
{
  startThingSpeakCmd();
  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += myAPIkey;
  getStr +="&field1=";
  getStr += String(a);
   getStr +="&field2=";
  getStr += String(b);
   getStr +="&field3=";
  getStr += String(c);
   getStr +="&field4=";
  getStr += String(d);
   getStr +="&field5=";
  getStr += String(e);
  
  getStr += "\r\n\r\n";
  GetThingspeakcmd(getStr); 
}

void startThingSpeakCmd(void)
{
  ESP8266.flush();
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com IP address
  cmd += "\",80";
  ESP8266.println(cmd);
  Serial.print("Start Commands: ");
   Serial.println(cmd);

  if(ESP8266.find("Error"))
  {
     Serial.println("AT+CIPSTART error");
    return;
  }
}

String GetThingspeakcmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ESP8266.println(cmd);
  Serial.println(cmd);

  if(ESP8266.find(">"))
  {
    ESP8266.print(getStr);
     Serial.println(getStr);
    delay(500);
    String messageBody = "";
    while (ESP8266.available()) 
    {
      String line = ESP8266.readStringUntil('\n');
      if (line.length() == 1) 
      { 
        messageBody = ESP8266.readStringUntil('\n');
      }
    }
   // Serial.print("MessageBody received: ");
   // Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    ESP8266.println("AT+CIPCLOSE");     
   // Serial.println("AT+CIPCLOSE"); 
  } 
}
