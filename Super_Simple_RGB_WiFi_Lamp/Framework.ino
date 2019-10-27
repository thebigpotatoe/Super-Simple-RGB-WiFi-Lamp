

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {

  
  EVERY_N_MILLISECONDS(SpeedDelay) {
    if (SparkleActive) {
      SparklePixel = random(NUM_LEDS);
      ledString[SparklePixel] = CRGB(red,green,blue);
     } 
     else ledString[SparklePixel] = CRGB(0,0,0);
     
     SparkleActive = !SparkleActive;
  }
}


void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  EVERY_N_MILLISECONDS(SpeedDelay) {
    wipePosition++;
    if (TurningOn) {
      fill_solid(ledString, wipePosition, CRGB(red, green, blue));
      
      if (wipePosition == NUM_LEDS) {
        TurningOn = false;
        wipePosition = -1;
      }
     } 
     else {
      fill_solid(ledString, wipePosition,CRGB( 0, 0, 0));
      
      if (wipePosition == NUM_LEDS) {
        TurningOn = true;
        wipePosition = -1;
        
      }
     }  
   }
}
