
#include "utils.h"
#include "Arduino.h"
#include "board.h"

void all_pins_init() {
  pinMode(TEST_GPIO2, OUTPUT);
  pinMode(TEST_GPIO3, OUTPUT);
  pinMode(TEST_GPIO4, OUTPUT);
  pinMode(TEST_GPIO5, OUTPUT);
  pinMode(TEST_GPIO6, OUTPUT);
  pinMode(TEST_GPIO7, OUTPUT);
  pinMode(TEST_GPIO8, OUTPUT);

  pinMode(TEST_GPIO10, OUTPUT);
  pinMode(TEST_GPIO11, OUTPUT);
  pinMode(TEST_GPIO12, OUTPUT);
  pinMode(TEST_GPIO13, OUTPUT);
  pinMode(TEST_GPIO14, OUTPUT);
  pinMode(TEST_GPIO15, OUTPUT);
  pinMode(TEST_GPIO16, OUTPUT);
  pinMode(TEST_GPIO17, OUTPUT);

  pinMode(TEST_GPIO34, OUTPUT);
  pinMode(TEST_GPIO35, OUTPUT);
  pinMode(TEST_GPIO36, OUTPUT);
  pinMode(TEST_GPIO37, OUTPUT);
  pinMode(TEST_GPIO38, OUTPUT);
  pinMode(TEST_GPIO39, OUTPUT);
  pinMode(TEST_GPIO40, OUTPUT);
  pinMode(TEST_GPIO41, OUTPUT);
  pinMode(TEST_GPIO42, OUTPUT);
  pinMode(TEST_GPIO43, OUTPUT);
  pinMode(TEST_GPIO44, OUTPUT);
}

void all_pins_test() {
  digitalWrite(TEST_GPIO2, HIGH);
  digitalWrite(TEST_GPIO3, HIGH);
  digitalWrite(TEST_GPIO4, HIGH);
  digitalWrite(TEST_GPIO5, HIGH);
  digitalWrite(TEST_GPIO6, HIGH);
  digitalWrite(TEST_GPIO7, HIGH);
  digitalWrite(TEST_GPIO8, HIGH);

  digitalWrite(TEST_GPIO10, HIGH);
  digitalWrite(TEST_GPIO11, HIGH);
  digitalWrite(TEST_GPIO12, HIGH);
  digitalWrite(TEST_GPIO13, HIGH);
  digitalWrite(TEST_GPIO14, HIGH);
  digitalWrite(TEST_GPIO15, HIGH);
  digitalWrite(TEST_GPIO16, HIGH);
  digitalWrite(TEST_GPIO17, HIGH);

  digitalWrite(TEST_GPIO34, HIGH);
  digitalWrite(TEST_GPIO35, HIGH);
  digitalWrite(TEST_GPIO36, HIGH);
  digitalWrite(TEST_GPIO37, HIGH);
  digitalWrite(TEST_GPIO38, HIGH);
  digitalWrite(TEST_GPIO39, HIGH);
  digitalWrite(TEST_GPIO40, HIGH);
  digitalWrite(TEST_GPIO41, HIGH);
  digitalWrite(TEST_GPIO42, HIGH);
  digitalWrite(TEST_GPIO43, HIGH);
  digitalWrite(TEST_GPIO44, HIGH);

  delay(1000);

  digitalWrite(TEST_GPIO2, LOW);
  digitalWrite(TEST_GPIO3, LOW);
  digitalWrite(TEST_GPIO4, LOW);
  digitalWrite(TEST_GPIO5, LOW);
  digitalWrite(TEST_GPIO6, LOW);
  digitalWrite(TEST_GPIO7, LOW);
  digitalWrite(TEST_GPIO8, LOW);

  digitalWrite(TEST_GPIO10, LOW);
  digitalWrite(TEST_GPIO11, LOW);
  digitalWrite(TEST_GPIO12, LOW);
  digitalWrite(TEST_GPIO13, LOW);
  digitalWrite(TEST_GPIO14, LOW);
  digitalWrite(TEST_GPIO15, LOW);
  digitalWrite(TEST_GPIO16, LOW);
  digitalWrite(TEST_GPIO17, LOW);

  digitalWrite(TEST_GPIO34, LOW);
  digitalWrite(TEST_GPIO35, LOW);
  digitalWrite(TEST_GPIO36, LOW);
  digitalWrite(TEST_GPIO37, LOW);
  digitalWrite(TEST_GPIO38, LOW);
  digitalWrite(TEST_GPIO39, LOW);
  digitalWrite(TEST_GPIO40, LOW);
  digitalWrite(TEST_GPIO41, LOW);
  digitalWrite(TEST_GPIO42, LOW);
  digitalWrite(TEST_GPIO43, LOW);
  digitalWrite(TEST_GPIO44, LOW);

  delay(2000);
}
