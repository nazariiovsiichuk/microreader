#define SERV_MENU_LINES 3
uint8_t servCursor = 0;

void enterToServiceMode(void) {
  oled.clear();
  oled.update();
  digitalWrite(LED_BUILTIN, LOW);

  while (!LittleFS.begin()) {
    LittleFS.format();
  }

  drawServiceMenu();

  while (1) {
    up.tick();
    ok.tick();
    down.tick();
    data.tick();

    if (up.click()) {
      servCursor = constrain(servCursor - 1, 0, SERV_MENU_LINES - 1);
      drawServiceMenu();
    } else if (down.click()) {
      servCursor = constrain(servCursor + 1, 0, SERV_MENU_LINES - 1);
      drawServiceMenu();
    }

    if (ok.click()) {
      switch (servCursor) {
        case 0:
          data.write();
          drawResetNotify();
          drawServiceMenu();
          break;

        case 1:
          LittleFS.format();
          drawFormatNotify();
          drawServiceMenu();
          break;

        case 2:
          LittleFS.end();
          return;
      }
    }

    yield();
  }
}

void drawResetNotify(void) {
  oled.clear();
  oled.setScale(2);
  oled.setCursorXY(10, 16);
  oled.print(F("настройки"));
  oled.setCursorXY(16, 32);
  oled.print(F("сброшены"));
  oled.setScale(1);
  oled.update();
  delay(1500);
}

void drawFormatNotify(void) {
  oled.clear();
  oled.setScale(2);
  oled.setCursorXY(34, 16);
  oled.print(F("файлы"));
  oled.setCursorXY(22, 32);
  oled.print(F("удалены"));
  oled.setScale(1);
  oled.update();
  delay(1500);
}

void drawServiceMenu(void) {
  oled.clear();
  oled.line(0, 10, 127, 10);
  oled.print(F("FW.V1.2"));
  oled.setCursor(0, 2);

  oled.print(F(
    " СБРОС НАСТРОЕК\r\n"
    " ФОРМАТИРОВАНИЕ\r\n"
    " ВЫХОД\r\n"));

  oled.setCursor(0, servCursor + 2);
  oled.print(">");
  checkBatteryCharge();
  drawBatteryCharge();
  oled.update();
}