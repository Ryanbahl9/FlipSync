#include "LCD1602A_Display.h"

// Display Constants
#define MAX_ROW 16
#define MAX_COL 2

LCD1602A_Display::LCD1602A_Display() : lcd(LCD_ADDR, MAX_COL, MAX_ROW) {
  message = new char[33];
  memset(message, ' ', 32);
  message[32] = '\0';
  cursorCol = 0;
  cursorRow = 0;
}

void LCD1602A_Display::init() {
  //LCD Setup
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.cursor_on();
}

void LCD1602A_Display::setMessage(const String &msg) {
  clearMessage();
  strncpy(message, msg.c_str(), std::min((int)msg.length(), 32));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Display::setMessage(char* msg) {
  clearMessage();
  strncpy(message, msg, std::min((int)strlen(msg), 32));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Display::setMessage(char* msg, int length) {
  clearMessage();
  strncpy(message, msg, std::min((int)length, 32));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Display::setMessage(char* topRow, char* botRow) {
  clearMessage();
  strncpy(message, topRow, std::min((int)strlen(topRow), 16));
  strncpy(message+16, botRow, std::min((int)strlen(botRow), 16));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Display::flashMessage(char* msg, unsigned long delayLen) {
  // Store the og message for after flash
  char* copiedMessage = new char[33]; 
  strcpy(copiedMessage, message);

  // Flash the message
  setMessage(msg);
  delay(delayLen);

  // Reset the message
  setMessage(copiedMessage);
}

char* LCD1602A_Display::getMessage() {
  return message;
}

void LCD1602A_Display::setCursor(int col, int row) {
  cursorCol = col;
  cursorRow = row;
  lcd.setCursor(col, row);
}

void LCD1602A_Display::incrementChar() {
  if (message[cursorRow * 16 + cursorCol] != '~')
    message[cursorRow * 16 + cursorCol] += 1;
  lcd.print(message[cursorRow * 16 + cursorCol]);
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Display::decrementChar() {
  if (message[cursorRow * 16 + cursorCol] != ' ')
    message[cursorRow * 16 + cursorCol] -= 1;
  lcd.print(message[cursorRow * 16 + cursorCol]);
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Display::incrementCursor() {
  if (cursorCol < 15) {
    cursorCol += 1;
  } else {
    cursorCol = 0;
    cursorRow = (cursorRow + 1) % 2;
  }
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Display::printMessage() {
  Serial.println(message);
}

// Private Methods
void LCD1602A_Display::displayMessage() {
  char* tmpStr;
  tmpStr = new char[17];
  strncpy(tmpStr, message, 16);
  setCursor(0, 0);
  lcd.print(tmpStr);

  setCursor(0, 1);
  lcd.print(message + 16);
  delete tmpStr;
}

void LCD1602A_Display::clearMessage() {
  memset(message,' ',32);
}

