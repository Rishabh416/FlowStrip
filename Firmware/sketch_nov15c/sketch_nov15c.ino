#include <Adafruit_NeoPixel.h>

const int LED_PIN = 3;
const int NUM_LEDS = 30;
const float STRIP_LENGTH = 1.0; // Changed to float to prevent integer division (1 / 30 = 0)
const float TIME_STEP = 0.02;   // seconds
const float x_values[] = {
  0.0000, 0.0009, 0.0037, 0.0084, 0.0150, 0.0235, 0.0339, 0.0463, 0.0607, 0.0772,
  0.0958, 0.1165, 0.1394, 0.1646, 0.1920, 0.2218, 0.2537, 0.2879, 0.3241, 0.3620,
  0.4014, 0.4418, 0.4827, 0.5234, 0.5635, 0.6023, 0.6395, 0.6747, 0.7077, 0.7383,
  0.7665, 0.7923, 0.8156, 0.8366, 0.8552, 0.8715, 0.8856, 0.8974, 0.9072, 0.9148,
  0.9203, 0.9238, 0.9251, 0.9246, 0.9222, 0.9179, 0.9118, 0.9039, 0.8941, 0.8824,
  0.8688, 0.8533, 0.8357, 0.8163, 0.7948, 0.7714, 0.7460, 0.7187, 0.6897, 0.6590,
  0.6269, 0.5936, 0.5595, 0.5250, 0.4905, 0.4564, 0.4231, 0.3911, 0.3606, 0.3317,
  0.3048, 0.2799, 0.2571, 0.2364, 0.2178, 0.2013, 0.1869, 0.1746, 0.1643, 0.1561,
  0.1499, 0.1457, 0.1435, 0.1433, 0.1448, 0.1481, 0.1531, 0.1598, 0.1683, 0.1785,
  0.1905, 0.2042, 0.2196, 0.2368, 0.2557, 0.2763, 0.2984, 0.3221, 0.3472, 0.3734,
  0.4007, 0.4288, 0.4574, 0.4861, 0.5148, 0.5430, 0.5704, 0.5969, 0.6221, 0.6459,
  0.6682, 0.6887, 0.7075, 0.7245, 0.7397, 0.7530, 0.7644, 0.7739, 0.7816, 0.7874,
  0.7914, 0.7934, 0.7937, 0.7923, 0.7893, 0.7848, 0.7788, 0.7711, 0.7620, 0.7513,
  0.7390, 0.7253, 0.7102, 0.6936, 0.6758, 0.6567, 0.6365, 0.6153, 0.5932, 0.5705,
  0.5474, 0.5240, 0.5007, 0.4776, 0.4549, 0.4330, 0.4119, 0.3919, 0.3730, 0.3555,
  0.3394, 0.3247, 0.3116, 0.3001, 0.2901, 0.2819, 0.2752, 0.2702, 0.2669, 0.2652,
  0.2652, 0.2666, 0.2693, 0.2733, 0.2787, 0.2854, 0.2934, 0.3026, 0.3131, 0.3247,
  0.3375, 0.3513, 0.3662, 0.3819, 0.3984, 0.4156, 0.4332, 0.4513, 0.4696, 0.4879,
  0.5061, 0.5240, 0.5415, 0.5584, 0.5745, 0.5898, 0.6041, 0.6174, 0.6295, 0.6404,
  0.6500, 0.6583, 0.6653, 0.6709, 0.6751, 0.6779, 0.6793, 0.6793, 0.6782, 0.6760,
  0.6728, 0.6685, 0.6633, 0.6570, 0.6497, 0.6416, 0.6325, 0.6226, 0.6120, 0.6007,
  0.5887, 0.5763, 0.5634, 0.5502, 0.5368, 0.5233, 0.5098, 0.4964, 0.4833, 0.4705,
  0.4582, 0.4464, 0.4353, 0.4250, 0.4154, 0.4067, 0.3989, 0.3921, 0.3864, 0.3816,
  0.3780, 0.3754, 0.3740, 0.3736, 0.3742, 0.3754, 0.3774, 0.3801, 0.3836, 0.3877,
  0.3924, 0.3978, 0.4038, 0.4104, 0.4174, 0.4249, 0.4328, 0.4410, 0.4495, 0.4582,
  0.4670
};

const int NUM_STEPS = sizeof(x_values) / sizeof(x_values[0]); // Automatically calculates array length (11)

float exactLedPos = 0.0; // Tracks sub-LED movement accurately
int currentTS = 0;       // Changed from float to int so it can index the array

float f(float x) {
  return x * x;
}

float df(float x) {
  float h = 0.0001;
  return (f(x + h) - f(x - h)) / (2.0 * h); // Added '*'
}

float integrand(float x) {
  float slope = df(x);
  return sqrt(1.0 + slope * slope);
}

float simpsonArcLength(float a, float b, int n) {
  if (n % 2 != 0) n++; 
  
  float h = (b - a) / n;
  float sum = integrand(a) + integrand(b);
  
  for (int i = 1; i < n; i++) {
    float x = a + i * h;
    if (i % 2 == 0) {
      sum += 2.0 * integrand(x); // Added '*'
    } else {
      sum += 4.0 * integrand(x); // Added '*'
    }
  }
  
  return (h / 3.0) * sum; // Added '*'
}

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(50);
  strip.clear();
  strip.show();
}

void loop() {
  // Reset simulation once we reach the second-to-last element
  if (currentTS >= NUM_STEPS - 1) {
    currentTS = 0;
    exactLedPos = 0.0;
  }

  // Round exact position to the nearest physical LED
  int currentLed = round(exactLedPos);

  // Turn on current LED (only if it is still within the strip)
  strip.clear();
  if (currentLed < NUM_LEDS) {
    strip.setPixelColor(currentLed, strip.Color(255, 0, 0));
  }
  strip.show();

  // Calculate arc length between current and next timestep
  float x0 = x_values[currentTS];
  float x1 = x_values[currentTS + 1];
  float length = simpsonArcLength(x0, x1, 20);

  // Calculate how many LEDs that arc length represents
  float metersPerLed = STRIP_LENGTH / NUM_LEDS;
  float ledsMoved = length / metersPerLed;

  // Update global variables (without 'int' in front)
  exactLedPos += ledsMoved;
  currentTS += 1;

  delay(TIME_STEP * 1000);
}