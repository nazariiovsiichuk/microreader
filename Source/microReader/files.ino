bool INVERT_IMG = 0;

void checkFileSystem(void) {
  Dir root = LittleFS.openDir("/");
  fileCount = badCount = 0;
  fileNames = "";
  for (int i = 0; root.next(); i++) {
    yield();
    File file = root.openFile("r");
    if (file) {
      su::Text filename(file.name());
      if ((filename.lengthUnicode() < MAX_FILENAME_LEN + 5) && (filename.endsWith(".txt") || filename.endsWith(".itxt") || filename.endsWith(".h") || filename.endsWith(".jpg"))) {
        fileCount++;
        fileNames += "/";
        fileNames += root.fileName();
      } else if (!filename.endsWith(".dat")) badCount++;
    } else badCount++;
    file.close();
  }
}

void drawPage(File file) {
  if (!file.available()) return;
  oled.clear();
  oled.home();
  while (!oled.isEnd() && file.available()) {
    oled.write(file.read());
  }
  oled.update();
}

void enterToReadTxtFile(void) {
  String fn = ("/" + selectedFile);
  File file = LittleFS.open(fn, "r");
  if (!file) {
    fileReadError();
    checkFileSystem();
    drawMainMenu();
    file.close();
    return;
  }

  drawPage(file);
  while (1) {
    up.tick();
    ok.tick();
    down.tick();
    if (ok.click()) {
      uiTimer = millis();
      drawMainMenu();
      file.close();
      return;
    }

    if (up.click() or up.step()) {
      uiTimer = millis();
      long pos = file.position() - 500;
      if (pos < 0) pos = 0;
      file.seek(pos);
      drawPage(file);
    } else if (down.click() or down.step()) {
      uiTimer = millis();
      drawPage(file);
    }
    yield();
  }
}

void enterToReadBmpFile(void) {
  String fn = ("/" + selectedFile);
  File file = LittleFS.open(fn, "r");
  if (!file) {
    fileReadError();
    checkFileSystem();
    drawMainMenu();
    file.close();
    return;
  }

  uint8_t *img = new uint8_t[128 * 64];
  if (parseItxt(img, file)) {
    fileReadError();
    delete[] img;
    uiTimer = millis();
    drawMainMenu();
    file.close();
    return;
  }

  oled.clear();
  oled.update();
  file.close();

  while (1) {
    ok.tick();
    down.tick();
    if (ok.click()) {
      uiTimer = millis();
      drawMainMenu();
      delete[] img;
      return;
    }
    if (down.click()) {
      File file = LittleFS.open(fn, "r");
      if (!file) {
        fileReadError();
        checkFileSystem();
        file.close();
        delete[] img;
        uiTimer = millis();
        drawMainMenu();
        return;
      }
      oled.clear();
      INVERT_IMG = !INVERT_IMG;
      if (parseItxt(img, file)) {
        fileReadError();
        delete[] img;
        uiTimer = millis();
        drawMainMenu();
        file.close();
        return;
      }
      oled.update();
      file.close();
    }
    yield();
  }
}

uint16_t jpgW = 0, jpgH = 0, kW, kH;
void enterToReadJpgFile(void) {
  String fn = ("/" + selectedFile);
  File file = LittleFS.open(fn, "r");
  if (!file) {
    fileReadError();
    checkFileSystem();
    drawMainMenu();
    file.close();
    return;
  }
  file.close();

  oled.clear();
  TJpgDec.getFsJpgSize(&jpgW, &jpgH, fn, LittleFS);
  kW = ceil(jpgW / 128.0) > 1 ? ceil(jpgW / 128.0) : 1;
  kH = ceil(jpgH / 64.0) > 1 ? ceil(jpgH / 64.0) : 1;
  TJpgDec.drawFsJpg(0, 0, fn, LittleFS);
  oled.update();

  while (1) {
    down.tick();
    ok.tick();
    if (ok.click()) {
      uiTimer = millis();
      drawMainMenu();
      return;
    }
    if (down.click()) {
      oled.clear();
      INVERT_IMG = !INVERT_IMG;
      TJpgDec.drawFsJpg(0, 0, fn, LittleFS);
      oled.update();
    }
    yield();
  }
}

bool oled_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  for (int _y = y; _y < y + h; _y++) {
    for (int _x = x; _x < x + w; _x++) {
      if (!(_x % kW) && !(_y % kH)) oled.dot(_x / kW, _y / kH, getBright(bitmap[(_x - x) + (_y - y) * w]) > 127 ? !INVERT_IMG : INVERT_IMG);
    }
  }
  return 1;
}

uint8_t getBright(uint16_t clr) {
  byte r = (clr & 0b1111100000000000) >> 8;
  byte g = (clr & 0b0000011111100000) >> 3;
  byte b = (clr & 0b0000000000011111) << 3;
  return max(max(r, g), b);
}

uint8_t parseItxt(uint8_t *img, File file) {
  int imgLen = 0;
  memset(img, 0, 1024);

  while (file.read() != '{') {
    if (!file.available()) return 1;
    yield();
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    su::TextParser p(line, ',');
    while (p.parse()) {
      uint8_t val = p.trim().toInt32HEX();
      if (INVERT_IMG) val = ~val;
      img[imgLen] = val;
      if (++imgLen >= 1023) return 0;
      yield();
    }
    yield();
  }

  return 1;
}