#include <LiquidCrystal.h>

#define PIN_LED 8
#define PIN_BUTTON 2
#define MILLIS_TO_WAIT_AFTER_PRESS 100.0f
#define TIME_BETWEEN_SCORE_DISPLAYS 3000.0f

enum class State {
  START,
  WAIT
};

auto lcd = LiquidCrystal(12, 11, 4, 5, 6, 7);
auto state = State::START;
auto light = false;
auto lastButtonPressTimestamp = 0.0f;
auto gameStartTimestamp = 0.0f;
auto timeUntilLight = 0.0f;
auto bestScore = -1.0f;

auto setLight(bool v = true) -> void {
  if (light == v) return; // Light is already what we want

  if (v) {
    light = true;
    digitalWrite(PIN_LED, HIGH);
  } else {
    light = false;
    digitalWrite(PIN_LED, LOW);
  }
}

auto onPress() -> void {
  switch (state) {
  case State::START: {
    state = State::WAIT;
    gameStartTimestamp = millis();
    timeUntilLight = random(1000, 5000);

    lcd.clear();
    lcd.print("Press when");
    lcd.setCursor(0, 1);
    lcd.print("the light is on");
  }
  break;

  case State::WAIT: {
    setLight(false);

    if (gameStartTimestamp + timeUntilLight > millis()) {
      state = State::START;

      lcd.clear();
      lcd.print("Too Early!");
      lcd.setCursor(0, 1);
      lcd.print("Press button  ->");
    } else {
      state = State::START;

      auto score = millis() - (gameStartTimestamp + timeUntilLight);
      auto bestScoreHasntBeenSetYet = bestScore < 0;
      if (score < bestScore || bestScoreHasntBeenSetYet) {
        bestScore = score;
      }

      lcd.clear();
      lcd.print("Score");
      lcd.setCursor(0, 1);
      lcd.print(String(score) + " ms");

      delay(TIME_BETWEEN_SCORE_DISPLAYS);

      lcd.clear();
      lcd.print("Best Score");
      lcd.setCursor(0, 1);
      lcd.print(String(bestScore) + " ms");

      delay(TIME_BETWEEN_SCORE_DISPLAYS);

      lcd.clear();
      lcd.print("Don't Blink!");
      lcd.setCursor(0, 1);
      lcd.print("Press button  ->");
    }
  }
  break;
  }
}

auto setup() -> void {
  randomSeed(analogRead(0));

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);

  setLight(false);

  lcd.begin(16, 2);
  lcd.print("Don't Blink!");
  lcd.setCursor(0, 1);
  lcd.print("Press button  ->");
}

auto loop() -> void {
  if (lastButtonPressTimestamp + MILLIS_TO_WAIT_AFTER_PRESS < millis()) {
    auto buttonPressed = digitalRead(PIN_BUTTON) == HIGH;
    if (buttonPressed) onPress();
    lastButtonPressTimestamp = millis();
  }

  if (state == State::WAIT) {
    if (gameStartTimestamp + timeUntilLight > millis()) setLight(false);
    else setLight(true);
  }
}
