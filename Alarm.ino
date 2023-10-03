#include <LiquidCrystal.h>          //the liquid crystal library contains commands for printing to the display
#include <stdio.h>
#include <time.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   // tell the RedBoard what pins are connected to the display

const char* appName = "Alarm";
const int screenWidth = 16;

const int increasePin = 7;
bool increaseButton = false;

bool switchButton = false; 

const int decreasePin = 6;
bool decreaseButton = false;

const int toggleAlarmPin = 5;
bool toggleAlarmButton = false;

const int buzzerPin = 3;

int hours = 0;
int minutes = 0;
int seconds = -1;
bool hrsSelected = false;

bool flashing = false;
unsigned long flashed = 0;

bool clockCounting = false;


char hrs[3];
char min[3];
char sec[3];

char* formatedTime = malloc(strlen("xx:xx")+2);
unsigned long timerStart = -1;

const int timeSpeed = 1; // How fast time is
const int timeDelta = 1000/timeSpeed;


void printTitle(const char* str){
  int dist = strlen(appName) + 1;

  for (int i = 0; i < screenWidth - dist; i++){
    lcd.setCursor(dist + i, 0);
    lcd.print(" ");
  }

  lcd.setCursor(screenWidth - strlen(str), 0);              
  lcd.print(str);     
}

void setStatus(){
  if (clockCounting)
    printTitle("");
  else
    printTitle("SET");
}

void setup() {
  pinMode(increasePin, INPUT_PULLUP);
  pinMode(decreasePin, INPUT_PULLUP);
  pinMode(toggleAlarmPin, INPUT_PULLUP);


  lcd.begin(screenWidth, 2, LCD_8BITMODE);              
  lcd.clear();                 
  
  lcd.setCursor(0, 0);              
  lcd.print(appName);

  flashed = millis();
  setStatus();
  UpdateTime();
  drawTime(1);
}

void UpdateTime(){
    if (hours > 9)
    sprintf(hrs, "%d", hours);
  else
    sprintf(hrs, "0%d", hours);

  if (minutes > 9)
    sprintf(min, "%d", minutes);
  else
    sprintf(min, "0%d", minutes);

  free(formatedTime);
  formatedTime = malloc(strlen("xx:xx")+2);
  strcpy(formatedTime, hrs);
  strcat(formatedTime, ":");
  strcat(formatedTime, min);
}

void clearRow(int row){
  for (int i = 0; i < screenWidth; i++){
    lcd.setCursor(i, row);
    lcd.print(" ");
  }
}

void drawTime(int row){
  lcd.setCursor(0, row);
  lcd.print(formatedTime);
}

void alarm(){

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HOLD YELLOW");
  lcd.setCursor(0, 3);
  lcd.print("BUTTON TO STOP");

  lcd.autoscroll();
  while(true){
    
    tone(buzzerPin, 130, 250);      //E6
    delay(275);
    tone(buzzerPin, 73, 250);       //G6
    delay(275);
    tone(buzzerPin, 65, 150);       //E7
    delay(175);
    tone(buzzerPin, 98, 500);       //C7
    delay(500);

    if (digitalRead(toggleAlarmPin) == LOW){
      setup();
      break;
    }
  }
}

void loop() {
  UpdateTime();

  if (clockCounting){
    lcd.setCursor(0, 1);              
    

    if (seconds > 9)
      sprintf(sec, "%d", seconds);
    else
      sprintf(sec, "0%d", seconds);

    strcat(formatedTime, ":");
    strcat(formatedTime, sec);
    lcd.print(formatedTime);
    }
  
    if (digitalRead(toggleAlarmPin) == LOW && !toggleAlarmButton){
      clockCounting = !clockCounting;
      clearRow(1);
      UpdateTime();
      drawTime(1);
      toggleAlarmButton = true;
      setStatus();
    }else if (digitalRead(toggleAlarmPin) != LOW && toggleAlarmButton){
      toggleAlarmButton = false;
      setStatus();
    }

    if (!clockCounting){
      if (hrsSelected){
        if (millis() - flashed > 1500 && !flashing)
        {
          flashing = true;
          lcd.setCursor(0, 1);
          lcd.print("  ");
          flashed = millis();
        }else if (millis() - flashed > 500 && flashing){
          flashing = false;
          lcd.setCursor(0, 1);
          lcd.print(hrs);
        }
      }else if (!hrsSelected){
        if (millis() - flashed > 1500 && !flashing)
        {
          flashing = true;
          lcd.setCursor(strlen("xx:"), 1);
          lcd.print("  ");
          flashed = millis();
        }else if (millis() - flashed > 500 && flashing){
          flashing = false;
          lcd.setCursor(strlen("xx:"), 1);
          lcd.print(min);
        }
      }

      if (digitalRead(increasePin) == LOW && digitalRead(decreasePin) == LOW && !switchButton){
        hrsSelected = !hrsSelected;
        switchButton = true;
        delay(1000);
        return;
      }else if (digitalRead(increasePin) != LOW && digitalRead(decreasePin) != LOW && switchButton)
        switchButton = false;

      if (digitalRead(increasePin) == LOW && !increaseButton){
        if (hrsSelected){
          hours += 1;
          UpdateTime();
          clearRow(1);
          drawTime(1);
        }
        else{
          if (minutes >= 59){
            hours += 1;
            minutes = 0;
          }else
            minutes += 1;
          UpdateTime();
          clearRow(1);
          drawTime(1);
        }
        increaseButton = true;
      }else if (digitalRead(increasePin) != LOW && increaseButton)
        increaseButton = false;
      


      if (digitalRead(decreasePin) == LOW && !decreaseButton){
        if (hrsSelected){
          if (hours > 0)
            hours -= 1;
          UpdateTime();
          clearRow(1);
          drawTime(1);
        }
        else{
          if (minutes <= 0){
            if (hours > 0){
            hours -= 1;
            minutes = 59;
            }else{
              hours = 0;
              minutes = 0;
            }
          }else
            minutes -= 1;
          UpdateTime();
          clearRow(1);
          drawTime(1);
        }
        decreaseButton = true;
      }else if (digitalRead(decreasePin) != LOW && decreaseButton)
        decreaseButton = false;
    }else{
      if (timerStart == -1){
        timerStart = millis();
        seconds = 59;
      }
      if (millis() - timerStart > timeDelta){
        if (seconds <= 0){
          if (minutes <= 0){
            if (hours <= 0){
              alarm();
              clockCounting = false;
              printTitle("RING!");
            }
            else{
            hours -= 1;
            minutes = 59;
            seconds = 59;
            }
          }else{
            minutes -= 1;
            seconds = 59;
          }
        }else
          seconds -= 1;
        timerStart = millis();
      }
    }
}
