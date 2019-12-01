void renderModeColorWipe() {
  EVERY_N_MILLISECONDS(colorWipeSpeed) {
    colorWipePosition++;
    if (TurningOn) {
      fill_solid(ledString, colorWipePosition, CRGB(colorWipeRed, colorWipeGreen, colorWipeBlue));
      if (colorWipePosition == NUM_LEDS) {
        TurningOn = false;
        colorWipePosition = -1;
      }
    }
    else {
      fill_solid(ledString, colorWipePosition, CRGB( 0, 0, 0));
      if (colorWipePosition == NUM_LEDS) {
        TurningOn = true;
        colorWipePosition = -1;
      }
    }
  }
}

