// Needs to have a file name that is sorted after all Mode*.ino files. Otherwise
// the classes can not be referenced here.
// TODO: Would be nice to have a structure where the modes can register
// on their own, without the need to update the modes structure here.

void ledModeInit()
{
  // All modes need to register an instance here
  modes["Colour"] = new ModeColour();
  modes["Rainbow"] = new ModeRainbow();
  modes["Clock"] = new ModeClock();
  modes["Bell Curve"] = new ModeBellCurve();
  modes["Night Rider"] = new ModeNightRider();
  modes["Circle"] = new ModeCircle();
  modes["Sparkle"] = new ModeSparkle();
  modes["Color Wipe"] = new ModeColorWipe();
  modes["Confetti"] = new ModeConfetti();
  modes["Visualiser"] = new ModeVisualiser();
  modes["Fireflies"] = new ModeFireflies();
  modes["Saturation Fade"] = new ModeSaturationFade();
}
