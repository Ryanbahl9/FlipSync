#ifndef LCD1602A_Display_h
#define LCD1602A_Display_h
#include <LiquidCrystal_I2C.h>
#include <stdexcept>
#include "Arduino.h"
#define LCD_ADDR 0x27

class LCD1602A_Display {
  public:
    LCD1602A_Display();
    void init();
    void setMessage(const String& msg);
    void setMessage(char* msg);
    void setMessage(char* msg, int length);
    void setMessage(char* topRow, char* bottomRow);
    void flashMessage(char* msg, unsigned long delay);
    char* getMessage();
    void setCursor(int col, int row);
    void incrementChar();
    void decrementChar();
    void incrementCursor();
    void printMessage();
    LiquidCrystal_I2C lcd;
  private:
    char* message;
    int cursorCol;
    int cursorRow;
    void displayMessage();
    void clearMessage();


};


#endif