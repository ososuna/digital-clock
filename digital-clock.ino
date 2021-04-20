

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
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <DHT.h>

#define DHTTYPE DHT11

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
const int DHTPin = 5; 
bool state_save, prev_state_save;
bool save = false;
bool sounding = false;
bool can_sound = true;
float temp;

DHT dht(DHTPin, DHTTYPE);

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
  
  // Set up the LCD's number of columns and rows:
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
  lcd.print(alarm.mins_0);
  lcd.setCursor(3, 1);
  lcd.print(alarm.mins_1);
  lcd.setCursor(1, 1);
  lcd.print(alarm.hrs_0);
  lcd.setCursor(0, 1);
  lcd.print(alarm.hrs_1);
  lcd.setCursor(6, 1);
  lcd.print("Alarma");
  // When the minutes button is pressed (debouncing)
  attachInterrupt(digitalPinToInterrupt(button_pin_mins),pin2ExtInterruptHandler,FALLING);
  // When the hours button is pressed (debouncing)
  attachInterrupt(digitalPinToInterrupt(button_pin_hrs),pin3ExtInterruptHandler,FALLING);

  // Begin DHT11
  dht.begin();
  temp = dht.readTemperature();
  lcd.setCursor(10, 0);
  lcd.print("T:");
  lcd.setCursor(12, 0);
  lcd.print(temp);
}

void loop() {
  // When the save button is pressed
  state_save = digitalRead(button_pin_save);
  if(state_save == HIGH && prev_state_save == LOW) {
    if (sounding) {
      can_sound = false;
      sounding = false;
    } else {
      if(save) {
        can_sound = true;
        lcd.setCursor(12, 1);
        lcd.print(" ");
        lcd.setCursor(6, 1);
        lcd.print("Alarma");
        save = false;
        EEPROM.update(3, alarm.mins_0);
        EEPROM.update(2, alarm.mins_1);
        EEPROM.update(1, alarm.hrs_0);
        EEPROM.update(0, alarm.hrs_1);
      }
    }
  }
  // Debouncing
  prev_state_save = state_save;

  /* Measure temperature and humidity.  If the functions returns
    true, then a measurement is available. */
  if( measure_environment(&temp) == true ) {
      // Shows in LCD display the actual value of the temperature
      lcd.setCursor(10, 0);
      lcd.print("T:");
      lcd.setCursor(12, 0);
      lcd.print(temp);
  }

}

// Interrupt function when timer is executed
void Temporizador(void)
{
  // Reads temperature
  // temp = dht.readTemperature();
  
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

  if ( (clock.hrs_0 == alarm.hrs_0) && (clock.hrs_1 == alarm.hrs_1) &&
          (clock.mins_0 >= alarm.mins_0) && (clock.mins_1 == alarm.mins_1) &&  (!save) )
  {
    if (can_sound) {
      sounding = true;
      tone(13, 294, 500);
    }
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
  if (sounding) {
    can_sound = false;
    sounding = false;
  } else {
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
    lcd.setCursor(4, 1);
    lcd.print(alarm.mins_0);
    lcd.setCursor(3, 1);
    lcd.print(alarm.mins_1);
  }
}

// Function that increments the hours counter in the alarm
void sum_hours(){
  if (sounding) {
    can_sound = false;
    sounding = false;
  } else {
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
    lcd.setCursor(1, 1);
    lcd.print(alarm.hrs_0);
    lcd.setCursor(0, 1);
    lcd.print(alarm.hrs_1);
  }
    
}

/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment(float *temperature) {
  
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul ) {
    if( !isnan(temp) )
    {
      measurement_timestamp = millis( );
      temp = dht.readTemperature();
      return( true );
    }
  }
  return( false );

}