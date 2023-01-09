#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define SSID "Greenhouse"

unsigned char serialData[2];
unsigned char serialDataIdx = 0;
float pH = -1;
unsigned short tds = -1;

AsyncWebServer server(80);

String processor(const String &var){
  if(var=="pH"){
    return String(pH);
  }else if(var=="tds"){
    return String(tds);
  }
  return "Invalid";
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  LittleFS.begin();
  WiFi.softAP(SSID);
  //Setup HTTP server and begin
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/styles.css", "text/css");
  });
  server.begin();
}

void loop() {
  if(Serial.available()){
    //Byte 0 determines type of data (ie pH, TDS, reservoirs)
    serialData[serialDataIdx]=Serial.read();
    serialDataIdx++;
    if(serialDataIdx==1){
      processData();
    }
  }
}

char lookup[64][32] = {
  {"pH"},{"TDS"},
};
void processData(){
  serialData[0]=0x01;
  char dataBuf[128];
  unsigned int lastPos=0;
  size_t readSize;
  File data = LittleFS.open("/data.csv","r+");
  //Read header
  //data.readBytesUntil('\n',dataBuf,128);

  char *name;
  char *value;
  char *time;
  Serial.println(lookup[1]);
  while(data.readBytesUntil('\n',dataBuf,128)){
    name = strtok(dataBuf,",");
    value = strtok(NULL, ",");
    time = strtok(NULL, "\r");
    Serial.println(name);
    Serial.println(value);
    Serial.println(time);
    if(strcmp(name,lookup[serialData[0]])) break;
    lastPos=data.position();
  }
  data.close();
}
