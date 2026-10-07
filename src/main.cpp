// ----- 1. LIBRARIES ----- //

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // #include <Adafruit_ST7735.h>
#include <SPI.h>

#ifdef WOKWI_SIMULATION
  #include <Adafruit_NeoPixel.h>
#else
  #include <Adafruit_DotStar.h>
#endif

// ----- FLASH STORAGE ----- //

#include <stddef.h>
#include <stdio.h>
#include <string.h>
// Reserve 64 KB of onboard flash for LittleFS
#define RP2040_FS_SIZE_KB 64
// Do not erase saved scores automatically
#define FORCE_REFORMAT false
#include <LittleFS_Mbed_RP2040.h>

// ----- 2. GLOBAL CONSTANT HELPERS ----- //

constexpr int FRET_COUNT = 5;
constexpr int STRUM_LED_COUNT = 2;
constexpr int REACTION_ROUNDS = 3;
constexpr int TOP_SCORE_COUNT = 4;
constexpr int INITIAL_COUNT = 4;
constexpr int MAX_DIFFICULTIES = 5;
constexpr int STRUM_INPUT_INDEX = FRET_COUNT;
constexpr int FRET_AND_STRUM_INPUT_COUNT = FRET_COUNT + 1;

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 80;

constexpr int SPEEDTEST_COUNT = 4;
constexpr int SIMONSAYS_COUNT = 5;
constexpr int REACTION_COUNT = 2;
constexpr int SOLO_COUNT = 3;
constexpr int MAIN_MENU_COUNT = 4;

constexpr int MAX_TARGET_QUEUE = 60;
constexpr int MAX_MUSIC_NOTES = 10;
constexpr int VISIBLE_MENU_ROWS = 4;

constexpr unsigned long MENU_COOLDOWN = 50;

// Animation speed constants
constexpr int RGB_STEP_FAST_MS = 40;
constexpr int RGB_STEP_NORMAL_MS = 70;
constexpr int RGB_STEP_SLOW_MS = 140;
constexpr int RGB_HOLD_SHORT_MS = 100;
constexpr int RGB_FADE_STEP_MS = 50;
constexpr int RGB_FADE_STEPS = 15;

// ----- 3. PIN DEFINITIONS ----- //

constexpr uint8_t TFT_CS = 17;
constexpr uint8_t TFT_DC = 16;
constexpr uint8_t TFT_RST = 20;

// Start and Select
constexpr uint8_t START_BUTTON_PIN = 0;
constexpr uint8_t SELECT_BUTTON_PIN = 2;

// Fret buttons
constexpr uint8_t BUTTON_PINS[FRET_COUNT] = {
  1,  // Green
  3,  // Red
  5,  // Yellow
  7,  // Blue
  9   // Orange
};

// RGB data
constexpr uint8_t RGB_NEOPIXEL_PIN = 11; // for simulation
constexpr uint8_t RGB_DATA_PIN = 11;  // for physical build
constexpr uint8_t RGB_CLOCK_PIN = 12; // for physical build

// Buzzers
constexpr uint8_t FX_BUZZER = 14;
constexpr uint8_t NOTE_BUZZER = 15;

// Strum buttons
constexpr uint8_t STRUM_UP_PIN = 21;
constexpr uint8_t STRUM_DOWN_PIN = 22;
// Joystick
constexpr uint8_t JOYSTICK_X_PIN = 26;
constexpr uint8_t JOYSTICK_Y_PIN = 27;
constexpr uint8_t JOYSTICK_BUTTON_PIN = 28;

constexpr uint8_t RGB_BRIGHTNESS = 192;

constexpr int JOYSTICK_CENTER = 512;
constexpr int JOYSTICK_DEADZONE = 180;

constexpr int JOYSTICK_LOW_THRESHOLD =
  JOYSTICK_CENTER - JOYSTICK_DEADZONE;

constexpr int JOYSTICK_HIGH_THRESHOLD =
  JOYSTICK_CENTER + JOYSTICK_DEADZONE;

  // ----- RGB LED INDEXES ----- //

constexpr int RGB_FRET1_A = 0;
constexpr int RGB_FRET1_B = 1;

constexpr int RGB_FRET2_A = 2;
constexpr int RGB_FRET2_B = 3;

constexpr int RGB_FRET3_A = 4;
constexpr int RGB_FRET3_B = 5;

constexpr int RGB_FRET4_A = 6;
constexpr int RGB_FRET4_B = 7;

constexpr int RGB_FRET5_A = 8;
constexpr int RGB_FRET5_B = 9;

constexpr int RGB_STRUM_UP = 10;
constexpr int RGB_STRUM_DOWN = 11;

constexpr int RGB_START = 12;
constexpr int RGB_SELECT = 13;

constexpr int RGB_LED_COUNT = 14;

constexpr int FRET_RGB_PIXELS[FRET_COUNT][2] = {
  { RGB_FRET1_A, RGB_FRET1_B },
  { RGB_FRET2_A, RGB_FRET2_B },
  { RGB_FRET3_A, RGB_FRET3_B },
  { RGB_FRET4_A, RGB_FRET4_B },
  { RGB_FRET5_A, RGB_FRET5_B }
};

// ----- 4. HARDWARE OBJECTS ----- //

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

LittleFS_MBED* scoreFileSystem = nullptr;

#ifdef WOKWI_SIMULATION

Adafruit_NeoPixel rgbLeds(
  RGB_LED_COUNT,
  RGB_NEOPIXEL_PIN,
  NEO_GRB + NEO_KHZ800
);

#else

Adafruit_DotStar rgbLeds(
  RGB_LED_COUNT,
  RGB_DATA_PIN,
  RGB_CLOCK_PIN,
  DOTSTAR_BRG
);

#endif

// ----- 5. THEME COLORS ----- // 

// CUSTOM COLOR PALETTE //
const uint16_t COLOR_TURQUOISE = tft.color565(64, 224, 208);
const uint16_t COLOR_MINT = tft.color565(100, 255, 190);
const uint16_t COLOR_SALMON_PINK = 0xFE19;
const uint16_t COLOR_PASTEL_YELLOW = tft.color565(255, 240, 150);
const uint16_t COLOR_LAVENDER = tft.color565(190, 140, 255);
const uint16_t COLOR_CREAM_YELLOW = tft.color565(255, 245, 180);

// Boot
const uint16_t COLOR_BG      = ILI9341_BLACK;               // background color     // Black
const uint16_t COLOR_START1  = COLOR_SALMON_PINK;                      // screen start color1  // Pink
const uint16_t COLOR_START2  = tft.color565(100, 255, 190); // screen start color2  // Mint green
const uint16_t COLOR_START3  = ILI9341_CYAN;                // screen start color3  // Cyan

const uint16_t COLOR_SPLASH1  = tft.color565(64, 224, 208);  // splash screen title1 color // Turqoiuse
const uint16_t COLOR_SPLASH2  = COLOR_SALMON_PINK;                      // splash screen title2 color // Pink
const uint16_t COLOR_SPLASH3  = tft.color565(64, 224, 208);  // splash screen title3 color // Turqoiuse

// text colors
const uint16_t COLOR_SELECT   = tft.color565(100, 255, 190);  // Menu selected text highlight color // Mint green
const uint16_t COLOR_BACK     = tft.color565(255, 105, 125);    // Menu BACK color // Red pastel
const uint16_t COLOR_TEXT     = tft.color565(255, 245, 180);  // Normal text color in menus         // Pale creamy yellow

const uint16_t COLOR_NORMAL_HS = tft.color565(79, 200, 175);  // normal mode highscores // sea-green
const uint16_t COLOR_GAME_OVER  = tft.color565(128, 0, 128);  // "GAME OVER" text

// Game title colors
const uint16_t COLOR_SPEEDTEST = COLOR_SALMON_PINK;          // pastel pink speedtest title
const uint16_t COLOR_SIMON     = tft.color565(190, 140, 255);;  // lavender yellow simon says title
const uint16_t COLOR_REACTION  = ILI9341_CYAN;    // reaction title
const uint16_t COLOR_SOLO      = ILI9341_MAGENTA; // solo title

const uint16_t COLOR_SUCCESS  = ILI9341_ORANGE; // new high score color
const uint16_t COLOR_ERROR    = ILI9341_RED;    // game over color

// Fret LED colors
const uint16_t COLOR_FRET1    = ILI9341_GREEN;  // Fret1: green
const uint16_t COLOR_FRET2    = ILI9341_RED;    // Fret2: red
const uint16_t COLOR_FRET3    = ILI9341_YELLOW; // Fret3: yellow
const uint16_t COLOR_FRET4    = ILI9341_BLUE;   // Fret4: blue
const uint16_t COLOR_FRET5    = ILI9341_ORANGE; // Fret5: orange

  // RGB LED COLORS //
const uint32_t LED_COLOR_GREEN =
  rgbLeds.Color(0, 255, 100);

const uint32_t LED_COLOR_RED =
  rgbLeds.Color(255, 40, 70);

const uint32_t LED_COLOR_YELLOW =
  rgbLeds.Color(255, 220, 80);

const uint32_t LED_COLOR_BLUE =
  rgbLeds.Color(40, 100, 255);

const uint32_t LED_COLOR_ORANGE =
  rgbLeds.Color(255, 90, 10);

const uint32_t LED_COLOR_TURQUOISE =
  rgbLeds.Color(64, 224, 208);

const uint32_t LED_COLOR_PINK =
  rgbLeds.Color(255, 105, 180);

const uint32_t LED_COLOR_LAVENDER =
  rgbLeds.Color(190, 140, 255);

const uint32_t LED_COLOR_OFF =
  rgbLeds.Color(0, 0, 0);

const uint32_t FRET_RGB_COLORS[FRET_COUNT] = {
  rgbLeds.Color(0, 255, 100),   // Green fret
  rgbLeds.Color(255, 40, 70),   // Red fret
  rgbLeds.Color(255, 220, 80),  // Yellow fret
  rgbLeds.Color(40, 100, 255),  // Blue fret
  rgbLeds.Color(255, 90, 20)    // Orange fret
};

const uint8_t FRET_RGB_RED[FRET_COUNT] = {
  0,
  255,
  255,
  40,
  255
};

const uint8_t FRET_RGB_GREEN[FRET_COUNT] = {
  255,
  40,
  220,
  100,
  90
};

const uint8_t FRET_RGB_BLUE[FRET_COUNT] = {
  100,
  70,
  80,
  255,
  20
};

// ----- 6. MUSIC NOTE FREQUENCIES ----- //

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

// solo root note names
const char* ROOT_NAMES[6] = {
  "G2",
  "E3",
  "C4",
  "A4",
  "F5",
  "D6"
};

// ----- 7. ENUMS ----- //

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

// ----- 8. STRUCTURES ----- //

// High score tracking variables & constants
struct HighScores {
  int normal[MAX_DIFFICULTIES][TOP_SCORE_COUNT];
  int noLight[MAX_DIFFICULTIES][TOP_SCORE_COUNT];
};

// Music note variables
struct MusicNote {
  int x;
  int y;
  bool active;
  int noteType;    // fret color
  int symbolIndex; // visual shape
};

struct ScoreSaveData {
  uint32_t magic;
  uint16_t version;
  uint16_t dataSize;

  // Scores
  HighScores speedScores;
  HighScores simonScores;
  HighScores reactionScores;

  // Speedtest initials
  char speedNormalNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  char speedNoLightNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  // Simon initials
  char simonNormalNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  char simonNoLightNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  // Reaction initials
  char reactionNormalNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  char reactionNoLightNames
    [MAX_DIFFICULTIES]
    [TOP_SCORE_COUNT]
    [INITIAL_COUNT + 1];

  // Used to detect corrupted flash data
  uint32_t checksum;
};

// Button input structure
struct InputState {

  // Five fret buttons
  bool fretHeld[FRET_COUNT];
  bool fretPressed[FRET_COUNT];
  bool fretReleased[FRET_COUNT];

  // Strum up
  bool strumUpHeld;
  bool strumUpPressed;
  bool strumUpReleased;

  // Strum down
  bool strumDownHeld;
  bool strumDownPressed;
  bool strumDownReleased;

  // Start
  bool startHeld;
  bool startPressed;
  bool startReleased;

  // Select
  bool selectHeld;
  bool selectPressed;
  bool selectReleased;

  // Joystick center button
  bool joystickHeld;
  bool joystickPressed;
  bool joystickReleased;

  // Joystick analog axes
  int joystickX;
  int joystickY;

  // Joystick directions
  bool joystickLeft;
  bool joystickRight;
  bool joystickUp;
  bool joystickDown;
};

struct PreviousInputState {

  bool fretHeld[FRET_COUNT];

  bool strumUpHeld;
  bool strumDownHeld;

  bool startHeld;
  bool selectHeld;

  bool joystickHeld;
};

// Replace all (digitalRead(JOYSTICK_BUTTON_PIN)) == LOW with "(input.fretPressed[#inputnumberhere#])"

InputState input = {};
PreviousInputState previousInput = {};

// ----- 9. FLASH STORAGE CONSTANTS ----- //

constexpr uint32_t SCORE_FILE_MAGIC = 0x50424152;

constexpr uint16_t SCORE_FILE_VERSION = 1;

const char SCORE_FILE_PATH[] =
  MBED_LITTLEFS_FILE_PREFIX "/polybar_scores.bin";

// ----- 10. MENU OPTIONS ----- //

// Game menu options
const char* mainMenuOptions[MAIN_MENU_COUNT] = {
  "speedtest",
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

// ----- 11. GLOBAL VARIABLES ----- //

  // ----- APPLICATION STATE ----- //

  // Current and previous state tracking
AppState currentState = STATE_SPLASH;
AppState previousState = STATE_SPLASH;

  // Default gametype at boot
GameType currentGame = GAME_SPEEDTEST;

  // ----- USER AND GAME SETTINGS ----- //

bool soundEnabled = true;
bool useLight = true;
bool useSound = true;

  // ----- MENU INDEXES ----- //

int mainMenuIndex = 0;
int speedtestIndex = 0;
int simonIndex = 0;
int reactionIndex = 0;
int soloIndex = 0;

  // ----- LEADERBOARD SCREEN STATE ----- //

bool speedLeaderboardOpen = false;
bool simonLeaderboardOpen = false;
bool reactionLeaderboardOpen = false;

  // ----- GAME RUNTIME VARIABLES ----- //

int numInputs = 0;

int targetQueue[MAX_TARGET_QUEUE] = {};
int queueSize = 0;

int lastScore = 0;
int soloRoot = 3;

bool newHighScore = false;
bool reactionRequireCorrectFret = false;

  // ----- SOLO ANIMATION STATE ----- //

MusicNote notes[MAX_MUSIC_NOTES] = {};
int activeNotes = 0;

  // ----- INITIALS ENTRY STATE ----- //

char speedNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char speedNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char simonNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char simonNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char reactionNormalInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char reactionNoLightInitials
  [MAX_DIFFICULTIES]
  [TOP_SCORE_COUNT]
  [INITIAL_COUNT + 1] = {};

char enteredInitials[INITIAL_COUNT + 1] = {
  'A', 'A', 'A', 'A', '\0'
};

int initialsPosition = 0;
int initialsCharacter = 'A';

char* pendingInitials = nullptr;

  // ----- HIGHSCORE STATE ----- //

HighScores speedHS = {};
HighScores simonHS = {};
HighScores reactionHS = {};

bool scoreStorageReady = false;

  // ----- INPUT EDGE STATE ----- //

unsigned long lastMenuInput = 0;

  /*/ Music notes for different frets
const char* symbols[5] = {
    "♪",
    "♫",
    "♩",
    "♬",
    "♭"
};*/

// ----- 12. FUNCTION PROTOTYPES ----- //

 // A. GENERIC UTILITIES //

int getFirstVisibleItem(
  int selectedIndex,
  int itemCount,
  int visibleRows
);

  // B. INPUT //
bool menuReady();
bool greenPressed();

bool redPressed();
bool yellowPressed();
bool bluePressed();
bool orangePressed();
bool strumUpPressed();
bool strumDownPressed();

bool startPressed();
bool selectPressed();
bool joystickButtonPressed();

void initializeInputs();
void updateInputs();

void consumeAllPressEvents();
void waitForAllGameInputsReleased();

  // C. LEDs and HARDWARE //
void updateMenuLEDs();
void playStartupFretSequence();
void waitForGreenPress();

void setFretRgb(
  int fret,
  uint8_t red,
  uint8_t green,
  uint8_t blue
);

void setFretColor(
  int fret,
  uint32_t color
);

void setAllFretsColor(
  uint32_t color
);

void setStrumColors(
  uint32_t upColor,
  uint32_t downColor
);

void setUtilityColors(
  uint32_t startColor,
  uint32_t selectColor
);

void setAllInputLightsOff();

void fadeAllFrets(
  uint8_t red,
  uint8_t green,
  uint8_t blue,
  unsigned long duration
);

void flashAllFrets(
  uint32_t color,
  unsigned long durationMs
);

void setUtilityColors(
  uint32_t startColor,
  uint32_t selectColor
);

void clearRgbLeds();

void animateSplashExit();
void animateGreenSelect();
void animateRedBack();
void animateYellowHighscores();

void animateGameStart(
  int activeFrets
);

void animateGameOver();
void animateNewHighscore();

  // D. SOUND //
void playTone(int buzzerPin, unsigned int frequency, unsigned long duration);
void playGameTone(int buzzerPin, unsigned int frequency, unsigned long duration);
void playSelectSound();
void playBackSound();
void playNavUpSound();
void playNavDownSound();
void playGameOverSound();
void playVictorySound();
void playInitialsClickUp();
void playInitialsClickDown();

  // E. FLASH STORAGE //
uint32_t calculateScoreChecksum(
  const ScoreSaveData& data
);
bool initializeScoreStorage();
bool loadHighScores();
bool saveHighScores();
void resetHighScores();

  // F. LEADERBOARD UTILITY PROTOTYPES //
int insertTopScore(
  int scores[TOP_SCORE_COUNT],
  int newScore
);

int insertTopReactionTime(
  int scores[TOP_SCORE_COUNT],
  int newTime
);

void prepareInitialsSlot(
  char initials[TOP_SCORE_COUNT][INITIAL_COUNT + 1],
  int position
);

  // G. GENERAL SCREENS //
void drawSplashScreen();
void drawMainMenu();
void drawMenuControls();
void drawPlayingScreen(int score);
void drawGameOverScreen(int score);

  // H. MENUS //
void drawSpeedtestMenu();
void drawSimonMenu();
void drawReactionMenu();
void drawSoloMenu();

void updateMainMenu();
void updateSpeedtestMenu();
void updateSimonMenu();
void updateReactionMenu();
void updateSoloMenu();

  // I. LEADERBOARDS //
void drawSpeedLeaderboard();
void drawSimonLeaderboard();
void drawReactionLeaderboard();

  // J. INITIALS FOR LEADERBOARDS //
void startInitialsEntry();
void drawInitialsEntry();
void updateInitialsEntry();

  // K. DIFFICULTY //
void configureSpeedtestDifficulty();
void configureSimonsaysDifficulty();
void configureReactionDifficulty();

  // L. REACTION HELPERS //
void drawReactionWaitScreen();
void drawReactionGoScreen();
void drawReactionTimeScreen(unsigned long reaction);
void startReactionCue(int targetFret);

  // M. SOLO HELPERS //
void spawnNote(int fret);
void drawNotes();
void updateNotes();
void cleanupNotes();
void drawSoloRoot();
void drawSoloPlayingScreen();

  // N. GAMES //
void runSpeedtestGame();
void runSimonsaysGame();

void runReactionGame();
void runSoloGame();


// ----- 13. FUNCTION DEFINITIONS ----- //

  // A. GENERIC UTILITY FUNCTIONS //

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

  // B. INPUT FUNCTIONS //

bool menuReady() {
  if (millis() - lastMenuInput < MENU_COOLDOWN) {
    return false;
  }
  lastMenuInput = millis();
  return true;
}

bool greenPressed() {
  if (
    input.fretPressed[0] &&
    menuReady()
  ) {
    input.fretPressed[0] = false;
    return true;
  }

  return false;
}

bool redPressed() {
  if (
    input.fretPressed[1] &&
    menuReady()
  ) {
    input.fretPressed[1] = false;
    return true;
  }

  return false;
}

bool yellowPressed() {
  if (
    input.fretPressed[2] &&
    menuReady()
  ) {
    input.fretPressed[2] = false;
    return true;
  }

  return false;
}

bool bluePressed() {
  if (
    input.fretPressed[3] &&
    menuReady()
  ) {
    input.fretPressed[3] = false;
    return true;
  }

  return false;
}

bool orangePressed() {
  if (
    input.fretPressed[4] &&
    menuReady()
  ) {
    input.fretPressed[4] = false;
    return true;
  }

  return false;
}

bool strumUpPressed() {
  if (
    input.strumUpPressed &&
    menuReady()
  ) {
    input.strumUpPressed = false;
    return true;
  }

  return false;
}

bool strumDownPressed() {
  if (
    input.strumDownPressed &&
    menuReady()
  ) {
    input.strumDownPressed = false;
    return true;
  }

  return false;
}

bool startPressed() {
  if (
    input.startPressed &&
    menuReady()
  ) {
    input.startPressed = false;
    return true;
  }

  return false;
}

bool selectPressed() {
  if (
    input.selectPressed &&
    menuReady()
  ) {
    input.selectPressed = false;
    return true;
  }

  return false;
}

bool joystickButtonPressed() {
  if (
    input.joystickPressed &&
    menuReady()
  ) {
    input.joystickPressed = false;
    return true;
  }

  return false;
}

void initializeInputs() {

  // Read initial fret states
  for (int fret = 0; fret < FRET_COUNT; fret++) {

    bool held =
      digitalRead(BUTTON_PINS[fret]) == LOW;

    previousInput.fretHeld[fret] = held;

    input.fretHeld[fret] = held;
    input.fretPressed[fret] = false;
    input.fretReleased[fret] = false;
  }

  // Read initial strum states
  previousInput.strumUpHeld =
    digitalRead(STRUM_UP_PIN) == LOW;

  previousInput.strumDownHeld =
    digitalRead(STRUM_DOWN_PIN) == LOW;

  // Read initial utility-button states
  previousInput.startHeld =
    digitalRead(START_BUTTON_PIN) == LOW;

  previousInput.selectHeld =
    digitalRead(SELECT_BUTTON_PIN) == LOW;

  // Read initial joystick button state
  previousInput.joystickHeld =
    digitalRead(JOYSTICK_BUTTON_PIN) == LOW;

  // Copy initial held states
  input.strumUpHeld =
    previousInput.strumUpHeld;

  input.strumDownHeld =
    previousInput.strumDownHeld;

  input.startHeld =
    previousInput.startHeld;

  input.selectHeld =
    previousInput.selectHeld;

  input.joystickHeld =
    previousInput.joystickHeld;

  // No edge events exist during initialization
  input.strumUpPressed = false;
  input.strumUpReleased = false;

  input.strumDownPressed = false;
  input.strumDownReleased = false;

  input.startPressed = false;
  input.startReleased = false;

  input.selectPressed = false;
  input.selectReleased = false;

  input.joystickPressed = false;
  input.joystickReleased = false;

  // Read joystick axes
  input.joystickX =
    analogRead(JOYSTICK_X_PIN);

  input.joystickY =
    analogRead(JOYSTICK_Y_PIN);

  input.joystickLeft = false;
  input.joystickRight = false;
  input.joystickUp = false;
  input.joystickDown = false;
}

void updateInputs() {

  // ----- FRET BUTTONS ----- //

  for (int fret = 0; fret < FRET_COUNT; fret++) {

    bool held =
      digitalRead(BUTTON_PINS[fret]) == LOW;

    input.fretPressed[fret] =
      held &&
      !previousInput.fretHeld[fret];

    input.fretReleased[fret] =
      !held &&
      previousInput.fretHeld[fret];

    input.fretHeld[fret] = held;

    previousInput.fretHeld[fret] = held;
  }

  // ----- STRUM UP ----- //

  bool strumUpHeld =
    digitalRead(STRUM_UP_PIN) == LOW;

  input.strumUpPressed =
    strumUpHeld &&
    !previousInput.strumUpHeld;

  input.strumUpReleased =
    !strumUpHeld &&
    previousInput.strumUpHeld;

  input.strumUpHeld = strumUpHeld;

  previousInput.strumUpHeld =
    strumUpHeld;

  // ----- STRUM DOWN ----- //

  bool strumDownHeld =
    digitalRead(STRUM_DOWN_PIN) == LOW;

  input.strumDownPressed =
    strumDownHeld &&
    !previousInput.strumDownHeld;

  input.strumDownReleased =
    !strumDownHeld &&
    previousInput.strumDownHeld;

  input.strumDownHeld =
    strumDownHeld;

  previousInput.strumDownHeld =
    strumDownHeld;

  // ----- START ----- //

  bool startHeld =
    digitalRead(START_BUTTON_PIN) == LOW;

  input.startPressed =
    startHeld &&
    !previousInput.startHeld;

  input.startReleased =
    !startHeld &&
    previousInput.startHeld;

  input.startHeld = startHeld;

  previousInput.startHeld =
    startHeld;

  // ----- SELECT ----- //

  bool selectHeld =
    digitalRead(SELECT_BUTTON_PIN) == LOW;

  input.selectPressed =
    selectHeld &&
    !previousInput.selectHeld;

  input.selectReleased =
    !selectHeld &&
    previousInput.selectHeld;

  input.selectHeld = selectHeld;

  previousInput.selectHeld =
    selectHeld;

  // ----- JOYSTICK CENTER BUTTON ----- //

  bool joystickHeld =
    digitalRead(JOYSTICK_BUTTON_PIN) == LOW;

  input.joystickPressed =
    joystickHeld &&
    !previousInput.joystickHeld;

  input.joystickReleased =
    !joystickHeld &&
    previousInput.joystickHeld;

  input.joystickHeld = joystickHeld;

  previousInput.joystickHeld =
    joystickHeld;

  // ----- JOYSTICK AXES ----- //

  input.joystickX =
    analogRead(JOYSTICK_X_PIN);

  input.joystickY =
    analogRead(JOYSTICK_Y_PIN);

  // Wokwi horizontal direction:
  // high = left, low = right
  input.joystickLeft =
    input.joystickX >
    JOYSTICK_HIGH_THRESHOLD;

  input.joystickRight =
    input.joystickX <
    JOYSTICK_LOW_THRESHOLD;

  // Wokwi vertical direction:
  // high = up, low = down
  input.joystickUp =
    input.joystickY >
    JOYSTICK_HIGH_THRESHOLD;

  input.joystickDown =
    input.joystickY <
    JOYSTICK_LOW_THRESHOLD;
}

void consumeAllPressEvents() {

  for (int fret = 0; fret < FRET_COUNT; fret++) {
    input.fretPressed[fret] = false;
  }

  input.strumUpPressed = false;
  input.strumDownPressed = false;

  input.startPressed = false;
  input.selectPressed = false;

  input.joystickPressed = false;
}

void waitForAllGameInputsReleased() {

  unsigned long waitStart = millis();

  while (true) {

    updateInputs();

    bool anyHeld =
      input.strumUpHeld ||
      input.strumDownHeld;

    for (int fret = 0; fret < FRET_COUNT; fret++) {
      anyHeld |= input.fretHeld[fret];
    }

    if (!anyHeld) {
      break;
    }

    // Safety timeout prevents an infinite wait
    if (
      millis() - waitStart >= 2000
    ) {
      Serial.println(
        "Input release timeout"
      );

      break;
    }

    delay(1);
  }

  // Discard release-phase edge events
  consumeAllPressEvents();
}

// C. HARDWARE & LED FUNCTIONS //

void updateMenuLEDs() {

  uint32_t startColor =
    input.startHeld
      ? LED_COLOR_TURQUOISE
      : LED_COLOR_OFF;

  uint32_t selectColor =
    input.selectHeld
      ? LED_COLOR_LAVENDER
      : LED_COLOR_OFF;

  // Joystick center overrides Select with yellow
  if (input.joystickHeld) {
    selectColor =
      LED_COLOR_YELLOW;
  }

  setUtilityColors(
    startColor,
    selectColor
  );

  // Fret pairs
  for (int fret = 0; fret < FRET_COUNT; fret++) {

    uint32_t fretColor =
      input.fretHeld[fret]
        ? FRET_RGB_COLORS[fret]
        : LED_COLOR_OFF;

    setFretColor(
      fret,
      fretColor
    );
  }

  // Strum pixels
  setStrumColors(
    input.strumUpHeld
      ? LED_COLOR_TURQUOISE
      : LED_COLOR_OFF,

    input.strumDownHeld
      ? LED_COLOR_PINK
      : LED_COLOR_OFF
  );

  rgbLeds.show();
}

void waitForGreenPress() {

  // Discard the press that led to this screen.
  waitForAllGameInputsReleased();
  consumeAllPressEvents();

  // Wait for a fresh Green press.
  while (true) {

    updateInputs();

    if (input.fretPressed[0]) {
      input.fretPressed[0] = false;
      break;
    }

    delay(1);
  }

  // Do not allow the same Green press to carry
  // into the next Reaction round.
  waitForAllGameInputsReleased();
  consumeAllPressEvents();
}

void playStartupFretSequence() {

  Serial.println("Startup sequence begin");

  // Make sure all fret LEDs begin off
  clearRgbLeds();

  // Blues-style note lengths
  const int noteLengths[FRET_COUNT] = {
    180, // Green: medium
    50, // Red: short
    210, // Yellow: long
    100, // Blue: short
    280 // Orange: long ending
  };

  // Delay after each LED/note
  const int noteGaps[FRET_COUNT] = {
    20,
    40,
    25,
    60,
    30
  };

  // Light fret RGB LED pairs one by one
  for (int fret = 0; fret < FRET_COUNT; fret++) {

    // Check for Green + Orange held during startup -> mute sounds
    if (
      digitalRead(BUTTON_PINS[0]) == LOW &&
      digitalRead(BUTTON_PINS[4]) == LOW  
    ) { 
      soundEnabled = false;
      
      noTone(NOTE_BUZZER);
      noTone(FX_BUZZER);
    }
  
    setFretColor(
      fret,
      FRET_RGB_COLORS[fret]
    );

    rgbLeds.show();

    playTone(
      NOTE_BUZZER,
      SOLO_BLUES[2][fret],
      noteLengths[fret]
    );
  
    delay(noteLengths[fret]);
    setFretColor(
      fret,
      LED_COLOR_OFF
    );

    rgbLeds.show();

    delay(noteGaps[fret]);
  }

  Serial.println("Startup frets complete");

  // Light strum-up LED
  rgbLeds.setPixelColor(
    RGB_STRUM_UP,
    LED_COLOR_TURQUOISE
  );

  rgbLeds.show();

  playTone(NOTE_BUZZER, 587, 180);

  delay(200);

  rgbLeds.setPixelColor(
    RGB_STRUM_UP,
    LED_COLOR_OFF
  );

  rgbLeds.show();

  delay(50);

  // Light strum-down LED
  rgbLeds.setPixelColor(
    RGB_STRUM_DOWN,
    LED_COLOR_PINK
  );

  rgbLeds.show();

  playTone(NOTE_BUZZER, 698, 180);

  delay(200);

  rgbLeds.setPixelColor(
    RGB_STRUM_DOWN,
    LED_COLOR_OFF
  );

  rgbLeds.show();

  delay(50);

  Serial.println("Startup strums complete");

  // Light all five fret RGB LED pairs
  for (int fret = 0; fret < FRET_COUNT; fret++) {

    setFretColor(
      fret,
      FRET_RGB_COLORS[fret]
    );
  }

  // Light both strum LEDs together
  setStrumColors(
    LED_COLOR_TURQUOISE,
    LED_COLOR_PINK
  );

  // Start and Select can use theme colors
  setUtilityColors(
    LED_COLOR_LAVENDER,
    LED_COLOR_YELLOW
  );

  // Send all final colors together
  rgbLeds.show();

  // Finishing sound
  playTone(FX_BUZZER, 440, 300);
  playTone(NOTE_BUZZER, 587, 300);

  delay(500);

  // Turn off all 14 RGB LEDs
  clearRgbLeds();

  noTone(FX_BUZZER);
  noTone(NOTE_BUZZER);
  
  Serial.println("Startup sequence complete");

  // Start the release timeout timer
  unsigned long releaseStart = millis();

  // Wait for Green or Red to be released,
  // but never wait longer than one second  
  while (
    digitalRead(BUTTON_PINS[0]) == LOW ||
    digitalRead(BUTTON_PINS[1]) == LOW
  ) {
    if (millis() - releaseStart >= 1000) {
      Serial.println(
        "Startup release timeout"
      );

      break;
    }

  delay(1);
  }
}

void setFretColor(
  int fret,
  uint32_t color
) {
  if (
    fret < 0 ||
    fret >= FRET_COUNT
  ) {
    return;
  }

  rgbLeds.setPixelColor(
    FRET_RGB_PIXELS[fret][0],
    color
  );

  rgbLeds.setPixelColor(
    FRET_RGB_PIXELS[fret][1],
    color
  );
}

void setFretRgb(
  int fret,
  uint8_t red,
  uint8_t green,
  uint8_t blue
) {
  setFretColor(
    fret,
    rgbLeds.Color(
      red,
      green,
      blue
    )
  );
}

void setAllFretsColor(
  uint32_t color
) {
  for (int fret = 0; fret < FRET_COUNT; fret++) {
    setFretColor(
      fret,
      color
    );
  }
}

void setStrumColors(
  uint32_t upColor,
  uint32_t downColor
) {
  rgbLeds.setPixelColor(
    RGB_STRUM_UP,
    upColor
  );

  rgbLeds.setPixelColor(
    RGB_STRUM_DOWN,
    downColor
  );
}

void setUtilityColors(
  uint32_t startColor,
  uint32_t selectColor
) {
  rgbLeds.setPixelColor(
    RGB_START,
    startColor
  );

  rgbLeds.setPixelColor(
    RGB_SELECT,
    selectColor
  );
}

void setAllInputLightsOff() {
  rgbLeds.clear();
}

void fadeAllFrets(
  uint8_t red,
  uint8_t green,
  uint8_t blue,
  unsigned long durationMs
) {
  unsigned long stepDelay =
    durationMs / RGB_FADE_STEPS;

  if (stepDelay < 1) {
    stepDelay = 1;
  }

  for (
    int step = RGB_FADE_STEPS;
    step >= 0;
    step--
  ) {
    for (int fret = 0; fret < FRET_COUNT; fret++) {
      setFretRgb(
        fret,
        red * step / RGB_FADE_STEPS,
        green * step / RGB_FADE_STEPS,
        blue * step / RGB_FADE_STEPS
      );
    }

    rgbLeds.show();
    delay(stepDelay);
  }

  clearRgbLeds();
}

void clearRgbLeds() {
  setAllInputLightsOff();
  rgbLeds.show();
}

void flashAllFrets(
  uint32_t color,
  unsigned long durationMs
) {
  setAllFretsColor(color);
  rgbLeds.show();

  delay(durationMs);

  setAllFretsColor(LED_COLOR_OFF);
  rgbLeds.show();
}

void animateSplashExit() {

  clearRgbLeds();

  const uint32_t splashColors[] = {
    LED_COLOR_GREEN,
    LED_COLOR_RED,
    LED_COLOR_YELLOW,
    LED_COLOR_BLUE,
    LED_COLOR_ORANGE,
    LED_COLOR_TURQUOISE,
    LED_COLOR_PINK,
    LED_COLOR_LAVENDER
  };

  constexpr int splashColorCount =
    sizeof(splashColors) /
    sizeof(splashColors[0]);

  // Light all 14 pixels individually
  for (
    int pixel = 0;
    pixel < RGB_LED_COUNT;
    pixel++
  ) {
    uint32_t color =
      splashColors[
        pixel % splashColorCount
      ];

    rgbLeds.setPixelColor(
      pixel,
      color
    );

    rgbLeds.show();
    delay(RGB_STEP_FAST_MS);
  }

  delay(RGB_HOLD_SHORT_MS);

  // Fade each pixel from its assigned color
  for (
    int step = RGB_FADE_STEPS;
    step >= 0;
    step--
  ) {
    for (
      int pixel = 0;
      pixel < RGB_LED_COUNT;
      pixel++
    ) {
      uint32_t originalColor =
        splashColors[
          pixel % splashColorCount
        ];

      uint8_t red =
        (originalColor >> 16) & 0xFF;

      uint8_t green =
        (originalColor >> 8) & 0xFF;

      uint8_t blue =
        originalColor & 0xFF;

      rgbLeds.setPixelColor(
        pixel,
        rgbLeds.Color(
          red * step / RGB_FADE_STEPS,
          green * step / RGB_FADE_STEPS,
          blue * step / RGB_FADE_STEPS
        )
      );
    }

    rgbLeds.show();
    delay(RGB_FADE_STEP_MS);
  }

  clearRgbLeds();
}

void animateGreenSelect() {

  clearRgbLeds();

  const uint32_t green =
    rgbLeds.Color(30, 255, 100);

  for (int fret = 0; fret < FRET_COUNT; fret++) {
    setFretColor(
      fret,
      green
    );

    rgbLeds.show();
    delay(RGB_STEP_FAST_MS);
  }

  delay(RGB_HOLD_SHORT_MS);

  clearRgbLeds();
}

void animateRedBack() {

  clearRgbLeds();

  const uint32_t red =
    rgbLeds.Color(255, 25, 45);

  // Fret 2
  setFretColor(1, red);
  rgbLeds.show();
  delay(RGB_STEP_FAST_MS);

  // Fret 1 and Fret 3
  setFretColor(0, red);
  setFretColor(2, red);
  rgbLeds.show();
  delay(RGB_STEP_FAST_MS);

  // Fret 4
  setFretColor(3, red);
  rgbLeds.show();
  delay(RGB_STEP_FAST_MS);

  // Fret 5
  setFretColor(4, red);
  rgbLeds.show();
  delay(RGB_HOLD_SHORT_MS);

  clearRgbLeds();
}

void animateYellowHighscores() {

  clearRgbLeds();

  const uint32_t yellow =
    rgbLeds.Color(255, 210, 45);

  // Outer frets
  setFretColor(0, yellow);
  setFretColor(4, yellow);
  rgbLeds.show();
  delay(RGB_STEP_NORMAL_MS);

  // Inner frets
  setFretColor(1, yellow);
  setFretColor(3, yellow);
  rgbLeds.show();
  delay(RGB_STEP_NORMAL_MS);

  // Center fret
  setFretColor(2, yellow);
  rgbLeds.show();
  delay(RGB_HOLD_SHORT_MS);

  clearRgbLeds();
}

void animateGameStart(
  int activeFrets
) {
  activeFrets = constrain(
    activeFrets,
    1,
    FRET_COUNT
  );

  clearRgbLeds();

  // Illuminate active frets in sequence
  for (int fret = 0; fret < activeFrets; fret++) {
    setFretColor(
      fret,
      FRET_RGB_COLORS[fret]
    );

    rgbLeds.show();
    delay(RGB_STEP_SLOW_MS);
  }

  delay(RGB_HOLD_SHORT_MS);

  // Fade only the active frets
  for (
    int step = RGB_FADE_STEPS;
    step >= 0;
    step--
  ) {
    for (int fret = 0; fret < activeFrets; fret++) {
      setFretRgb(
        fret,
        FRET_RGB_RED[fret] *
          step / RGB_FADE_STEPS,

        FRET_RGB_GREEN[fret] *
          step / RGB_FADE_STEPS,

        FRET_RGB_BLUE[fret] *
          step / RGB_FADE_STEPS
      );
    }

    rgbLeds.show();
    delay(RGB_FADE_STEP_MS);
  }

  clearRgbLeds();
}

void animateGameOver() {

  clearRgbLeds();

  setAllFretsColor(
    LED_COLOR_RED
  );

  rgbLeds.show();

  delay(RGB_HOLD_SHORT_MS);

  fadeAllFrets(
    255,
    20,
    35,
    500
  );
}

void animateNewHighscore() {

  clearRgbLeds();

  // Each fret uses its assigned color
  for (int fret = 0; fret < FRET_COUNT; fret++) {
    setFretColor(
      fret,
      FRET_RGB_COLORS[fret]
    );
  }

  rgbLeds.show();
  delay(180);

  // Gold
  const uint8_t goldRed = 255;
  const uint8_t goldGreen = 165;
  const uint8_t goldBlue = 20;

  setAllFretsColor(
    rgbLeds.Color(
      goldRed,
      goldGreen,
      goldBlue
    )
  );

  rgbLeds.show();
  delay(180);

  fadeAllFrets(
    goldRed,
    goldGreen,
    goldBlue,
    500
  );
}

  // D. SOUND FUNCTIONS //

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

void playGameTone(
  int buzzerPin,
  unsigned int frequency,
  unsigned long duration
) {
  if (
    !soundEnabled ||
    !useSound
  ) {
    return;
  }

  tone(
    buzzerPin,
    frequency,
    duration
  );
}

void playSelectSound() {
  playTone(FX_BUZZER, 277, 16); // tone(FX_BUZZER, 330, 8); v1
  delay(16);
  playTone(FX_BUZZER, 370, 24); // tone(FX_BUZZER, 660, 8); v1
}

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

void playInitialsClickUp() {
  // Short, bright upward click
  playTone(
    FX_BUZZER,
    1800,
    5
  );
}

void playInitialsClickDown() {
  // Short, slightly lower downward click
  playTone(
    FX_BUZZER,
    1250,
    5
  );
}
  // E. FLASH STORAGE //

bool initializeScoreStorage() {

  scoreFileSystem = new LittleFS_MBED();

  if (scoreFileSystem == nullptr) {
    Serial.println(
      "ERROR: Could not create LittleFS object"
    );

    scoreStorageReady = false;
    return false;
  }

  if (!scoreFileSystem->init()) {
    Serial.println(
      "ERROR: LittleFS mount failed"
    );

    scoreStorageReady = false;
    return false;
  }

  scoreStorageReady = true;

  Serial.println(
    "LittleFS score storage ready"
  );

  return true;
}

uint32_t calculateScoreChecksum(
  const ScoreSaveData& data
) {
  const uint8_t* bytes =
    reinterpret_cast<const uint8_t*>(&data);

  const size_t checksumOffset =
    offsetof(ScoreSaveData, checksum);

  uint32_t hash = 2166136261UL;

  for (size_t i = 0; i < checksumOffset; i++) {
    hash ^= bytes[i];
    hash *= 16777619UL;
  }

  return hash;
}

void resetHighScores() {

  memset(&speedHS, 0, sizeof(speedHS));
  memset(&simonHS, 0, sizeof(simonHS));
  memset(&reactionHS, 0, sizeof(reactionHS));

  memset(
    speedNormalInitials,
    0,
    sizeof(speedNormalInitials)
  );

  memset(
    speedNoLightInitials,
    0,
    sizeof(speedNoLightInitials)
  );

  memset(
    simonNormalInitials,
    0,
    sizeof(simonNormalInitials)
  );

  memset(
    simonNoLightInitials,
    0,
    sizeof(simonNoLightInitials)
  );

  memset(
    reactionNormalInitials,
    0,
    sizeof(reactionNormalInitials)
  );

  memset(
    reactionNoLightInitials,
    0,
    sizeof(reactionNoLightInitials)
  );
}

bool loadHighScores() {

  if (!scoreStorageReady) {
    Serial.println(
      "Scores not loaded: flash unavailable"
    );

    resetHighScores();
    return false;
  }

  FILE* file = fopen(
    SCORE_FILE_PATH,
    "rb"
  );

  if (file == nullptr) {

    Serial.println(
      "No score file found, using empty scores"
    );

    resetHighScores();
    return false;
  }

  ScoreSaveData data = {};

  size_t bytesRead = fread(
    &data,
    1,
    sizeof(data),
    file
  );

  fclose(file);

  if (bytesRead != sizeof(data)) {

    Serial.println(
      "Score file has incorrect size"
    );

    resetHighScores();
    return false;
  }

  if (data.magic != SCORE_FILE_MAGIC) {

    Serial.println(
      "Score file has invalid identifier"
    );

    resetHighScores();
    return false;
  }

  if (data.version != SCORE_FILE_VERSION) {

    Serial.println(
      "Score file version is incompatible"
    );

    resetHighScores();
    return false;
  }

  if (data.dataSize != sizeof(ScoreSaveData)) {

    Serial.println(
      "Score structure size has changed"
    );

    resetHighScores();
    return false;
  }

  uint32_t expectedChecksum =
    calculateScoreChecksum(data);

  if (data.checksum != expectedChecksum) {

    Serial.println(
      "Score file checksum failed"
    );

    resetHighScores();
    return false;
  }

  // Restore score structures
  speedHS = data.speedScores;
  simonHS = data.simonScores;
  reactionHS = data.reactionScores;

  // Restore Speedtest initials
  memcpy(
    speedNormalInitials,
    data.speedNormalNames,
    sizeof(speedNormalInitials)
  );

  memcpy(
    speedNoLightInitials,
    data.speedNoLightNames,
    sizeof(speedNoLightInitials)
  );

  // Restore Simon initials
  memcpy(
    simonNormalInitials,
    data.simonNormalNames,
    sizeof(simonNormalInitials)
  );

  memcpy(
    simonNoLightInitials,
    data.simonNoLightNames,
    sizeof(simonNoLightInitials)
  );

  // Restore Reaction initials
  memcpy(
    reactionNormalInitials,
    data.reactionNormalNames,
    sizeof(reactionNormalInitials)
  );

  memcpy(
    reactionNoLightInitials,
    data.reactionNoLightNames,
    sizeof(reactionNoLightInitials)
  );

  Serial.println(
    "Highscores loaded from flash"
  );

  return true;
}

bool saveHighScores() {

  if (!scoreStorageReady) {

    Serial.println(
      "Scores not saved: flash unavailable"
    );

    return false;
  }

  ScoreSaveData data = {};

  data.magic = SCORE_FILE_MAGIC;
  data.version = SCORE_FILE_VERSION;
  data.dataSize = sizeof(ScoreSaveData);

  // Copy score structures
  data.speedScores = speedHS;
  data.simonScores = simonHS;
  data.reactionScores = reactionHS;

  // Copy Speedtest initials
  memcpy(
    data.speedNormalNames,
    speedNormalInitials,
    sizeof(speedNormalInitials)
  );

  memcpy(
    data.speedNoLightNames,
    speedNoLightInitials,
    sizeof(speedNoLightInitials)
  );

  // Copy Simon initials
  memcpy(
    data.simonNormalNames,
    simonNormalInitials,
    sizeof(simonNormalInitials)
  );

  memcpy(
    data.simonNoLightNames,
    simonNoLightInitials,
    sizeof(simonNoLightInitials)
  );

  // Copy Reaction initials
  memcpy(
    data.reactionNormalNames,
    reactionNormalInitials,
    sizeof(reactionNormalInitials)
  );

  memcpy(
    data.reactionNoLightNames,
    reactionNoLightInitials,
    sizeof(reactionNoLightInitials)
  );

  data.checksum =
    calculateScoreChecksum(data);

  // Write temporary file first
  const char temporaryPath[] =
    MBED_LITTLEFS_FILE_PREFIX
    "/polybar_scores.tmp";

  FILE* file = fopen(
    temporaryPath,
    "wb"
  );

  if (file == nullptr) {

    Serial.println(
      "ERROR: Could not open temporary score file"
    );

    return false;
  }

  size_t bytesWritten = fwrite(
    &data,
    1,
    sizeof(data),
    file
  );

  fflush(file);
  fclose(file);

  if (bytesWritten != sizeof(data)) {

    Serial.println(
      "ERROR: Incomplete score write"
    );

    remove(temporaryPath);
    return false;
  }

  // Remove previous valid file
  remove(SCORE_FILE_PATH);

  // Rename completed temporary file
  if (
    rename(
      temporaryPath,
      SCORE_FILE_PATH
    ) != 0
  ) {
    Serial.println(
      "ERROR: Could not finalize score file"
    );

    return false;
  }

  Serial.println(
    "Highscores saved to flash"
  );

  return true;
}

  // F. LEADERBOARD UTILITIES //

// Insert a higher-is-better score
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

// Insert a lower-is-better reaction time
int insertTopReactionTime(
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
      return i;
    }
  }

  return -1;
}

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

  // G. GENERAL SCREENS //

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

void drawMainMenu() {

  tft.fillScreen(COLOR_BG);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);

  for (int i = 0; i < MAIN_MENU_COUNT; i++) {
    int y = 2 + (i * 18);

    if (i == mainMenuIndex) {
      tft.setTextColor(COLOR_SELECT);

      int16_t x1, y1;
      uint16_t w, h;
      
      tft.getTextBounds(mainMenuOptions[i], 0, 0, &x1, &y1, &w, &h);

      tft.setCursor((SCREEN_WIDTH -w) / 2, y);

      tft.print(mainMenuOptions[i]);

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
  tft.drawLine(3, 75, 4, 77, COLOR_SELECT);
  tft.drawLine(2, 75, 5, 77, COLOR_SELECT);
  tft.drawLine(5, 77, 10, 72, COLOR_SELECT);
  tft.drawLine(5, 77, 9, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 77, COLOR_BACK);
  tft.drawLine(152, 72, 156, 76, COLOR_BACK);
  tft.drawLine(156, 72, 152, 78, COLOR_BACK);
  tft.drawLine(157, 72, 151, 78, COLOR_BACK);
}

void drawMenuControls() {

  // Bottom-right1 ✓ check mark
  tft.drawLine(139, 75, 140, 77, COLOR_SELECT);
  tft.drawLine(138, 75, 141, 77, COLOR_SELECT);
  tft.drawLine(141, 77, 146, 72, COLOR_SELECT);
  tft.drawLine(141, 77, 145, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 77, COLOR_BACK);
  tft.drawLine(152, 72, 156, 76, COLOR_BACK);
  tft.drawLine(156, 72, 152, 78, COLOR_BACK);
  tft.drawLine(157, 72, 151, 78, COLOR_BACK);

  // Small medal icon, top-right corner
  // Medal ribbons
  tft.drawLine(149, 2, 153, 7, COLOR_PASTEL_YELLOW);
  tft.drawLine(157, 2, 153, 7, COLOR_PASTEL_YELLOW);  
  // Medal circle
  tft.drawCircle(153, 10, 4, COLOR_PASTEL_YELLOW);
  // Medal center dot
  tft.fillCircle(153, 10,  1,  COLOR_PASTEL_YELLOW);
}

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

  // H. MENU SCREENS //

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

  int firstVisible = getFirstVisibleItem(
    speedtestIndex,
    SPEEDTEST_COUNT,
    VISIBLE_MENU_ROWS
  );

  // Difficulties + highscores
  for (int row = 0; row < VISIBLE_MENU_ROWS; row++) {
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

  // Calculate the first visible menu option
  int firstVisible = getFirstVisibleItem(
    simonIndex,
    SIMONSAYS_COUNT,
    VISIBLE_MENU_ROWS
  );

  // Draw visible difficulty rows
  for (int row = 0; row < VISIBLE_MENU_ROWS; row++) {

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

    snprintf(normal, sizeof(normal), "%d", reactionHS.normal[i][0]);
    snprintf(noLight, sizeof(noLight), "%d", reactionHS.noLight[i][0]);

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
  tft.drawLine(139, 75, 140, 77, COLOR_SELECT);
  tft.drawLine(138, 75, 141, 77, COLOR_SELECT);
  tft.drawLine(141, 77, 146, 72, COLOR_SELECT);
  tft.drawLine(141, 77, 145, 72, COLOR_SELECT);

  // Bottom-right2 ✗ back mark
  tft.drawLine(152, 71, 158, 77, COLOR_BACK);
  tft.drawLine(152, 72, 156, 76, COLOR_BACK);
  tft.drawLine(156, 72, 152, 78, COLOR_BACK);
  tft.drawLine(157, 72, 151, 78, COLOR_BACK);
}

  // I. LEADERBOARDS //

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

  // J. INITIALS ENTRY

void startInitialsEntry() {

  enteredInitials[0] = 'A';
  enteredInitials[1] = 'A';
  enteredInitials[2] = 'A';
  enteredInitials[3] = 'A';
  enteredInitials[4] = '\0';

  initialsPosition = 0;
  initialsCharacter = 'A';

  clearRgbLeds();

  currentState = STATE_ENTER_INITIALS;
}

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
/*
  // Small control instructions
  tft.setTextSize(1);
  tft.setTextColor(COLOR_PASTEL_YELLOW);
  tft.setCursor(2, 70);
  tft.print("STRUM:CHANGE G:OK R:BACK");
*/

}

void updateInitialsEntry() {

  // Next printable ASCII character
  if (strumUpPressed()) {
    
    initialsCharacter++;

    if (initialsCharacter > 126) {
      initialsCharacter = 32;
    }

    enteredInitials[initialsPosition] =
      static_cast<char>(initialsCharacter);

    playInitialsClickUp();

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

    playInitialsClickDown();

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

      // Save every game's scores and initials
      saveHighScores();

      newHighScore = false;
      pendingInitials = nullptr;

      // Open the leaderboard for the game just played
      if (currentGame == GAME_SPEEDTEST) {
        speedLeaderboardOpen = true;
        currentState = STATE_SPEEDTEST_MENU;

      } else  if (currentGame == GAME_SIMON) {
        simonLeaderboardOpen = true;
        currentState = STATE_SIMON_MENU;

      } else if (currentGame == GAME_REACTION) {
        reactionLeaderboardOpen = true;
        currentState = STATE_REACTION_MENU;
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

  // K. DIFFICULTY HELPERS //

void configureSpeedtestDifficulty() {
  switch (speedtestIndex) {
    case 0: numInputs = 3; break;
    case 1: numInputs = 4; break;
    case 2: numInputs = 5; break;
    case 3: numInputs = 6; break;
  }
}

void configureSimonsaysDifficulty() {
  switch (simonIndex) {
    case 0: numInputs = 2; break;
    case 1: numInputs = 3; break;
    case 2: numInputs = 4; break;
    case 3: numInputs = 5; break;
    case 4: numInputs = 6; break; // 5 frets + strum bar
  }
}

void configureReactionDifficulty() {
    reactionRequireCorrectFret =
        (reactionIndex == 1);
}

  // L. REACTION SCREEN HELPERS //

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

void startReactionCue(int targetFret) {

  clearRgbLeds();

  drawReactionGoScreen();

  // Visual cue
  if (useLight) {
    if (reactionIndex == 0) {

      // Simple mode accepts any fret
      for (int fret = 0; fret < FRET_COUNT; fret++) {
        setFretColor(
          fret,
          FRET_RGB_COLORS[fret]
        );
      }

      rgbLeds.show();

    } else {

      // Chaos mode requires the target specific fret
      setFretColor(
        targetFret,
        FRET_RGB_COLORS[targetFret]
      );

      rgbLeds.show();
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

  // M. SOLO-JAM ANIMATION HELPERS //

void spawnNote(int fret) {

    if (activeNotes >= MAX_MUSIC_NOTES)
        return;

    notes[activeNotes].active = true;

    notes[activeNotes].symbolIndex = fret;
    notes[activeNotes].noteType = random(0, FRET_COUNT);
    
    // notes[activeNotes].color = noteColor;

    notes[activeNotes].x = 155;
    notes[activeNotes].y = 75 - (soloRoot * 15 + fret * 5);

    activeNotes++;
}

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

void drawSoloRoot() {

    tft.fillRect(120, 0, 40, 16, COLOR_BG);

    tft.setTextSize(2);
    tft.setTextColor(COLOR_SOLO);

    tft.setCursor(140, 0);
    tft.print(ROOT_NAMES[soloRoot]);
}

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

  // N. MENU UPDATES //

void updateMainMenu() {
  
  if (strumUpPressed()) {
    mainMenuIndex++;

    if (
      mainMenuIndex >=
      MAIN_MENU_COUNT
    ) {
      mainMenuIndex = 0;
    }

    playTone(
      FX_BUZZER,
      MENU_SCALE_4[mainMenuIndex],
      32
    );

    drawMainMenu();
  }

  if (strumDownPressed()) {

    mainMenuIndex--;

    if (mainMenuIndex < 0) {
      mainMenuIndex = 
      MAIN_MENU_COUNT - 1;
    }
    playTone(FX_BUZZER,
      MENU_SCALE_4[mainMenuIndex],
      32
    );
    drawMainMenu();
  }

  if (greenPressed()) {
    playSelectSound();
    animateGreenSelect();
    
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
      animateRedBack();
      speedLeaderboardOpen = false;
      drawSpeedtestMenu();
    }    
    return;
  }

  // Start speedtest game
  if (greenPressed()) {
    playSelectSound();
    configureSpeedtestDifficulty();
    animateGameStart(
      min(numInputs, FRET_COUNT)
    );
    useLight = true;
    useSound = true;
    currentGame = GAME_SPEEDTEST;
    runSpeedtestGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    animateRedBack();
    
    currentState = STATE_MAIN_MENU;
  }

  // Show speedtest leaderboard
  if (yellowPressed()) {
    playSelectSound(); // tähän voi tehdä oman äänen EDIT
    animateYellowHighscores();
    speedLeaderboardOpen = true;
    drawSpeedLeaderboard();
    return;
  }
  
  // Start speedtest game with sound only
  if (orangePressed()) {

    playSelectSound();

    configureSpeedtestDifficulty();

    animateGameStart(
      min(numInputs, FRET_COUNT)
    );

    useLight = false;
    useSound = true;

    currentGame = GAME_SPEEDTEST;
    runSpeedtestGame();
  }

  // Navigate through speedtest difficulties
  if (strumUpPressed()) {
    speedtestIndex++;

    if (speedtestIndex >= SPEEDTEST_COUNT) {
      speedtestIndex = 0;
    }

      playTone(FX_BUZZER, MENU_SCALE_4[speedtestIndex], 32);
    drawSpeedtestMenu();
  }

  if (strumDownPressed()) {
    speedtestIndex--;

    if (speedtestIndex < 0) {
      speedtestIndex = SPEEDTEST_COUNT - 1;
    }
    playTone(FX_BUZZER, MENU_SCALE_4[speedtestIndex], 32);
    drawSpeedtestMenu();
  }
}

void updateSimonMenu() {

  // If the leaderboard is open, check for yellow button to close it
  if (simonLeaderboardOpen) {
    if (strumUpPressed()) {
      simonIndex++;

      if (simonIndex >= SIMONSAYS_COUNT) {
        simonIndex = 0;
      }
      drawSimonLeaderboard();
    } 

    if (strumDownPressed()) {
      simonIndex--;

      if (simonIndex < 0) {
        simonIndex =
          SIMONSAYS_COUNT -1;
      }
      
      drawSimonLeaderboard();
    }

    if (yellowPressed()) {
      animateRedBack();
      simonLeaderboardOpen = false;
      drawSimonMenu();
    }
    return;
  }

  // Start SimonSays game
  if (greenPressed()) {
    playSelectSound();
    configureSimonsaysDifficulty();
    animateGameStart(
      min(numInputs, FRET_COUNT)
    );
    useLight = true;
    useSound = true;
    currentGame = GAME_SIMON;
    runSimonsaysGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    animateRedBack();
    currentState = STATE_MAIN_MENU;
  }

  // Show SimonSays leaderboard
  if (yellowPressed()) {
    playSelectSound(); // tähän voi tehdä oman äänen EDIT
    animateYellowHighscores();
    
    simonLeaderboardOpen = true;
    drawSimonLeaderboard();
    return;
  }

  // Start SimonSays game with sound only
  if (orangePressed()) {
    playSelectSound();

    configureSimonsaysDifficulty();

    animateGameStart(
      min(numInputs, FRET_COUNT)
    );

    useLight = false;
    useSound = true;

    currentGame = GAME_SIMON;
    runSimonsaysGame();
  }

  // Navigate through SimonSays difficulties
  if (strumUpPressed()) {
    simonIndex++;
    if (simonIndex >= SIMONSAYS_COUNT) {
      simonIndex = 0;
    }
      playTone(FX_BUZZER, SOLO_BLUES[2][simonIndex], 32);
    drawSimonMenu();
  }
  if (strumDownPressed()) {
    simonIndex--;
    if (simonIndex < 0) {
      simonIndex = SIMONSAYS_COUNT - 1;
    }
    playTone(FX_BUZZER, SOLO_BLUES[2][simonIndex], 32);
    
    drawSimonMenu();
  }
}

void updateReactionMenu() {

  // If the leaderboard is open, check for yellow button to close it
  if (reactionLeaderboardOpen) {

    if (strumUpPressed()) {

      reactionIndex++;

    if (reactionIndex >= REACTION_COUNT) {
      reactionIndex = 0;
    }

    drawReactionLeaderboard();

  }

  if (strumDownPressed()) {

    reactionIndex--;

    if (reactionIndex < 0) {
      reactionIndex = REACTION_COUNT -1;
    }

    drawReactionLeaderboard();
  }

    if (yellowPressed()) {
      animateRedBack();
      reactionLeaderboardOpen = false;
      drawReactionMenu();
    }

    return;
  }

  // Start Reaction game
  if (greenPressed()) {
    playSelectSound();
    configureReactionDifficulty();
    animateGameStart(FRET_COUNT);
    
    useLight = true;
    useSound = true;
    currentGame = GAME_REACTION;
    runReactionGame();
  }

  // Back to main menu
  if (redPressed()) {
    playBackSound();
    animateRedBack();
    
    currentState = STATE_MAIN_MENU;
  }

  // Show Reaction leaderboard
  if (yellowPressed()) {
    playSelectSound(); // tähän voi tehdä oman äänen EDIT
    animateYellowHighscores();
    
    reactionLeaderboardOpen = true;
    drawReactionLeaderboard();
    return;
  }

  // Start Reaction game with sound only
  if (orangePressed()) {
    playSelectSound();
    configureReactionDifficulty();

    animateGameStart(
      min(numInputs, FRET_COUNT)
    );
    
    useLight = false;
    useSound = true;
    currentGame = GAME_REACTION;
    runReactionGame();
  }

  // Navigate through Reaction difficulties
  if (strumUpPressed()) {
    reactionIndex++;
    if (reactionIndex >= REACTION_COUNT) {
      reactionIndex = 0;
    }
    playTone(FX_BUZZER,MENU_SCALE_3[reactionIndex], 32);
    drawReactionMenu();
  }
  if (strumDownPressed()) {
    reactionIndex--;
    if (reactionIndex < 0) {
      reactionIndex = REACTION_COUNT - 1;
    }
      playTone(FX_BUZZER, MENU_SCALE_3[reactionIndex], 32);
    drawReactionMenu();
  }
}

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
    animateRedBack();
    currentState = STATE_MAIN_MENU;
  }

  // Navigate through Solo difficulties
  if (strumUpPressed()) {
    soloIndex++;
    if (soloIndex >= SOLO_COUNT) {
      soloIndex = 0;
    }

    playTone(FX_BUZZER, MENU_SCALE_3[soloIndex], 32);
    drawSoloMenu();
  }

  if (strumDownPressed()) {
    soloIndex--;
    if (soloIndex < 0) {
      soloIndex = SOLO_COUNT - 1;
    }
      playTone(FX_BUZZER, MENU_SCALE_3[soloIndex], 32);
    drawSoloMenu();
  }
}

// ------ 14. RUN GAME FUNCTIONS ----- //

  // A. SPEEDTEST     //
void runSpeedtestGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_SPEEDTEST;

  drawPlayingScreen(0);

  // Wait until the menu-start button and other
  // controls have been released.
  waitForAllGameInputsReleased();

  clearRgbLeds();

  int score = 0;

  unsigned long currentDelay = 800;
  const unsigned long minDelay = 120;
  const unsigned long speedStep = 10;

  queueSize = 0;

  int lastInput = -1;

  unsigned long lastStepTime =
    millis();

  bool gameActive = true;

  while (gameActive) {

    // Speedtest is blocking, so it must update
    // the centralized input state internally.
    updateInputs();

    unsigned long now = millis();

    // ----- SPAWN NEXT TARGET ----- //

    if (
      now - lastStepTime >= currentDelay
    ) {
      lastStepTime = now;

      int nextInput;

      do {
        nextInput =
          random(0, numInputs);
      }
      while (nextInput == lastInput);

      lastInput = nextInput;

      if (
        queueSize < MAX_TARGET_QUEUE
      ) {
        targetQueue[queueSize] =
          nextInput;

        queueSize++;
      }

      // ----- TARGET SOUND ----- //

      if (useSound) {

        if (nextInput < FRET_COUNT) {

          playTone(
            NOTE_BUZZER,
            250 + nextInput * 100,
            50
          );

        } else {

          playTone(
            NOTE_BUZZER,
            1000,
            50
          );
        }
      }

      // ----- TARGET LIGHT ----- //

      if (useLight) {

        if (nextInput < FRET_COUNT) {

          setFretColor(
            nextInput,
            FRET_RGB_COLORS[nextInput]
          );

          rgbLeds.show();

          delay(RGB_HOLD_SHORT_MS);

          setFretColor(
            nextInput,
            LED_COLOR_OFF
          );

          rgbLeds.show();

        } else {

          setStrumColors(
            LED_COLOR_TURQUOISE,
            LED_COLOR_TURQUOISE
          );

          rgbLeds.show();

          delay(RGB_HOLD_SHORT_MS);

          setStrumColors(
            LED_COLOR_OFF,
            LED_COLOR_OFF
          );

          rgbLeds.show();
        }
      }

      // Increase the game speed
      if (currentDelay > minDelay) {

        if (
          currentDelay >
          minDelay + speedStep
        ) {
          currentDelay -=
            speedStep;

        } else {
          currentDelay =
            minDelay;
        }
      }
    }

    // ----- FRET INPUTS ----- //

    for (
      int fret = 0;
      fret < min(numInputs, FRET_COUNT);
      fret++
    ) {
      if (!input.fretPressed[fret]) {
        continue;
      }

      // Consume this press immediately
      input.fretPressed[fret] = false;

      bool correctInput =
        queueSize > 0 &&
        targetQueue[0] == fret;

      if (correctInput) {

        // Remove the first queued target
        for (
          int queueIndex = 0;
          queueIndex < queueSize - 1;
          queueIndex++
        ) {
          targetQueue[queueIndex] =
            targetQueue[queueIndex + 1];
        }

        queueSize--;
        score++;

        drawPlayingScreen(score);

      } else {

        gameActive = false;
        break;
      }
    }

    if (!gameActive) {
      break;
    }

    // ----- STRUM INPUT ----- //

    if (
      numInputs ==
        FRET_AND_STRUM_INPUT_COUNT &&
      (
        input.strumUpPressed ||
        input.strumDownPressed
      )
    ) {
      // Consume both strum press events
      input.strumUpPressed = false;
      input.strumDownPressed = false;

      bool correctInput =
        queueSize > 0 &&
        targetQueue[0] ==
          STRUM_INPUT_INDEX;

      if (correctInput) {

        // Remove the first queued target
        for (
          int queueIndex = 0;
          queueIndex < queueSize - 1;
          queueIndex++
        ) {
          targetQueue[queueIndex] =
            targetQueue[queueIndex + 1];
        }

        queueSize--;
        score++;

        drawPlayingScreen(score);

      } else {

        gameActive = false;
      }
    }

    // ----- OPTIONAL EMERGENCY EXIT ----- //

    if (
      input.startHeld &&
      input.selectHeld
    ) {
      Serial.println(
        "Speedtest aborted"
      );

      gameActive = false;
    }

    delay(2);
  }

  // Prevent the final incorrect press from
  // activating something on the next screen.
  consumeAllPressEvents();

  clearRgbLeds();

  // ----- HIGHSCORE PROCESSING ----- //

  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    // Normal/light mode
    highScorePosition =
      insertTopScore(
        speedHS.normal[speedtestIndex],
        score
      );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        speedNormalInitials[
          speedtestIndex
        ],
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
    highScorePosition =
      insertTopScore(
        speedHS.noLight[speedtestIndex],
        score
      );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        speedNoLightInitials[
          speedtestIndex
        ],
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

  // ----- NEW HIGHSCORE ----- //

  if (
    newHighScore &&
    pendingInitials != nullptr
  ) {
    playVictorySound();
    animateYellowHighscores();
    animateNewHighscore();

    startInitialsEntry();
    return;
  }

  // ----- NORMAL GAME OVER ----- //

  playGameOverSound();
  animateGameOver();

  currentState = STATE_GAMEOVER;
}

  // B. SIMON SAYS    //
void runSimonsaysGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_SIMON;

  drawPlayingScreen(0);

  // Prevent the menu-starting button press from
  // becoming Simon's first player input.
  waitForAllGameInputsReleased();
  consumeAllPressEvents();

  clearRgbLeds();

  constexpr int MAX_SIMON_SEQUENCE = 100;

  int sequence[MAX_SIMON_SEQUENCE] = {};

  int sequenceLength = 1;
  int score = 0;

  sequence[0] =
    random(0, numInputs);

  bool gameActive = true;

  while (gameActive) {

    // ----- SHOW SEQUENCE ----- //

    for (
      int sequenceIndex = 0;
      sequenceIndex < sequenceLength;
      sequenceIndex++
    ) {
      int targetInput =
        sequence[sequenceIndex];

      // Fret target
      if (targetInput < FRET_COUNT) {

        if (useLight) {
          setFretColor(
            targetInput,
            FRET_RGB_COLORS[targetInput]
          );

          rgbLeds.show();
        }

        if (useSound) {
          playTone(
            NOTE_BUZZER,
            250 + targetInput * 100,
            250
          );
        }

        delay(250);

        if (useLight) {
          setFretColor(
            targetInput,
            LED_COLOR_OFF
          );

          rgbLeds.show();
        }

      } else {

        // Strum target
        if (useLight) {
          setStrumColors(
            LED_COLOR_TURQUOISE,
            LED_COLOR_PINK
          );

          rgbLeds.show();
        }

        if (useSound) {
          playTone(
            NOTE_BUZZER,
            1000,
            250
          );
        }

        delay(250);

        if (useLight) {
          setStrumColors(
            LED_COLOR_OFF,
            LED_COLOR_OFF
          );

          rgbLeds.show();
        }
      }

      // Gap between displayed inputs
      delay(250);
    }

    // Inputs made while Simon displayed the sequence
    // must not count as player answers.
    waitForAllGameInputsReleased();
    consumeAllPressEvents();

    // ----- PLAYER REPEATS SEQUENCE ----- //

    for (
      int sequenceIndex = 0;
      sequenceIndex < sequenceLength;
      sequenceIndex++
    ) {
      int pressedInput = -1;

      unsigned long inputWaitStart =
        millis();

      while (
        pressedInput == -1 &&
        gameActive
      ) {
        updateInputs();

        // Two-second timeout
        if (
          millis() - inputWaitStart >
          2000
        ) {
          gameActive = false;
          break;
        }

        // ----- FRET INPUTS ----- //

        for (
          int fret = 0;
          fret < min(numInputs, FRET_COUNT);
          fret++
        ) {
          if (!input.fretPressed[fret]) {
            continue;
          }

          // Consume the event.
          input.fretPressed[fret] = false;

          pressedInput = fret;

          if (useSound) {
            playTone(
              NOTE_BUZZER,
              250 + fret * 100,
              RGB_HOLD_SHORT_MS
            );
          }

          if (useLight) {
            setFretColor(
              fret,
              FRET_RGB_COLORS[fret]
            );

            rgbLeds.show();

            delay(RGB_HOLD_SHORT_MS);

            setFretColor(
              fret,
              LED_COLOR_OFF
            );

            rgbLeds.show();
          }

          break;
        }

        // ----- STRUM INPUT ----- //

        if (
          pressedInput == -1 &&
          numInputs ==
            FRET_AND_STRUM_INPUT_COUNT &&
          (
            input.strumUpPressed ||
            input.strumDownPressed
          )
        ) {
          pressedInput =
            STRUM_INPUT_INDEX;

          // Consume both because either direction
          // represents the same Simon input.
          input.strumUpPressed = false;
          input.strumDownPressed = false;

          if (useSound) {
            playTone(
              NOTE_BUZZER,
              1000,
              RGB_HOLD_SHORT_MS
            );
          }

          if (useLight) {
            setStrumColors(
              LED_COLOR_TURQUOISE,
              LED_COLOR_PINK
            );

            rgbLeds.show();

            delay(RGB_HOLD_SHORT_MS);

            setStrumColors(
              LED_COLOR_OFF,
              LED_COLOR_OFF
            );

            rgbLeds.show();
          }
        }

        delay(2);
      }

      if (!gameActive) {
        break;
      }

      // Wrong input
      if (
        pressedInput !=
        sequence[sequenceIndex]
      ) {
        gameActive = false;
        break;
      }
    }

    // ----- ROUND COMPLETE ----- //

    if (gameActive) {

      score++;

      drawPlayingScreen(score);

      if (
        sequenceLength <
        MAX_SIMON_SEQUENCE
      ) {
        sequence[sequenceLength] =
          random(0, numInputs);

        sequenceLength++;

      } else {
        // Maximum safe sequence length reached.
        gameActive = false;
      }
    }
  }

  consumeAllPressEvents();
  clearRgbLeds();

  // ----- HIGHSCORE PROCESSING ----- //

  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    highScorePosition =
      insertTopScore(
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

    highScorePosition =
      insertTopScore(
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

  // ----- NEW HIGHSCORE ----- //

  if (
    newHighScore &&
    pendingInitials != nullptr
  ) {
    playVictorySound();
    animateYellowHighscores();
    animateNewHighscore();

    startInitialsEntry();
    return;
  }

  // ----- NORMAL GAME OVER ----- //

  playGameOverSound();
  animateGameOver();

  currentState = STATE_GAMEOVER;
}

  // C. REACTION TIME //
void runReactionGame() {

  currentState = STATE_PLAYING;
  currentGame = GAME_REACTION;

  unsigned long reactionTimes[
    REACTION_ROUNDS
  ] = {};

  // Prevent the menu-starting press from becoming
  // an immediate early Reaction press.
  waitForAllGameInputsReleased();
  consumeAllPressEvents();

  clearRgbLeds();

  for (
    int round = 0;
    round < REACTION_ROUNDS;
    round++
  ) {
    // ----- PREPARE ROUND ----- //

    drawReactionWaitScreen();

    clearRgbLeds();

    waitForAllGameInputsReleased();
    consumeAllPressEvents();

    unsigned long waitTime =
      random(500, 2001);

    unsigned long waitStart =
      millis();

    bool earlyPress = false;

    // ----- RANDOM WAIT PERIOD ----- //

    while (
      millis() - waitStart <
      waitTime
    ) {
      updateInputs();

      for (
        int fret = 0;
        fret < FRET_COUNT;
        fret++
      ) {
        if (input.fretPressed[fret]) {
          input.fretPressed[fret] = false;
          earlyPress = true;
          break;
        }
      }

      if (earlyPress) {
        break;
      }

      delay(1);
    }

    // ----- TOO EARLY ----- //

    if (earlyPress) {

      consumeAllPressEvents();
      clearRgbLeds();

      noTone(NOTE_BUZZER);
      noTone(FX_BUZZER);

      tft.fillScreen(COLOR_ERROR);

      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(2);

      int16_t x1;
      int16_t y1;
      uint16_t width;
      uint16_t height;

      tft.getTextBounds(
        "TOO FAST!",
        0,
        0,
        &x1,
        &y1,
        &width,
        &height
      );

      tft.setCursor(
        (SCREEN_WIDTH - width) / 2,
        30
      );

      tft.print("TOO FAST!");

      animateGameOver();
      playGameOverSound();

      delay(500);

      reactionLeaderboardOpen = false;
      currentState = STATE_REACTION_MENU;

      return;
    }

    // ----- START REACTION CUE ----- //

    int targetFret =
      random(0, FRET_COUNT);

    startReactionCue(targetFret);

    // Begin measuring after the cue is active.
    unsigned long reactionStart =
      millis();

    bool success = false;
    bool wrongFret = false;

    // ----- WAIT FOR PLAYER RESPONSE ----- //

    while (!success) {

      updateInputs();

      for (
        int fret = 0;
        fret < FRET_COUNT;
        fret++
      ) {
        if (!input.fretPressed[fret]) {
          continue;
        }

        // Consume the press.
        input.fretPressed[fret] = false;

        if (
          reactionIndex == 0 ||
          fret == targetFret
        ) {
          reactionTimes[round] =
            millis() - reactionStart;

          success = true;

        } else {
          wrongFret = true;
          success = true;
        }

        break;
      }

      delay(1);
    }

    clearRgbLeds();
    noTone(NOTE_BUZZER);

    // ----- WRONG FRET ----- //

    if (wrongFret) {

      tft.fillScreen(COLOR_ERROR);

      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(2);

      tft.setCursor(10, 20);
      tft.print("WRONG FRET");

      tft.setCursor(10, 50);
      tft.print("PRESS GREEN");

      waitForGreenPress();

      // Repeat this round.
      round--;
      continue;
    }

    // ----- SHOW ROUND RESULT ----- //

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

  // ----- CALCULATE AVERAGE ----- //

  unsigned long total = 0;

  for (
    int round = 0;
    round < REACTION_ROUNDS;
    round++
  ) {
    total += reactionTimes[round];
  }

  unsigned long average =
    total / REACTION_ROUNDS;

  consumeAllPressEvents();
  clearRgbLeds();

  // ----- HIGHSCORE PROCESSING ----- //

  newHighScore = false;
  pendingInitials = nullptr;

  int highScorePosition = -1;

  if (useLight) {

    highScorePosition =
      insertTopReactionTime(
        reactionHS.normal[reactionIndex],
        static_cast<int>(average)
      );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        reactionNormalInitials[
          reactionIndex
        ],
        highScorePosition
      );

      pendingInitials =
        reactionNormalInitials
          [reactionIndex]
          [highScorePosition];

      newHighScore = true;
    }

  } else {

    highScorePosition =
      insertTopReactionTime(
        reactionHS.noLight[reactionIndex],
        static_cast<int>(average)
      );

    if (highScorePosition >= 0) {

      prepareInitialsSlot(
        reactionNoLightInitials[
          reactionIndex
        ],
        highScorePosition
      );

      pendingInitials =
        reactionNoLightInitials
          [reactionIndex]
          [highScorePosition];

      newHighScore = true;
    }
  }

  lastScore =
    static_cast<int>(average);

  // ----- NEW HIGHSCORE ----- //

  if (
    newHighScore &&
    pendingInitials != nullptr
  ) {
    playVictorySound();
    animateYellowHighscores();
    animateNewHighscore();

    startInitialsEntry();
    return;
  }

  // ----- NORMAL GAME OVER ----- //

  playGameOverSound();
  animateGameOver();

  currentState = STATE_GAMEOVER;
}

  // D. SOLO-JAM      //
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

  clearRgbLeds();

  // Track previous fret states for press-edge detection
  bool previousFretState[FRET_COUNT];

  for (int fret = 0; fret < FRET_COUNT; fret++) {
    previousFretState[fret] =
      digitalRead(BUTTON_PINS[fret]);
  }

  while (true) {

    bool anyFretHeld = false;
    bool rgbChanged = false;

    // ----- FRET INPUTS ----- //

    for (int fret = 0; fret < FRET_COUNT; fret++) {

      bool currentFretState =
        digitalRead(BUTTON_PINS[fret]);

      bool fretHeld =
        currentFretState == LOW;

      bool fretJustPressed =
        currentFretState == LOW &&
        previousFretState[fret] == HIGH;

      anyFretHeld |= fretHeld;

      // Keep both RGB pixels illuminated while held
      if (fretHeld) {
        setFretColor(
          fret,
          FRET_RGB_COLORS[fret]
        );

        rgbChanged = true;
        lastActivity = millis();
      } else {
        setFretColor(
          fret,
          LED_COLOR_OFF
        );

      rgbChanged = true;
      }

      // Play the note only once when initially pressed
      if (fretJustPressed) {

        int note;

        if (soloIndex == 0) {
          note =
            SOLO_MINOR[soloRoot][fret];

        } else if (soloIndex == 1) {
          note =
            SOLO_MAJOR[soloRoot][fret];

        } else {
          note =
            SOLO_BLUES[soloRoot][fret];
        }

        playTone(
          NOTE_BUZZER,
          note,
          200
        );

        spawnNote(fret);
      }

      previousFretState[fret] =
        currentFretState;
    }

    // Send all fret RGB changes together
    if (rgbChanged) {
      rgbLeds.show();
    }

    // ----- STRUM-UP: NEXT ROOT ----- //

    if (strumUpPressed()) {

      lastActivity = millis();

      rgbLeds.setPixelColor(
        RGB_STRUM_UP,
        LED_COLOR_TURQUOISE
      );

      rgbLeds.show();

      soloRoot++;

      if (soloRoot > 5) {
        soloRoot = 0;
      }

      drawSoloRoot();

      delay(80);

      rgbLeds.setPixelColor(
        RGB_STRUM_UP,
        LED_COLOR_OFF
      );

      rgbLeds.show();
    }

    // ----- STRUM-DOWN: PREVIOUS ROOT ----- //

    if (strumDownPressed()) {

      lastActivity = millis();

      rgbLeds.setPixelColor(
        RGB_STRUM_DOWN,
        LED_COLOR_PINK
      );

      rgbLeds.show();

      soloRoot--;

      if (soloRoot < 0) {
        soloRoot = 5;
      }

      drawSoloRoot();

      delay(80);

      rgbLeds.setPixelColor(
        RGB_STRUM_DOWN,
        LED_COLOR_OFF
      );

      rgbLeds.show();
    }

    // ----- MANUAL EXIT COMBINATION ----- //

    if (
      digitalRead(BUTTON_PINS[1]) == LOW &&
      digitalRead(BUTTON_PINS[4]) == LOW &&
      digitalRead(STRUM_UP_PIN) == LOW
    ) {
      animateRedBack();
      playBackSound();

      clearRgbLeds();

      currentState = STATE_SOLO_MENU;
      return;
    }

    // ----- INACTIVITY EXIT ----- //

    if (
      !anyFretHeld &&
      millis() - lastActivity > 5000
    ) {
      animateRedBack();
      playBackSound();

      clearRgbLeds();

      currentState = STATE_SOLO_MENU;
      return;
    }

    // ----- MUSIC-NOTE ANIMATION ----- //

    tft.fillRect(
      0,
      18,
      SCREEN_WIDTH,
      SCREEN_HEIGHT - 18,
      COLOR_BG
    );

    updateNotes();
    cleanupNotes();
    drawNotes();

    delay(30);
  }
}

// ----- 15. ARDUINO ENTRY POINTS - HARDWARE TEST & BOOT SEQUENCE ----- //

void setup() {

  Serial.begin(115200);
  delay(500);

  // Fret buttons
  for (int i = 0; i < FRET_COUNT; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  // Strum buttons
  pinMode(STRUM_UP_PIN, INPUT_PULLUP);
  pinMode(STRUM_DOWN_PIN, INPUT_PULLUP);
  
  // Start & Select buttons
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);

  // Joystick axes
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  // Joystick center button
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);

  // Fret LEDs
  rgbLeds.begin();
  rgbLeds.setBrightness(RGB_BRIGHTNESS);
  clearRgbLeds();

  // Buzzers
  pinMode(NOTE_BUZZER, OUTPUT);
  pinMode(FX_BUZZER, OUTPUT);

  noTone(NOTE_BUZZER);
  noTone(FX_BUZZER);

  delay(20);

  Serial.println();
  Serial.println("POLYBAR ARCADE STARTING");
  Serial.println("Initializing score storage...");

  if (initializeScoreStorage()) {
    loadHighScores();
  } else {
    Serial.println("No valid highscore file loaded");
    resetHighScores();
  }

  // Optional HighScore Reset Combination red+yellow+orange
  if (
    digitalRead(BUTTON_PINS[0]) == LOW &&
    digitalRead(BUTTON_PINS[2]) == LOW &&
    digitalRead(BUTTON_PINS[4]) == LOW
  ) {
    resetHighScores();
    saveHighScores();

    Serial.println(
      "All highscores reset"
    );
  }

  // Optional Mute Combination red+blue
  if (
    digitalRead(BUTTON_PINS[1]) == LOW &&
    digitalRead(BUTTON_PINS[3]) == LOW  
  ) { 
    soundEnabled = false;
  
    noTone(NOTE_BUZZER);
    noTone(FX_BUZZER);
  }

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
  
  // Run the startup hardware test
  playStartupFretSequence();

  // Initialize state
  currentState = STATE_SPLASH;
  previousState = STATE_SPLASH;

  drawSplashScreen();
  Serial.println("Joystick and buttons ready");
}

void loop() {

  // Read all hardware inputs once
  updateInputs();

  // ----- DRAW SCREEN AFTER STATE CHANGE ----- //

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
        // Game functions draw their own screens.
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

  // ----- LIVE RGB INPUT FEEDBACK ----- //

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

  // ----- UPDATE CURRENT STATE ----- //

  switch (currentState) {

    case STATE_SPLASH: {

      bool anyInputPressed =
        input.strumUpPressed ||
        input.strumDownPressed ||
        input.startPressed ||
        input.selectPressed ||
        input.joystickPressed;

      for (int fret = 0; fret < FRET_COUNT; fret++) {
        if (input.fretPressed[fret]) {
          anyInputPressed = true;
          break;
        }
      }

      if (anyInputPressed) {

        // Consume all press events so the same input
        consumeAllPressEvents();

        playSelectSound();
        animateSplashExit();

        currentState = STATE_MAIN_MENU;
      }

      break;
    }

    case STATE_MAIN_MENU: {

      updateMainMenu();

      if (redPressed()) {
        playBackSound();
        animateRedBack();
        clearRgbLeds();

        currentState = STATE_SPLASH;
      }

      break;
    }

    case STATE_SPEEDTEST_MENU: {

      updateSpeedtestMenu();
      break;
    }

    case STATE_SIMON_MENU: {

      updateSimonMenu();
      break;
    }

    case STATE_REACTION_MENU: {

      updateReactionMenu();
      break;
    }

    case STATE_SOLO_MENU: {

      updateSoloMenu();
      break;
    }

    case STATE_PLAYING: {

      // Blocking game functions currently handle
      // their own input processing.
      break;
    }

    case STATE_GAMEOVER: {

      if (greenPressed()) {

        playSelectSound();
        animateGreenSelect();

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

      } else if (redPressed()) {

        playBackSound();
        animateRedBack();

        switch (currentGame) {

          case GAME_SPEEDTEST:
            currentState =
              STATE_SPEEDTEST_MENU;
            break;

          case GAME_SIMON:
            currentState =
              STATE_SIMON_MENU;
            break;

          case GAME_REACTION:
            currentState =
              STATE_REACTION_MENU;
            break;

          case GAME_SOLO:
            currentState =
              STATE_SOLO_MENU;
            break;
        }
      }

      break;
    }

    case STATE_ENTER_INITIALS: {

      updateInitialsEntry();
      break;
    }
  }

  delay(10);
}
