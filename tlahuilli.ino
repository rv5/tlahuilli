#include <avr/pgmspace.h>
#include "tlahuilli.h"
#include <Wire.h>
#include <Button.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>
#include <SPI.h>
#include <U8glib.h>
#include <EEPROM.h>

// push button related parameters
long debounceDelay = 30;                          // the debounce time (ms); increase if the output flickers
int buttonState[BUTTONS] = {1, 1, 1, 1, 1};       // the current reading from the input pin
int lastButtonState[BUTTONS] = {1, 1, 1, 1, 1};   // the previous reading from the input pin
long lastDebounceTime[BUTTONS] = {0, 0, 0, 0, 0};
int buttonPin[BUTTONS] = {3, 4, 5, 6, 7};

int currentField;
int screenRedraw = TRUE;
struct s_Mode Mode;
struct s_ISO ISO;
struct s_Step Step;
struct s_TimeExposure TimeExposure;
struct s_Aperture Aperture;
int *currentIndex;
int *currentMaximum;
float currentISO;
float currentExpTime;
float currentFstop;
float coef = 400.0 * 6.82 / 264.1; // to convert to lux from counts at gain 400
const char **currentLabel;
unsigned short int *currentIndexTable;
char buf[10];

unsigned long lux;
int batteryVoltage;
short int validData;
short int tslErrorFlag;    // set to TRUE when there is a problem detecting TSL2591
short int invers;          // flag to indicate time exposure is a fraction of a second, stored values are integers

U8GLIB_SH1106_128X64 u8g(OLED_CLK, OLED_MOSI, OLED_CS, OLED_DC, OLED_RESET);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

enum {incrementValue = 0, readLight, decrementCommand, decrementValue, incrementCommand};
enum {modeField = 0, stepField, isoField, timeExposureField};

class DisplayText
{
  private:
    short int labelX, valueX;
    short int labelY, valueY;
    char textLabel[MAX_LABEL];
    char textText[MAX_TEXT];
    short int selected = FALSE;

  public:
    virtual void initObject(short int coordLabelX, short int coordLabelY, short int coordValueX, short int coordValueY);
    void setLabel(const char *label);
    void setText(const char *text);
    virtual void drawObject(void);
    virtual void Select(void);
    virtual void deSelect(void);
};

class DisplayBitmap : public DisplayText
{
  private:
    short int labelX, valueX;
    short int labelY, valueY;
    char textLabel[MAX_LABEL];
    char *bitmap;
    short int count_bytes, count_rows;
    short int selected = FALSE;

  public:
    virtual void initObject(short int coordLabelX, short int coordLabelY, short int coordValueX, short int coordValueY,
                            short int count, short int rows);
    virtual void drawObject();
    void setBitmap(char *ptr);
    virtual void Select(void);
    virtual void deSelect(void);
};

void DisplayBitmap::setBitmap(char *ptr)
{
  bitmap = ptr;
}

void DisplayText::deSelect(void)
{
  selected = FALSE;
}

void DisplayText::Select(void)
{
  selected = TRUE;
}
void DisplayBitmap::deSelect(void)
{
  selected = FALSE;
}

void DisplayBitmap::Select(void)
{
  selected = TRUE;
}

void DisplayText::initObject(short int coordLabelX, short int coordLabelY, short int coordValueX, short int coordValueY)
{
  labelX = coordLabelX;
  labelY = coordLabelY;
  valueX = coordValueX;
  valueY = coordValueY;
}

void DisplayText::setLabel(const char *label)
{
  strcpy(textLabel, (const char*) label);
}

void DisplayText::setText(const char *text)
{
  int i;
  for (i = 0; i < strlen(text); i++)
    if ( text[i] != ' ') break;
  strcpy(textText, (const char*) &text[i]);
}

void DisplayText::drawObject(void)
{
  uint8_t i, h;
  u8g_uint_t w, d;
  h = u8g.getFontAscent() - u8g.getFontDescent();
  w = u8g.getStrWidth(textLabel) + u8g.getStrWidth(textText);
  if (selected == TRUE)
  {
    u8g.drawRBox(labelX - 2, labelY - 2, w + 4, h + 4, 4);
    u8g.setColorIndex(OLED_BLACK);
  }
  u8g.drawStr(labelX, labelY, textLabel);
  u8g.drawStr(valueX, valueY, (const char *) textText);
  u8g.setColorIndex(OLED_WHITE);
}

void DisplayBitmap::initObject(short int coordLabelX, short int coordLabelY, short int coordValueX, \
                               short int coordValueY, short int count, short int rows)
{
  labelX = coordLabelX;
  labelY = coordLabelY;
  valueX = coordValueX;
  valueY = coordValueY;
  count_bytes = count;
  count_rows = rows;
}

void DisplayBitmap::drawObject()
{
  if (selected == TRUE)
  {
    u8g.setColorIndex(OLED_WHITE);
    u8g.drawRBox(valueX - 2, valueY - 2, (count_bytes * 8) + 4, count_rows + 4, 4 );
    u8g.setColorIndex(OLED_BLACK);
  }
  if ( labelX != NO_LABEL )
    u8g.drawStr(labelX, labelY, textLabel);
  u8g.drawBitmapP(valueX, valueY, count_bytes, count_rows, (const u8g_pgm_uint8_t *)  bitmap);
  u8g.setColorIndex(OLED_WHITE);
}

// screen objects
DisplayBitmap battery;
DisplayBitmap mode;
DisplayText iso;
DisplayText stepSize;
DisplayText EV;
DisplayText eTime;
DisplayText fStop;


/*
 * Configure gain for the TSL2591, integration time is fixed to 100ms
 * g must be an integer in the range of 0 - 3
 */
void configureSensor(int g)
{
  // set new gain to adapt to brighter/dimmer light situations
  if ( g == TSL2591_LOW_GAIN )
    tsl.setGain(TSL2591_GAIN_LOW);               // 1x gain (bright light)
  else if ( g == TSL2591_MED_GAIN)
    tsl.setGain(TSL2591_GAIN_MED);               // 25x gain (most situations)
  else if  (g == TSL2591_HIGH_GAIN )
    tsl.setGain(TSL2591_GAIN_HIGH);              // 438x gain (low light condition)
  else if  (g == TSL2591_MAX_GAIN )
    tsl.setGain(TSL2591_GAIN_MAX);               // 10074x gain (ultra low light condition)

  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // Always use shortest integration time
}


/*
 * Read IR and Full Spectrum counters
 */
inline void readLuminosity(uint16_t *full, uint16_t *ir)
{
  // wait 100 ms and get counter0 and counter1
  tsl.enable();
  delay(TSL_WAIT);
  *ir = tsl.read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW);
  *full = tsl.read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW);
  tsl.disable();
}

/*
 * Draw all elemts on screen, as of today Dec/21/2016 include:
 * - battery charge
 * - measuring mode {ambient, flash, IR}
 * - sensor step increment {1, 1/2, 1/3}
 * - sensor sensitivity ISO {50 - 51200}
 * - Exposure value result
 * - F-stop result
 * - exposure time set {30sec to 1/8000}
 */
void drawScreen(void) {

  u8g.firstPage();
  do {
    battery.drawObject();    // Battery bitmap
    mode.drawObject();       // Mode bitmap

    // Set font coord position
    u8g.setFont(u8g_font_7x14B);
    u8g.setFontPosTop();
    stepSize.drawObject();   // Step text
    iso.drawObject();        // Next ISO speed
    EV.drawObject();         // Exposure Value

    // Use larger font for F-stop and Exposure time
    u8g.setFont(u8g_font_helvB12);
    u8g.setFontPosTop();
    fStop.drawObject();
    eTime.drawObject();      // Exposure time
  } while ( u8g.nextPage() );
}

/*
 * Save current status to EEPROM memory, called after light is sampled
 */
void saveStatus(void)
{
  EEPROM.write(0, (byte) Step.index);
  EEPROM.write(1, (byte) Mode.index);
  EEPROM.write(2, (byte) ISO.index[0]);
  EEPROM.write(3, (byte) ISO.index[1]);
  EEPROM.write(4, (byte) ISO.index[2]);
  EEPROM.write(5, (byte) TimeExposure.index[0]);
  EEPROM.write(6, (byte) TimeExposure.index[1]);
  EEPROM.write(7, (byte) TimeExposure.index[2]);
}

/*
 * Restore status saved in EEPROM
 */
void restoreStatus(void)
{
  if ( (Step.index = (int) EEPROM.read(0)) == 255 ) Step.index = 0;
  if ( (Mode.index = (int) EEPROM.read(1)) == 255 ) Mode.index = 0;
  if ( (ISO.index[0] = (int) EEPROM.read(2)) == 255 ) ISO.index[0] = 0;
  if ( (ISO.index[1] = (int) EEPROM.read(3)) == 255 ) ISO.index[1] = 0;
  if ( (ISO.index[2] = (int) EEPROM.read(4)) == 255 ) ISO.index[2] = 0;
  if ( (TimeExposure.index[0] = (int) EEPROM.read(5)) == 255 ) TimeExposure.index[0] = 0;
  if ( (TimeExposure.index[1] = (int) EEPROM.read(6)) == 255 ) TimeExposure.index[1] = 0;
  if ( (TimeExposure.index[2] = (int) EEPROM.read(7)) == 255 ) TimeExposure.index[1] = 0;
}

/*
 * Set bitmap that show battery charge left, 100%, 75%, 50%, 25% and 0%
 */
void batteryBitmap() {
  {
    screenRedraw = TRUE;
    if ( batteryVoltage > BATT_75 )
      battery.setBitmap((char *)batteryFull);
    else if ( batteryVoltage <= BATT_75 && batteryVoltage > BATT_50 )
      battery.setBitmap((char *)battery75);
    else if ( batteryVoltage <= BATT_50 && batteryVoltage > BATT_25 )
      battery.setBitmap((char *)battery50);
    else if ( batteryVoltage <= BATT_25 && batteryVoltage > BATT_00 )
      battery.setBitmap((char *)battery25);
    else
      battery.setBitmap((char *)battery00);
  }
}

/*
 * Set mode bitmap to ambient, flash or Infra Red
 */
void modeBitmap(int newMode) {

  screenRedraw = TRUE;
  if ( newMode == MODE_AMBIENT )
    mode.setBitmap((char *) modeAmbient);
  else if ( newMode == MODE_FLASH )
    mode.setBitmap((char *) modeFlash);
  else
    mode.setBitmap((char *) modeIR);
}

/*
 * To save flash memory, each time exposure, ISO and fstop values are saved in one byte each, there is a
 * scale factor that change every time saved value is larger than 255
 */
inline float scaleTimeExposure(int index) {
  float retval;

  // First 20 time values are in seconds, after that are fractions of a second, set flag
  invers = FALSE;
  if (index > 20)
    invers = TRUE;

  // only one byte is used to save each elemtnt in array
  retval = (float) pgm_read_byte(&TEValues[index]);

  // Scale value, index will tell common multiplying factor
  if ( index >= TE_MULT_01_LOW && index <= TE_MULT_01_HIGH )
    retval /= 10.0;
  else if ( index >= TE_MULT_10_LOW && index <= TE_MULT_10_HIGH )
    retval *= 10.0;
  else if ( index >= TE_MULT_100_LOW && index <= TE_MULT_100_HIGH )
    retval *= 100.0;
  return retval;
}

// Like exposure time, same scaling method is used for ISO
inline float scaleISO(int index) {
  float retval;

  retval = (float) pgm_read_byte(&ISOValues[index]);

  if ( index >= ISO_MULT_10_LOW && index <= ISO_MULT_10_HIGH )
    retval *= 10.0;
  else if ( index >= ISO_MULT_100_LOW && index <= ISO_MULT_100_HIGH )
    retval *= 100.0;
  else if ( index >= ISO_MULT_400_LOW && index <= ISO_MULT_400_HIGH)
    retval *= 400.0;
  return retval;
}

// Like exposure time and ISO, same scaling method is used for lens aperture
inline float scaleAperture(int index) {
  float retval;

  retval = (float) pgm_read_byte(&ApertureValues[index]);

  // First 26 values are multiplied by 10 to fit them in one byte and save program memory
  return index < APERTURE_INTEGER ? retval / 10. : retval;
}

/*
 * First method called from Arduino main program, init electronics, display, etc.
 */
void setup(void)
{
  int button;
  uint16_t value;
  int index;
  Serial.begin(9600);             //in case it is needed

  // Init display
  u8g.begin();

  validData = FALSE;             // there is no data availabe just yet

  tslErrorFlag = TRUE;           // If no sensor is present, use fstop to display an error
  if (tsl.begin())
  {
    tslErrorFlag = FALSE;        // TSL sensor found, change flag and set gain to 1.0
    configureSensor(TSL2591_LOW_GAIN);
  }

  // configure buttons input ports with pull up resistors
  for (button = 0; button < BUTTONS; button++)
    pinMode(buttonPin[button], INPUT_PULLUP);

  // Splash screen is project name "tlahuilli" as bitmap to save program memory
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_7x14B);
    u8g.setFontPosTop();
    u8g.setColorIndex(OLED_WHITE);
    u8g.drawBitmapP(2, 20, 16, 24, (const u8g_pgm_uint8_t *)  splash);
  } while ( u8g.nextPage() );

  // set flag to not display anything else, then wait 2 seconds before erase spash screen
  screenRedraw = FALSE;

  // wait 2 seconds
  delay(2000);

  // If status was saved to eeprom, restore it
  restoreStatus();

  // initialize indexes, start with ISO field selected
  currentField = isoField;
  currentIndexTable = stepIndexTable[Step.index];
  currentIndex = &ISO.index[Step.index];
  currentMaximum = &ISO.maximum[Step.index];

  // Initialize battery, channel 0 ADC, it is a two 3.7 volt battery, charged go up to
  // 8.4V, this exceed input voltage in microcontroller, a simple 2 same value resistors
  // in series divide voltage in two, so it goes from 3.7 to 4.2V. Set apropiate bit map
  pinMode( A0, INPUT);
  batteryVoltage = analogRead(0);
  battery.initObject(-1, -1, 2, 5, 2, 7);
  batteryBitmap();

  // Init mode to EEPROM saved one
  mode.initObject(MODE_LBL_X, MODE_LBL_Y, MODE_VAL_X, MODE_VAL_Y, 2, 10);
  modeBitmap(Mode.index);

  // Init stop increments for ISO, Exposure time and F-stop, may be 1, 1/2 or 1/3
  stepSize.initObject(S_LBL_X, S_LBL_Y, S_VAL_X, S_VAL_Y);
  stepSize.setLabel("St ");
  stepSize.setText(StepSize[Step.index]);

  // ISO value
  iso.initObject(ISO_LBL_X, ISO_LBL_Y, ISO_VAL_X, ISO_VAL_Y);
  iso.setLabel("ISO ");
  currentISO = scaleISO(currentIndexTable[ISO.index[Step.index]]);
  dtostrf(currentISO + 0.001, 8, 1, buf);
  if ( buf[7] == '0' ) buf[6] = '\0';    // if last digit is zero, remove it
  iso.setText(buf);
  iso.Select();

  // init EV calculated value, we have not taken a sample, so it is empty until then
  EV.initObject(EV_LBL_X, EV_LBL_Y, EV_VAL_X, EV_VAL_Y);
  EV.setLabel("EV");
  EV.setText("");

  // set exposure time
  eTime.initObject(T_LBL_X, T_LBL_Y, T_VAL_X, T_VAL_Y);
  eTime.setLabel("T ");
  currentExpTime = scaleTimeExposure(currentIndexTable[TimeExposure.index[Step.index]]);
  dtostrf(currentExpTime + 0.001, 7, 1, &buf[2]);
  buf[0] = buf[1] = ' ';
  if ( buf[8] == '0' ) buf[7] = '\0';
  for (index = 0; index < strlen(buf); index++)
    if (buf[index] != ' ') break;
  if ( invers )
  {
    buf[index - 2] = '1';
    buf[index - 1] = '/';
    currentExpTime = 1.0 / currentExpTime;
  }
  eTime.setText(buf);

  // F-stop calculated value
  fStop.initObject(F_LBL_X, F_LBL_Y, F_VAL_X, F_VAL_Y);
  fStop.setLabel("F");
  fStop.setText("  ");
  // if flag is set, set text to "LS E" (Light Sensor Error) to tell user tsl2591 was not found
  if ( tslErrorFlag ) fStop.setText("LS E");
  screenRedraw = TRUE;
}

/*
 * Read switch and debounce debouncTime ms as defined above
 */
int readSwitches(void)
{
  int reading;
  int index;
  int returnValue = -1;

  for (index = 0; index < BUTTONS; index++) {
    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:
    reading = digitalRead(buttonPin[index]);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState[index]) {
      // reset the debouncing timer
      lastDebounceTime[index] = millis();
    }

    if ((millis() - lastDebounceTime[index]) > debounceDelay) {
      // if the button state has changed:
      if (reading != buttonState[index]) {
        buttonState[index] = reading;
        if ( buttonState[index] == 1 ) returnValue = index;
      }
    }
    lastButtonState[index] = reading;
  }
  return returnValue;
}

/*
 * cycle thru field values, if there is a valid lux read, then modifying ISO
 * and/or exposure time, recalculate aperture
 */
void changeValue(int dir) {
  float timeE;
  int index;
  short int updateFlag = 0;

  // first make this a circular list
  *currentIndex += dir;
  if ( *currentIndex == *currentMaximum )
    *currentIndex = 0;
  if ( *currentIndex < 0 )
    *currentIndex = *currentMaximum - 1;

  // if selected field is mode, set new bitmap and flag screen redraw
  if (currentField == modeField)
  {
    screenRedraw = TRUE;
    modeBitmap(*currentIndex);
  }

  // if current field is step, change the increment and flag redraw
  if (currentField == stepField)
  {
    screenRedraw = TRUE;
    stepSize.setText(StepSize[*currentIndex]);
    updateFlag = TRUE;
  }

  // set new ISO value and if there is a valid data available, call aperture to update
  // it with new ISO
  if (currentField == isoField || updateFlag )
  {
    screenRedraw = TRUE;
    currentIndexTable = stepIndexTable[Step.index];
    currentISO = scaleISO(currentIndexTable[ISO.index[Step.index]]);
    dtostrf(currentISO + 0.001, 7, 1, buf);
    if ( buf[6] == '0' ) buf[5] = '\0';
    iso.setText(buf);
    if ( validData ) aperture();
  }

  // Set new exposure time, if there is a valid data available and meter is not in flash mode,
  // call aperture to update fstop with new exposure
  if (currentField == timeExposureField || updateFlag ) {
    screenRedraw = TRUE;
    invers = FALSE;
    currentIndexTable = stepIndexTable[Step.index];
    currentExpTime = scaleTimeExposure(currentIndexTable[TimeExposure.index[Step.index]]);
    dtostrf(currentExpTime + 0.001, 7, 1, &buf[2]);
    buf[0] = buf[1] = ' ';
    if ( buf[8] == '0' ) buf[7] = '\0';
    for (index = 0; index < strlen(buf); index++)
      if (buf[index] != ' ') break;
    if ( invers )
    {
      buf[index - 2] = '1';
      buf[index - 1] = '/';
      currentExpTime = 1.0 / currentExpTime;
    }
    eTime.setText(buf);

    // If there is valid sample for ambient and IR modes, recalculate aperture using new
    // exposure time, display spaces if mode is flash and new exposure time is given.
    // It does not make sense to recalculate aperture under those circumstances
    if ( validData && Mode.index != 1 )
      aperture();
    else {
      fStop.setText("    ");
      EV.setText("    ");
      drawScreen();
      validData = FALSE;  // invalidate data and if wanted take a new flash measurement
    }
  }
}

/*
 * Move cursor to next field to modify its value, current field is shown in reverse video
 */
int changeField(int dir) {
  if (currentField == modeField)
    mode.deSelect();
  else if (currentField == stepField)
    stepSize.deSelect();
  else if (currentField == isoField)
    iso.deSelect();
  else
    eTime.deSelect();

  currentField += dir;
  screenRedraw = TRUE;
  if ( currentField == FIELDS) currentField = 0;
  if ( currentField < 0) currentField = 3;

  if (currentField == modeField) {
    currentIndex = &Mode.index;
    currentMaximum = &Mode.maximum;
    mode.Select();
  }
  else if (currentField == stepField) {
    currentIndex = &Step.index;
    currentMaximum = &Step.maximum;
    stepSize.Select();
  }
  else if (currentField == isoField) {
    currentIndex = &ISO.index[Step.index];
    currentMaximum = &ISO.maximum[Step.index];
    iso.Select();
  }
  else if (currentField == timeExposureField) {
    currentIndex = &TimeExposure.index[Step.index];
    currentMaximum = &TimeExposure.maximum[Step.index];
    eTime.Select();
  }
}

/*
 * Take current lux read and calculate F-stop given an ISO and time exposure settings
 */
void aperture() {
  float lumisphere = 3.3;
  float constant = 320.0; // like the one used by Sekonic
  float fstop, fs, fsPrev, apertureTmp;
  float Ev;
  short int srch;
  unsigned short int *indexTable;

  Ev = (float) lux * lumisphere * currentISO / constant;

  fstop = sqrt( Ev * currentExpTime );
  Ev = log10(Ev) / log10(2.0);
  dtostrf(Ev, 4, 1, buf);
  EV.setText(buf);
  screenRedraw = TRUE;
  /*
   * First test if F-stop is out of limits, let user know if true.
   * If fstop is less than 1, then display "< 1" also check if it is larger
   * than 64 and show ">64"
  */
  if ( fstop < 1.0 || fstop > 64.0 ) {
    fstop < 1.0 ? fStop.setText("< 1") : fStop.setText("> 64");
    return;
  }

  /*
   * make a linear search on F-stop array to find the standard values above and
   * below calculates one, then check which one of this two is closest to current
   */
  indexTable = stepIndexTable[Step.index];
  screenRedraw = TRUE;

  for (srch = 1, fsPrev = 1.0; srch < Aperture.maximum[Step.index]; srch++) {
    fs = scaleAperture(indexTable[srch]);
    if (fstop > fsPrev && fstop <= fs ) {
      if (fstop > sqrt(fs * fsPrev))
        dtostrf(fs + 0.001, 4, 1, buf);
      else
        dtostrf(fsPrev + 0.001, 4, 1, buf);

      if ( buf[3] == '0' ) buf[2] = '\0';
      fStop.setText(buf);
    }
    fsPrev = fs;
  }
}


/*
 * Autorange light read, first do a reading with gain 1 and use it to set gain for
 * real calculations
 */
int getExposure(void) {
  short int i, j;
  int k;
  long int average;
  uint16_t fullSample, irSample;
  uint16_t fullBefore, fullAfter, irBefore, irAfter, fullFlashMinus2, irFlashMinus2;
  uint16_t fullFlash, irFlash;
  float irAtten = 1.67, sensorGain, cpl, atime = 100.0, irSensitivity = 3.16;
  /*
   * uncomment following block to print 5 samples at each gain to verify gain values.
   * a value of 37888 mean saturation, reduce light intensity and restest, may need
   * different intensities to get them all
   */
  /*k = 0;
  configureSensor(0);
  for (j = 0; j < 4; j++) {
    for (i = 0; i < 5; i++) {
      readLuminosity(&fullSample, &irSample);
      Serial.print(fullSample, DEC);
      Serial.print(" ");
    }
    Serial.print("\n");
    configureSensor(++k);
    readLuminosity(&fullSample, &irSample);
  }*/

  /*
   * save current status to EEPROM, next time power is applied
   * settings will be restored
   */
  saveStatus();
  /*
   * Read sensor once and use visible light counter to set gain and sample
   * ambient light again
   */
  validData = FALSE;
  if ( Mode.index == MODE_AMBIENT ) {
    configureSensor(TSL2591_LOW_GAIN);
    sensorGain = 1.0;
    readLuminosity(&fullSample, &irSample);
    readLuminosity(&fullSample, &irSample);
    if ( fullSample > TSL2591_MED_GAIN_LIMIT && fullSample <= TSL2591_LOW_GAIN_LIMIT ) {
      configureSensor(TSL2591_MED_GAIN);
      sensorGain = 25.0;
    }
    else if ( fullSample > TSL2591_HIGH_GAIN_LIMIT && fullSample <= TSL2591_MED_GAIN_LIMIT ) {
      configureSensor(TSL2591_HIGH_GAIN);
      sensorGain = 438.0;
    }
    else if ( fullSample <= TSL2591_HIGH_GAIN_LIMIT ) {
      configureSensor(TSL2591_MAX_GAIN);
      sensorGain = 10074.0;
    }
    readLuminosity(&fullSample, &irSample);
    readLuminosity(&fullSample, &irSample);
    lux = ((float) fullSample - irAtten * (float) irSample) * coef / sensorGain;
    validData = TRUE;
  }
  else if ( Mode.index == MODE_FLASH ) {
    // give visual indication that we are waiting for flash to fire
    fStop.setText(" ** ");
    drawScreen();
    average = 0;
    configureSensor(TSL2591_LOW_GAIN);
    sensorGain = 1.0;
    readLuminosity(&fullSample, &irSample);

    /*
     * Take average luninosity for one second to set limit of sensor and detect flash,
     * there is no connection between flash and sensor
     */
    for (i = 0; i < 10; i++) {
      readLuminosity(&fullSample, &irSample);
      average += fullSample;
    }
    average /= 10;
    tsl.clearInterrupt();
    // set limits for non persistent register, low is zero, max is twice ambient
    tsl.registerInterrupt(0, (uint16_t) (average << 1));

    /*
     * Save two samples before flash is detected, sample at t-2 is used as ambient
     * light seen by camera, and then t-1 and t are used together with t+1 to find
     * flash light detected, then twice the ambient are substracted to get flash
     * plus ambient
     */
    for ( k = 0; k < FLASH_WAIT; k++ ) {
      fullFlashMinus2 = fullBefore;
      irFlashMinus2 = irBefore;
      fullBefore = fullSample;
      irBefore = irSample;
      readLuminosity(&fullSample, &irSample);
      i = tsl.getStatus();
      tsl.clearInterrupt();
      if ( i & NPI ) break;   // Check NPI status flag to know if limit was exeeded
    }
    tsl.clearInterrupt();
    if ( k != FLASH_WAIT ) {
      readLuminosity(&fullAfter, &irAfter);
      fullFlash = fullBefore + fullSample + fullAfter - 3 * fullFlashMinus2;
      irFlash = irBefore + irSample + irAfter - 3 * irFlashMinus2;

      // use same equation as TSL2591 library, rearranged and with different values for
      // attenuation of IR counter and coef, it is calculated as:
      // coef = (gainBase * 6.82) / sensitivty
      // gainBase: gain used to get counts/(uW/cm2)
      // 1 uW/cm2 is 6.82 lux
      // channel0 sensitivity is 264.1 counts/uW/cm2
      // a fudge factor 4.0 is used here, in my tests, lux is overestimated by that much
      lux = (((float) fullFlashMinus2 - irAtten * (float) irFlashMinus2) +
             ((float) fullFlash  - irAtten * (float) irFlash) * 100.0 ) * coef / (4.0 * sensorGain);
      validData = TRUE;
    }
    else {
      fStop.setText("    ");
      validData = FALSE;
      drawScreen();
    }
  }

  // IR mode is similar to ambient, except that full spectrum is not used at all
  else if ( Mode.index == MODE_IR ) {
    configureSensor(TSL2591_LOW_GAIN);
    sensorGain = 1.0;
    readLuminosity(&fullSample, &irSample);
    if ( irSample > TSL2591_MED_GAIN_LIMIT && irSample <= TSL2591_LOW_GAIN_LIMIT ) {
      configureSensor(TSL2591_MED_GAIN);
      sensorGain = 25.0;
    }
    else if ( irSample > TSL2591_HIGH_GAIN_LIMIT && irSample <= TSL2591_MED_GAIN_LIMIT ) {
      configureSensor(TSL2591_HIGH_GAIN);
      sensorGain = 438.0;
    }
    else if ( irSample <= TSL2591_HIGH_GAIN_LIMIT ) {
      configureSensor(TSL2591_MAX_GAIN);
      sensorGain = 10074.0;
    }
    readLuminosity(&fullSample, &irSample);
    readLuminosity(&fullSample, &irSample);
    lux = irSensitivity * (float) irSample / sensorGain;
    validData = TRUE;
  }
}

/*
 * Main loop, read switches and do appropiate calculations, settings, etc
 *
 * This is most frequently called function from arduino environment, first
 * check if screen has chanfged and draw it again, then check switches status
 * and act on any switch pressed.
*/

void loop(void)
{
  static int switchCommand;
  unsigned int exposure;
  /*
   * If screen redraw is needed, this is the code that does it, there is one in setup
   * to show splash screen and then set settings used last time
   */
  if ( screenRedraw ) {
    drawScreen();
    screenRedraw = FALSE;      // screen updated, reset redraw flag
  }

  switchCommand = readSwitches();

  if ( switchCommand == incrementValue )
    changeValue(VALUE_INC);
  else if (switchCommand == decrementValue )
    changeValue(VALUE_DEC);
  else if (switchCommand == incrementCommand)
    changeField(FIELD_INC);
  else if (switchCommand == decrementCommand)
    changeField(FIELD_DEC);
  else if (switchCommand == readLight) {
    getExposure();
    if ( validData ) aperture();
  }

}
