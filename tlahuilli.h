#define FIELDS       4      // four modifyable fields
#define BUTTONS      5      // five buttons: up, down, left, right and select
#define FALSE        0
#define TRUE         1
#define TSL_WAIT   104      // used in TSL delay function call it mean milliseconds
                            // this value worked fine in my meter
#define FLASH_WAIT 600      // sixty seconds in 100 ms units
#define NPI       0x20      // Non Persistent Interrupt flag on TSL2591 status register
#define FIELD_INC    1
#define FIELD_DEC   -1
#define VALUE_INC    1
#define VALUE_DEC   -1
#define NO_LABEL    -1

#define MAX_LABEL    5      // Chars allowed in screen element label
#define MAX_TEXT    10      // Chars allowed in screen text element

// Software SPI Arduino pins and use:
#define OLED_CLK     8
#define OLED_MOSI    9
#define OLED_RESET  10
#define OLED_DC     11
#define OLED_CS     12

// oled display colors
#define OLED_WHITE   1
#define OLED_BLACK   0

// position and size of battery indicator bitmap
#define BATT_X       2
#define BATT_Y       5
#define B_BYTES      2
#define B_ROWS       7

// mode label position and icon size, position is needed
// even if there is no label used here
#define MODE_LBL_X  30
#define MODE_LBL_Y   2
#define MODE_VAL_X  50
#define MODE_VAL_Y  MODE_LBL_Y

// Stop increments, 1, 1/2 or 1/3 step
#define S_LBL_X     80
#define S_LBL_Y      2
#define S_VAL_X     97
#define S_VAL_Y    S_LBL_Y

// Sensor speed, label and value
#define ISO_LBL_X    2
#define ISO_LBL_Y   20
#define ISO_VAL_X   30
#define ISO_VAL_Y   ISO_LBL_Y

// exposure value output, label and value
#define EV_LBL_X    80
#define EV_LBL_Y    20
#define EV_VAL_X    97
#define EV_VAL_Y    EV_LBL_Y

// F-stop label and value
#define F_LBL_X      2
#define F_LBL_Y     40
#define F_VAL_X     15
#define F_VAL_Y     F_LBL_Y

// Exposure time
#define T_LBL_X     62
#define T_LBL_Y     40
#define T_VAL_X     75
#define T_VAL_Y     T_LBL_Y

// TSL2591 gain settings and limits used to set correct gain
#define TSL2591_LOW_GAIN           0
#define TSL2591_MED_GAIN           1
#define TSL2591_HIGH_GAIN          2
#define TSL2591_MAX_GAIN           3
#define TSL2591_LOW_GAIN_LIMIT  1300
#define TSL2591_MED_GAIN_LIMIT    80
#define TSL2591_HIGH_GAIN_LIMIT    2

// Bit patterns used in bitmaps
#define XXXXXXXX  0xff
#define XXXXXXX_  0xfe
#define XXXXXX__  0xfc
#define XXXXX__X  0xf9
#define XXXXX___  0xf8
#define XXXX__XX  0xf3
#define XXXX___X  0xf1
#define XXXX____  0xf0
#define XXX_XXXX  0xef
#define XXX_XX__  0xec
#define XXX__XXX  0xe7
#define XXX___X_  0xe2
#define XXX____X  0xe1
#define XXX_____  0xe0
#define XX_XXXXX  0xdf
#define XX_XX___  0xd8
#define XX__XXXX  0xcf
#define XX__XX__  0xcc
#define XX___XXX  0xc7
#define XX___X__  0xc4
#define XX____XX  0xc3
#define XX_____X  0xc1
#define XX______  0xc0
#define X__XXXX_  0x9e
#define X___XXXX  0x8f
#define X____XXX  0x87
#define X____X__  0x84
#define X_____XX  0x83
#define X______X  0x81
#define X_______  0x80
#define _XXXXXXX  0x7f
#define _XXXXX__  0x7c
#define _XXXX__X  0x79
#define _XXXX___  0x78
#define _XXX____  0x70
#define _XX_XXXX  0x6f
#define _XX___XX  0x63
#define _XX_____  0x60
#define _X____XX  0x43
#define _X______  0x40
#define __XXXXXX  0x3f
#define __XXXXX_  0x3e
#define __XXXX__  0x3c
#define __XXX___  0x38
#define __XX_XXX  0x37
#define __XX____  0x30
#define __X_XXXX  0x2f
#define __X____X  0x21
#define __X_____  0x20
#define ___XXXXX  0x1f
#define ___XXXX_  0x1e
#define ___XXX__  0x1c
#define ___XX___  0x18
#define ___X__XX  0x13
#define ___X____  0x10
#define ____XXXX  0x0f
#define ____XXX_  0x0e
#define ____XX__  0x0c
#define ____X___  0x08
#define _____XXX  0x07
#define _____XX_  0x06
#define ______XX  0x03
#define _______X  0x01
#define ________  0x00

#define BATT_75    834
#define BATT_50    808
#define BATT_25    783
#define BATT_00    757

const PROGMEM unsigned char batteryFull[] = {
  __XXXXXX, XXXXXXXX,
  __X_____, _______X,
  XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX,
  XXXXXXXX, XXXXXXXX,
  __X_____, _______X,
  __XXXXXX, XXXXXXXX
};

const PROGMEM unsigned char battery75[] = {
  __XXXXXX, XXXXXXXX,
  __X_____, _______X,
  XX___XXX, XXXXXXXX,
  X___XXXX, XXXXXXXX,
  XX_XXXXX, XXXXXXXX,
  __X_____, _______X,
  __XXXXXX, XXXXXXXX
};

const PROGMEM unsigned char battery50[] = {
  __XXXXXX, XXXXXXXX,
  __X_____, _______X,
  XX______, _XXXXXXX,
  X_______, XXXXXXXX,
  XX_____X, XXXXXXXX,
  __X_____, _______X,
  __XXXXXX, XXXXXXXX
};

const PROGMEM unsigned char battery25[] = {
  __XXXXXX, XXXXXXXX,
  __X_____, _______X,
  XX______, _____XXX,
  X_______, ____XXXX,
  XX______, ___XXXXX,
  __X_____, _______X,
  __XXXXXX, XXXXXXXX
};

const PROGMEM unsigned char battery00[] = {
  __XXXXXX, XXXXXXXX,
  __X_____, _______X,
  X_______, _______X,
  X_______, _______X,
  X_______, _______X,
  __X_____, _______X,
  __XXXXXX, XXXXXXXX
};

const PROGMEM unsigned char modeAmbient[] = {

  _______X, X_______,
  __X____X, X____X__,
  __XX_XXX, XXX_XX__,
  ___XX___, ___XX___,
  _XXX____, ____XXX_,
  _XXX____, ____XXX_,
  ___XX___, ___XX___,
  __XX_XXX, XXX_XX__,
  __X____X, X____X__,
  _______X, X_______
};

const PROGMEM unsigned char modeFlash[] = {

  ________, XXXX____,
  _______X, XXX_____,
  ______XX, XX______,
  _____XXX, X_______,
  ____XXXX, XXXX____,
  ___XXXXX, XXX_____,
  _____XXX, X_______,
  ____XXXX, ________,
  ___XXX__, ________,
  __X_____, ________
};
const PROGMEM unsigned char modeIR[] = {

  ________, ________,
  XXXX__XX, XXXXXXX_,
  _XX___XX, ______XX,
  _XX___XX, ______XX,
  _XX___XX, ______XX,
  _XX___XX, XXXXXXX_,
  _XX___XX, _XXX____,
  _XX___XX, __XXX___,
  _XX___XX, ___XXX__,
  XXXX__XX, ____XXX_
};

// Splash screen is project name "Tlahuilli"
const PROGMEM unsigned char splash[24][16] = {
  ________, ________, _____XXX, ________, ________, ________, _XXXX___, ________, ________, ________, ______XX, X_______, XXX_____, __XXX___, ____XXX_, ________,
  ________, ________, __XXXXXX, ________, ________, ______XX, XXXXX___, ________, ________, ________, ______XX, XX___XXX, XXX____X, XXXXX___, ____XXXX, ________,
  ________, ________, ___XXXXX, ________, ________, _______X, XXXX____, ________, ________, ________, _____XXX, XX____XX, XXX_____, XXXXX___, ___XXXXX, ________,
  ________, ________, ____XXXX, ________, ________, ________, _XXX____, ________, ________, ________, ______XX, X______X, XXX_____, _XXXX___, ____XXX_, ________,
  ________, ___XX___, ____XXXX, ________, ________, ________, XXXX____, ________, ________, ________, ________, _______X, XXX_____, _XXXX___, ________, ________,
  ________, __XXX___, ____XXX_, ________, ________, ________, XXXX____, ________, ________, ________, ________, _______X, XX______, _XXX____, ________, ________,
  ________, __XXX___, ____XXX_, ________, ________, ________, XXXX____, ________, ________, ________, ________, _______X, XX______, _XXX____, ________, ________,
  ________, _XXX____, ___XXXX_, ________, ________, ________, XXX_____, ________, ________, ________, ________, ______XX, XX______, XXXX____, ________, ________,
  ________, XXXX____, ___XXXX_, ________, _XXXXXXX, XX______, XXX____X, XX______, __XXX___, ___XXX__, _____XXX, ______XX, XX______, XXXX____, ___XXX__, ________,
  ______XX, XXXXXXXX, ___XXXX_, _______X, XX__XXXX, XX_____X, XXX__XXX, XXX____X, XXXXX___, ___XXX__, __XXXXXX, ______XX, XX______, XXXX____, XXXXXX__, ________,
  ________, XXXX____, ___XXXX_, ______XX, X____XXX, X______X, XXX_XXXX, XXXX____, _XXXX___, __XXXX__, ___XXXXX, ______XX, XX______, XXXX____, _XXXXX__, ________,
  ________, XXXX____, ___XXX__, _____XXX, _____XXX, X______X, XXXXX__X, XXXX____, _XXXX___, __XXXX__, ____XXXX, ______XX, X_______, XXX_____, __XXXX__, ________,
  ________, XXX_____, ___XXX__, ____XXXX, _____XXX, X______X, XXX_____, XXXX____, _XXXX___, __XXXX__, ____XXXX, ______XX, X_______, XXX_____, __XXXX__, ________,
  ________, XXX_____, __XXXX__, ____XXX_, _____XXX, X______X, XX______, XXX_____, _XXX____, __XXX___, ____XXXX, _____XXX, X______X, XXX_____, __XXXX__, ________,
  _______X, XXX_____, __XXXX__, ___XXXX_, _____XXX, X_____XX, XX______, XXX_____, _XXX____, __XXX___, ____XXX_, _____XXX, X______X, XXX_____, __XXX___, ________,
  _______X, XXX_____, __XXXX__, ___XXXX_, _____XXX, ______XX, XX_____X, XXX_____, XXXX____, _XXXX___, ____XXX_, _____XXX, X______X, XXX_____, __XXX___, ________,
  _______X, XXX_____, __XXX___, ___XXXX_, _____XXX, ______XX, XX_____X, XXX_____, XXXX____, _XXXX___, ___XXXX_, _____XXX, _______X, XX______, _XXXX___, ________,
  _______X, XXX_____, __XXX___, ___XXXX_, _____XXX, ______XX, XX_____X, XXX_____, XXXX____, _XXXX___, ___XXXX_, _____XXX, _______X, XX______, _XXXX___, ________,
  _______X, XX______, __XXX___, ___XXXX_, ____XXXX, ______XX, X______X, XX______, XXX_____, _XXXX___, ___XXXX_, _____XXX, _______X, XX______, _XXXX___, ________,
  _______X, XX______, _XXXX___, ___XXXX_, ____XXXX, _____XXX, X______X, XX______, XXX_____, _XXX____, ___XXX__, ____XXXX, ______XX, XX______, _XXX____, ________,
  _______X, XX______, _XXXX___, ___XXXX_, ____XXXX, _____XXX, X_____XX, XX______, XXXX___X, XXXX____, ___XXX__, ____XXXX, ______XX, XX______, _XXX____, ________,
  _______X, XXX___X_, _XXXX___, X__XXXX_, ____XXXX, _____XXX, X_____XX, XX______, XXXXXXXX, _XXXX__X, __XXXXX_, ____XXXX, ___X__XX, XX___X__, XXXXX___, ________,
  _______X, XXXXXXX_, _XXXXXXX, X___XXXX, __XXXXXX, _____XXX, X_____XX, XXXXX___, XXXXXX__, _XXXXXXX, ___XXXXX, XX__XXXX, XXXX__XX, XXXXXX__, _XXXXXXX, ________,
  ________, XXXX____, __XXXX__, _____XXX, XX__XXXX, XXX__XXX, X______X, XXXX____, _XXXX___, _XXXX___, ___XXXXX, _____XXX, X______X, XXX_____, _XXXXX__, ________
};

struct s_Step {
  int index;
  int maximum = 3;
};

#define MODE_AMBIENT 0
#define MODE_FLASH   1
#define MODE_IR      2
struct s_Mode {
  int index;
  int maximum = 3;
};

// ISO values go from 50 to 64000
struct s_ISO {
  int index[3] = {0, 0, 0};
  int maximum[3] = {11, 21, 31};
};

// Exposure time go from 30 sec to 1/8000
struct s_TimeExposure {
  int index[3] = {0, 0, 0};
  int maximum[3] = {19, 37, 55};
};

// Aperture go from 1 to 64
struct s_Aperture {
  int index[3] = {0, 0, 0};
  int maximum[3] = {13, 25, 36};
};

// index arrays used to point to values from a single array based on full,
// half or third stops
unsigned short int fullStep[19] =
{
  0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72
};
unsigned short int halfStep[37] =
{ 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
  42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72
};
unsigned short int thirdStep[55] =
{ 0, 1, 3, 4, 5, 7, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20, 21, 23, 24, 25, 27, 28,
  29, 31, 32, 33, 35, 36, 37, 39, 40, 41, 43, 44, 45, 47, 48, 49, 51, 52, 53, 55,
  56, 57, 59, 60, 61, 63, 64, 65, 67, 68, 69, 71, 72
};

unsigned short int *stepIndexTable[] = {fullStep, halfStep, thirdStep};

char *StepSize[] = {"1", "1/2", "1/3"};


#define APERTURE_INTEGER   27
const unsigned short int ApertureValues[49] PROGMEM =
{ 10, 11, 12, 13, 14, 16, 17, 18, 20, 22, 24, 25, 28, 32,   // Multiplier = 0.1 (0-26)
  33, 35, 40, 45, 48, 50, 56, 63, 67, 71, 80, 90, 95, 10,   // Multiplier = 1   (27-48)
  11, 12, 13, 14, 16, 18, 19, 20, 22, 26, 27, 28, 32, 36,
  38, 40, 45, 52, 54, 57, 64
};

#define ISO_MULT_10_LOW    10
#define ISO_MULT_10_HIGH   23
#define ISO_MULT_100_LOW   24
#define ISO_MULT_100_HIGH  35
#define ISO_MULT_400_LOW   37
#define ISO_MULT_400_HIGH  40
const unsigned short int ISOValues[41] PROGMEM =
{ 50, 64, 70, 80, 100, 125, 140, 160, 200, 250,               // Multiplier = 1   (0-9)
  28, 32, 40, 50, 56, 64, 80, 100, 110, 125, 160, 200, 220, 250,// Multiplier = 10  (10-23)
  32, 40, 44, 50, 64, 80, 88, 100, 128, 160, 176, 200,        // Multiplier = 100 (24-35)
  64, 80, 90, 100, 128                                        // Multiplier = 400 (37-40)
};

#define TE_MULT_01_LOW     13
#define TE_MULT_01_HIGH    27
#define TE_MULT_10_LOW     53
#define TE_MULT_10_HIGH    65
#define TE_MULT_100_LOW    66
#define TE_MULT_100_HIGH   72
const unsigned short int TEValues[73] PROGMEM =
{ 30, 25, 22, 20, 15, 13, 12, 10, 8, 7, 6, 5, 4,              // Multiplier = 1   (0-12)
  35, 30, 25, 20, 17, 15, 13, 10, 13, 15, 17, 20, 25, 30, 35, // Multiplier = 0.1 (13-27)
  4, 5, 6, 7, 8, 10, 12, 13, 15, 20, 22, 25, 30,              // Multiplier = 1   (28-52)
  40, 45, 50, 60, 80, 90, 100, 125, 160, 180, 200, 250,       //
  32, 36, 40, 50, 64, 75, 80, 100, 125, 150, 160, 200, 250,   // Multiplier by 10 (53-65)
  30, 32, 40, 50, 60,  64, 80                                 // Multiplier by 100 (66-72)
};
