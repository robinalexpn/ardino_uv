#include <AFMotor.h>
#include <Arduino.h>
#include <Servo.h>

#define echoPin A1
#define trigPin A2

long duration;

Servo myservo;
AF_DCMotor motor_back_left(1, MOTOR12_1KHZ);
AF_DCMotor motor_front_right(2, MOTOR34_1KHZ);
AF_DCMotor motor_front_left(3, MOTOR34_1KHZ);
AF_DCMotor motor_back_right(4, MOTOR12_1KHZ);

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  myservo.attach(9);
  Serial.println("Motor test!");
  set_speed(150);
  moveforward();
}

void set_speed(int speed) {
  for (int i = 0; i < speed; i++) {
    motor_front_right.setSpeed(i);
    motor_front_left.setSpeed(i);
    motor_back_left.setSpeed(i);
    motor_back_right.setSpeed(i);
    delay(10);
  }
}

int changehead(int *prev_incr) {
  int current_pos, increment;
  current_pos = myservo.read();

  if (current_pos >= 175 && *prev_incr == 5) {
    increment = -5;
  } else if (current_pos <= 5 && *prev_incr == -5) {
    increment = 5;
  } else {
    increment = *prev_incr;
  }

  *prev_incr = increment;
  myservo.write(current_pos + increment);
  
  return current_pos;
}

int checkObstacles() {
  int distance = 0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  delay(2);

  return distance;
}

void moveforward() {
  motor_back_left.run(FORWARD);
  motor_front_right.run(FORWARD);
  motor_front_left.run(FORWARD);
  motor_back_right.run(FORWARD);
}

void move_backwards() {
  motor_back_left.run(BACKWARD);
  motor_front_right.run(BACKWARD);
  motor_front_left.run(BACKWARD);
  motor_back_right.run(BACKWARD);
}

void move_left() {
  motor_back_left.run(BACKWARD);
  motor_front_left.run(BACKWARD);
  motor_back_right.run(FORWARD);
  motor_front_right.run(FORWARD);
}

void move_right() {
  motor_back_left.run(FORWARD);
  motor_front_left.run(FORWARD);
  motor_back_right.run(BACKWARD);
  motor_front_right.run(BACKWARD);
}

void stop() {
  Serial.print("Stopping the motor ");
  set_speed(20);
}

const int GO_FORWARD = 11;
const int STOP = 12;
const int GO_LEFT = 13;
const int GO_RIGHT = 14;
const int CHECK = 15;
const int GO_BACK = 16;

int STATE = GO_FORWARD;
bool stopped = false;
int distance = 0;
int increment = 5;

void loop() {
  while (true) {
    if (stopped) {
      changehead(&increment);
    }
    distance = checkObstacles();

    if (distance <= 5 && !stopped) {
      stopped = true;
      stop();
    }

    if (distance > 5) {
      if (stopped) {
        set_speed(150);
      }
      stopped = false;
      STATE = GO_FORWARD;
      int currentPos = myservo.read();
      if (currentPos >= 0 && currentPos <= 45) {
        STATE = GO_RIGHT;
      } else if (currentPos >= 135 && currentPos <= 185) {
        STATE = GO_LEFT;
      }
    }
    if (distance <= 5 && stopped) {
      set_speed(150);
      STATE = GO_BACK;
    }

    switch (STATE) {
      case GO_FORWARD:
        moveforward();
        break;
      case GO_LEFT:
        move_left();
        break;
      case GO_RIGHT:
        move_right();
        break;
      case GO_BACK:
        move_backwards();
        break;
    }
  }
}
