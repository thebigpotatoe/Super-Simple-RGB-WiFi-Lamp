void renderModeConfetti() {
  EVERY_N_MILLISECONDS(confettiSpeed) {
    if (confettiActive) {
      confettiPixel = random(NUM_LEDS);
      fadeToBlackBy(ledString, NUM_LEDS, 10);
      uint8_t pos = random8(NUM_LEDS);
      ledString[pos] += CHSV(random8(), random8(), random8());
    }
  }
}
