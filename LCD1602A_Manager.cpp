#include <string>
#include "LCD1602A_Manager.h"

LCD1602A_Manager::LCD1602A_Manager() : lcd(LCD_ADDR, 2, 16) {
  message = new char[33];
  memset(message, ' ', 32);
  message[32] = '\0';
  cursorCol = 0;
  cursorRow = 0;
}

void LCD1602A_Manager::init() {
  //LCD Setup
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.cursor_on();
}

void LCD1602A_Manager::setMessage(char* msg) {
  clearMessage();
  strncpy(message, msg, std::min((int)strlen(msg), 32));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Manager::setMessage(char* msg, int length) {
  clearMessage();
  strncpy(message, msg, std::min((int)length, 32));
  displayMessage();
  setCursor(0,0);
}

void LCD1602A_Manager::setMessage(char* topRow, char* botRow) {
  clearMessage();
  strncpy(message, topRow, std::min((int)strlen(topRow), 16));
  strncpy(message+16, botRow, std::min((int)strlen(botRow), 16));
  displayMessage();
  setCursor(0,0);
}

char* LCD1602A_Manager::getMessage() {
  return message;
}

void LCD1602A_Manager::setCursor(int col, int row) {
  cursorCol = col;
  cursorRow = row;
  lcd.setCursor(col, row);
}

void LCD1602A_Manager::incrementChar() {
  if (message[cursorRow * 16 + cursorCol] != '~')
    message[cursorRow * 16 + cursorCol] += 1;
  lcd.print(message[cursorRow * 16 + cursorCol]);
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Manager::decrementChar() {
  if (message[cursorRow * 16 + cursorCol] != ' ')
    message[cursorRow * 16 + cursorCol] -= 1;
  lcd.print(message[cursorRow * 16 + cursorCol]);
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Manager::incrementCursor() {
  if (cursorCol < 15) {
    cursorCol += 1;
  } else {
    cursorCol = 0;
    cursorRow = (cursorRow + 1) % 2;
  }
  setCursor(cursorCol, cursorRow);
}

void LCD1602A_Manager::printMessage() {
  Serial.println(message);
}

// Private Methods
void LCD1602A_Manager::displayMessage() {
  char* tmpStr;
  tmpStr = new char[17];
  strncpy(tmpStr, message, 16);
  setCursor(0, 0);
  lcd.print(tmpStr);

  setCursor(0, 1);
  lcd.print(message + 16);
  delete tmpStr;
}

void LCD1602A_Manager::clearMessage() {
  memset(message,' ',32);
}

