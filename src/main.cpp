/*
#include <FastLED.h>
#define NUM_LEDS 60
CRGB leds[NUM_LEDS];
void setup() { FastLED.addLeds<WS2812B, 3>(leds, NUM_LEDS); }
void loop() {
	leds[0] = CRGB::White; FastLED.show(); delay(30);
	leds[0] = CRGB::Black; FastLED.show(); delay(30);
}*/


#include <Arduino.h>
#include <Time.h>
#include <ezButton.h>
#include <FastLED.h>

#define DEBUG

// inputs / Outputs
#define HOUR_INCREASE_BTN_PIN 7
#define MIN_INCREASE_BTN_PIN 6
#define BRIGTNESS_BTN_PIN 12
#define STRIP_PIN 3
ezButton increaseHourBtn(HOUR_INCREASE_BTN_PIN);
ezButton increaseMinBtn(MIN_INCREASE_BTN_PIN);

#define MAX_LED_PER_WORD 4

const int MOT_IL_EST[MAX_LED_PER_WORD] = {1, 2, 0, 0};

// Hours from 1 to 11
const int MOTS_HOURS[11][MAX_LED_PER_WORD] = {
    {3, 0, 0, 0},   // 1
    {4, 0, 0, 0},   // 2
    {5, 0, 0, 0},   // 3
    {6, 0, 0, 0},   // 4
    {7, 0, 0, 0},   // 5
    {8, 0, 0, 0},   // 6
    {9, 0, 0, 0},   // 7
    {10, 0, 0, 0},  // 8
    {11, 0, 0, 0},  // 9
    {12, 0, 0, 0},  // 10
    {13, 0, 0, 0}}; // 11

const int MOT_HEURE[MAX_LED_PER_WORD] = {0, 0, 0, 0};
const int MOT_MIDI[MAX_LED_PER_WORD] = {0, 0, 0, 0};
const int MOT_MINUIT[MAX_LED_PER_WORD] = {3, 4, 0, 0};

// Minutes
const int MOT_MOINS[MAX_LED_PER_WORD] = {0, 0};

const int MOTS_MINUTES[6][MAX_LED_PER_WORD] = {
    {48, 49, 0, 0},  // 5
    {50, 51, 52, 0}, // 10
    {58, 0, 0, 0},   // 15
    {59, 0, 0, 0},   // 20
    {59, 0, 48, 49}, // 25
    {59, 0, 48, 49}  // 30 / ET DEMI
};

// Led strip configuration
#define NUM_STRIPS 1
#define NUM_LEDS 60
#define BRIGHTNESS 10
#define LED_TYPE WS2812B
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#define FRAMES_PER_SECOND 60
#define COOLING 55
#define SPARKING 120
CRGB leds[NUM_LEDS];

// Clock
int tm_hour;
int tm_min;
int tm_sec;
int tm_ms;

void refreshDisplay();

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");

  //pinMode(HOUR_INCREASE_BTN_PIN, INPUT);
  //pinMode(MIN_INCREASE_BTN_PIN, INPUT);
  //pinMode(BRIGTNESS_BTN_PIN, INPUT);

  //Init led strips
  FastLED.addLeds<WS2812B, STRIP_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);

  tm_hour = 0;
  tm_min = 0;
  tm_sec = 0;
  tm_ms = 0;

  refreshDisplay();
}

void displayHour(int text)
{
  if (text != 0)
  {
    Serial.print(text);
    Serial.print(" : ");
  }

  Serial.print(tm_hour);
  Serial.print(":");
  Serial.print(tm_min);
  Serial.print(":");
  Serial.println(tm_sec);
}
void displayHour()
{
  displayHour(0);
}

void loop()
{
  increaseHourBtn.loop();
  increaseMinBtn.loop();

  if (increaseHourBtn.isPressed())
  {
    tm_hour = tm_hour + 1;
    Serial.println("Hour +1");
    refreshDisplay();
  }

  if (increaseMinBtn.isPressed())
  {
    tm_min = (tm_min + 1);
    tm_ms = 0;
    Serial.println("Min +1");
    refreshDisplay();
  }

  //analogRead(BRIGTNESS_BTN_PIN)

  // calculate hour

  if (tm_ms < 1000)
  {
    delay(1);
    tm_ms = tm_ms + 1;
  }

  if (tm_ms > 999)
  {
    tm_sec = tm_sec + 1;
    tm_ms = 0;
    displayHour();
  }

  if (tm_sec > 59)
  {
    tm_sec = 0;
    tm_min = (tm_min + 1);

    // refresh display only every five minutes
    if ((tm_min % 5) == 0)
      refreshDisplay(); // Here we refresh the display
  }

  if (tm_min > 59)
  {
    tm_min = 0;
    tm_hour = tm_hour + 1;
  }

  if (tm_hour > 23)
  {
    tm_hour = 0;
  }
}

void updateLed(int addr, bool value)
{
  if (value == true)
  {
    Serial.print("    led ON : ");
    Serial.println(addr);
    //leds[addr = CRGB::Goldenrod];
    leds[addr].setRGB(255, 255, 255);
  }
  else
  {
    //leds[addr = CRGB::Black];
    leds[addr].setRGB(0, 0, 0);
  }
}

void updateLed(int addr, int R, int G, int B)
{
  leds[addr].setRGB(R, G, B);
}

void updateWord(const int leds[], bool value)
{
  for (int led = 0; led < MAX_LED_PER_WORD; led++)
  { // for each led of the word
    if (leds[led] == 0)
    {
      return;
    }

#ifdef DEBUG
    if (leds[led] >= NUM_LEDS)
    {
      Serial.println("##### ERROR : Num LED to high !! #######");
      return;
    }
#endif

    if (value)
    {
      updateLed(leds[led], true);
    }
    else
    {
      updateLed(leds[led], false);
    }
  }
}

void refreshDisplay()
{
  Serial.println("### REFRESH DISPLAY - START");
  updateWord(MOT_IL_EST, true);

  // update hours
  Serial.println("HOURS :");

  if (tm_hour == 12)
  {
    // Allumer MIDI
    Serial.println("Midi");
    updateWord(MOT_HEURE, false);
    updateWord(MOT_MIDI, true);
    updateWord(MOT_MINUIT, true);
  }
  else if (tm_hour == 24)
  {
    // Allumer MINUIT
    Serial.println("Minuit");
    updateWord(MOT_HEURE, false);
    updateWord(MOT_MIDI, false);
    updateWord(MOT_MINUIT, true);
  }
  else
  {
    int hour;

    if (tm_hour > 12)
    {
      hour = ((tm_hour - 12) - 1);
    }
    else
    {
      hour = tm_hour - 1;
    }
    // Allumer HEURES
    Serial.println("Heure");
    updateWord(MOT_HEURE, true);
    updateWord(MOT_MIDI, false);
    updateWord(MOT_MINUIT, false);

    for (int i = 0; i < 11; i++)
    { // for each word representing an hour
      if (MOTS_HOURS[i] == 0)
        break;
      Serial.print("  Word n. ");
      Serial.print(i + 1);
      Serial.print(" = ");
      Serial.println(hour == i);
      updateWord(MOTS_HOURS[i], hour == i);
    }
  }

  int min;
  if (tm_min > 34)
  {
    min = 60 - tm_min;
    Serial.println("Moins");
    updateWord(MOT_MOINS, true);
  }
  else
  {
    min = tm_min;
    updateWord(MOT_MOINS, false);
  }

  min = (min / 5) - 1;

  // update minutes
  Serial.println("MINUTES :");
  for (int i = 0; i < 6; i++)
  { // for each word representing a minute
    if (MOTS_MINUTES[i] == 0)
      break;
    Serial.print("  Word n.");
    Serial.print(i + 1);
    Serial.print(" : ");
    Serial.println(min == i);
    updateWord(MOTS_MINUTES[i], min == i);
  }

  Serial.println("### REFRESH DISPLAY - STOP");
  FastLED.show();
}

