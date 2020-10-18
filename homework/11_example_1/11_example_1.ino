#include <Servo.h>

// Arduino pin assignment
#define PIN_LED 9
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 180 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 360 // maximum distance to be measured (unit: mm)
#define _DIST_ALPHA 0.5

#define _DUTY_MIN 500 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1500 // servo neutral position (90 degree)
#define _DUTY_MAX 2500 // servo full counterclockwise position (180 degree)
#define _DUTY_PER_DEGREE 11.111

// global variables
float timeout; // unit: us
float dist_min, dist_max, dist_raw, dist_prev, dist_ema, alpha; // unit: mm
unsigned long last_sampling_time; // unit: ms
float scale; // used for pulse duration to distance conversion
Servo myservo;

void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  digitalWrite(PIN_TRIG, LOW); 
  pinMode(PIN_ECHO,INPUT);

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

// initialize USS related variables
  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  alpha = _DIST_ALPHA;
  timeout = (INTERVAL / 2) * 1000.0; // precalculate pulseIn() timeout value. (unit: us)
  dist_raw = dist_prev = 0.0; // raw distance output from USS (unit: mm)
  scale = 0.001 * 0.5 * SND_VEL;

// initialize serial port
  Serial.begin(115200);

// initialize last sampling time
  last_sampling_time = 0;
}

void loop() {
// wait until next sampling time. 
// millis() returns the number of milliseconds since the program started. Will overflow after 50 days.
  if(millis() < last_sampling_time + INTERVAL) return;

// get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

// output the read value to the serial port
  Serial.print("Min:100,Low:180,raw:");
  Serial.print(dist_raw);
  Serial.print(",ema:");
  Serial.print(dist_ema);
  Serial.print(",servo:");
  Serial.print(myservo.read());
  Serial.println(",Max:300");

  // adjust servo position according to the USS read value

  if (dist_min <= dist_raw && dist_raw <= dist_max) {
    dist_prev = dist_raw;
    dist_ema = dist_raw * alpha + (1 - alpha) * dist_ema;
    myservo.writeMicroseconds(_DUTY_MIN + ((int) (dist_ema - _DIST_MIN) * _DUTY_PER_DEGREE));
  } else {
    analogWrite(PIN_LED, 0);
    dist_raw = dist_prev;
    if (dist_raw < dist_min) {
      myservo.writeMicroseconds(_DUTY_MIN);
    } else {
      myservo.writeMicroseconds(_DUTY_MAX);
    }
  }

  if(_DIST_MIN <= dist_raw && dist_raw < _DIST_MAX) {
     myservo.writeMicroseconds(_DUTY_MIN + ((int) (dist_raw - _DIST_MIN) * _DUTY_PER_DEGREE));
  }
   
// update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  float reading;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  reading = pulseIn(ECHO, HIGH, timeout) * scale; // unit: mm
  if(reading < dist_min || reading > dist_max) {
    reading = 0.0; // return 0 when out of range.
    analogWrite(PIN_LED, 255);
  } else {
    analogWrite(PIN_LED, 0);
  }

  if(reading == 0.0) reading = dist_prev;
  else dist_prev = reading;
  
  return reading;
}
