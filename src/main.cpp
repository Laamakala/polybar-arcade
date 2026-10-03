// ----- LIBRARIES ----- //
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // #include <Adafruit_ST7735.h>
#include <SPI.h>

// ----- PIN DEFINITIONS ----- //
#define TFT_CS 17
#define TFT_DC 16
#define TFT_RST 20

// ----- SCREEN DIMENSIONS
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 80

// ----- HARDWARE OBJECTS ----- //
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ----- THEME COLORS ----- // 
// Boot
const uint16_t COLOR_BG      = ILI9341_BLACK;               // background color     // Black
const uint16_t COLOR_START1  = 0xFE19;                // screen start color1  // Pink
const uint16_t COLOR_START2  = tft.color565(100, 255, 190); // screen start color2  // Mint green
const uint16_t COLOR_START3  = ILI9341_CYAN;                // screen start color3  // Cyan

const uint16_t COLOR_SPLASH1  = tft.color565(64, 224, 208);;  // splash screen title1 color // Turqoiuse
const uint16_t COLOR_SPLASH2  = 0xFE19;                 // splash screen title2 color // Pink
const uint16_t COLOR_SPLASH3  = tft.color565(64, 224, 208);;  // splash screen title3 color // Turqoiuse

// text colors
const uint16_t COLOR_SELECT   = tft.color565(100, 255, 190);  // Menu selected text highlight color // Mint green
const uint16_t COLOR_BACK   = tft.color565(255, 105, 125);    // Menu BACK color // Red pastel
const uint16_t COLOR_TEXT     = tft.color565(255, 245, 180);  // Normal text color in menus         // Pale creamy yellow

// Game title colors
const uint16_t COLOR_SPEEDTEST = 0xFE19;    // ILI9341_RED;     // speedtest title
const uint16_t COLOR_SIMON     = ILI9341_YELLOW;  // simon says title
const uint16_t COLOR_REACTION  = ILI9341_CYAN;    // reaction title
const uint16_t COLOR_SOLO      = ILI9341_MAGENTA; // solo title

const uint16_t COLOR_NORMAL_HS = tft.color565(79, 200, 175);  // normal mode highscores // sea-green
const uint16_t COLOR_LIGHT_HS  = tft.color565(0, 255, 255);    // onlylight mode highscores // Cyan
const uint16_t COLOR_SOUND_HS  = tft.color565(255, 165, 0);  // onlysound mode highscores // Orange
const uint16_t COLOR_GAME_OVER  = tft.color565(128, 0, 128);  // "GAME OVER" text

const uint16_t COLOR_SUCCESS  = ILI9341_ORANGE; // new high score color
const uint16_t COLOR_ERROR    = ILI9341_RED;    // game over color


const uint16_t COLOR_FRET1    = ILI9341_GREEN;  // Fret1: green
const uint16_t COLOR_FRET2    = ILI9341_RED;    // Fret2: red
const uint16_t COLOR_FRET3    = ILI9341_YELLOW; // Fret3: yellow
const uint16_t COLOR_FRET4    = ILI9341_BLUE;   // Fret4: blue
const uint16_t COLOR_FRET5    = ILI9341_ORANGE; // Fret5: orange

const uint16_t COLOR_TURQUOISE = tft.color565(64, 224, 208); // Turquoise - blue
const uint16_t COLOR_MINT = tft.color565(100, 255, 190);     // Mint - green
const uint16_t COLOR_LAVENDER = tft.color565(190, 140, 255); // Lavender - violet or #a86cce
const uint16_t COLOR_CORAL = 0xF0AC29;    // Coral - #df9b1c
// Pale creamy yellow
const uint16_t COLOR_CREAM_YELLOW = tft.color565(255, 245, 180);
const uint16_t COLOR_PASTEL_YELLOW = tft.color565(255, 240, 150);

// ----- HARDWARE PIN ARRAYS ----- //

// 5 Fret buttons
const int BUTTON_PINS[5] = {
  1, // Green
  3, // Red
  5, // Yellow
  7, // Blue
  9  // Orange
};

// 5 Fret setup
const int LED_PINS[5] = {
  2, // Green
  4, // Red
  6, // Yellow
  8, // Blue
  10  // Orange
};

// 2 Strum setup
const int STRUM_UP_PIN = 27;
const int STRUM_DOWN_PIN = 28;

// 2 Strum LEDs
const int LED_STRUM_PINS[2] {
  22, // Cyan, strum up
  21  // Violet, strum down
};

// 2 Buzzer setup
const int NOTE_BUZZER = 15;
const int FX_BUZZER = 26;

// ----- MUSIC NOTE FREQUENCIES ----- //

// Notes for solo game, minor
const int SOLO_MINOR[6][5] = {

  // Root 0
  { 98, 117, 123, 147, 165 },    // G2 Bb2 B2 D3 E3

  // Root 1
  { 165, 196, 208, 247, 294 },   // E3 G3 Ab3 B3 D4

  // Root 2
  { 262, 311, 330, 392, 466 },   // C4 Eb4 E4 G4 Bb4

  // Root 3
  { 440, 523, 554, 659, 784 },   // A4 C5 C#5 E5 G5

  // Root 4
  { 698, 831, 880, 1047, 1245 }, // F5 Ab5 A5 C6 Eb6

  // Root 5
  { 1175, 1397, 1480, 1760, 2093 } // D6 F6 F#6 A6 C7
};

// Notes for solo game, major
const int SOLO_MAJOR[6][5] = {

  // Root 0
  { 98, 110, 123, 131, 147 },   // G2 A2 B2 C3 D3

  // Root 1
  { 165, 175, 196, 220, 247 },  // E3 F3 G3 A3 B3

  // Root 2
  { 262, 294, 330, 349, 392 },  // C4 D4 E4 F4 G4

  // Root 3
  { 440, 494, 523, 587, 659 },  // A4 B4 C5 D5 E5

  // Root 4
  { 698, 784, 880, 988, 1047 }, // F5 G5 A5 B5 C6

  // Root 5
  { 1175, 1319, 1397, 1568, 1760 } // D6 E6 F6 G6 A6
};

// Notes for solo game, blues
const int SOLO_BLUES[6][5] = {

  // G blues
  { 98, 117, 131, 147, 175 },

  // E blues
  { 165, 196, 220, 247, 294 },

  // C blues
  { 262, 311, 349, 392, 466 },

  // A blues
  { 440, 523, 587, 659, 784 },

  // F blues
  { 698, 831, 932, 1047, 1245 },

  // D blues
  { 1175, 1397, 1568, 1760, 2093 }
};

// Menu sound scale_4
const int MENU_SCALE_4[4] = {
    220, // A
    262, // C
    311, // Eb
    392  // G
};

// Menu sound scale_3
const int MENU_SCALE_3[3] = {
    220, // A
    262, // C
    311 // Eb
};

// ----- GAME DIFFICULTY MENU TRACKING ----- //

// speedtest game difficulty menu index
int speedtestIndex = 0;
const int SPEEDTEST_COUNT = 4;

// simonsays game difficulty menu index
int simonIndex = 0;
const int SIMONSAYS_COUNT = 5;

// reaction game difficulty menu index
int reactionIndex = 0;
const int REACTION_COUNT = 2;

// solo game difficulty menu index
int soloIndex = 0;
const int SOLO_COUNT = 3;

// Main menu setup
int mainMenuIndex = 0;
const int MAIN_MENU_COUNT = 4;

// ----- MENU OPTIONS ----- //

// Game menu options
const char* mainMenuOptions[MAIN_MENU_COUNT] = {
  "spedetest",
  "simon says",
  "r3action",
  "solo-jam"
};

// speedtest menu difficulty options
const char* speedtestOptions[SPEEDTEST_COUNT] = {
  "3Fs",
  "4Fs",
  "5Fs",
  "all"
};

// SimonSays menu difficulty options
const char* simonOptions[SIMONSAYS_COUNT] = {
  "2Fs",
  "3Fs",
  "4Fs",
  "5Fs",
  "strum"
};

// Reaction menu difficulty options
const char* reactionOptions[REACTION_COUNT] = {
  "simpl",
  "chaos"
};

// solo menu difficulty options
const char* soloOptions[SOLO_COUNT] = {
  "minor",
  "major",
  "blues"
};

// solo root note names
const char* ROOT_NAMES[6] = {
  "G2",
  "E3",
  "C4",
  "A4",
  "F5",
  "D6"
};

// ----- HIGH SCORE TRACKING ----- //
const int TOP_SCORE_COUNT = 4;
const int MAX_DIFFICULTIES = 5;

// ----- STRUCTURES AND ENUMS ----- //

// High score tracking variables
struct HighScores {
  int normal[MAX_DIFFICULTIES][TOP_SCORE_COUNT];
  int noLight[MAX_DIFFICULTIES][TOP_SCORE_COUNT];
};

// High score writing initials
const int INITIAL_COUNT = 4;

// Initials for Speedtest normal mode
char speedNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials for Speedtest no-light mode
char speedNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials for Simon normal mode
char simonNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials for Simon no-light mode
char simonNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials for Reaction normal mode
char reactionNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials for Reaction no-light mode
char reactionNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1];

// Initials currently being edited
char enteredInitials[INITIAL_COUNT + 1] = {
  'A', 'A', 'A', 'A', '\0'
};

int initialsPosition = 0;
int initialsCharacter = 'A';

// Points to the initials slot being edited
char* pendingInitials = nullptr;

// Rolling scrolling for every menu
int getFirstVisibleItem(
  int selectedIndex,
  int itemCount,
  int visibleRows
) {
  int firstVisible =
    selectedIndex - visibleRows + 2;

  if (firstVisible < 0) {
    firstVisible = 0;
  }

  int maxFirstVisible =
    itemCount - visibleRows;

  if (maxFirstVisible < 0) {
    maxFirstVisible = 0;
  }

  if (firstVisible > maxFirstVisible) {
    firstVisible = maxFirstVisible;
  }

  return firstVisible;
}

// High score instances
HighScores speedHS;
HighScores simonHS;
HighScores reactionHS;
HighScores soloHS;

// Music note variables
struct MusicNote {
  int x;
  int y;
  bool active;
  int noteType;    // fret color
  int symbolIndex; // visual shape
};

// Game state setup tracking
enum AppState {
  STATE_SPLASH,
  STATE_MAIN_MENU,
  STATE_SPEEDTEST_MENU,
  STATE_SIMON_MENU,
  STATE_REACTION_MENU,
  STATE_SOLO_MENU,
  STATE_PLAYING,
  STATE_GAMEOVER,
  STATE_ENTER_INITIALS
};

// Game type tracking
enum GameType {
  GAME_SPEEDTEST,
  GAME_SIMON,
  GAME_REACTION,
  GAME_SOLO
};

// ----- GLOBAL STATE VARIABLES ----- //

// Current and previous state tracking
AppState currentState = STATE_SPLASH;
AppState previousState = STATE_SPLASH;

// Default gametype at boot
GameType currentGame = GAME_SPEEDTEST;

// Game state variables
int numInputs = 0;
int targetQueue[60];
int queueSize = 0;
int lastScore = 0;
int soloRoot  = 3;

MusicNote notes[10];
int activeNotes = 0;

  /*/ Music notes for different frets
const char* symbols[5] = {
    "♪",
    "♫",
    "♩",
    "♬",
    "♭"
};*/

// ----- FUNCTION PROTOTYPES ----- //

// Game functions
void runSpeedtestGame();
void runSimonsaysGame();
void runReactionGame();
void runSoloGame();

// Drawing game menus
void drawSplashScreen();
void drawMainMenu();
void drawSpeedtestMenu();
void drawSimonMenu();
void drawReactionMenu();
void drawSoloMenu();
void drawMenuControls();

// Drawing game screens
void drawSoloRoot();
void drawSoloPlayingScreen();

void drawPlayingScreen(int score);
void drawGameOverScreen(int score);

void drawReactionWaitScreen();
void drawReactionGoScreen();
void drawReactionTimeScreen(unsigned long reaction);
void drawTooEarlyScreen();

// Drawing Leaderboard screens
void drawSpeedLeaderboard();
void drawSimonLeaderboard();
void drawReactionLeaderboard();
void startInitialsEntry();
void drawInitialsEntry();
void updateInitialsEntry();

void prepareInitialsSlot(
  char initials[TOP_SCORE_COUNT][INITIAL_COUNT + 1],
  int position
);

// Update functions
void updateMainMenu();
void updateSpeedtestMenu();
void updateSimonMenu();
void updateReactionMenu();
void updateSoloMenu();

// Hardware helpers
void playStartupFretSequence();
void startReactionCue(int targetFret);
void waitForGreenPress();
void turnOffAllInputLights();
void updateMenuLEDs();
void playTone(int buzzerPin, unsigned int frequency, unsigned long duration);

// Configuration helpers
void configureSpeedtestDifficulty();
void configureSimonsaysDifficulty();
void configureReactionDifficulty();

// Input helpers
bool menuReady();
bool greenPressed();
bool redPressed();
bool yellowPressed();
bool bluePressed();
bool orangePressed();
bool strumUpPressed();
bool strumDownPressed();
bool menuReady();


// ----- GLOBAL STATE VARIABLES ----- //

// Game fret light and sound start state
bool useLight = true;
bool useSound = true;
bool newHighScore = false;

// Buttons start not pressed
bool lastUp = HIGH;
bool lastDown = HIGH;
bool lastGreen = HIGH;
bool lastRed = HIGH;
bool lastYellow = HIGH;
bool lastBlue = HIGH;
bool lastOrange = HIGH;
bool lastStrumUp = HIGH;
bool lastStrumDown = HIGH;
bool reactionRequireCorrectFret = false;

// Global function to insert a new score into the top scores array
int insertTopScore(
  int scores[TOP_SCORE_COUNT],
  int newScore
);

bool insertTopReactionTime(
  int scores[TOP_SCORE_COUNT],
  int newTime
) {
  for (int i = 0; i < TOP_SCORE_COUNT; i++) {

    // Zero means the position is empty.
    // Otherwise, a lower reaction time is better.
    if (
      scores[i] == 0 ||
      newTime < scores[i]
    ) {

      for (
        int j = TOP_SCORE_COUNT - 1;
        j > i;
        j--
      ) {
        scores[j] = scores[j - 1];
      }

      scores[i] = newTime;
      return true;
    }
  }

  return false;
}

// Global function to insert a new reaction time into the top reaction times array
bool insertTopReactionTime(
  int scores[TOP_SCORE_COUNT],
  int newTime
);

// Global key-press cooldown
unsigned long lastMenuInput = 0;
const unsigned long MENU_COOLDOWN = 50;
unsigned long reactionTimes[5];

// ----- FUNCTION DEFINITIONS ----- //

// cooldown after select or back
bool menuReady() {
  if (millis() - lastMenuInput < MENU_COOLDOWN) {
    return false;
  }
  lastMenuInput = millis();
  return true;
}

// Global Green helper
bool greenPressed() {
  bool green = digitalRead(BUTTON_PINS[0]);
  bool pressed =
      (green == LOW &&
       lastGreen == HIGH &&
       menuReady());
  lastGreen = green;
  return pressed;
}

// Global Red helper
bool redPressed() {
  bool red = digitalRead(BUTTON_PINS[1]);
  bool pressed =
      (red == LOW &&
       lastRed == HIGH &&
       menuReady());
  lastRed = red;
  return pressed;
}

// Global Yellow helper
bool yellowPressed() {
  bool yellow = digitalRead(BUTTON_PINS[2]);
  bool pressed =
      (yellow == LOW &&
       lastYellow == HIGH &&
       menuReady());
  lastYellow = yellow;
  return pressed;
}

// Global Blue helper
bool bluePressed() {
  bool blue = digitalRead(BUTTON_PINS[3]);
  bool pressed =
      (blue == LOW &&
       lastBlue == HIGH &&
       menuReady());
  lastBlue = blue;
  return pressed;
}

// Global Orange helper
bool orangePressed() {
  bool orange = digitalRead(BUTTON_PINS[4]);
  bool pressed =
      (orange == LOW &&
       lastOrange == HIGH &&
       menuReady());
  lastOrange = orange;
  return pressed;
}

// Global strumUp helper
bool strumUpPressed() {
  bool strumUp = digitalRead(STRUM_UP_PIN);
  bool pressed =
      (strumUp == LOW &&
       lastStrumUp == HIGH &&
       menuReady());
  lastStrumUp = strumUp;
  return pressed;
}

// Global StrumDown helper
bool strumDownPressed() {
  bool strumDown = digitalRead(STRUM_DOWN_PIN);
  bool pressed =
      (strumDown == LOW &&
       lastStrumDown == HIGH &&
       menuReady());
  lastStrumDown = strumDown;
  return pressed;
}

bool soundEnabled = true;

// Global sound enabled helper
int insertTopScore(
  int scores[TOP_SCORE_COUNT],
  int newScore
) {
  for (int i = 0; i < TOP_SCORE_COUNT; i++) {
    if (newScore > scores[i]) {

      // Move lower scores down
      for (
        int j = TOP_SCORE_COUNT - 1;
        j > i;
        j--
      ) {
        scores[j] = scores[j - 1];
      }

      scores[i] = newScore;
      // Return leaderboard position: 0-3
      return i;
    }
  }
  // Score did not reach top four
  return -1;
}

// Leaderboard open state helper
bool speedLeaderboardOpen = false;
bool simonLeaderboardOpen = false;
bool reactionLeaderboardOpen = false;

// ----- SOUND FUNCTIONS ----- //

// Global helper for playing tones with sound enabled check
void playTone(
  int buzzerPin,
  unsigned int frequency,
  unsigned long duration
) {
  if (!soundEnabled) {
    return;
  }

  tone(
    buzzerPin,
    frequency,
    duration
  );
}

// Navigate select Sounds
void playSelectSound() {
  playTone(FX_BUZZER, 277, 16); // tone(FX_BUZZER, 330, 8); v1
  delay(16);
  playTone(FX_BUZZER, 370, 24); // tone(FX_BUZZER, 660, 8); v1
}

// Navigate back sound
void playBackSound() {
  playTone(FX_BUZZER, 349, 16); // tone(FX_BUZZER, 660, 8); v1
  delay(16);
  playTone(FX_BUZZER, 247, 24); // tone(FX_BUZZER, 330, 8); v1
}

// Navigate up sound // EDIT
void playNavUpSound() {
  playTone(FX_BUZZER, 330, 8);
  delay(16);
  playTone(NOTE_BUZZER, 300, 8);
}

// navigate down sound // EDIT
void playNavDownSound() {
  playTone(FX_BUZZER, 262, 8);
  delay(32);
  playTone(NOTE_BUZZER, 275, 8);
}

// Game over Sound
void playGameOverSound() {
  playTone(FX_BUZZER, 220, 30);
  playTone(NOTE_BUZZER, 180, 50);
  delay(20);

  playTone(FX_BUZZER, 120, 70);
  playTone(NOTE_BUZZER, 100, 40);
  delay(80);

  noTone(FX_BUZZER);
  noTone(NOTE_BUZZER);
}

// New high score sound
void playVictorySound() {
  playTone(FX_BUZZER, 523, 16);
  delay(5);
  playTone(FX_BUZZER, 659, 16);
  delay(5);
  playTone(FX_BUZZER, 784, 16);
  delay(5);
  playTone(FX_BUZZER, 1047, 50);
  delay(60);
  noTone(FX_BUZZER);
}

// ----- GAMEPLAY FUNCTIONS ----- //

// Spawn music note for background animations
void spawnNote(int fret) {

    if (activeNotes >= 10)
        return;

    notes[activeNotes].active = true;

    notes[activeNotes].symbolIndex = fret;
    notes[activeNotes].noteType = random(0, 5);
    
    // notes[activeNotes].color = noteColor;

    notes[activeNotes].x = 155;
    notes[activeNotes].y = 75 - (soloRoot * 15 + fret * 5);

    activeNotes++;
}

// Print music notes for the background animation
void drawNotes() {
    for (int i = 0; i < activeNotes; i++) {
      uint16_t noteColor;      
      switch (notes[i].symbolIndex) {
        case 0: noteColor = COLOR_FRET1; break;
        case 1: noteColor = COLOR_FRET2; break;
        case 2: noteColor = COLOR_FRET3; break;
        case 3: noteColor = COLOR_FRET4; break;
        case 4: noteColor = COLOR_FRET5; break;
        default:
            noteColor = COLOR_TEXT;
      }
    switch (notes[i].noteType) {
    // Quarter note
    case 0:
        tft.fillCircle(
            notes[i].x,
            notes[i].y,
            3,
            noteColor
        );
        tft.drawFastVLine(
            notes[i].x + 3,
            notes[i].y - 10,
            10,
            noteColor
        );
        break;

    // Eighth note
    case 1:
        tft.fillCircle(
            notes[i].x,
            notes[i].y,
            3,
            noteColor
        );
        tft.drawFastVLine(
            notes[i].x + 3,
            notes[i].y - 10,
            10,
            noteColor
        );
        tft.drawLine(
            notes[i].x + 3,
            notes[i].y - 10,
            notes[i].x + 8,
            notes[i].y - 6,
            noteColor
        );
        break;

    // Double flag note
    case 2:
        tft.fillCircle(
            notes[i].x,
            notes[i].y,
            3,
            noteColor
        );
        tft.drawFastVLine(
            notes[i].x + 3,
            notes[i].y - 12,
            12,
            noteColor
        );
        tft.drawLine(
            notes[i].x + 3,
            notes[i].y - 12,
            notes[i].x + 8,
            notes[i].y - 8,
            noteColor
        );
        tft.drawLine(
            notes[i].x + 3,
            notes[i].y - 8,
            notes[i].x + 8,
            notes[i].y - 4,
            noteColor
        );
        break;

    // Hollow note
    case 3:
        tft.drawCircle(
            notes[i].x,
            notes[i].y,
            3,
            noteColor
        );
        tft.drawFastVLine(
            notes[i].x + 3,
            notes[i].y - 10,
            10,
            noteColor
        );
        break;

    // Diamond note
    case 4:
        tft.drawLine(
            notes[i].x,
            notes[i].y - 3,
            notes[i].x + 3,
            notes[i].y,
            noteColor
        );
        tft.drawLine(
            notes[i].x + 3,
            notes[i].y,
            notes[i].x,
            notes[i].y + 3,
            noteColor
        );
        tft.drawLine(
            notes[i].x,
            notes[i].y + 3,
            notes[i].x - 3,
            notes[i].y,
            noteColor
        );
        tft.drawLine(
            notes[i].x - 3,
            notes[i].y,
            notes[i].x,
            notes[i].y - 3,
            noteColor
        );
        break;
    }
  }
}

// Music note background animation
void updateNotes() {
    for (int i = 0; i < activeNotes; i++) {
        if (!notes[i].active)
            continue;
        notes[i].x -= 5;
        if (notes[i].x < -10) {
            notes[i].active = false;
        }
    }
}

// cleanup old notes
void cleanupNotes() {
    int writeIndex = 0;
    for (int i = 0; i < activeNotes; i++) {
        if (notes[i].active) {
            notes[writeIndex] = notes[i];
            writeIndex++;
        }
    }
    activeNotes = writeIndex;
}

// ----- Boot Splash screen and Main Menu ----- //

// Start up Splash screen
void drawSplashScreen() {
  tft.fillScreen(COLOR_BG);
  int16_t x1, y1;
  uint16_t w, h;

  // min/max font
  tft.setTextColor(COLOR_SPLASH1);
  tft.setTextSize(2);
  tft.getTextBounds("min-max", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, 4);
  tft.print("min/max");

  // ARCADE font
  tft.setTextColor(COLOR_SPLASH2);
  tft.setTextSize(4);
  tft.getTextBounds("ARCADE", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, 28);
  tft.print("ARCADE");

  // min/max font
  tft.setTextColor(COLOR_SPLASH3);
  tft.setTextSize(2);
  tft.getTextBounds("-laamakala-", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, 68);
  tft.print("-laamakala-");
}

// draw main menu to select game
void drawMainMenu() {

  tft.fillScreen(COLOR_BG);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);

  for (int i = 0; i < MAIN_MENU_COUNT; i++) {
    int y = 2 + (i * 18);

    if (i == mainMenuIndex) {
      tft.setTextColor(COLOR_SELECT);
      char buffer [32];
      sprintf(buffer, "> %s <", mainMenuOptions[i]);
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
      tft.setCursor((SCREEN_WIDTH -w) / 2, y+4);
      tft.print(buffer);

    } else {
      tft.setTextColor(COLOR_TEXT);
      int16_t x1, y1;
      uint16_t w, h;

      tft.getTextBounds(mainMenuOptions[i], 0, 0, &x1, &y1, &w, &h);
      tft.setCursor((SCREEN_WIDTH -w) / 2, y);
      tft.print(mainMenuOptions[i]);
    }
  }

  // Bottom-right1 ✓ check mark
  tft.drawLine(2, 75, 4, 77, COLOR_SELECT);
  tft.drawLine(4, 77, 9, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 78, COLOR_BACK);
  tft.drawLine(158, 71, 152, 78, COLOR_BACK);
}


// ----- Draw Game Screens ----- //

void prepareInitialsSlot(
  char initials[TOP_SCORE_COUNT][INITIAL_COUNT + 1],
  int position
) {
  // Move existing initials down
  for (
    int i = TOP_SCORE_COUNT - 1;
    i > position;
    i--
  ) {
    strcpy(
      initials[i],
      initials[i - 1]
    );
  }

  // Default initials for the new score
  strcpy(initials[position], "AAAA");
}

// Enter initials to new top4 highscore
void startInitialsEntry() {

  enteredInitials[0] = 'A';
  enteredInitials[1] = 'A';
  enteredInitials[2] = 'A';
  enteredInitials[3] = 'A';
  enteredInitials[4] = '\0';

  initialsPosition = 0;
  initialsCharacter = 'A';

  turnOffAllInputLights();

  currentState = STATE_ENTER_INITIALS;
}

// Draw initials entry screen after new highscore
void drawInitialsEntry() {
  tft.fillScreen(COLOR_BG);

  // Title
  tft.setTextColor(COLOR_SUCCESS);
  tft.setTextSize(2);
  tft.setCursor(2, 2);
  tft.print("NEW HIGHSCORE");

  // Achieved score
  tft.setTextColor(COLOR_SPEEDTEST);
  tft.setTextSize(3);

  char scoreBuffer[12];

  snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", lastScore);

  int16_t scoreX1, scoreY1;
  uint16_t scoreW, scoreH;

  tft.getTextBounds(
    scoreBuffer,
    0, 0,
    &scoreX1, &scoreY1,
    &scoreW, &scoreH
  );

  tft.setCursor(
    (SCREEN_WIDTH -scoreW) / 2,
    20
  );

  tft.print(scoreBuffer);

  // Four initials
  tft.setTextSize(3);

  for (int i = 0; i < INITIAL_COUNT; i++) {

    int x = 22 + i * 35;
    int y = 50;

    if (i == initialsPosition) {

      // Selected letter
      tft.setTextColor(COLOR_SELECT);

      // Selection box
      tft.drawRect(
        x - 4,
        y - 4,
        27,
        31,
        COLOR_SELECT
      );

    } else {
      tft.setTextColor(COLOR_TEXT);
    }

    tft.setCursor(x, y);
    tft.print(enteredInitials[i]);
  }

  soundEnabled = false;

/*
  // Small control instructions
  tft.setTextSize(1);
  tft.setTextColor(COLOR_PASTEL_YELLOW);
  tft.setCursor(2, 70);
  tft.print("STRUM:CHANGE G:OK R:BACK");
*/
}

// Final Score screen
void drawPlayingScreen(int score) {

    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(4);
    
    char buffer[10];
    sprintf(buffer, "%d", score);

    int16_t x1, y1;
    uint16_t w, h;

    tft.getTextBounds(buffer,0,0,&x1,&y1,&w,&h);
    tft.setCursor((SCREEN_WIDTH -w)/2, (SCREEN_HEIGHT -h)/2);
    tft.print(buffer);
}

// Screen for playing solo mode
void drawSoloPlayingScreen() {

  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_SOLO);
  tft.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(
    "solo",
    0, 0,
    &x1, &y1,
    &w, &h
  );

  tft.setCursor(
    (SCREEN_WIDTH - w) / 3,
    2
  );

  tft.print("solo");
  drawSoloRoot();
}

// Game over screen with score and new high score notification
void drawGameOverScreen(int score) {

  tft.fillScreen(COLOR_BG);

  if (newHighScore) {
    tft.setTextColor(COLOR_SUCCESS);
    tft.setTextSize(2);

    int16_t x1, y1;
    uint16_t w, h;

    tft.getTextBounds("NEW HIGHSCORE", 0, 0,
                      &x1, &y1, &w, &h);

    tft.setCursor((SCREEN_WIDTH - w) / 2, 6);
    tft.print("NEW HIGHSCORE");

/*/ From this part is new
    if (newHighScore && pendingInitials != nullptr) {

    tft.setTextSize(1);
    tft.setTextColor(COLOR_SELECT);

    int16_t promptX1, promptY1;
    uint16_t promptW, promptH;

    tft.getTextBounds(
      "ENTER INITIALS",
      0, 0,
      &promptX1, &promptY1,
      &promptW, &promptH
    );

    tft.setCursor(
      (SCREEN_WIDTH - promptW) / 2,
      69
    );

    tft.print("ENTER INITIALS");
  }
// to this part is new
*/

  } else {
    tft.setTextColor(COLOR_GAME_OVER);
    tft.setTextSize(2);

    int16_t x1, y1;
    uint16_t w, h;

    tft.getTextBounds("GAME OVER", 0, 0,
                      &x1, &y1, &w, &h);

    tft.setCursor((SCREEN_WIDTH - w) / 2, 6);
    tft.print("GAME OVER");
  }

  // Score
  char buffer[10];
  sprintf(buffer, "%d", score);

  int16_t x1, y1;
  uint16_t w, h;

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(4);

  tft.getTextBounds(buffer, 0, 0,
                    &x1, &y1, &w, &h);

  int scoreY = newHighScore ? 34 : 28;

  tft.setCursor((SCREEN_WIDTH - w) / 2, scoreY);
  tft.print(buffer);

  // High score
  tft.setTextSize(1);
}

// Reaction game: Draw screen to wait for the player to press the green button
void drawReactionWaitScreen() {

  tft.fillScreen(COLOR_BG);

  tft.setTextColor(COLOR_REACTION);
  tft.setTextSize(3);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds("WAIT", 0, 0, &x1, &y1, &w, &h);

  tft.setCursor((SCREEN_WIDTH - w) / 2, 25);
  tft.print("WAIT");
}

// Reaction game: result screen after 5 rounds
void drawReactionResultScreen(int average, int best, int worst) {

    tft.fillScreen(COLOR_BG);

    tft.setTextColor(COLOR_REACTION);
    tft.setTextSize(2);

    tft.setCursor(10, 2);
    tft.print("RESULT");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);

    tft.setCursor(10, 25);
    tft.print("AVG:");
    tft.print(average);

    tft.setCursor(10, 45);
    tft.print("BEST:");
    tft.print(best);

    tft.setCursor(10, 65);
    tft.print("WORST:");
    tft.print(worst);
}

// Reaction game: Draw screen after pressing the button, showing reaction time
void drawReactionTimeScreen(unsigned long reaction) {

    tft.fillScreen(COLOR_SELECT);
    tft.setTextColor(COLOR_BG);
    tft.setTextSize(2);

    tft.setCursor(10, 10);
    tft.print(reaction);
    tft.print(" ms");

    tft.setCursor(10, 40);
    tft.print("PRESS");

    tft.setTextColor(COLOR_TEXT);
    tft.print(" G");
}

/* // Reaction game: Draw screen for when the player presses a button too early
void drawTooEarlyScreen() {

    tft.fillScreen(COLOR_ERROR);

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);

    tft.setCursor(10, 10);
    tft.print("NOT YET");

    tft.setCursor(10, 40);
    tft.print("PRESS");

    tft.setTextColor(COLOR_NORMAL_HS);
    tft.print(" G");
}
    */

// Reaction game: Wait for the player to press the green button before starting the next round
void waitForGreenPress() {

    // release
    while (digitalRead(BUTTON_PINS[0]) == LOW)
        delay(1);

    // press
    while (digitalRead(BUTTON_PINS[0]) == HIGH)
        delay(1);

    // release again
    while (digitalRead(BUTTON_PINS[0]) == LOW)
        delay(1);
}

// Reaction game: turn off all fret LEDs
void turnOffAllInputLights() {
  for (int i= 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  for (int i= 0; i < 2; i++) {
    digitalWrite(LED_STRUM_PINS[i], LOW);
  }
}

// Reaction game: go screen
void drawReactionGoScreen() {

  tft.fillScreen(COLOR_SELECT);

  tft.setTextColor(COLOR_REACTION);
  tft.setTextSize(4);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds("GO", 0, 0, &x1, &y1, &w, &h);

  tft.setCursor((SCREEN_WIDTH - w) / 2, 20);

  tft.print("GO");
}

// ----- HELPER FUNCTIONS FOR CONFIGURING DIFFICULTY ----- //

// Reaction game: function for drawing different screens
void startReactionCue(int targetFret) {

  turnOffAllInputLights();

  drawReactionGoScreen();

  // Visual cue
  if (useLight) {
    if (reactionIndex == 0) {
      // Simple mode accepts any fret
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PINS[i], HIGH);
      }

    } else {
      // Chaos mode requires the target fret
      digitalWrite(
        LED_PINS[targetFret],
        HIGH
      );
    }
  }

  // Audio cue
  if (useSound) {
    if (reactionIndex == 0) {
      playTone(
        NOTE_BUZZER,
        575,
        120
      );

    } else {
      // Each target fret has a different note      
      playTone(
        NOTE_BUZZER,
        SOLO_BLUES[3][targetFret],
        120
      );
    }
  }
}

// speedtest difficulty helper
void configureSpeedtestDifficulty() {
  switch (speedtestIndex) {
    case 0: numInputs = 3; break;
    case 1: numInputs = 4; break;
    case 2: numInputs = 5; break;
    case 3: numInputs = 6; break;
  }
}

// simonsays difficulty helper
void configureSimonsaysDifficulty() {
  switch (simonIndex) {
    case 0: numInputs = 2; break;
    case 1: numInputs = 3; break;
    case 2: numInputs = 4; break;
    case 3: numInputs = 5; break;
    case 4: numInputs = 6; break; // 5 frets + strum bar
  }
}

// Reaction difficulty helper
void configureReactionDifficulty() {
    reactionRequireCorrectFret =
        (reactionIndex == 1);
}
 
// ----- DRAW LEADERBOARD SCREENS ----- //

// Speedtest leaderboard
void drawSpeedLeaderboard() {

  tft.fillScreen(COLOR_BG);

  // Title
  tft.setTextColor(COLOR_SPEEDTEST);
  tft.setTextSize(2);
  tft.setCursor(2, 2);
  tft.print("fastestests"); // "fast AF"

  // Small medal icon, top-right corner
  // Medal ribbons
  tft.drawLine(149, 2, 153, 7, COLOR_PASTEL_YELLOW);
  tft.drawLine(157, 2, 153, 7, COLOR_PASTEL_YELLOW);
  
  // Medal circle
  tft.drawCircle(153, 10, 4, COLOR_PASTEL_YELLOW);
  // Medal center dot
  tft.fillCircle(153, 10,  1,  COLOR_PASTEL_YELLOW);

  // Print highscores
  for (int i = 0; i < TOP_SCORE_COUNT; i++) {

    int y;

    if (i == 0) {
      y = 20;
    } else {
      y = 40 + (i - 1) * 12;
    }

    // Position number
    tft.setTextSize(i == 0 ? 2 : 1);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(2, y);
    tft.print(i + 1);

    if (i == 0) {
      // Custom dot closer to the text-size-2 number
      tft.fillCircle(13, y +12, 1, COLOR_TEXT);
    } else {
      // Normal font spacing is fine for places 2-4
      tft.print("."); 
    }
    
    // Normal-mode score
    tft.setTextColor(COLOR_SELECT);
    tft.setCursor(i == 0 ? 22 : 18, y);
    tft.print(speedHS.normal[speedtestIndex][i]);

    // Normal-mode initials
    tft.setTextSize(1);
    tft.setCursor(50, y);

    if (speedNormalInitials[speedtestIndex][i][0] == '\0') {
      tft.print("----");
    } else {
      tft.print(
        speedNormalInitials[speedtestIndex][i]
      );
    }

    // No-light score
    tft.setTextSize(i == 0 ? 2 : 1);
    tft.setTextColor(COLOR_PASTEL_YELLOW);
    tft.setCursor(i == 0 ? 88 : 90, y);
    tft.print(speedHS.noLight[speedtestIndex][i]);

    // No-light initials
    tft.setTextSize(1);
    tft.setCursor(120, y);

    if (speedNoLightInitials[speedtestIndex][i][0] == '\0') {
      tft.print("----");
    } else {
      tft.print(
        speedNoLightInitials[speedtestIndex][i]
      );
    }
  }

}

// Simon Says leaderboard
void drawSimonLeaderboard() {
  tft.fillScreen(COLOR_BG);

  // Title
  tft.setTextColor(COLOR_SIMON);
  tft.setTextSize(2);
  tft.setCursor(2, 2);
  tft.print("memoriests");

  // Small medal icon, top-right corner
  // Medal ribbons
  tft.drawLine(149, 2, 153, 7, COLOR_PASTEL_YELLOW);
  tft.drawLine(157, 2, 153, 7, COLOR_PASTEL_YELLOW);
  
  // Medal circle
  tft.drawCircle(153, 10, 4, COLOR_PASTEL_YELLOW);
  // Medal center dot
  tft.fillCircle(153, 10,  1,  COLOR_PASTEL_YELLOW);

  // Print Highscores, 1st place larger font, 2nd-4th smaller font
  tft.setTextSize(2);
  
  for (int i = 0; i < TOP_SCORE_COUNT; i++) {
    int y;

    if (i == 0) {
      tft.setTextSize(2); // First place
      y = 20;
    } else {
      tft.setTextSize(1); // Places 2-4
      y = 43 + (i-1) * 16;
    } 

    // Position number
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(2, y);
    tft.print(i + 1);
    tft.print(".");

    // Normal Mode High Score
    tft.setTextColor(COLOR_SELECT);
    tft.setCursor(35, y);
    tft.print(simonHS.normal[simonIndex][i]);

    // No Light Mode High Score
    tft.setTextColor(COLOR_PASTEL_YELLOW);
    tft.setCursor(105, y);
    tft.print(simonHS.noLight[simonIndex][i]);
  }
}

// Reaction leaderboard
void drawReactionLeaderboard() {
  tft.fillScreen(COLOR_BG);

  // Title
  tft.setTextColor(COLOR_REACTION);
  tft.setTextSize(2);
  tft.setCursor(2, 2);
  tft.print("pinglessest");

  // Small medal icon, top-right corner
  // Medal ribbons
  tft.drawLine(149, 2, 153, 7, COLOR_PASTEL_YELLOW);
  tft.drawLine(157, 2, 153, 7, COLOR_PASTEL_YELLOW);
  
  // Medal circle
  tft.drawCircle(153, 10, 4, COLOR_PASTEL_YELLOW);
  // Medal center dot
  tft.fillCircle(153, 10,  1,  COLOR_PASTEL_YELLOW);

  // Print Highscores, 1st place larger font, 2nd-4th smaller font
  tft.setTextSize(2);
  
  for (int i = 0; i < TOP_SCORE_COUNT; i++) {
    int y;
    if (i == 0) {
      tft.setTextSize(2); // First place
      y = 20;
    } else {
      tft.setTextSize(1); // Places 2-4
      y = 43 + (i-1) * 16;
    } 

    // Position number
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(2, y);
    tft.print(i + 1);
    tft.print(".");

    // Normal Mode High Score
    tft.setTextColor(COLOR_SELECT);
    tft.setCursor(35, y);
    tft.print(reactionHS.normal[reactionIndex][i]);

    // No Light Mode High Score
    tft.setTextColor(COLOR_PASTEL_YELLOW);
    tft.setCursor(105, y);
    tft.print(reactionHS.noLight[reactionIndex][i]);
  }
}

// ----- DRAW MENU SCREENS ----- //

// Draw menu controls for select, highscores and back
void drawMenuControls() {

  // Bottom-right1 ✓ check mark
  tft.drawLine(139, 75, 141, 77, COLOR_SELECT);
  tft.drawLine(141, 77, 146, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 78, COLOR_BACK);
  tft.drawLine(158, 71, 152, 78, COLOR_BACK);

  // Small medal icon, top-right corner
  // Medal ribbons
  tft.drawLine(149, 2, 153, 7, COLOR_PASTEL_YELLOW);
  tft.drawLine(157, 2, 153, 7, COLOR_PASTEL_YELLOW);
  
  // Medal circle
  tft.drawCircle(153, 10, 4, COLOR_PASTEL_YELLOW);
  // Medal center dot
  tft.fillCircle(153, 10,  1,  COLOR_PASTEL_YELLOW);
}

// Speedtest difficulty menu
void drawSpeedtestMenu() {

  // Title, font, color, background
  tft.fillScreen(COLOR_BG);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_SPEEDTEST);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(
      "speedtest",
      0, 0,
      &x1, &y1,
      &w, &h
  );

  // Title
  tft.setCursor((SCREEN_WIDTH - w) / 2, 2);
  tft.print("speedtest");

  // Show up to four difficulties at a time
  const int VISIBLE_ROWS = 4;

  int firstVisible = getFirstVisibleItem(
    speedtestIndex,
    SPEEDTEST_COUNT,
    VISIBLE_ROWS
  );

  // Difficulties + highscores
  for (int row = 0; row < VISIBLE_ROWS; row++) {
    int optionIndex = firstVisible + row;
    // Stop if there are no more options
    if (optionIndex >= SPEEDTEST_COUNT) {
      break;
    }

    int y = 18 + row * 16;
    char normal[12];
    char noLight[12];

    snprintf(
      normal,
      sizeof(normal),
      "%d",
      speedHS.normal[optionIndex][0]
    );

    snprintf(
      noLight,
      sizeof(noLight),
      "%d",
      speedHS.noLight[optionIndex][0]
    );

    if (optionIndex == speedtestIndex) {
      tft.setTextSize(2);
      tft.setTextColor(COLOR_SELECT);
      tft.setCursor(0, y);
      tft.print("> ");
      tft.print(speedtestOptions[optionIndex]);

    } else {
      tft.setTextSize(1);
      tft.setTextColor(COLOR_TEXT);
      tft.setCursor(10, y);
      tft.print(speedtestOptions[optionIndex]);
    }

    // Highscore text size depends on selection
    int hsSize =
      optionIndex == speedtestIndex ? 2 : 1;
    tft.setTextSize(hsSize);
    // Normal-mode first-place score
    tft.setTextColor(COLOR_NORMAL_HS);
    tft.setCursor(70, y);
    tft.print(normal);
  }
  drawMenuControls();
}

// draw SimonSays difficulty menu
void drawSimonMenu() {

  // Title, font, color, background
  tft.fillScreen(COLOR_BG);

  tft.setTextSize(2);
  tft.setTextColor(COLOR_SIMON);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(
    "simon says",
    0, 0,
    &x1, &y1,
    &w, &h
  );

  // Title
  tft.setCursor((SCREEN_WIDTH - w) / 2, 2);
  tft.print("simon says");

  // Number of menu rows shown at once
  const int VISIBLE_ROWS = 4;

  // Calculate the first visible menu option
  int firstVisible = getFirstVisibleItem(
    simonIndex,
    SIMONSAYS_COUNT,
    VISIBLE_ROWS
  );

  // Draw visible difficulty rows
  for (int row = 0; row < VISIBLE_ROWS; row++) {

    int optionIndex = firstVisible + row;

    // Safety check
    if (optionIndex >= SIMONSAYS_COUNT) {
      break;
    }

    int y = 18 + row * 15;

    char normal[12];

    snprintf(
      normal,
      sizeof(normal),
      "%d",
      simonHS.normal[optionIndex][0]
    );
    
    // print difficulty options and current top1 highscore
    if (optionIndex == simonIndex) {
      tft.setTextSize(2);
      tft.setTextColor(COLOR_SELECT);
      tft.setCursor(0, y);
      tft.print("> ");
      tft.print(simonOptions[optionIndex]);

    } else {
      tft.setTextSize(1);
      tft.setTextColor(COLOR_TEXT);
      tft.setCursor(10, y);

      tft.print(simonOptions[optionIndex]);
    }

    int hsSize =
      optionIndex == simonIndex ? 2 : 1;

    tft.setTextSize(hsSize);

    // Normal-mode first-place score
    tft.setTextColor(COLOR_NORMAL_HS);
    tft.setCursor(105, y);
    tft.print(normal);
  }

  drawMenuControls();
}

// draw Reaction difficulty menu
void drawReactionMenu() {

  // Title, font, color, background
  tft.fillScreen(COLOR_BG);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_REACTION);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds("REACTION", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, 2);
  tft.print("reaction");

  // Difficulties + highscores
  for (int i = 0; i < REACTION_COUNT; i++) {
    int y = 20 + (i * 20);
    char normal[12];
    char noLight[12];

    snprintf(normal, sizeof(normal), "%lu", reactionHS.normal[i][0]);
    snprintf(noLight, sizeof(noLight), "%lu", reactionHS.noLight[i][0]);

    if (i == reactionIndex) {
      tft.setTextSize(2);
      tft.setTextColor(COLOR_SELECT);

      char buffer[20];
      sprintf(buffer, "> %s", reactionOptions[i]);

      tft.setCursor(0, y);
      tft.print(buffer);

    } else {
      tft.setTextSize(1);
      tft.setTextColor(COLOR_TEXT);

      tft.setCursor(10, y);
      tft.print(reactionOptions[i]);
    }
      
    // Highscore text size depending on selection 1 or 2
    int hsSize =
        (i == reactionIndex)
            ? 2
            : 1;
    tft.setTextSize(hsSize);

    // Normal mode highscore
    tft.setTextColor(COLOR_NORMAL_HS);
    tft.setCursor(100, y);
    tft.print(normal);
  }
  drawMenuControls();
}

// draw Solo menu
void drawSoloMenu() {

  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_SOLO);
  tft.setTextSize(2);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds("solo", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, 2);
  tft.print("solo JAM");
  tft.setTextSize(2);

  for (int i = 0; i < SOLO_COUNT; i++) {
    int y = 20 + (i * 20);
    if (i == soloIndex) {
      tft.setTextColor(COLOR_SELECT);
      char buffer[20];
      sprintf(buffer, "> %s", soloOptions[i]);
      tft.setCursor(0, y);
      tft.print(buffer);
    } else {
      tft.setTextColor(COLOR_TEXT);
      tft.setCursor(10, y);
      tft.print(soloOptions[i]);
    }
  }

  // Bottom-right1 ✓ check mark
  tft.drawLine(139, 75, 141, 77, COLOR_SELECT);
  tft.drawLine(141, 77, 146, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 78, COLOR_BACK);
  tft.drawLine(158, 71, 152, 78, COLOR_BACK);
}

// draw Solo root note
void drawSoloRoot() {

    tft.fillRect(120, 0, 40, 16, COLOR_BG);

    tft.setTextSize(2);
    tft.setTextColor(COLOR_SOLO);

    tft.setCursor(140, 0);
    tft.print(ROOT_NAMES[soloRoot]);
}

// ----- MENU UPDATE FUNCTIONS ----- //

// update main menu UI
void updateMainMenu() {
  
  if (strumUpPressed()) {
    mainMenuIndex++;

    if (mainMenuIndex >= MAIN_MENU_COUNT)
      mainMenuIndex = 0;
      playTone(FX_BUZZER, MENU_SCALE_4[mainMenuIndex], 32);
    drawMainMenu();
  }

  if (strumDownPressed()) {

    mainMenuIndex--;

    if (mainMenuIndex < 0)
      mainMenuIndex = MAIN_MENU_COUNT - 1;      
      playTone(FX_BUZZER, MENU_SCALE_4[mainMenuIndex], 32);
    drawMainMenu();
  }

  if (greenPressed()) {
    playSelectSound();
    switch (mainMenuIndex) {
      case 0:
        currentState = STATE_SPEEDTEST_MENU;
        break;
      case 1:
        currentState = STATE_SIMON_MENU;
        break;
      case 2:
        currentState = STATE_REACTION_MENU;
        break;
      case 3:
        currentState = STATE_SOLO_MENU;
        break;
    }
  }
}

// update speedtest difficulty menu
void updateSpeedtestMenu() {
  // If leaderboard is currently open
  if (speedLeaderboardOpen) {
    
    // Strum changes displayed difficulty
    
    if (strumUpPressed()) {
      speedtestIndex++;
      if (speedtestIndex >= SPEEDTEST_COUNT) {
        speedtestIndex = 0;
      }      
      drawSpeedLeaderboard();
    }

    if (strumDownPressed()) {
      speedtestIndex--;
      if (speedtestIndex < 0) {
        speedtestIndex = SPEEDTEST_COUNT - 1;
      }
      drawSpeedLeaderboard();
    }
    
    // Yellow returns to difficulty menu
    if (yellowPressed()) {
      speedLeaderboardOpen = false;
      drawSpeedtestMenu();
    }    
    return;
  }
  
  // Yellow opens leaderboard
  if (yellowPressed()) {
    speedLeaderboardOpen = true;
    drawSpeedLeaderboard();
    return;
  }

  // Start speedtest game
  if (greenPressed()) {
    playSelectSound();
    configureSpeedtestDifficulty();
    useLight = true;
    useSound = true;
    currentGame = GAME_SPEEDTEST;
    runSpeedtestGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    currentState = STATE_MAIN_MENU;
  }

  // Show speedtest leaderboard
  if (yellowPressed()) {
    speedLeaderboardOpen = true;
    drawSpeedLeaderboard();
    return;
  }

  // Start speedtest game with light only
  if (bluePressed()) {
    playSelectSound();
    configureSpeedtestDifficulty();
    useLight = true;
    useSound = false;
    currentGame = GAME_SPEEDTEST;
    runSpeedtestGame();
  }
  
  // Start speedtest game with sound only
  if (orangePressed()) {
    playSelectSound();
    configureSpeedtestDifficulty();
    useLight = false;
    useSound = true;
    currentGame = GAME_SPEEDTEST;
    runSpeedtestGame();
  }

  // Navigate through speedtest difficulties
  if (strumUpPressed()) {
    // playNavDownSound();
    speedtestIndex++;

    if (speedtestIndex >= SPEEDTEST_COUNT)
      speedtestIndex = 0;
      playTone(FX_BUZZER, MENU_SCALE_4[speedtestIndex], 32);
    drawSpeedtestMenu();
  }

  if (strumDownPressed()) {
    // playNavUpSound();
    speedtestIndex--;

    if (speedtestIndex < 0)
      speedtestIndex = SPEEDTEST_COUNT - 1;
      playTone(FX_BUZZER, MENU_SCALE_4[speedtestIndex], 32);
    drawSpeedtestMenu();
  }
}

// update SimonSays difficulty menu
void updateSimonMenu() {

  // If the leaderboard is open, check for yellow button to close it
  if (simonLeaderboardOpen) {
    if (yellowPressed()) {
      simonLeaderboardOpen = false;
      drawSimonMenu();
    }
    return;
  }

  // Start SimonSays game
  if (greenPressed()) {
    playSelectSound();
    configureSimonsaysDifficulty();
    useLight = true;
    useSound = true;
    currentGame = GAME_SIMON;
    runSimonsaysGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    currentState = STATE_MAIN_MENU;
  }

  // Show SimonSays leaderboard
  if (yellowPressed()) {
    simonLeaderboardOpen = true;
    drawSimonLeaderboard();
    return;
  }

  // Start SimonSays game with light only
  if (bluePressed()) {
    playSelectSound();
    configureSimonsaysDifficulty();
    useLight = true;
    useSound = false;
    currentGame = GAME_SIMON;
    runSimonsaysGame();
  }

  // Start SimonSays game with sound only
  if (orangePressed()) {
    playSelectSound();
    configureSimonsaysDifficulty();
    useLight = false;
    useSound = true;
    currentGame = GAME_SIMON;
    runSimonsaysGame();
  }

  // Navigate through SimonSays difficulties
  if (strumUpPressed()) {
    simonIndex++;
    if (simonIndex >= SIMONSAYS_COUNT)
      simonIndex = 0;
      playTone(FX_BUZZER, SOLO_BLUES[2][simonIndex], 32);
    drawSimonMenu();
  }
  if (strumDownPressed()) {
    simonIndex--;
    if (simonIndex < 0)
      simonIndex = SIMONSAYS_COUNT - 1;
      playTone(FX_BUZZER, SOLO_BLUES[2][simonIndex], 32);
    drawSimonMenu();
  }
}

// update Reaction difficulty menu
void updateReactionMenu() {

  // If the leaderboard is open, check for yellow button to close it
  if (reactionLeaderboardOpen) {
    if (yellowPressed()) {
      reactionLeaderboardOpen = false;
      drawReactionMenu();
    }
    return;
  }

  // Start Reaction game
  if (greenPressed()) {
    playSelectSound();
    configureReactionDifficulty();
    useLight = true;
    useSound = true;
    currentGame = GAME_REACTION;
    runReactionGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    currentState = STATE_MAIN_MENU;
  }

  // Show Reaction leaderboard
  if (yellowPressed()) {
    reactionLeaderboardOpen = true;
    drawReactionLeaderboard();
    return;
  }

  // Start Reaction game with light only
  if (bluePressed()) {
    playSelectSound();
    configureReactionDifficulty();
    useLight = true;
    useSound = false;
    currentGame = GAME_REACTION;
    runReactionGame();
  }

  // Start Reaction game with sound only
  if (orangePressed()) {
    playSelectSound();
    configureReactionDifficulty();
    useLight = false;
    useSound = true;
    currentGame = GAME_REACTION;
    runReactionGame();
  }

  // Navigate through Reaction difficulties
  if (strumUpPressed()) {
    reactionIndex++;
    if (reactionIndex >= REACTION_COUNT)
      reactionIndex = 0;
      playTone(FX_BUZZER, MENU_SCALE_3[reactionIndex], 32);
    drawReactionMenu();
  }
  if (strumDownPressed()) {
    reactionIndex--;
    if (reactionIndex < 0)
      reactionIndex = REACTION_COUNT - 1;    
      playTone(FX_BUZZER, MENU_SCALE_3[reactionIndex], 32);
    drawReactionMenu();
  }
}

// update Solo difficulty menu
void updateSoloMenu() {

  // Start Solo game
  if (greenPressed()) {
    playSelectSound();
    currentGame = GAME_SOLO;
    runSoloGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    currentState = STATE_MAIN_MENU;
  }

  // Navigate through Solo difficulties
  if (strumUpPressed()) {
    soloIndex++;
    if (soloIndex >= SOLO_COUNT)
      soloIndex = 0;
      playTone(FX_BUZZER, MENU_SCALE_3[soloIndex], 32);
    drawSoloMenu();
  }

  if (strumDownPressed()) {
    soloIndex--;
    if (soloIndex < 0)
      soloIndex = SOLO_COUNT - 1;
      playTone(FX_BUZZER, MENU_SCALE_3[soloIndex], 32);
    drawSoloMenu();
  }
}

// update menu LEDs based on button states
void updateMenuLEDs() {
  
  // Fret LEDs
  for (int i = 0; i < 5; i++) {
    digitalWrite(
      LED_PINS[i],
      digitalRead(BUTTON_PINS[i]) == LOW ? HIGH : LOW
    );
  }

  // Strum-up LED
  digitalWrite(
    LED_STRUM_PINS[0],
    digitalRead(STRUM_UP_PIN) == LOW ? HIGH : LOW
  );

  // Strum-down LED
  digitalWrite(
    LED_STRUM_PINS[1],
    digitalRead(STRUM_DOWN_PIN) == LOW ? HIGH : LOW
  );
}

// Update new highscore initials screen
void updateInitialsEntry() {

  // Next printable ASCII character
  if (strumUpPressed()) {

    initialsCharacter++;

    if (initialsCharacter > 126) {
      initialsCharacter = 32;
    }

    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);

    playTone(
      FX_BUZZER,
      300 + initialsCharacter,
      25
    );

    drawInitialsEntry();
  }

  // Previous printable ASCII character
  if (strumDownPressed()) {

    initialsCharacter--;

    if (initialsCharacter < 32) {
      initialsCharacter = 126;
    }

    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);

    playTone(
      FX_BUZZER,
      300 + initialsCharacter,
      25
    );

    drawInitialsEntry();
  }

  // Green confirms the current character
  if (greenPressed()) {

    playSelectSound();

    // Move to the next character
    if (initialsPosition < INITIAL_COUNT - 1) {

      initialsPosition++;

      initialsCharacter =
        enteredInitials[initialsPosition];

      drawInitialsEntry();

    } else {

      // Fourth character confirmed, save initials
      enteredInitials[INITIAL_COUNT] = '\0';

      if (pendingInitials != nullptr) {
        strcpy(
          pendingInitials,
          enteredInitials
        );
      }

      newHighScore = false;
      pendingInitials = nullptr;

      // Open the leaderboard for the game just played
      if (currentGame == GAME_SPEEDTEST) {

        speedLeaderboardOpen = true;
        currentState = STATE_SPEEDTEST_MENU;

      } else  if (currentGame == GAME_SIMON) {

        simonLeaderboardOpen = true;
        currentState = STATE_SIMON_MENU;
      }
    }
  }

  // Red returns to the previous character
  if (redPressed()) {

    if (initialsPosition > 0) {

      playBackSound();

      initialsPosition--;

      initialsCharacter =
        enteredInitials[initialsPosition];

      drawInitialsEntry();
    }
  }

  // Yellow fret toggles uppercase/lowercase
  if (yellowPressed()) {
    // A-Z -> a-z
    if (
      initialsCharacter >= 'A' &&
      initialsCharacter <= 'Z'
    ) {
      initialsCharacter += 32;
    } else if (
      initialsCharacter >= 'a' &&
      initialsCharacter <= 'z'
    ) {
      initialsCharacter -= 32;
    }
    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);

      drawInitialsEntry();
  } 

  // Blue fret jumps to numbers and back to A
  if (bluePressed()) {
    // A-Z -> a-z
    if (
      initialsCharacter >= 'A' &&
      initialsCharacter <= 'z'
    ) {
      initialsCharacter = '0';
    } else if (
      initialsCharacter < 'A' ||
      initialsCharacter > 'z'
    ) {
      initialsCharacter = 'A';
    }
    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);

      drawInitialsEntry();
  } 

  // Orange makes a space
  if (orangePressed()) {
    initialsCharacter = ' ';
    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);
    drawInitialsEntry();
  }
}

// ----- RUN GAME FUNCTIONS ----- //

// speedtest game
void runSpeedtestGame() {

  currentState = STATE_PLAYING;

  drawPlayingScreen(0);
  while (
    digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW ||
    digitalRead(BUTTON_PINS[2]) == LOW ||
    digitalRead(BUTTON_PINS[3]) == LOW ||
    digitalRead(BUTTON_PINS[4]) == LOW ||
    digitalRead(STRUM_UP_PIN) == LOW ||
    digitalRead(STRUM_DOWN_PIN) == LOW
  ) {
    delay(1);
  }
  turnOffAllInputLights();

  int score = 0;
  unsigned long currentDelay = 800;
  unsigned long minDelay = 120;
  unsigned long speedStep = 10;
  queueSize = 0;
  int lastInput = -1;
  unsigned long lastStepTime = millis();
  bool gameActive = true;

  while (gameActive) {
    unsigned long now = millis();

    // Spawn next target
    if (now - lastStepTime >= currentDelay) {
      lastStepTime = now;
      int nextInput;

      do {
            nextInput = random(0, numInputs);
      }
      while (nextInput == lastInput);
        lastInput = nextInput;

      if (queueSize < 60) {
          targetQueue[queueSize++] = nextInput;
      }

      if (useSound) {
        if (nextInput < 5)
          playTone(NOTE_BUZZER,
                  250 + (nextInput * 100),
                  50);
          else
            playTone(NOTE_BUZZER, 1000, 50);
      }
      if (useLight) {

        if (nextInput < 5) {

          // Light the target fret
          digitalWrite(
            LED_PINS[nextInput],
            HIGH
          );

        delay(100);

        digitalWrite(
          LED_PINS[nextInput],
          LOW
        );

        } else {

          // Input 5 represents either strum direction
          digitalWrite(
            LED_STRUM_PINS[0],
            HIGH
          );

          digitalWrite(
            LED_STRUM_PINS[1],
            HIGH
          );

          delay(100);

          digitalWrite(
          LED_STRUM_PINS[0],
            LOW
          );

          digitalWrite(
            LED_STRUM_PINS[1],
            LOW
          );
        }
      }

    if (currentDelay > minDelay)
        currentDelay -= speedStep;
    }

    // Fret buttons
    for (int i = 0; i < min(numInputs, 5); i++) {

      if (digitalRead(BUTTON_PINS[i]) == LOW) {

        if (queueSize > 0 &&
            targetQueue[0] == i) {

              for (int j = 0; j < queueSize - 1; j++)
                targetQueue[j] = targetQueue[j + 1];

                queueSize--;
                score++;
                drawPlayingScreen(score);
            }
            else {

                gameActive = false;
            }

          while (digitalRead(BUTTON_PINS[i]) == LOW);
        }
    }

      // Strum input (5+1 difficulty)
      if (numInputs == 6) {

          if (digitalRead(STRUM_UP_PIN) == LOW ||
              digitalRead(STRUM_DOWN_PIN) == LOW) {

              if (queueSize > 0 &&
                  targetQueue[0] == 5) {

                  for (int j = 0; j < queueSize - 1; j++)
                      targetQueue[j] = targetQueue[j + 1];

                  queueSize--;
                  score++;
                  drawPlayingScreen(score);
              }
              else {

                  gameActive = false;
              }

              while (digitalRead(STRUM_UP_PIN) == LOW ||
                   digitalRead(STRUM_DOWN_PIN) == LOW);
          }
        }

      delay(2);
  }

  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    // Normal mode
    highScorePosition = insertTopScore(
      speedHS.normal[speedtestIndex],
      score
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        speedNormalInitials[speedtestIndex],
        highScorePosition
      );

      pendingInitials =
        speedNormalInitials
          [speedtestIndex]
          [highScorePosition];

      newHighScore = true;
    }

    } else {

    // No-light mode
    highScorePosition = insertTopScore(
      speedHS.noLight[speedtestIndex],
      score
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        speedNoLightInitials[speedtestIndex],
        highScorePosition
      );

      pendingInitials =
        speedNoLightInitials
          [speedtestIndex]
          [highScorePosition];

      newHighScore = true;
      }
    }

  lastScore = score;

  if (newHighScore && pendingInitials != nullptr) {
    playVictorySound();
    startInitialsEntry();
    return;
  }
    
  playGameOverSound();
  currentState = STATE_GAMEOVER;
  return;
}

// simon says game
void runSimonsaysGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_SIMON;

  drawPlayingScreen(0);

  // Wait for all buttons released
  while (
    digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW ||
    digitalRead(BUTTON_PINS[2]) == LOW ||
    digitalRead(BUTTON_PINS[3]) == LOW ||
    digitalRead(BUTTON_PINS[4]) == LOW ||
    digitalRead(STRUM_UP_PIN) == LOW ||
    digitalRead(STRUM_DOWN_PIN) == LOW
  ) {
    delay(1);
  }

  int sequence[100];
  int sequenceLength = 1;
  int score = 0;

  sequence[0] = random(0, numInputs);

  bool gameActive = true;

  while (gameActive) {

    // ----- SHOW SEQUENCE ----- //

    for (int i = 0; i < sequenceLength; i++) {

      int input = sequence[i];
      if (useLight) {

        if (input < 5) {
        
        digitalWrite(LED_PINS[input], HIGH);
        delay(250);
        digitalWrite(LED_PINS[input], LOW);
        } else {
          for (int j = 0; j < 5; j++)
               digitalWrite(LED_PINS[j], HIGH);

          delay(250);

          for (int j = 0; j < 5; j++)
               digitalWrite(LED_PINS[j], LOW);
        }
      }
      if (useSound) {

        if (input < 5)
          playTone(NOTE_BUZZER,
                     250 + (input * 100),
                     250);
        else
          playTone(NOTE_BUZZER, 1000, 250);
      }

      delay(250);
    }

    // ----- PLAYER REPEATS SEQUENCE ----- //

    for (int i = 0; i < sequenceLength; i++) {

      int pressed = -1;

      unsigned long startTime = millis();
         
      while (pressed == -1) {

        // timeout
        if (millis() - startTime > 2000) {

          gameActive = false;
          break;
        }

        // fret buttons
        for (int b = 0; b < min(numInputs, 5); b++) {

          if (digitalRead(BUTTON_PINS[b]) == LOW) {

            pressed = b;

            if (useSound) {
              playTone(
                NOTE_BUZZER,
                250 + (pressed * 100),
                100
                );
            }
            
            if (useLight) {
                digitalWrite(LED_PINS[pressed], HIGH);
                delay(100);
                digitalWrite(LED_PINS[pressed], LOW);
            }

            while (digitalRead(BUTTON_PINS[b]) == LOW);
          }
        }

        // strum in F6 mode
        if (numInputs == 6 && pressed == -1) {

          if (digitalRead(STRUM_UP_PIN) == LOW ||
              digitalRead(STRUM_DOWN_PIN) == LOW) {

            pressed = 5;

            if(useSound) {
              playTone(NOTE_BUZZER, 1000, 100);
            }

            // Light up strum LEDs
            if (useLight) {

              digitalWrite(LED_STRUM_PINS[0], HIGH);
              digitalWrite(LED_STRUM_PINS[1], HIGH);
              delay(100);
              digitalWrite(LED_STRUM_PINS[0], LOW);
              digitalWrite(LED_STRUM_PINS[1], LOW);
            }

            while (digitalRead(STRUM_UP_PIN) == LOW ||
                   digitalRead(STRUM_DOWN_PIN) == LOW);
          }
        }

        delay(2);
      }

      if (!gameActive)
        break;

      // wrong note
      if (pressed != sequence[i]) {

        gameActive = false;
        break;
      }
    }

    // ----- ROUND COMPLETE ----- //
    if (gameActive) {

      score++;

      drawPlayingScreen(score);

      sequence[sequenceLength] =
          random(0, numInputs);

      sequenceLength++;

      if (sequenceLength >= 100)
        break;
    }
  }

  // ----- GAME OVER ----- //
  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    // Normal mode
    highScorePosition = insertTopScore(
      simonHS.normal[simonIndex],
      score
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        simonNormalInitials[simonIndex],
        highScorePosition
      );

      pendingInitials =
        simonNormalInitials
          [simonIndex]
          [highScorePosition];

      newHighScore = true;
    }

    } else {

    // No-light mode
    highScorePosition = insertTopScore(
      simonHS.noLight[simonIndex],
      score
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        simonNoLightInitials[simonIndex],
        highScorePosition
      );

      pendingInitials =
        simonNoLightInitials
          [simonIndex]
          [highScorePosition];

      newHighScore = true;
      }
    }

  lastScore = score;

  if (newHighScore && pendingInitials != nullptr) {
    playVictorySound();
    startInitialsEntry();
    return;
  }
    
  playGameOverSound();
  currentState = STATE_GAMEOVER;
  return;
}

// reaction game
void runReactionGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_REACTION;

  const int REACTION_ROUNDS = 3;
  unsigned long reactionTimes[REACTION_ROUNDS];

  // Wait for release
  while (
    digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW ||
    digitalRead(BUTTON_PINS[2]) == LOW ||
    digitalRead(BUTTON_PINS[3]) == LOW ||
    digitalRead(BUTTON_PINS[4]) == LOW
  ) {
    delay(1);
  }
  while (digitalRead(BUTTON_PINS[0]) == LOW)
      delay(1);

  for (int round = 0; round < REACTION_ROUNDS; round++) {

    // ----- RANDOM WAIT ----- //
    drawReactionWaitScreen();

    unsigned long waitTime =
      random(500, 2001);
    // Wait for release
    while (
      digitalRead(BUTTON_PINS[0]) == LOW ||
      digitalRead(BUTTON_PINS[1]) == LOW ||
      digitalRead(BUTTON_PINS[2]) == LOW ||
      digitalRead(BUTTON_PINS[3]) == LOW ||
      digitalRead(BUTTON_PINS[4]) == LOW
      ) {
    delay(1);
    }

    unsigned long waitStart =
      millis();

    bool earlyPress = false;

    while (millis() - waitStart < waitTime) {

      for (int i = 0; i < 5; i++) {

        if (digitalRead(BUTTON_PINS[i]) == LOW) {

          earlyPress = true;
          break;
        }
      }

      if (earlyPress)
        break;
    }

    // ----- TOO EARLY ----- //
    if (earlyPress) {

      turnOffAllInputLights();
      noTone(NOTE_BUZZER);
      noTone(FX_BUZZER);

      tft.fillScreen(COLOR_ERROR);

      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(2);

      int16_t x1, y1;
      uint16_t w, h;

      tft.getTextBounds(
        "TOO FAST!",
        0, 0,
        &x1, &y1,
        &w, &h
      );

      tft.setCursor(
        (SCREEN_WIDTH - w) / 2,
        30
      );

      tft.print("TOO FAST!");

      playGameOverSound();

      delay(500);

      reactionLeaderboardOpen = false;
      currentState = STATE_REACTION_MENU;

      return;
    }

    // ----- START REACTION CUE ----- //
    int targetFret = random(0, 5);

    // Change WAIT to GO, activate LEDs and play beep
    startReactionCue(targetFret);

    // Begin measuring after the cue becomes active
    unsigned long startTime = millis();

    bool success = false;
    bool wrongFret = false;

    while (!success) {

      for (int i = 0; i < 5; i++) {

        if (digitalRead(BUTTON_PINS[i]) == LOW) {

          if (
            reactionIndex == 0 ||
            i == targetFret
          ) {

            reactionTimes[round] =
              millis() - startTime;

            success = true;
          } else {

            wrongFret = true;
            success = true;
          }
/* 
          } else {

            tft.fillScreen(COLOR_ERROR);

            tft.setTextColor(COLOR_TEXT);
            tft.setTextSize(2);

            tft.setCursor(10, 20);
            tft.print("WRONG FRET");

            tft.setCursor(10, 50);
            tft.print("PRESS GREEN");

            waitForGreenPress();

            round--;
            success = true;
          }
*/

          while (
            digitalRead(BUTTON_PINS[i])
            == LOW
          );
        }
      }
    }

    turnOffAllInputLights();
    noTone(NOTE_BUZZER);

    if (wrongFret) {

      tft.fillScreen(COLOR_ERROR);

      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(2);

      tft.setCursor(10, 20);
      tft.print("WRONG FRET");

      tft.setCursor(10, 50);
      tft.print("PRESS GREEN");

      waitForGreenPress();

      round--;
      continue;
    }

    // ----- TURN LEDS OFF ----- //
    turnOffAllInputLights();
    noTone(NOTE_BUZZER);

    // ----- SHOW RESULT ----- //
    tft.fillScreen(COLOR_SELECT);

    tft.setTextColor(COLOR_BG);
    tft.setTextSize(2);

    tft.setCursor(10, 10);
    tft.print(reactionTimes[round]);
    tft.print(" ms");

    tft.setCursor(10, 45);
    tft.print("PRESS GREEN");

    waitForGreenPress();
  }

  // ----- CALCULATE RESULT ----- // 

  unsigned long total = 0;

  for (int i = 0; i < REACTION_ROUNDS; i++) {
    total += reactionTimes[i];
  }

  unsigned long average =
    total / REACTION_ROUNDS;

  // ----- GAME OVER ----- //
  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    // Normal mode
    highScorePosition = insertTopReactionTime(
      reactionHS.normal[reactionIndex],
      average
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        reactionNormalInitials[reactionIndex],
        highScorePosition
      );

      pendingInitials =
        reactionNormalInitials
          [reactionIndex]
          [highScorePosition];

      newHighScore = true;
    }

    } else {

    // No-light mode
    highScorePosition = insertTopReactionTime(
      reactionHS.noLight[reactionIndex],
      average
    );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        reactionNoLightInitials[reactionIndex],
        highScorePosition
      );

      pendingInitials =
        reactionNoLightInitials
          [reactionIndex]
          [highScorePosition];

      newHighScore = true;
      }
    }

  lastScore = average;

  if (newHighScore && pendingInitials != nullptr) {
    playVictorySound();
    startInitialsEntry();
    return;
  }
    
  playGameOverSound();
  currentState = STATE_GAMEOVER;
  return;
}

// solo game
void runSoloGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_SOLO;

  soloRoot = 2; // Start with C4 scale

  // Draw black screen + scale tone + game title for playing screen
  drawSoloPlayingScreen();
  drawSoloRoot();

  tft.setTextColor(COLOR_SOLO);

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(
      "solo",
      0, 0,
      &x1, &y1,
      &w, &h
  );

  // Title
  tft.setCursor((SCREEN_WIDTH - w) / 3, 2);
  tft.print("solo");

  unsigned long lastActivity = millis();

  while (
    digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW ||
    digitalRead(BUTTON_PINS[2]) == LOW ||
    digitalRead(BUTTON_PINS[3]) == LOW ||
    digitalRead(BUTTON_PINS[4]) == LOW ||
    digitalRead(STRUM_UP_PIN) == LOW ||
    digitalRead(STRUM_DOWN_PIN) == LOW
  ) {
    delay(1);
  }

  while (true) {

    for (int fret = 0; fret < 5; fret++) {

      if (digitalRead(BUTTON_PINS[fret]) == LOW) {

        lastActivity = millis();

        int noteIndex = soloRoot + fret;

        int note;

        if (soloIndex == 0) {
          note = SOLO_MINOR[soloRoot][fret];
        } else if (soloIndex == 1) {
          note = SOLO_MAJOR[soloRoot][fret];
        } else {
          note = SOLO_BLUES[soloRoot][fret];
        }

          playTone(NOTE_BUZZER, note, 200);
        spawnNote(fret);

        while (digitalRead(BUTTON_PINS[fret]) == LOW);
      }
    }

    if (strumUpPressed()) {

      lastActivity = millis();

      soloRoot++;

      if (soloRoot > 5)
        soloRoot = 0;

      drawSoloRoot();
    }

    if (strumDownPressed()) {

      lastActivity = millis();

      soloRoot--;

      if (soloRoot < 0)
        soloRoot = 5;
        
      drawSoloRoot();
    }

    // Exit Solo by 5s inactivity
    if (millis() - lastActivity > 5000) {

      playBackSound();

      currentState = STATE_SOLO_MENU;
      Serial.println("SOLO EXIT");
      return;

    // Exit Solo by red+orange+strumup
    } else if (
      digitalRead(BUTTON_PINS[1]) == LOW &&
      digitalRead(BUTTON_PINS[4]) == LOW &&
      digitalRead(STRUM_UP_PIN) == LOW
    ) {
      playBackSound();
      Serial.println("SOLO EXIT");
      currentState = STATE_SOLO_MENU;
      return;
    }

    tft.fillRect(0, 18, SCREEN_WIDTH, SCREEN_HEIGHT - 18, COLOR_BG);
    updateNotes();
    cleanupNotes();
    drawNotes();

    delay(30);
  }
}

// ----- HARDWARE TEST & BOOT SEQUENCE ----- //

// Startup test for fret and strum LEDs
void playStartupFretSequence() {

  // Make sure all fret LEDs begin off
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }

  // Make sure both strum LEDs begin off
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_STRUM_PINS[i], LOW);
  }

  // Blues-style note lengths
  const int noteLengths[5] = {
    180, // Green: medium
    50, // Red: short
    210, // Yellow: long
    100, // Blue: short
    280 // Orange: long ending
  };

  // Delay after each LED/note
  const int noteGaps[5] = {
    20,
    40,
    25,
    60,
    30
  };

  // Light fret LEDs one by one
  for (int i = 0; i < 5; i++) {
    // Check for Green + Orange held during startup -> mute sounds
    if (
      digitalRead(BUTTON_PINS[0]) == LOW &&
      digitalRead(BUTTON_PINS[4]) == LOW  
    ) { 
      soundEnabled = false;
      
      noTone(NOTE_BUZZER);
      noTone(FX_BUZZER);
    }
  
      digitalWrite(LED_PINS[i], HIGH);

      playTone(
        NOTE_BUZZER,
        SOLO_BLUES[2][i],
        noteLengths[i]
      );
  
      delay(noteLengths[i]);
      digitalWrite(LED_PINS[i], LOW);
      delay(noteGaps[i]);
  }

  // Light strum-up LED
  digitalWrite(LED_STRUM_PINS[0], HIGH);
  playTone(NOTE_BUZZER, 587, 180);
  delay(200);
  digitalWrite(LED_STRUM_PINS[0], LOW);
  delay(50);

  // Light strum-down LED
  digitalWrite(LED_STRUM_PINS[1], HIGH);
  playTone(NOTE_BUZZER, 698, 180);
  delay(200);
  digitalWrite(LED_STRUM_PINS[1], LOW);
  delay(50);

  // Light all fret LEDs together
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], HIGH);
  }

  // Light both strum LEDs together
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_STRUM_PINS[i], HIGH);
  }

  // Finishing sound
  playTone(FX_BUZZER, 440, 300);
  playTone(NOTE_BUZZER, 587, 300);

  delay(500);

/* 
  for (int i = 0; i <5; i++) {
    playTone(FX_BUZZER, 500 - 70 * i, 16);
    delay(12);
  }
*/

  // Turn off all fret LEDs
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PINS[i], LOW);
  delay(50);
  }

  // Turn off both strum LEDs
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_STRUM_PINS[i], LOW);
  }

  noTone(FX_BUZZER);
  noTone(NOTE_BUZZER);

  while (digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW) {
  delay(1);
  }
}

// Boot machine
void setup() {

  Serial.begin(115200);

  // Keep the known-working display initialization unchanged
  tft.begin();

  tft.setRotation(0);
  tft.fillScreen(COLOR_START1);
  delay(500);

  tft.setRotation(1);
  tft.fillScreen(COLOR_START2);
  delay(500);

  tft.setRotation(2);
  tft.fillScreen(COLOR_START3);
  delay(500);

  tft.begin();
  tft.setRotation(3);

  // Check for Green + Orange held during startup -> mute sounds
  if (
    digitalRead(BUTTON_PINS[0]) == LOW &&
    digitalRead(BUTTON_PINS[4]) == LOW  
  ) { 
    soundEnabled = false;
  
    noTone(NOTE_BUZZER);
    noTone(FX_BUZZER);
  }
  

  // Fret buttons
  for (int i = 0; i < 5; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  // Fret LEDs
  for (int i = 0; i < 5; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  // Strum buttons
  pinMode(STRUM_UP_PIN, INPUT_PULLUP);
  pinMode(STRUM_DOWN_PIN, INPUT_PULLUP);

  // Strum LEDs
  for (int i = 0; i < 2; i++) {
    pinMode(LED_STRUM_PINS[i], OUTPUT);
    digitalWrite(LED_STRUM_PINS[i], LOW);
  }

  // Buzzers
  pinMode(NOTE_BUZZER, OUTPUT);
  pinMode(FX_BUZZER, OUTPUT);

  noTone(NOTE_BUZZER);
  noTone(FX_BUZZER);
/*
  // Check for Green + Orange held during startup -> mute sounds
  if (
    digitalRead(BUTTON_PINS[0]) == LOW // &&
    // digitalRead(BUTTON_PINS[4]) == LOW  
  ) { 
    soundEnabled = false;
    noTone(NOTE_BUZZER);
    noTone(FX_BUZZER);
  }
*/

  // Run the startup hardware test
  playStartupFretSequence();

  // Initialize state
  currentState = STATE_SPLASH;
  previousState = STATE_SPLASH;

  // Read actual button states
  lastGreen = digitalRead(BUTTON_PINS[0]);
  lastRed = digitalRead(BUTTON_PINS[1]);
  lastYellow = digitalRead(BUTTON_PINS[2]);
  lastBlue = digitalRead(BUTTON_PINS[3]);
  lastOrange = digitalRead(BUTTON_PINS[4]);

  lastStrumUp = digitalRead(STRUM_UP_PIN);
  lastStrumDown = digitalRead(STRUM_DOWN_PIN);

  drawSplashScreen();
}

// Main loop
void loop() {

  // Check for state changes and update the display accordingly
  if (currentState != previousState) {

    switch (currentState) {

      case STATE_SPLASH:
        drawSplashScreen();
        break;

      case STATE_MAIN_MENU:
        drawMainMenu();
        break;

      case STATE_SPEEDTEST_MENU:
        if (speedLeaderboardOpen) {
          drawSpeedLeaderboard();
        } else {
          drawSpeedtestMenu();
        }
        break;

      case STATE_SIMON_MENU:
        if (simonLeaderboardOpen) {
          drawSimonLeaderboard();
        } else {
          drawSimonMenu();
        }
        break;

      case STATE_REACTION_MENU:
        if (reactionLeaderboardOpen) {
          drawReactionLeaderboard();
        } else {
          drawReactionMenu();
        }
        break;

      case STATE_SOLO_MENU:
        drawSoloMenu();
        break;
        
      case STATE_PLAYING:
        break;

      case STATE_GAMEOVER:
        drawGameOverScreen(lastScore);
        break;

      case STATE_ENTER_INITIALS:
        drawInitialsEntry();
        break;
    }
    previousState = currentState;
  }
  
  // Global button state updates for LEDs and menu navigation
  if (
    currentState == STATE_SPLASH ||
    currentState == STATE_MAIN_MENU ||
    currentState == STATE_SPEEDTEST_MENU ||
    currentState == STATE_SIMON_MENU ||
    currentState == STATE_REACTION_MENU ||
    currentState == STATE_SOLO_MENU ||
    currentState == STATE_ENTER_INITIALS
  ) {
    updateMenuLEDs();
  }

  switch (currentState) {

    case STATE_SPLASH:

    if (digitalRead(BUTTON_PINS[0]) == LOW ||
        digitalRead(BUTTON_PINS[1]) == LOW ||
        digitalRead(BUTTON_PINS[2]) == LOW ||
        digitalRead(BUTTON_PINS[3]) == LOW ||
        digitalRead(BUTTON_PINS[4]) == LOW ||
        digitalRead(STRUM_UP_PIN) == LOW ||
        digitalRead(STRUM_DOWN_PIN) == LOW) {

      playSelectSound();
      currentState = STATE_MAIN_MENU;
    }

    break;

    case STATE_MAIN_MENU:
    updateMainMenu();

    if (redPressed()) {
        playBackSound();
        turnOffAllInputLights();
        currentState = STATE_SPLASH;
    }
      break;

    case STATE_SPEEDTEST_MENU:
      updateSpeedtestMenu();
      break;

    case STATE_SIMON_MENU:
      updateSimonMenu();
      break;

    case STATE_REACTION_MENU:
      updateReactionMenu();
      break;

    case STATE_SOLO_MENU:
      updateSoloMenu();
      break;
        
    case STATE_PLAYING:
      break;
      
    case STATE_GAMEOVER:

    if (greenPressed()) {

        playSelectSound();

        if (
          newHighScore &&
          pendingInitials != nullptr
        ) {
          startInitialsEntry();
          break;
        }

        switch (currentGame) {

            case GAME_SPEEDTEST:
                runSpeedtestGame();
                break;

            case GAME_SIMON:
                runSimonsaysGame();
                break;

            case GAME_REACTION:
                runReactionGame();
                break;

            case GAME_SOLO:
                runSoloGame();
                break;
        }
    }

    if (redPressed()) {

        playBackSound();

        switch (currentGame) {

            case GAME_SPEEDTEST:
                currentState = STATE_SPEEDTEST_MENU;
                break;

            case GAME_SIMON:
                currentState = STATE_SIMON_MENU;
                break;

            case GAME_REACTION:
                currentState = STATE_REACTION_MENU;
                break;

            case GAME_SOLO:
                currentState = STATE_SOLO_MENU;
                break;
        }
    }
      break;

    case STATE_ENTER_INITIALS:
      updateInitialsEntry();
      break;

  }
  delay(10);
}
