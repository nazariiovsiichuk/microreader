void enterToWifiMenu(void) {
  oled.clear();
  oled.home();
  oled.line(0, 10, 127, 10);
  oled.print(F("WI-FI МЕНЮ"));
  checkBatteryCharge();
  drawBatteryCharge();
  oled.update();

  if (sets.staModeEn) {
    oled.clear();
    oled.home();
    oled.line(0, 10, 127, 10);
    oled.print(F("WI-FI МЕНЮ"));
    oled.setCursor(0, 2);
    oled.print(F("Connecting..."));
    checkBatteryCharge();
    drawBatteryCharge();
    oled.update();

    WiFi.mode(WIFI_STA);
    WiFi.begin(sets.staSsid, sets.staPass);
    oled.setCursor(66, 2);
    for (uint8_t i = 0; i < 10; i++) {
      if (WiFi.status() != WL_CONNECTED) {
        oled.print(".");
        oled.update();
        delay(1000);
      } else {
        drawStaMenu();
        ui.attachBuild(build);
        ui.attach(action);
        ui.start();
        ui.enableOTA();
        while (1) {
          ok.tick();
          up.tick();
          down.tick();
          data.tick();

          if (up.click() || up.hold()) {
            uiTimer = millis();
            sets.dispContrast = constrain(sets.dispContrast + 10, 10, 100);
            byte con = map(sets.dispContrast, 10, 100, 1, 255);
            oled.setContrast(con);
          }

          if (down.click() || down.hold()) {
            uiTimer = millis();
            sets.dispContrast = constrain(sets.dispContrast - 10, 10, 100);
            byte con = map(sets.dispContrast, 10, 100, 1, 255);
            oled.setContrast(con);
          }

          if (ok.click() || ((millis() - uiTimer) >= WIFI_TIMEOUT_S * 1000)) {
            uiTimer = millis();
            checkFileSystem();
            drawMainMenu();
            data.update();
            WiFi.mode(WIFI_OFF);
            return;
          }

          if (millis() - batTimer >= 5000) {
            batTimer = millis();
            drawStaMenu();
          }
          ui.tick();
          yield();
        }
      }
    }
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(sets.apSsid, sets.apPass);
  drawApMenu();
  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
  ui.enableOTA();
  while (1) {
    ok.tick();
    up.tick();
    down.tick();
    data.tick();

    if (up.click() || up.hold()) {
      uiTimer = millis();
      sets.dispContrast = constrain(sets.dispContrast + 10, 10, 100);
      uint8_t con = map(sets.dispContrast, 10, 100, 1, 255);
      oled.setContrast(con);
    }

    if (down.click() || down.hold()) {
      uiTimer = millis();
      sets.dispContrast = constrain(sets.dispContrast - 10, 10, 100);
      uint8_t con = map(sets.dispContrast, 10, 100, 1, 255);
      oled.setContrast(con);
    }

    if (ok.click() || ((millis() - uiTimer) / 1000 >= WIFI_TIMEOUT_S)) {
      uiTimer = millis();
      checkFileSystem();
      drawMainMenu();
      data.update();
      WiFi.softAPdisconnect();
      WiFi.mode(WIFI_OFF);
      return;
    }
    if (millis() - batTimer >= 5000) {
      batTimer = millis();
      drawApMenu();
    }
    ui.tick();
    yield();
  }
}