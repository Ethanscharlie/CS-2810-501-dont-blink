# Don't Blink

A reaction time game for the Arduino Uno. The display prompts you to press a button, then waits a random amount of time, then turns on an led and you have to press the button as quickly as possible. Your reaction time in milliseconds is displayed, along with your fastest time for that session. Pressing the button before the led turns on makes you start over.

Built for CS 2810 (Computer Architecture), Summer 2026, by Matheus Marcelino, Jaret Izumi, and Ethan J. Hadley.

## Hardware

| Part | Quantity |
|---|---|
| Arduino Uno R3 | 1 |
| LCD1602 character display (HD44780 compatible, 16 pins) | 1 |
| 5 mm LED | 1 |
| Tactile push button (4-leg) | 1 |
| 220 Ω resistor | 2 |
| 10 kΩ resistor | 1 |
| Solderless breadboard | 1 |
| Jumper wires | ~15 |

Everything runs from USB power. No external supply is needed.

## Wiring

| Component / signal | Connection |
|---|---|
| Push button, terminal A | 5 V rail |
| Push button, terminal B | Digital pin 2, plus 10 kΩ to GND |
| LED anode (long leg) | Digital pin 8 |
| LED cathode (short leg) | 220 Ω to GND |
| LCD 1 (VSS) | GND |
| LCD 2 (VDD) | 5 V |
| LCD 3 (V0) | GND |
| LCD 4 (RS) | Digital pin 12 |
| LCD 5 (RW) | GND |
| LCD 6 (E) | Digital pin 11 |
| LCD 11-14 (D4-D7) | Digital pins 4, 5, 6, 7 |
| LCD 15 (A) | 220 Ω to 5 V |
| LCD 16 (K) | GND |

LCD pins 7 through 10 are unused. The display runs in 4-bit parallel mode, which is why only D4 through D7 are connected.

Notes:

- The button uses an **external pull-down**, so that when it is pressed it is read as `HIGH`. Without the 10 kΩ resistor, the input pin would float and be noisy.
- The tactile switch's two legs on the same side are shorted internally. Seat it so its two independent terminals land in **different breadboard columns**, otherwise it conducts permanently and pin 2 reads `HIGH` no matter what.
- LCD `V0` is tied to ground for maximum contrast instead of using a potentiometer. If your display is hard to read, wire a 10 kΩ pot with its wiper to `V0`, its ends to 5 V and GND.
- Analog pin `A0` must be left **unconnected**. It is read as a noise source to seed the random number generator.

## How to play

1. On power-up the display shows `Don't Blink!` and prompts you to press the button.
2. Press to start a round. The display asks you to press when the light is on.
3. After a random delay of 1 to 5 seconds, the LED turns on.
4. Press as fast as you can. Your score appears for 3 seconds, then the session best for 3 seconds, then the title screen returns.
5. Press before the LED lights and you get `Too Early!` with no score recorded. Your best is unaffected.

## Code structure

Single sketch file. Two states, three functions.

- **`State` enum** — `START` (idle, waiting to begin) and `WAIT` (round in progress). The state machine exists because there is only one button, so the same physical press has to mean two different things.
- **`setup()`** — configures pins, initializes the display, seeds `random()` from the floating analog pin, and records the button's current level so that a reset performed while the button is held is not read as a press.
- **`onPress()`** — called only on a genuine new press. From `START` it begins a round and draws a random delay. From `WAIT` it ends the round, either voiding it as too early or computing the score and updating the best.
- **`setLight()`** — drives the LED, returning early if the requested state already matches so no redundant writes are issued.
- **`loop()`** — reads the button every iteration and acts only on a `LOW` to `HIGH` transition, with a 100 ms lockout to absorb contact bounce. It also compares the current time against the scheduled illumination time on every pass, which is what keeps the program responsive during the countdown so an early press can be caught.

Timestamps are `unsigned long` to match `millis()`. Scores are printed with separate `print()` calls rather than through `String` concatenation, to avoid heap allocation on a part with 2 KB of SRAM.
