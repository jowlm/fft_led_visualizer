#include <FastLED.h>

#define LED_TYPE    WS2811
#define COLOR_ORDER    GRB
#define NUM_LEDS       250
#define LED_PIN          6

const int analogInPin = A0;
uint8_t hue = 300;

CRGB leds[NUM_LEDS];

void setup(){
   FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS);
   FastLED.setBrightness(100);
   Serial.begin(115200);
  
}

void loop() {
    static int count = 0;    
    multi_loop();
    Serial.println(count);
    count++;
    if(count>1){
      exit(0); 
    }
    
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
      
      hue++;
    }
      //while(1){}
      return sawtooth;
}
