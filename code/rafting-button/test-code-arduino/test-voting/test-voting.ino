#define DEVICE_1 2
#define DEVICE_2 3
#define DEVICE_3 4
// #define DEVICE_4 ?
#define DEVICE_5 5


void pushButton(uint8_t device) {
  digitalWrite(device, HIGH);
  delay(100);
  digitalWrite(device, LOW);
}

void setup() {

  Serial.begin(9600);
  Serial.println("Program has been started");
  Serial.println("Enter device number");

  // Config pins
  pinMode(DEVICE_1, OUTPUT);
  pinMode(DEVICE_2, OUTPUT);
  pinMode(DEVICE_3, OUTPUT);
  // pinMode(DEVICE_4, OUTPUT); // not active device
  pinMode(DEVICE_5, OUTPUT);

  // Turn off all devices
  digitalWrite(DEVICE_1, LOW);
  digitalWrite(DEVICE_2, LOW);
  digitalWrite(DEVICE_3, LOW);
  // digitalWrite(DEVICE_4, LOW); // not active device
  digitalWrite(DEVICE_5, LOW);
}

void loop() {
  while (Serial.available() == 0) {}
  char c = Serial.read();
  switch (c) {
    case '1':
      pushButton(DEVICE_1);
      Serial.println("DEVICE_1");
      break;
    case '2':
      pushButton(DEVICE_2);
      Serial.println("DEVICE_2");
      break;
    case '3':
      pushButton(DEVICE_3);
      Serial.println("DEVICE_3");
      break;
    case '5':
      pushButton(DEVICE_5);
      Serial.println("DEVICE_5");
      break;
  }
}
