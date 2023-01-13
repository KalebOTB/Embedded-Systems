#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
//#include <time.h>
#include <stdio.h>
#include <string.h>

#define SSID "Greenhouse"
#define ID_COUNT 3

unsigned char serialData[2];
unsigned char serialDataIdx = 0;
float pH = -1;
short tds = -1;
/*
 * Order of bits starting from bit 0: phosphorus, nitrogen, potassium
 * calcium, magnesium
 */
unsigned char nutrientReservoirLevels = 0b00000000; //(IE phosphorus (ID=2) is 
unsigned long times[ID_COUNT];  //times identified by id

AsyncWebServer server(80);

String convertTime(unsigned long t){
  float ret_t = (float)(millis()-t)/1000; //Convert to seconds
  if(ret_t >= 60) ret_t=ret_t/60; //Convert to minutes
  else return String(ret_t)+"s ago";
  if(ret_t >= 60) ret_t=ret_t/60; //Convert to hours
  else return String(ret_t)+"m ago";
  if(ret_t >= 24) ret_t=ret_t/24; //Convert to hours
  return String(ret_t)+"d ago";
}

String processorHTML(const String &var){
  unsigned char temp;
  if(var=="pH"){
    return String(pH);
  }else if(var=="tds"){
    return String(tds);
  }else if(var=="phosphorus"){
    temp = ((nutrientReservoirLevels>>0) & 0b00000001);
    return temp ? String("Good") : String("Low");
  }else if(var=="nitrogen"){
    temp = ((nutrientReservoirLevels>>1) & 0b00000001);
    return temp ? String("Good") : String("Low");
  }else if(var=="potassium"){
    temp = ((nutrientReservoirLevels>>2) & 0b00000001);
    return temp ? String("Good") : String("Low");
  }else if(var=="calcium"){
    temp = ((nutrientReservoirLevels>>3) & 0b00000001);
    return temp ? String("Good") : String("Low");
  }else if(var=="magnesium"){
    temp = ((nutrientReservoirLevels>>4) & 0b00000001);
    return temp ? String("Good") : String("Low");
  }else if(var=="pHTime"){
    return String(times[0]);
  }else if(var=="tdsTime"){
    return String(times[1]);
  }else if(var=="phosphorusTime"){
    return String(times[1]);
  }else if(var=="nitrogenTime"){
    return String(times[1]);
  }else if(var=="potassiumTime"){
    return String(times[1]);
  }else if(var=="calciumTime"){
    return String(times[1]);
  }else if(var=="magnesiumTime"){
    return String(times[1]);
  }
  return "Invalid";
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  LittleFS.begin();
  WiFi.softAP(SSID);
  //Setup HTTP server and begin
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processorHTML);
  });
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/styles.css", "text/css");
  });
  server.begin();
  initSensorData();
  //TESTING
  randomSeed(0);
  //END
}

void loop() {
  if(Serial.available()){
    //Byte 0 determines type of data (ie pH, TDS, reservoirs)
    serialData[serialDataIdx]=Serial.read();
    serialDataIdx++;
    if(serialDataIdx==2){
      updateSensorData();
      serialDataIdx=0;
    }
  }
}

 /*
  * On power up we will initalize the time checked to 0
  */
void initSensorData(){
  unsigned char dataBuf[6];
  size_t readSize;
  File data = LittleFS.open("/sensorData.bin","r");
  File tempData = LittleFS.open("/sensorData.bin.tmp","w+");
  while(readSize = data.readBytes((char *)dataBuf,6)){
    Serial.write(dataBuf[0]); //Request sensor data
    Serial.println("Waiting on Sensor Data");
    while(!Serial.available()); //Wait for response
    Serial.println("Sensor Data Recieved");
    dataBuf[1] = (unsigned char)Serial.read();
    memset(&dataBuf[2],0,sizeof(unsigned long));
    tempData.write(dataBuf,6);
  }
  data.close();
  tempData.close();
  LittleFS.remove("/sensorData.bin");
  LittleFS.rename("/sensorData.bin.tmp", "/sensorData.bin");
  readSensorData();
}
/*
 * Sensor data will be stored in a binary file.
 * Each set of data will be 6 (0 to 5) bytes long broken down as follows:
 * Byte 0 = Sensor ID,
 * Byte 1 = Sensor value
 * Byte 2-5 = Milliseconds based on execution time of ESP8266 
 */
void updateSensorData(){
  //FOR TESTING PURPOSES ONLY
  serialData[0]=(unsigned char)random(0,2);
  Serial.println(serialData[0]);
  serialData[1]=(unsigned char)random(0,256);
  //END
  unsigned char dataBuf[6];
  size_t readSize;
  File data = LittleFS.open("/sensorData.bin","r");
  File tempData = LittleFS.open("/sensorData.bin.tmp","w+");
  while(readSize = data.readBytes((char *)dataBuf,6)){
    if(dataBuf[0]==serialData[0]){
      dataBuf[1]=serialData[1];
      unsigned long m = millis();
      memcpy(&dataBuf[2],&m,sizeof(unsigned long));
    }
    tempData.write(dataBuf,6);
  }
  data.close();
  tempData.close();
  LittleFS.remove("/sensorData.bin");
  LittleFS.rename("/sensorData.bin.tmp", "/sensorData.bin");
  readSensorData();
}
 
void readSensorData(){
  unsigned char id;
  unsigned char val;
  unsigned long time;
  unsigned char dataBuf[6];
  size_t readSize;
  File data = LittleFS.open("/sensorData.bin","r");
  while(readSize = data.readBytes((char *)dataBuf,6)){
    id=dataBuf[0];
    val=dataBuf[1];
    time=(unsigned long)dataBuf[5]<<24;
    time|=(unsigned long)dataBuf[4]<<16;
    time|=(unsigned long)dataBuf[3]<<8;
    time|=(unsigned long)dataBuf[2];
    setSensorData(id,val,time);
  }
  data.close();
}
/*
 * Sensor IDS:
 * 0 = pH,
 * 1 = TDS,
 * 2 = Nutrient Reservoirs
 */
void setSensorData(unsigned char id, unsigned char val, unsigned long time){
  switch(id){
    case 0:
      pH = ((float)val/256)*14;
      times[id] = time;
      break;
    case 1:
      tds = ((float)val/256)*10000;
      times[id] = time;
      break;
    case 2:
      nutrientReservoirLevels = val;
      times[id] = time;
      break;
    default:
      Serial.println("Error: Data id not found!");
      break;
  }



  
}
