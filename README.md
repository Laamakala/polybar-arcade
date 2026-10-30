Polybar Arcade

A Raspberry Pi Pico and Pico W arcade game collection built with C++, PlatformIO, and Wokwi.

Polybar Arcade combines five illuminated fret buttons, a two-direction strum control, an analog joystick, two buzzers, and a TFT display.

The current firmware includes menu navigation, multiple difficulty levels, persistent top-four leaderboards, four-character player initials, startup light and sound effects, and four game modes.

Polybar Arcade Wokwi layoutPolybar Arcade main menuPolybar Arcade SpeedtestPolybar Arcade highscoresPolybar Arcade
Current games

    Speedtest: follow accelerating light and sound cues.
    Simon Says: repeat an increasingly long sequence of fret and strum inputs.
    Reaction: respond to a visual or audio cue as quickly as possible.
    Solo Jam: play minor, major, and blues note layouts.

Current features

    Five fret buttons with addressable RGB lighting
    Strum Up and Strum Down with dedicated LEDs
    Start and Select buttons with dedicated LEDs
    Analog joystick with center-click input
    Two buzzers for musical notes and sound effects
    ILI9341 display in the Wokwi simulation
    Physical 160 × 80 ST7735 display support
    Nine logical APA102/SK9822 LED outputs
    Adjustable volume and RGB brightness
    RGB hardware-test animation
    Normal and no-light score categories
    Persistent scores and initials using LittleFS
    Top-four leaderboards per difficulty
    Four-character initials using printable ASCII characters
    Startup hardware test and blues-inspired boot melody
    Green + Orange held during startup to mute sound

Project structure
```
polybar-arcade/
├── platformio.ini
├── diagram.json
├── wokwi.toml
├── src/
│   └── main.cpp
└── docs/
    ├── index.html
    ├── style.css
    └── assets/
        ├── diagram.png
        ├── polybar-arcade.png
        ├── polybar-arcade_menu.png
        ├── polybar-arcade_speedtest.png
        └── polybar-arcade_highscores.png
```

 GPIO mapping
Physical ST7735 display
GP9: TFT D/C
GP14: TFT SCK
GP15: TFT MOSI
GP16: TFT CS
RST: tied directly to 3V3
VCC: 3V3
GND: GND

The physical build uses the ST7735 driver. The Wokwi simulation uses an ILI9341 display as a simulator-compatible substitute.

Start and Select
GP10: Start
GP11: Select
Fret controls
GP17: Green fret
GP18: Red fret
GP19: Yellow fret
GP20: Blue fret
GP21: Orange fret
Strum controls
GP7: Strum Up
GP8: Strum Down
Joystick
GP26: Horizontal axis
GP27: Vertical axis
GP22: Joystick center click
Hall sensor
GP28: Hall sensor / whammy input

The current Arcade firmware reserves this input for future whammy support.

APA102 / SK9822 LEDs
GP3: LED Data / DI
GP6: LED Clock / CI

Audio
GP2: Effects buzzer
GP4: Note buzzer

LED chain
LED 0: Start
LED 1: Select
LED 2: Strum Down
LED 3: Strum Up
LED 4: Green fret
LED 5: Red fret
LED 6: Yellow fret
LED 7: Blue fret
LED 8: Orange fret
