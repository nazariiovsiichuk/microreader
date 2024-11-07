void build() {               
  GP.BUILD_BEGIN(400);          
  GP.THEME(GP_DARK);              
  GP.PAGE_TITLE("Wi-Fi Reader");   
  GP.FORM_BEGIN("/cfg");         
  GP.GRID_RESPONSIVE(600);       
  M_BLOCK(                         
    GP.SUBMIT("Зберегти");       
    M_BLOCK_TAB(                  
      "Точка доступа",           
      GP.TEXT("apSsid", "ім'я мережі", sets.apSsid, "", 20);
      GP.BREAK();
      GP.TEXT("apPass", "Пароль", sets.apPass, "", 20); GP.BREAK(););
    M_BLOCK_TAB(                  
      "Підключення до мережі",        
      GP.TEXT("staSsid", "ім'я мережі", sets.staSsid, "", 20);
      GP.BREAK();
      GP.TEXT("staPass", "Пароль", sets.staPass, "", 20); GP.BREAK();
      M_BOX(GP_CENTER, GP.LABEL("Автоматичне підключення"); GP.SWITCH("staEn", sets.staModeEn);););
    M_BLOCK_TAB(                 
      "Другое",            
      M_BOX(GP_CENTER, GP.LABEL("Режим лівші"); GP.SWITCH("leftEn", sets.leftmode);); GP.BREAK();
      
      M_BOX(GP_CENTER, GP.LABEL("Яркість"); GP.SLIDER("con", sets.dispContrast, 10, 100););););
  GP.FORM_END();               
  M_BLOCK_TAB(                  
    "Оновлення прошивки",      
    GP.OTA_FIRMWARE();          
  );
  M_BLOCK_TAB(                
    "Файлова система",      
    GP.FILE_UPLOAD("file_upl");  
    GP.FILE_MANAGER(&LittleFS);
  );
  GP.BUILD_END();                  
}

void action(GyverPortal& p) {      
  if (p.form("/cfg")) {           
    p.copyStr("apSsid", sets.apSsid);
    p.copyStr("apPass", sets.apPass);
    p.copyStr("staSsid", sets.staSsid);
    p.copyStr("staPass", sets.staPass);
    p.copyBool("staEn", sets.staModeEn);
    p.copyBool("leftEn", sets.leftmode);
    p.copyInt("con", sets.dispContrast);
    byte con = map(sets.dispContrast, 10, 100, 1, 255);
    oled.setContrast(con);            
    uint8_t tmp_tseg;
    p.copyInt("tetrSeg", tmp_tseg);
    if (tmp_tseg != sets.tetrisSegment) {      
      sets.tetrisSegment = tmp_tseg;
      WIDTH = 64 / tmp_tseg - 16 / tmp_tseg;   
      HEIGHT = 128 / tmp_tseg;               
    }

    if (strlen(sets.apSsid) < 1)  strcpy(sets.apSsid, AP_DEFAULT_SSID);  
    if (strlen(sets.apPass) < 8)  strcpy(sets.apPass, AP_DEFAULT_PASS);   

    if (sets.staModeEn) {                                                 
      if (strlen(sets.staSsid) < 1 || strlen(sets.staPass) < 8) {         
        sets.staModeEn = false;                                         
      }
    }

    uint8_t contrast = map(sets.dispContrast, 10, 100, 1, 255);
    oled.setContrast(contrast);   
    data.update();              
  }
}
