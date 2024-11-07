void checkBatteryCharge(void) {
  if (millis() - batTimer >= 5000) {
    batTimer = millis();
    batMv = ESP.getVcc();
  }
}

void drawBatteryCharge(void) {
  byte charge = constrain(map(batMv, VBAT_EMPTY_MV, VBAT_FULL_MV, 0, 12), 0, 12);
  oled.setCursorXY(110, 0);
  oled.drawByte(0b00111100);
  oled.drawByte(0b00111100);
  oled.drawByte(0b11111111);
  for (uint8_t i = 0; i < 12; i++) {
    if (i < 12 - charge) oled.drawByte(0b10000001);
    else oled.drawByte(0b11111111);
  }
  oled.drawByte(0b11111111);
}

void drawMainMenu(void) {
  oled.clear();
  oled.home();
  oled.line(0, 10, 127, 10);
  oled.print(F("Files:"));
  oled.print(fileCount);
  if (badCount) oled.printf("[%i]", fileCount + badCount);

  int sidx = (cursor < 6 ? 0 : cursor - 5);
  int i = 2;
  su::TextParser p(fileNames, '/');
  while (p.parse()) {
    if (p.index() > sidx) {
      oled.setCursor(6, i++);
      oled.print(p);
    }
    if (p.index() == cursor + 1) {
      selectedFile = String(p);
    }
    if (p.index() > sidx + 5) break;
    yield();
  }

  int cp = constrain(cursor, 0, 5) + 2;
  oled.setCursor(0, cp);
  oled.print(">");
  checkBatteryCharge();
  drawBatteryCharge();
  oled.update();
}

void drawStaMenu(void) {
  oled.clear();
  oled.home();
  oled.line(0, 10, 127, 10);
  oled.print(F("РЕЖИМ STA"));
  oled.setCursor(0, 2);
  oled.print(F("Network: "));
  oled.print(sets.staSsid);
  oled.setCursor(0, 4);
  oled.print(F("Локал.IP:"));
  oled.print(WiFi.localIP());
  checkBatteryCharge();
  drawBatteryCharge();
  oled.update();
}

void drawApMenu(void) {
  oled.clear();
  oled.home();
  oled.line(0, 10, 127, 10);
  oled.print(F("РЕЖИМ AP"));
  oled.setCursor(0, 2);
  oled.print(F("network: "));
  oled.print(sets.apSsid);
  oled.setCursor(0, 4);
  oled.print(F("Key: "));
  oled.print(sets.apPass);
  oled.setCursor(0, 6);
  oled.print(F("Local.IP:"));
  oled.print(F("192.168.4.1"));
  checkBatteryCharge();
  drawBatteryCharge();
  oled.update();
}

void fileReadError(void) {
  oled.clear();
  oled.setScale(2);
  oled.setCursorXY(28, 8);
  oled.print(F("Error"));
  oled.setCursorXY(28, 24);
  oled.print(F("reading"));
  oled.setCursorXY(34, 40);
  oled.print(F("file"));
  oled.setScale(1);
  oled.update();
  delay(1500);
}