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

int segs_0 = 0;
int segs_1 = 0;
int mins_0 = 0;
int mins_1 = 0;
int hrs_0 = 0;
int hrs_1 = 0;
int alarm_mins_0 = 0;
int alarm_mins_1 = 0;
int alarm_hrs_0 = 0;
int alarm_hrs_1 = 0;

const int button_pin_mins = 2;
const int button_pin_hrs = 3;
const int button_pin_save = 4;
bool state_save, prev_state_save;
bool save = false;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
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
  lcd.print(alarm_mins_0);
  lcd.setCursor(3, 1);
  lcd.print(alarm_mins_1);
  lcd.setCursor(1, 1);
  lcd.print(alarm_hrs_0);
  lcd.setCursor(0, 1);
  lcd.print(alarm_hrs_1);
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
  segs_0++;
  // Reset the counter when gets to 1000 seconds
  if(segs_0>9){
    segs_0=0;
    segs_1++;
  }
  if(segs_1>5){
    segs_0=0;
    segs_1=0;
    mins_0++;
  }

  if(mins_0>9){
    mins_0=0;
    mins_1++;
  }

  if(mins_1>5){
    mins_0=0;
    mins_1=0;
    hrs_0++;
  }

  if(hrs_0>9){
    hrs_0=0;
    hrs_1++;
  }

  if(hrs_1>23){
    hrs_0=0;
    hrs_1=0;
  }
  
  // Shows in LCD display the actual value of the timer
  lcd.setCursor(7, 0);
  lcd.print(segs_0);
  lcd.setCursor(6, 0);
  lcd.print(segs_1);
  lcd.setCursor(4, 0);
  lcd.print(mins_0);
  lcd.setCursor(3, 0);
  lcd.print(mins_1);
  lcd.setCursor(1, 0);
  lcd.print(hrs_0);
  lcd.setCursor(0, 0);
  lcd.print(hrs_1);
}

// Function executed when the minutes button is pressed
void pin2ExtInterruptHandler(){
  sum_minutes();
}

// Function executed when the hours button is pressed
void pin3ExtInterruptHandler(){
  sum_hours();
}

// Function that increments the minutes counter
void sum_minutes() {
  save = true;
  lcd.setCursor(6, 1);
  lcd.print("Guardar");
  if (alarm_mins_0 > 8) {
    alarm_mins_0 = 0;
    if (alarm_mins_1 > 4) {
      alarm_mins_1 = 0;
    } else {
      alarm_mins_1++;
    }
  } else {
    alarm_mins_0++;
  }
  lcd.setCursor(4, 1);
  lcd.print(alarm_mins_0);
  lcd.setCursor(3, 1);
  lcd.print(alarm_mins_1);
}

// Function that increments the hours counter
void sum_hours(){
  save = true;
  lcd.setCursor(6, 1);
  lcd.print("Guardar");
  if (alarm_hrs_0 > 8) {
    alarm_hrs_0 = 0;
    if (alarm_hrs_1 > 1) {
      alarm_hrs_1 = 0;
    } else {
    alarm_hrs_1++;
    }      
  } else {
    alarm_hrs_0++;
    if (alarm_hrs_1 > 1) {
      if (alarm_hrs_0 > 3) {
        alarm_hrs_0 = 0;
        alarm_hrs_1 = 0;
      }
    }
  }
  lcd.setCursor(1, 1);
  lcd.print(alarm_hrs_0);
  lcd.setCursor(0, 1);
  lcd.print(alarm_hrs_1);
}
