#define MAX_FILENAME_LEN  12
#define AP_DEFAULT_SSID   "Reader AP"   
#define AP_DEFAULT_PASS   "00000000"   
#define STA_DEFAULT_SSID  ""           
#define STA_DEFAULT_PASS  ""            
#define STA_CONNECT_EN    0            
#define OLED_CONTRAST     100           
#define LEFT_MODE         0             
#define WIFI_TIMEOUT_S    300           
#define UP_BTN_PIN        14            
#define OK_BTN_PIN        12            
#define DWN_BTN_PIN       13            
#define IIC_SDA_PIN       4             
#define IIC_SCL_PIN       5             
#define EE_KEY            'B'           
#define VBAT_FULL_MV      3600          
#define VBAT_EMPTY_MV     2600          
#define _EB_DEB           25            
#define GAME_SPEED        350           
#define CALCUL_TYPE       int64_t       

#define T_SEGMENT 4            
#define MAX_WIDTH 64
#define MAX_HEIGHT 128

#include <Wire.h>           
#include <FileData.h>       
#include <LittleFS.h>       
#include <GyverPortal.h>    
#include <StringUtils.h>    
#include <GyverOLED.h>  
#include <EncButton.h>      
#include <TJpg_Decoder.h>   
#include <GyverTimer.h>     

GyverPortal ui(&LittleFS);          
GyverOLED<SSD1306_128x64> oled;     
Button up(UP_BTN_PIN);              
Button ok(OK_BTN_PIN);              
Button down(DWN_BTN_PIN);           
ADC_MODE(ADC_VCC);                  
GTimer_ms gameTimer(GAME_SPEED); 

struct {                                
  char apSsid[21] = AP_DEFAULT_SSID;    
  char apPass[21] = AP_DEFAULT_PASS;    
  char staSsid[21] = STA_DEFAULT_SSID;  
  char staPass[21] = STA_DEFAULT_PASS;  
  bool staModeEn = STA_CONNECT_EN;      
  int dispContrast = OLED_CONTRAST;     
  bool leftmode = LEFT_MODE;            
  uint8_t tetrisSegment = T_SEGMENT;    
  uint16_t dinoBestScore = 0;           
  uint16_t tetrBestScore = 0;           
  uint16_t snakeBestScore = 0;          
} sets;
FileData data(&LittleFS, "/data.dat", EE_KEY, &sets, sizeof(sets));  
#define SEGMENT (sets.tetrisSegment)

String selectedFile = "";  
String fileNames = "";     
int16_t fileCount = 0;     
int16_t badCount = 0;      
int16_t cursor = 0;        
int16_t batMv = 3000;      
uint32_t uiTimer = 0;      
uint32_t batTimer = 0;     
uint8_t oledbuf[MAX_WIDTH * MAX_HEIGHT]; 
uint8_t buttons;

bool loadingFlag = 0;      
#define X0 16  
uint8_t WIDTH = (64/SEGMENT - 16/SEGMENT);          
uint8_t HEIGHT = (128/SEGMENT);                     

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);         
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(OK_BTN_PIN, INPUT_PULLUP);
  pinMode(DWN_BTN_PIN, INPUT_PULLUP);   

  batMv = ESP.getVcc();

  if (!digitalRead(UP_BTN_PIN)) {       
    enterToServiceMode();               
  }

  ok.setHoldTimeout(1500);              
  up.setHoldTimeout(1500);
  up.setStepTimeout(100);
  down.setStepTimeout(100);

  selectedFile.reserve(MAX_FILENAME_LEN + 6);
  fileNames.reserve(4096);              

  while (!LittleFS.begin()) {           
    LittleFS.format();
  }

  data.read();  

  for (uint8_t i = 0; i < 6; i++) {  
    digitalWrite(LED_BUILTIN, LOW);
    delay(30);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
  }

  digitalWrite(LED_BUILTIN, HIGH);

  Wire.setClock(600E3);
  oled.flipH(sets.leftmode); 
  oled.flipV(sets.leftmode); 
  if (sets.leftmode) {  
    up = Button(DWN_BTN_PIN);
    down = Button(UP_BTN_PIN);
  }
  oled.clear();              
  oled.update();             
  oled.autoPrintln(true);    

  checkFileSystem();
  drawMainMenu();

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(oled_output); 
  
  WIDTH = (64/SEGMENT - 16/SEGMENT);          
  HEIGHT = (128/SEGMENT);                     
}

void loop() {
  up.tick();
  ok.tick();
  down.tick();
  data.tick();  

  if (up.click()) {                                    
    uiTimer = millis();                                
    cursor = constrain(cursor - 1, 0, fileCount - 1);  
    drawMainMenu();                                    
  } else if (down.click()) {                           
    uiTimer = millis();                                
    cursor = constrain(cursor + 1, 0, fileCount - 1);  
    drawMainMenu();                                    
  }

  if (ok.click()) {                   
    uiTimer = millis();                 
    if (fileCount) {
      if (selectedFile.endsWith(".txt")) {  
        enterToReadTxtFile();              
      } else if (selectedFile.endsWith(".itxt") || selectedFile.endsWith(".h")) {  
        enterToReadBmpFile();              
      } else if (selectedFile.endsWith(".jpg")) {
        enterToReadJpgFile();
      }
    }
  }

  if (ok.hold()) {
    uiTimer = millis();  
    enterToWifiMenu();  
  }

  if (up.hold()) {
    uiTimer = millis();  
    enterToGameMode();
  }

  if (millis() - uiTimer >= 5000) {  
    uiTimer = millis();
    drawMainMenu();
  }
}