#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFiUdp.h>

const char* ssid = "MI4 LTE";
const char* password = "12345678";
String APIKEY = "bb627c72e172c70a7bd7c5e2e76eb9bc";                                 
String CityID = "1566083";                                 
int refresh_time = 10;

const long utcOffsetInSeconds = 25200;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;
char servername[]="api.openweathermap.org";            
String result;

int  counter = 60;                                      

String weatherDescription ="";
String data ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float tmrTemperature;
float tmrHumidity;
float tmrFeel;

int Hr;
int Min;
int Sec;
int Day;

void setup() {
  Serial.begin(115200);
  Serial.println("");  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
  }
  Serial.flush();
  Serial.print("@");
  timeClient.begin();
  getWeatherData();
  getTime();
  delay(1000);
  serialSend(weatherDescription, Temperature, Pressure, Humidity, Day, Hr, Min, Sec);
}

unsigned long timer1;
unsigned long timer2;
unsigned long timer3;
void loop() {
    if(counter == 3)                              
    {
      counter = 0;
      getWeatherData();
    }else
    {
      if ( (unsigned long) (millis() - timer2) > 3600000)
      {
        counter++;
        timer2 = millis();
      }
    }
//    if ( (unsigned long) (millis() - timer3) > 20000)
//      {
//        displayWeather(weatherLocation,weatherDescription);
//        displayConditions(Temperature,Humidity,Pressure);
//        timer3 = millis();
//      }
    if ( (unsigned long) (millis() - timer1) > 10000)
    {
        serialSend(weatherDescription, Temperature, Pressure, Humidity, Day, Hr, Min, Sec);
        getTime();
        timer1 = millis();   
    }    
}

void getTime(){
        timeClient.update();
        Day = timeClient.getDay();  
        Hr = timeClient.getHours();
        Min = timeClient.getMinutes();
        Sec = timeClient.getSeconds();
}

void getWeatherData()                             
{
    if (client.connect(servername, 80))   
            {                                       
            client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
            client.println("Host: api.openweathermap.org");
            client.println("User-Agent: ArduinoWiFi/1.1");
            client.println("Connection: close");
            client.println();
            } 
    else {
           Serial.println("!");        //error message if no client connect
         }
  
    while(client.connected() && !client.available()) 
    delay(1); 
    //waits for data
    boolean k = false;
    while (client.connected() || client.available())    
         {                                           
           char c = client.read();   
           //gets byte from ethernet buffer
           if (c=='{') {
              k=true;
           }
           if (k==true) result = result+c;
         }
    k= false;
    client.stop();                                     
    result.replace('[', ' ');
    result.replace(']', ' ');
    char jsonArray [result.length()+1];
    result.toCharArray(jsonArray,sizeof(jsonArray));
    jsonArray[result.length() + 1] = '\0';
    DynamicJsonBuffer json_buf(1024);
    JsonObject &root = json_buf.parseObject(jsonArray);
    
    if (!root.success())
      {
        Serial.println("parseObject() failed");
      }

      String location = root["name"];
      String country = root["sys"]["country"];
      float temperature = root["main"]["feels_like"];
      float humidity = root["main"]["humidity"];
      String weather = root["weather"]["main"];
      String description = root["weather"]["description"];
      float pressure = root["main"]["pressure"];
      weatherDescription = description;
      weatherLocation = location;
      Country = country;
      Temperature = temperature;
      Humidity = humidity;
      Pressure = pressure;
      client.flush();

}

void serialSend(String description, float temperature, float pressure, float humidity, int d, int h, int m, int s)
{
      Serial.print("#");
      if (h<10) {
          Serial.print(0);
         Serial.print(int(h));
      } else Serial.print(int(h));
      delay(100);
////      Serial.print("%");
      if (m<10) {
          Serial.print(0);
         Serial.print(int(m));
      } else Serial.print(int(m));
      delay(100);
////      Serial.print("%");
////      Serial.print("%");
      Serial.print(0);
      Serial.print(d);
      delay(100);
      Serial.print(int(temperature));
      delay(100);
      Serial.print(int(humidity));
      delay(100);
////      Serial.print("%");
//      Serial.print(h);
////      Serial.print("%");
//      Serial.print(m);
////      Serial.print("%");
//      Serial.print(s);
////      Serial.print("%");      
//      Serial.print("*");
//      Serial.print(weatherDescription);
//      Serial.print("*"); 
//      Serial.println("$");
}

void displayWeather(String location,String description)
{
      Serial.print("Country: ");
      Serial.println(Country );
      Serial.print("Weather Description: ");
      Serial.println(description);
}

void displayConditions(float Temperature,float Humidity, float Pressure)
{
     //Printing Temperature
     Serial.print("Nhiet do: "); 
     Serial.print(Temperature,1);
     Serial.print("*");
     Serial.println("C "); 
     //Printing Humidity                                        
     Serial.print("Do am:");                       
     Serial.print(Humidity,0);
     Serial.println(" %"); 
     //Printing Pressure 
     Serial.print("Ap suat: ");
     Serial.print(Pressure,1);
     Serial.println(" hPa"); 
     Serial.println("####################");
}

void displayGettingData()
{
     Serial.println("Getting data.........");
}
