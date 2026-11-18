// to be updated

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 3;
const int NUM_LEDS = 30;
const float STRIP_LENGTH = 1.0; // meters
const float TIME_STEP = 0.02; // seconds
const float x_values[] = {0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7}; // x values from physics engine for each timestep
int currentLed = 0;
int currentTS = 0;

float f(float x) { // approximated function
  return x * x;
}

float df(float x) { // derivative of f(x)
  float h = 0.0001;
  return (f(x + h) - f(x - h)) / (2.0 * h);
}

// The integrand: sqrt(1 + (f'(x))^2)
float integrand(float x) {
  float slope = df(x);
  return sqrt(1.0 + slope * slope);
}

float simpsonArcLength(float a, float b, int n) {
  if (n % 2 != 0) n++; // Ensure n is even
  
  float h = (b - a) / n;
  float sum = integrand(a) + integrand(b);
  
  for (int i = 1; i < n; i++) {
    float x = a + i * h;
    if (i % 2 == 0) {
      sum += 2.0 * integrand(x);
    } else {
      sum += 4.0 * integrand(x);
    }
  }
  
  return (h / 3.0) * sum;
}

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(50); // (0-255)
  strip.show();
  strip.clear();
}

void loop() {
  // turn on currentLED
  strip.setPixelColor(currentLed, strip.Color(255, 0, 0));
  strip.show();

  // x0 at timestep 0
  float x0 = x_values[currentTS];

  // x1 at timestep 1
  float x1 = x_values[currentTS + 1];

  // arclength between x0 and x1
  float length = simpsonArcLength(x0, x1, 20);

  // STRIP_LENGTH / arclength = NumOfLedsToSkip
  float metersPerLed = STRIP_LENGTH / NUM_LEDS;
  float NumOfLedsToSkip = length / metersPerLed;

  // turn off previous led or led group
  strip.clear();

  // increment currentLed value by NumOfLedsToSkip
  currentLed += NumOfLedsToSkip;

  // delay for time step
  currentTS += 1;
  delay(TIME_STEP * 1000);
} 
