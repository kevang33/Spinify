#include <Wire.h> 
#include <LiquidCrystal_I2C.h>



// Uncomment for I2C LCD (Source: https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/):
#include <LiquidCrystal_I2C.h>

#define PLAY_CHAR 0
#define PAUSE_CHAR 1

#define TILT_SWITCH_PIN 2

// If using an I2C LCD, use I2C Scanner sketch to find address:
//LiquidCrystal_I2C LCD(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LiquidCrystal_I2C LCD(0x27, 16, 2);

byte playChar[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01110,
  B01100,
  B01000,
  B00000
};

byte pauseChar[] = {
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B00000
};

int volume = -1; // If we use a rotary encoder, need EEPROM for val to persist through power cycles

unsigned long previousScrollMillis = 0;
unsigned const long scrollInterval = 500;
int scrollPos = 0;
String songPlaying;

unsigned long previousVolumeUpdate = 0;
unsigned long volumePersistenceTime = 1000;

const int MOTOR_PIN = 3;
const uint8_t MOTOR_SPEED = 100;

String songTimeRemaining;

bool poweredOff = false;
bool isPaused = true;



// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

String command;
String on_off;
String time_left;
String curr_track;
float coms = -1;
int play_type;
int vol;

#define NEXT_TRACK 

void setup() {
  // put your setup code here, to run once:
  lcd.begin();
  Serial.begin(9600); // Baud rate

  // initialize 
  curr_track = "<SONG NAME>";
  time_left = 100;
  play_type = 1;
  vol = 50;

  // Set up the LCD's number of columns and rows:
  LCD.begin();
  LCD.createChar(PLAY_CHAR, playChar);
  LCD.createChar(PAUSE_CHAR, pauseChar);
  
  LCD.clear();
  LCD.setCursor(0, 0);
  String songDetails = curr_track; // CURR
  String nowPlaying = String("Now playing: ");
  updateSongPlaying(curr_track, nowPlaying);

  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(TILT_SWITCH_PIN, INPUT); //set pin2 as INPUT
  digitalWrite(TILT_SWITCH_PIN, HIGH);//set pin2 as HIGH

}

void scrollMessage(int row, String message, int totalColumns) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousScrollMillis >= scrollInterval) {
      previousScrollMillis = currentMillis;
      if (scrollPos >= message.length()) {
        scrollPos = 0;
      }
      else {
        scrollPos++;
      }
      LCD.setCursor(0, row);
      LCD.print(message.substring(scrollPos, scrollPos + totalColumns));
    }
}


void updateSongPlaying(String songDetails, String nowPlaying) {
  songPlaying = String(nowPlaying + songDetails);
  int totalColumns = 16;
  for (int i=0; i < totalColumns; i++) {
    songPlaying = " " + songPlaying;  
  } 
  songPlaying = songPlaying + " ";
}

void printSecondLine(unsigned long remainingTime) {
  int newVolume = analogRead(A0);
  newVolume = map(newVolume, 0, 1015, 0, 100);
  Serial.print("Volume: ");
  //Serial.println(volume);

  Serial.print("New Volume: ");
  //Serial.println(newVolume);
  
  LCD.setCursor(0, 1);
  unsigned long currentMillis = millis();

  if (newVolume != volume) {
    previousVolumeUpdate = currentMillis;
  }

  if (currentMillis - previousVolumeUpdate <= volumePersistenceTime) {
    volume = newVolume;
    LCD.print("Volume: ");
    LCD.setCursor(8, 1);
    LCD.print(volume);
    uint8_t offset = log10(volume);
    LCD.setCursor(9 + offset, 1);
    LCD.print("% ");
  }
  else {
    unsigned long seconds = remainingTime / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
//    Serial.print("Hours: ");
//    Serial.println(hours);
//    Serial.print("Minutes: ");
//    Serial.println(minutes);
//    Serial.print("Seconds: ");
//    Serial.println(seconds);
    
    remainingTime %= 1000;
    seconds %= 60;
    minutes %= 60;
    hours %= 24;

//    Serial.print("Hours: ");
//    Serial.println(hours);
//    Serial.print("Minutes: ");
//    Serial.println(minutes);
//    Serial.print("Seconds: ");
//    Serial.println(seconds);
    String secondsStr = String(seconds, DEC);
    if (seconds < 10) {
      secondsStr = "0" + secondsStr; // pad with zero
    }

    String minutesStr = String(minutes, DEC);
    if (minutes < 10 && hours > 0) {
      minutesStr = "0" + minutesStr; // pad with zero
    }
    
    songTimeRemaining = minutesStr + ":" + secondsStr;
    
    if (hours > 0) {
      songTimeRemaining = String(hours, DEC) + ":" + songTimeRemaining;
    }

    songTimeRemaining += " Left";

    for (int i = songTimeRemaining.length(); i < 16; i++) {
      songTimeRemaining += " ";  
    }
    //Serial.print("Time remaining: ");
    //Serial.println(songTimeRemaining);
    if (isPaused) {
      LCD.write(PAUSE_CHAR);
    }
    else {
      LCD.write(PLAY_CHAR);
    }
    LCD.setCursor(1, 1);
    LCD.print(" ");
    LCD.setCursor(2, 1);
    LCD.print(songTimeRemaining);
  }

}


void updateDisplay() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousScrollMillis >= scrollInterval) {
    previousScrollMillis = currentMillis;
  }
}

void turnOnMotor() {
  analogWrite(MOTOR_PIN, MOTOR_SPEED);
}

void turnOffMotor() {
  analogWrite(MOTOR_PIN, 0);
}

void powerOff() {
  LCD.clear();
  LCD.noBacklight();
  turnOffMotor();
}



void loop() {
  delay(2000);
  // put your main code here, to run repeatedly:
  const char delim[4] = ",";


  String coms = String(play_type) + "," + String(vol); 

  Serial.println(coms);

  if (poweredOff == true) {
    powerOff();
  }

  else {
    isPaused = digitalRead(TILT_SWITCH_PIN);
    scrollMessage(0, songPlaying, 16);
    printSecondLine(time_left); // TODO change to song remaining time
  }

  delay(10);
  if (Serial.available()) {
    
    command = Serial.readStringUntil(';');
    command.trim(); // remove whitespace

    // Split the input string by commas and convert each substring to an integer
    int commaIndex1 = command.indexOf(',');
    int commaIndex2 = command.indexOf(',', commaIndex1 + 1);
    int endIndex = command.indexOf(':', commaIndex2 + 1);

    if (commaIndex1 != -1 && commaIndex2 != -1) {
      on_off = command.substring(0, commaIndex1);
      time_left = command.substring(commaIndex1 + 1, commaIndex2);
      curr_track = command.substring(commaIndex2 + 1, endIndex);
    }
    // const char* rasp_to_ard = command.c_str();
    // ### DEBUG PRINTS TO LCD ### 
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print(on_off);
    // lcd.print(" ");
    // lcd.print(time_left);
    // lcd.print(" ");
    // lcd.print(curr_track);
    // lcd.setCursor(0, 1);
    // lcd.print(commaIndex1);
    // lcd.print(" ");
    // lcd.print(commaIndex2);
    // lcd.print(" ");

  }
  digitalWrite(LED_BUILTIN, 0);
}
