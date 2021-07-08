#include <FastLED.h>
#include <arduinoFFT.h>

#define LED_TYPE    WS2811
#define COLOR_ORDER    GRB
#define NUM_LEDS       250
#define LED_PIN         32

const int analogInPin = A0;
uint8_t hue = 300;
CRGB leds[NUM_LEDS];

#define SAMPLES         1024          
#define SAMPLING_FREQ   18000         
#define AMPLITUDE       1000          
#define AUDIO_IN_PIN    35   
#define NOISE           500 
#define NUM_BANDS       16 
#define SCL_FREQUENCY   0x02

unsigned int sampling_period_us;
byte peak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};              // The length of these arrays must be >= NUM_BANDS
int oldBarHeights[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int bandValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime;
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setup() {
    FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(100);
    Serial.begin(115200);
    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
}

void loop() {
 
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us) { /* chill */ }
  }
  FFT.DCRemoval();
  FFT.Windowing(vReal, SAMPLES,FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal,vImag,SAMPLES);
  double mag = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQ);
  
  //Serial.print(mag);
  
  //Serial.println("Computed magnitudes:");
  //PrintVector(vReal, (SAMPLES >> 1), SCL_FREQUENCY);
  //Serial.print(" ");
  
  Serial.print(maxArray(vReal, (SAMPLES >> 1)));
  Serial.println();

  if(maxArray(vReal, (SAMPLES >> 1))>10000.0){
    multi_loop();
  }
}


void PrintVector(double *vData, uint16_t bufferSize)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    
    abscissa = ((i * 1.0 * SAMPLING_FREQ) / SAMPLES);
  
    
    Serial.print(abscissa, 6);
    
    Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  //Serial.println();
}

float maxArray(double *vData, uint16_t bufferSize){
  float max_v = 0.0;
  for (uint16_t i = 0; i < bufferSize; i++){
    if(vData[i]>NOISE)
    {
       if ( vData[i] > max_v )
        {
          max_v = vData[i];
       }
    }
  }
  return max_v;
}
void show_func(int val){
      
      //Serial.println(val);
      leds[val] = CHSV(hue, 255, 255);
      FastLED.show(); 
      fadeToBlackBy(leds,NUM_LEDS,40);   
      hue++;
}

int multi_loop(){        
    
    int prev_sawtooth = 0;
    int sawtooth = 0;
    while (prev_sawtooth <= sawtooth){
      prev_sawtooth = sawtooth;
      sawtooth = map(beat8(80, 0), 0, 255, 0, 200);
      show_func(sawtooth);
      
      
    }
      //while(1){}
      return sawtooth;
}
