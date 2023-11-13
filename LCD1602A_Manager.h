#ifndef LCD1602A_Manager_h
#define LCD1602A_Manager_h
#include <LiquidCrystal_I2C.h>
#include <stdexcept>

#define LCD_ADDR 0x27
#include "Arduino.h"

class LCD1602A_Manager {
  public:
    LCD1602A_Manager();
    void init();
    void setMessage(char* msg);
    void setMessage(char* msg, int length);
    void setMessage(char* topRow, char* bottomRow);
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