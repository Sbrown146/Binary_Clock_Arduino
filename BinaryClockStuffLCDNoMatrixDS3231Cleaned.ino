// Troubleshoot days and months not lining up properly.
// Other than that, done.  Just clean up code.

#include <RTClib.h>
#include <LiquidCrystal.h>


// RTC Declaration
RTC_DS3231 rtc;

// 74HC595 Pins
int dataPin = A0;
int latchPin = A1;
int clockPin = A2;

// Switch Pins
int militaryPin = 14;
int timeChangePin = A3;

// Button Pins -> Increment only
int minutesPin = 9;
int hoursPin = 8;
int monthsPin = 10;
int daysPin = 11;
int yearOnePin = 12;
int yearTwoPin = 13;

// Pins for LCD Display via Liquid Crystal Library
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


// Starting Clock Variables
long startingSeconds = 0;
long startingMinutes = 58;
long startingHours = 14;
long startingMonth = 4;
long startingDay = 1;
long startingYearOne = 20;
long startingYearTwo = 22;
String dayNight = "AM";
String currentTimeDisplay;
String currentDateDisplay;

// Trackers
long militaryTracker = 0;
long timeStopTracker = 0;
long hoursTracker = startingHours;
long minutesTracker = startingMinutes;
long secondsTracker = startingSeconds;
long monthsTracker = startingMonth;
long daysTracker = startingDay;
long yearOneTracker = startingYearOne;
long yearTwoTracker = startingYearTwo;

// Tracker to handle spliting and then combining year
int yearTracker = (String(startingYearOne) + String(startingYearTwo)).toInt();


// RTC Library Time Variables
int current_sec;
int current_minute;
int current_hour;
int current_day;
int current_month;
int current_year;
int current_dayWeek;
int previous_sec;
char* currentDayLCDTop;


// Button Matrix Variables
int hourPlusTracker = 0;
int minutePlusTracker = 0;
int dayPlusTracker = 0;
int monthPlusTracker = 0;
int year1PlusTracker = 0;
int year2PlusTracker = 0;


// Holders for Array References for Hours, Months and Days
long hoursLEDHolder;
long monthsLEDHolder;
long daysLEDHolder;

// String Time Variables for LCD Display
String hoursLCD;
String minuteLCD;
String secondLCD;
String monthLCD;
String dayLCD;
String yearOneLCD;
String yearTwoLCD;

// Tracker for differing number of days in certain months
int monthsTrackerMax;

// Time Sensitive Variables
unsigned long previousTime = 0;
unsigned long switchTime = 0;
unsigned long previousButtonTime = 0;
unsigned long ledDelay = 1000;
unsigned long buttonDelay = 150;
unsigned long buttonTime = 0;

int serialTracker = 0;

// LED arrays -> Takes in hexadecimal and converts to binary for shift register
            //-> Hexadecimal is easier to write out and troubleshoot for this project

byte yearArray[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
                    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
                    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
                    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
                    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
                    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99};


// Febraury
byte days28Array[] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28};

// 4, 6, 9, 11
byte days30Array[] = {0x30, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                      0x30};

                      
// 1, 3, 5, 7, 8, 10, 12
byte days31Array[] = {0x31, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                      0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
                      0x30, 0x31};


byte monthsArray[] = {0x12, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
                      0x10, 0x11, 0x92};


byte hoursNormalArray[] = {0x12, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
                           0x10, 0x11, 0x92, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
                           0x88, 0x89, 0x90, 0x91};
                           

byte hoursMilitaryArray[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
                           0x10, 0x11, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 
                           0xA0, 0xA1, 0xA2, 0xA3};
                           

byte minutesArray[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                       0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
                       0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
                       0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9};


byte secondsArray[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                       0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
                       0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
                       0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9};


byte hoursNormalLCDArray[] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                              12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};


byte daysOfWeekHexArray[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00};


char daysOfWeekArray[7][12] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday",
                               "Thursday", "Friday"};

char daysOfWeekLCDTop[7][5] = {"SAT", "SUN", "MON", "TUE", "WED", "THU", "FRI"}; 


void setup() {
//   Serial.begin(57600);
   pinMode(latchPin, OUTPUT);
   pinMode(dataPin, OUTPUT);  
   pinMode(clockPin, OUTPUT);
   pinMode(timeChangePin, OUTPUT);
   pinMode(minutesPin, OUTPUT);
   pinMode(hoursPin, OUTPUT);
   pinMode(monthsPin, OUTPUT);
   pinMode(daysPin, OUTPUT);
   pinMode(yearOnePin, OUTPUT);
   pinMode(yearTwoPin, OUTPUT);

   rtc.begin();
   rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, startingSeconds));

   currentDayLCDTop = daysOfWeekLCDTop[zellersAlgorithm(startingMonth, startingDay, 
                                                  startingYearOne, startingYearTwo)];

   LCDTimeDisplay();

   // Set monthsTrackerMax on startup
   if (monthsTracker == 1) {
      monthsTrackerMax = 28;
   }
   else if (monthsTracker == 3 || monthsTracker == 5 || monthsTracker == 8 || monthsTracker == 10) {
      monthsTrackerMax = 30; 
   }
   else {
      monthsTrackerMax = 31;
   }
   
  }

  


// Pushes out hex code to shift register
// Follows circuit pattern -> Days of Week, Hours, Minutes, Seconds, Dots/Slashes, 
//                            Months, Days, Year00__, Year__00

void updateShiftRegister(byte LED1, byte LED2, byte LED3, byte LED4,
                         byte LED5, byte LED6, byte LED7, byte LED8, byte LED9) {
                         
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, LED7);
   shiftOut(dataPin, clockPin, LSBFIRST, LED6);
   shiftOut(dataPin, clockPin, LSBFIRST, LED4);
   shiftOut(dataPin, clockPin, LSBFIRST, LED5);
   shiftOut(dataPin, clockPin, LSBFIRST, LED8);
   shiftOut(dataPin, clockPin, LSBFIRST, LED1);
   shiftOut(dataPin, clockPin, LSBFIRST, LED2);
   shiftOut(dataPin, clockPin, LSBFIRST, LED3);
   shiftOut(dataPin, clockPin, LSBFIRST, LED9);
   digitalWrite(latchPin, HIGH);

}


// Outputs Time and Date to LCD screen
void LCDTimeDisplay() {

    // Functions to fix time/spacing issues on LCD screen
    AMPMCheck();
    MilitaryAndHourCheck();
    TimeCheck(minutesTracker, minuteLCD);
    TimeCheck(secondsTracker, secondLCD);
    TimeCheckMonthDay(monthsTracker - 1, monthLCD);
    TimeCheckMonthDay(daysTracker - 1, dayLCD);
    TimeCheck(yearOneTracker, yearOneLCD);
    TimeCheck(yearTwoTracker, yearTwoLCD);

    currentTimeDisplay = " " + String(hoursLCD) + ":" + 
                              String(minuteLCD) + "." + 
                              String(secondLCD) + dayNight +
                              " " + currentDayLCDTop + " ";


    currentDateDisplay = " " + String(monthLCD) + "/" + 
                                    String(dayLCD) + "/" + 
                                    String(yearOneLCD) + 
                                    String(yearTwoLCD) +
                                    " DAY ";

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(currentTimeDisplay);
    lcd.setCursor(0, 1);
    lcd.print(currentDateDisplay);
}



// LCD Check Functions
// LCD Check Functions
// LCD Check Functions

// Handles Hour Output -> Option for military time disabled without matrix
void MilitaryAndHourCheck() {
  if ((digitalRead(militaryPin) == HIGH) && (hoursTracker >= 10)) {
    militaryTracker = 1;
    hoursLCD = String(hoursTracker);
  }
  else if ((digitalRead(militaryPin) == HIGH) && (hoursTracker < 10)) {
    militaryTracker = 1;
    hoursLCD = "0" + String(hoursTracker);
  }
  else if ((digitalRead(militaryPin) == LOW) && (hoursNormalLCDArray[hoursTracker] >= 10)) {
    militaryTracker = 0;
    hoursLCD = String(hoursNormalLCDArray[hoursTracker]);
  }
  else if ((digitalRead(militaryPin) == LOW) && (hoursNormalLCDArray[hoursTracker] < 10)) {
    militaryTracker = 0;
    hoursLCD = "0" + String(hoursNormalLCDArray[hoursTracker]);
  }
}


// Handles everything but month and day
void TimeCheck(long tracker, String& LCDString) {
  if (tracker >= 10) {
    LCDString = String(tracker);
  }
  else if (tracker < 10) {
    LCDString = "0" + String(tracker);
  }
}


// Function to handle month and day
void TimeCheckMonthDay(long tracker, String& LCDString) {
  if ((tracker + 1) >= 10) {
    LCDString = String(tracker + 1);
  }
  else if ((tracker + 1) < 10) {
    LCDString = "0" + String(tracker + 1);
  }
}


// Handles AM/PM
void AMPMCheck() {
  if (hoursTracker > 11) {
    dayNight = "PM";
  }
  else {
    dayNight = "AM";
  }
}


// LED Check Functions
// LED Check Functions
// LED Check Functions

// Military Time Check for LEDS
void MilitaryLEDCheck() {
  if (militaryTracker == 1) {
    hoursLEDHolder = hoursMilitaryArray[hoursTracker];
  }
  else if (militaryTracker == 0) {
    hoursLEDHolder = hoursNormalArray[hoursTracker];
  }
}


// Handles max number of day for specified month
void MonthLoopCheck() {
  if (monthsLEDHolder == 2) {
    daysLEDHolder = days28Array[daysTracker];
    monthsTrackerMax = 28;
  }
  else if ((monthsLEDHolder == 4) || (monthsLEDHolder == 6) || 
           (monthsLEDHolder == 9) || (monthsLEDHolder ==  11)) {

            daysLEDHolder = days30Array[daysTracker];
            monthsTrackerMax = 30;
           }
  else {
     daysLEDHolder = days31Array[daysTracker];
     monthsTrackerMax = 31;
  }
}


// Change Time Functions
// Change Time Functions
// Change Time Functions

// Stops Time on Switch
void TimeStop() {
  if (digitalRead(timeChangePin) == HIGH) {
    timeStopTracker = 1;
  }
  else if (digitalRead(timeChangePin) == LOW) {
    timeStopTracker = 0;
  }
}


// This is to handle the difference with days in different months
void DaysChange() {
  if ((digitalRead(daysPin) == HIGH) && (buttonTime - previousButtonTime > buttonDelay)) {
    MonthLoopCheck();
    daysTracker++;
    secondsTracker = 0;

    if (daysTracker > monthsTrackerMax) {
      daysTracker = 1;
    }

    rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, secondsTracker));
                       

    hoursLEDHolder = hoursNormalArray[hoursTracker];
    monthsLEDHolder = monthsArray[monthsTracker];
    currentDayLCDTop = daysOfWeekLCDTop[zellersAlgorithm(monthsTracker, daysTracker, 
                                                    yearOneTracker, yearTwoTracker)];

    // Functions to check flags for LEDS
    MilitaryLEDCheck();
    MonthLoopCheck();
    
    LCDTimeDisplay();

    
    updateShiftRegister(secondsArray[secondsTracker], minutesArray[minutesTracker],
                  hoursLEDHolder, daysLEDHolder, monthsLEDHolder, 
                  yearArray[yearOneTracker], yearArray[yearTwoTracker], 0xFF,
                  daysOfWeekHexArray[zellersAlgorithm(monthsTracker, daysTracker, 
                                                  yearOneTracker, yearTwoTracker)]);

    previousButtonTime = buttonTime;
  }
}

// Allows changing all time variables via one function -> long& acts as a pointer so tracker 
// value is stored properly
void TimeChange(int pinInput, long& tracker, int trackerMax) {
  if ((digitalRead(pinInput) == HIGH) && (buttonTime - previousButtonTime > buttonDelay)) {
    tracker++;
    secondsTracker = 0;

    if (tracker >= trackerMax) {
      tracker = 0;
    }

    rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, secondsTracker));


    MonthLoopCheck();
    

    hoursLEDHolder = hoursNormalArray[hoursTracker];
    monthsLEDHolder = monthsArray[monthsTracker];
    currentDayLCDTop = daysOfWeekLCDTop[zellersAlgorithm(monthsTracker, daysTracker, 
                                                    yearOneTracker, yearTwoTracker)];

    

    // Functions to check flags for LEDS
    MilitaryLEDCheck();
    MonthLoopCheck();
    
    LCDTimeDisplay();

    
    updateShiftRegister(secondsArray[secondsTracker], minutesArray[minutesTracker],
                  hoursLEDHolder, daysLEDHolder, monthsLEDHolder, 
                  yearArray[yearOneTracker], yearArray[yearTwoTracker], 0xFF,
                  daysOfWeekHexArray[zellersAlgorithm(monthsTracker, daysTracker, 
                                                  yearOneTracker, yearTwoTracker)]);

    previousButtonTime = buttonTime;
  }
}


// Function to handle months
void TimeChangeMonth(int pinInput, long& tracker, int trackerMax) {
  if ((digitalRead(pinInput) == HIGH) && (buttonTime - previousButtonTime > buttonDelay)) {
    tracker++;
    secondsTracker = 0;

    if (tracker > trackerMax) {
      tracker = 1;
    }

    rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, secondsTracker));


    MonthLoopCheck();
    

    hoursLEDHolder = hoursNormalArray[hoursTracker];
    monthsLEDHolder = monthsArray[monthsTracker];
    currentDayLCDTop = daysOfWeekLCDTop[zellersAlgorithm(monthsTracker, daysTracker, 
                                                    yearOneTracker, yearTwoTracker)];

    

    // Functions to check flags for LEDS
    MilitaryLEDCheck();
    MonthLoopCheck();
    
    LCDTimeDisplay();

    
    updateShiftRegister(secondsArray[secondsTracker], minutesArray[minutesTracker],
                  hoursLEDHolder, daysLEDHolder, monthsLEDHolder, 
                  yearArray[yearOneTracker], yearArray[yearTwoTracker], 0xFF,
                  daysOfWeekHexArray[zellersAlgorithm(monthsTracker, daysTracker, 
                                                  yearOneTracker, yearTwoTracker)]);

    previousButtonTime = buttonTime;
  }
}


// Function to handle breaking year into two variables and then concatenate them together
void TimeChangeYear(int pinInput, long& tracker, int trackerMax, int addMinus) {
  if ((digitalRead(pinInput) == HIGH) && (buttonTime - previousButtonTime > buttonDelay)) {

    if (addMinus == 1) {
      tracker++;
    }
    else if (addMinus == -1) {
      tracker--;
    }
    
    secondsTracker = 0;

    if (tracker >= trackerMax) {
      tracker = 0;
    }

    rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, secondsTracker));


    MonthLoopCheck();


    hoursLEDHolder = hoursNormalArray[hoursTracker];
    monthsLEDHolder = monthsArray[monthsTracker];
    currentDayLCDTop = daysOfWeekLCDTop[zellersAlgorithm(monthsTracker, daysTracker, 
                                                    yearOneTracker, yearTwoTracker)];

    
    // Functions to check flags for LEDS
    MilitaryLEDCheck();
    MonthLoopCheck();
    
    LCDTimeDisplay();

    
    updateShiftRegister(secondsArray[secondsTracker], minutesArray[minutesTracker],
                  hoursLEDHolder, daysLEDHolder, monthsLEDHolder, 
                  yearArray[yearOneTracker], yearArray[yearTwoTracker], 0xFF,
                  daysOfWeekHexArray[zellersAlgorithm(monthsTracker, daysTracker, 
                                                  yearOneTracker, yearTwoTracker)]);

    previousButtonTime = buttonTime;
  }
}



// Uses Zellers Algorithm to properly handle day of week based on date
int zellersAlgorithm(int m, int d, int y1, int y2) {

  int totalYear = (String(y1) + String(y2)).toInt();

  if (((m == 1) || (m==2)) && (totalYear % 100 == 0)) {
    m+=12;
    y1--;
    y2 = 99;
  }
  else if ((m == 1) || (m==2)) {
    m+=12;
    y2--;
  }

  int monthCalc = (13 * (m + 1))/5;
  int year2Calc = (y2/4);
  int year1Calc = (y1/4);
  int year1EndCalc = 2 * y1;

  int dayOfWeek = d + monthCalc + y2 + year2Calc + year1Calc - year1EndCalc;
  int dayOfWeekInt = dayOfWeek % 7;

  return dayOfWeekInt;
}



// Main Loop
void loop() {
  
  // Tracks time for avoid button bouncing
  buttonTime = millis();

  DateTime current_time = rtc.now();

  current_sec = current_time.second();
  secondsTracker = current_sec;
 
  TimeStop();

  // Allows increment of time variables if time is stopped
  if ((timeStopTracker == 1)) {

    if (serialTracker == 0) {
      serialTracker = 1;
    }
    
    TimeChange(minutesPin, minutesTracker, 60);
    TimeChange(hoursPin, hoursTracker, 24);
    TimeChangeMonth(monthsPin, monthsTracker, 12);
    TimeChangeYear(yearOnePin, yearTwoTracker, 100, 1);
    TimeChangeYear(yearTwoPin, yearTwoTracker, 100, -1);
    DaysChange();

    rtc.adjust(DateTime(yearTracker, startingMonth, startingDay, 
                       startingHours, startingMinutes, secondsTracker));
  }

  // Allows to time to move forward each second
  else if ((current_sec != previous_sec) && (timeStopTracker == 0)) {

    TimeStop();
    
    // Sets max number of days for specific month
    MonthLoopCheck();

    // Holders for array point references
    hoursLEDHolder = hoursNormalArray[hoursTracker];
    monthsLEDHolder = monthsArray[monthsTracker];

    // Functions to check flags for LEDS
    MilitaryLEDCheck();
    MonthLoopCheck();

    // Update Display
    LCDTimeDisplay();

    // Update Shift Registers -> Get current_dayWeek working
    updateShiftRegister(secondsArray[secondsTracker], minutesArray[minutesTracker],
                  hoursLEDHolder, daysLEDHolder, monthsLEDHolder, 
                  yearArray[yearOneTracker], yearArray[yearTwoTracker], 0xFF,
                  daysOfWeekHexArray[zellersAlgorithm(monthsTracker, daysTracker, 
                                                  yearOneTracker, yearTwoTracker)]);


    //  Loop to increment time elements using seconds from RTC 
    if (secondsTracker >= 59) {
      minutesTracker++;
    }
    
    if (minutesTracker >= 60) {
      minutesTracker = 0;
      hoursTracker++;
    }

    if (hoursTracker >= 24) {
      hoursTracker = 0;
      daysTracker++;
    }

    if (daysTracker > monthsTrackerMax) {
      daysTracker = 1;
      monthsTracker++;
    }

    if (monthsTracker >= 12) {
      monthsTracker = 0;
      yearTwoTracker++;
    }

    if (yearTwoTracker >= 99) {
      yearTwoTracker = 0;
      yearOneTracker++;
    }

    if (yearOneTracker >= 99) {
      yearOneTracker = 0;
    }


    previous_sec = current_sec;
  }
}
