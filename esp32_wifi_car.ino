// ESP32 + L293D  Motor Test

#define IN1 23
#define IN2 22

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  // Motor Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(2000);

  // Motor Stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(1000);

  // Motor Reverse
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(2000);

  // Motor Stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(2000);
}
