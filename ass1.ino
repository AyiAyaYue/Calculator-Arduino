/*
 Author: Yunyue Li
 Date: 19.09.2020
 Assignment 1: Calculator
 */

#define POT A0
#define BTN 9
#define LED_YELLOW 7
#define LED_BLUE 6
#define LED_GREEN 5
#define LED_RED 4

//#define is processed by the preprocessor before the compiler, in this case I think using #define is faster even though const is preferred in most case

#include "Display.h"

int buttonState = HIGH;
int lastButtonState = HIGH;

enum STATES { OFF, VALUE1, OPERATOR, VALUE2, RESULT };
enum OPERATORS { ADD, SUBTRACT, TIMES, DIVIDE };

int currentState = STATES::OFF;
int value1 = 0;
int operation = 0;
int value2 = 0;

void setup() {
  Serial.begin(9600);

  pinMode(POT, INPUT);
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  Display.clear();
}

void loop() {
  buttonState = digitalRead(BTN);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      Display.clear();
      
      if (currentState == STATES::OFF) {
        Display.show("----");
        setLeds(HIGH);
        delay(2000);
        setLeds(LOW);
      } else if (currentState == STATES::RESULT) {
        reset();
      }

      // prefix: ++currentState (first increment, then logic)
      // postfix: currentState++ (first logic, then increment)

      if (++currentState > STATES::RESULT) {
        currentState = STATES::VALUE1;
      }

      digitalWrite(LED_BLUE, currentState != STATES::RESULT);
    }

    lastButtonState = buttonState;
  }

  if (currentState == STATES::VALUE1) {
    value1 = map(analogRead(POT), 0, 1023, -10, 10);
    Display.show(value1);
  } else if (currentState == STATES::OPERATOR) {
    // raise max_in and max_out by 1; fix even distrubtion
    // https://forum.arduino.cc/index.php?topic=72153.0
    operation = map(analogRead(POT), 0, 1024, 0, 4);

    if (operation == OPERATORS::ADD) {
      Display.show("a");
    } else if (operation == OPERATORS::SUBTRACT) {
      Display.show("s");
    } else if (operation == OPERATORS::TIMES) {
      Display.show("t");
    } else if (operation == OPERATORS::DIVIDE) {
      Display.show("d");
    }
  } else if (currentState == STATES::VALUE2) {
    value2 = map(analogRead(POT), 0, 1023, -10, 10);
    Display.show(value2);
  } else if (currentState == STATES::RESULT) {
    if (invalidResult()) {
      showError();
      return;
    }

    float result = 0;

    if (operation == OPERATORS::ADD) {
      result = value1 + value2;
    } else if (operation == OPERATORS::SUBTRACT) {
      result = value1 - value2;
    } else if (operation == OPERATORS::TIMES) {
      result = value1 * value2;
    } else if (operation == OPERATORS::DIVIDE) {
      result = value1 / value2;
    }

    showResult(result);
  }
}

void setLeds(bool value) {
  digitalWrite(LED_BLUE, value);
  digitalWrite(LED_YELLOW, value);
  digitalWrite(LED_GREEN, value);
  digitalWrite(LED_RED, value);
}

bool invalidResult() {
  return (operation == OPERATORS::DIVIDE && value2 == 0) || 
         (operation == OPERATORS::TIMES && value1 == -10 && value2 == 10) ||
         (operation == OPERATORS::TIMES && value2 == -10 && value1 == 10);
}

void showError() {
  Display.show("Err");
  digitalWrite(LED_RED, HIGH);
}

void showResult(float result) {
  Display.show(result);
  digitalWrite(LED_GREEN, HIGH);
}

void reset() {
  setLeds(LOW);
  delay(500);
}
