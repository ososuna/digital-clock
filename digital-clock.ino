/*
  Digital Clock

  Added 31 Mar 2021
  by Oswaldo Osuna
  Modified 31 Mar 2021
  by Oswaldo Osuna

 */

// include the library code
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <EEPROM.h>

struct Clock {
  int segs_0 = 0;
  int segs_1 = 0;
  int mins_0 = 0;
  int mins_1 = 0;
  int hrs_0 = 0;
  int hrs_1 = 0;
};

struct Alarm {
  int mins_0 = 9;
  int mins_1 = 5;
  int hrs_0 = 3;
  int hrs_1 = 2;
};

const int button_pin_mins = 2;
const int button_pin_hrs = 3;
const int button_pin_save = 4;
bool state_save, prev_state_save;
bool save = false;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Create the instance of Alarm struct
Clock clock;

// Create the instance of Alarm struct
Alarm alarm;

void setup() {
  // Read the alarm attributes from EEPROM
  alarm.mins_0 = EEPROM.read(3);
  alarm.mins_1 = EEPROM.read(2);
  alarm.hrs_0 = EEPROM.read(1);
  alarm.hrs_1 = EEPROM.read(0);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Timer1.initialize(1000000); // Configures TIMER en 1 second
  Timer1.attachInterrupt(Temporizador) ; // Configures the Timer 1 inerruption
  lcd.setCursor(5, 0);
  lcd.print(':');
  lcd.setCursor(2, 0);
  lcd.print(':');
  lcd.setCursor(2, 1);
  lcd.print(':');
  lcd.setCursor(4, 1);
  lcd.print(EEPROM.read(3));
  lcd.setCursor(3, 1);
  lcd.print(EEPROM.read(2));
  lcd.setCursor(1, 1);
  lcd.print(EEPROM.read(1));
  lcd.setCursor(0, 1);
  lcd.print(EEPROM.read(0));
  lcd.setCursor(6, 1);
  lcd.print("Alarma");
  // When the minutes button is pressed (debouncing)
  attachInterrupt(digitalPinToInterrupt(button_pin_mins),pin2ExtInterruptHandler,FALLING);
  // When the hours button is pressed (debouncing)
  attachInterrupt(digitalPinToInterrupt(button_pin_hrs),pin3ExtInterruptHandler,FALLING);
}

void loop() {
  // When the save button is pressed
  state_save = digitalRead(button_pin_save);
  if(state_save == HIGH && prev_state_save == LOW) {
    if(save) {
      lcd.setCursor(12, 1);
      lcd.print(" ");
      lcd.setCursor(6, 1);
      lcd.print("Alarma");
      save = false;
    }
  }
  // Debouncing
  prev_state_save = state_save;
}

// Interrupt function when timer is executed
void Temporizador(void)
{
  // Timer increments
  clock.segs_0++;
  // Reset the counter when gets to 1000 seconds
  if(clock.segs_0>9){
    clock.segs_0=0;
    clock.segs_1++;
  }
  if(clock.segs_1>5){
    clock.segs_0=0;
    clock.segs_1=0;
    clock.mins_0++;
  }

  if(clock.mins_0>9){
    clock.mins_0=0;
    clock.mins_1++;
  }

  if(clock.mins_1>5){
    clock.mins_0=0;
    clock.mins_1=0;
    clock.hrs_0++;
  }

  if(clock.hrs_0>9){
    clock.hrs_0=0;
    clock.hrs_1++;
  }

  if(clock.hrs_1>23){
    clock.hrs_0=0;
    clock.hrs_1=0;
  }
  
  // Shows in LCD display the actual value of the timer
  lcd.setCursor(7, 0);
  lcd.print(clock.segs_0);
  lcd.setCursor(6, 0);
  lcd.print(clock.segs_1);
  lcd.setCursor(4, 0);
  lcd.print(clock.mins_0);
  lcd.setCursor(3, 0);
  lcd.print(clock.mins_1);
  lcd.setCursor(1, 0);
  lcd.print(clock.hrs_0);
  lcd.setCursor(0, 0);
  lcd.print(clock.hrs_1);
}

// Function executed when the minutes button is pressed
void pin2ExtInterruptHandler(){
  sum_minutes();
}

// Function executed when the hours button is pressed
void pin3ExtInterruptHandler(){
  sum_hours();
}

// Function that increments the minutes counter in the alarm
void sum_minutes() {
  save = true;
  lcd.setCursor(6, 1);
  lcd.print("Guardar");
  if (alarm.mins_0 > 8) {
    alarm.mins_0 = 0;
    if (alarm.mins_1 > 4) {
      alarm.mins_1 = 0;
    } else {
      alarm.mins_1++;
    }
  } else {
    alarm.mins_0++;
  }
  EEPROM.update(3, alarm.mins_0);
  EEPROM.update(2, alarm.mins_1);
  lcd.setCursor(4, 1);
  lcd.print(EEPROM.read(3));
  lcd.setCursor(3, 1);
  lcd.print(EEPROM.read(2));
}

// Function that increments the hours counter in the alarm
void sum_hours(){
  save = true;
  lcd.setCursor(6, 1);
  lcd.print("Guardar");
  if (alarm.hrs_0 > 8) {
    alarm.hrs_0 = 0;
    if (alarm.hrs_1 > 1) {
      alarm.hrs_1 = 0;
    } else {
    alarm.hrs_1++;
    }      
  } else {
    alarm.hrs_0++;
    if (alarm.hrs_1 > 1) {
      if (alarm.hrs_0 > 3) {
        alarm.hrs_0 = 0;
        alarm.hrs_1 = 0;
      }
    }
  }
  EEPROM.update(1, alarm.hrs_0);
  EEPROM.update(0, alarm.hrs_1);
  lcd.setCursor(1, 1);
  lcd.print(EEPROM.read(1));
  lcd.setCursor(0, 1);
  lcd.print(EEPROM.read(0));
}
