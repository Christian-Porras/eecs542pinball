
// TO DO
// add score displays - move credits to unit 8, player score units 4 thru 7.
// add high score rotation - top four on displays 4 thru 7.
// mod the enumuration of switches, solemoids, based on as-built chart
//

#include <Wire.h>
#include <SPI.h>
//#include <SoftwareSerial.h>
#include <EEPROM.h>

//SoftwareSerial mySerial(0, 1); // RX, TX

#define pinMOSI 11
#define PDB_COMMAND_WRITE 1

//#define RETROSPA 1
//#define SCIFI 0
//#define TESTMODE 0 //1 //1
//#define TESTMODE_PLUS 0 //1
//#define DIAGNOSTIC 0

#define DEBOUNCE_MODE 1 //1

#define FREEPLAY 1

#define LIGHTS_ENABLED 1
#define SCORE_ENABLED 1
#define GAMERULES_ENABLED 1
#define DIAGNOSTIC_ENABLED 0 // uses same serial port as lights

// ASCII CODE DEFINES
#define ASCII_A 65
#define ASCII_B 66
#define ASCII_C 67
#define ASCII_D 68
#define ASCII_E 69
#define ASCII_F 70
#define ASCII_G 71
#define ASCII_H 72
#define ASCII_I 73
#define ASCII_J 74
#define ASCII_K 75
#define ASCII_L 76
#define ASCII_M 77
#define ASCII_N 78
#define ASCII_O 79
#define ASCII_P 80
#define ASCII_Q 81
#define ASCII_R 82
#define ASCII_S 83
#define ASCII_T 84
#define ASCII_U 85
#define ASCII_V 86
#define ASCII_W 87
#define ASCII_X 88
#define ASCII_Y 89
#define ASCII_Z 90
#define ASCII_EQUAL 61
#define ASCII_DOLLAR 36
#define ASCII_COMMA 44
#define ASCII_CR 13
#define ASCII_LF 10
/*

 PinScore adaptor

 */

int EEPROM_address = 0;

unsigned char upDown=1;	// start off going UP
unsigned char cylon=0;  // determines which LED is on 0 to 4

unsigned char selectDigit = 0;	// start no digit selected
unsigned char digitBCD = 0;	// start no digit selected
const unsigned char selectLatch = 12;	// start no digit selected
const unsigned char selectBlank = 13;	// start no digit selected
const unsigned char selectOffset = 0;	// start no digit selected

unsigned char selectSolenoid = 0;	//
unsigned char dataSolenoid = 0;	//
unsigned char dataSolenoidChime = 0;	//
unsigned char selectBank = 1;	//

unsigned char j = 0;	//
unsigned char i = 0;	//
unsigned char k = 0;	//

byte lastData = 0;
byte board = 0;

// --------------------------
//    SWITCHES
unsigned char switchStateNow[40];	// start no digit selected
unsigned char switchStatePrev[40];	// start no digit selected
unsigned char switchNew[40];	//
unsigned char switchDebounce[40];

//unsigned char solenoidFireFlag[8];	// start no digit selected
unsigned char solenoidFireFlag[16];	// start no digit selected

unsigned char gameCredits = 99; //5;	//
unsigned char gameCreditsX10 = 9; //5;	//
unsigned char gameCreditsX1 = 9; //5;	//
unsigned char gameBalls = 0;	//
unsigned char gameTilted = 0;	//

//unsigned int scorePlayerOne = 0; //
//unsigned int scorePlayerTwo = 0; //
//unsigned int scorePlayer[2] = {0,0}; //
unsigned long scorePlayerOne = 0; //
unsigned long scorePlayerTwo = 0; //
//float scorePlayer[4] = {0,0,0,0}; //

unsigned char player = 0; // 0== player one, 1 == player two
unsigned int scoreBonus = 0; //

//unsigned int scoreHigh = 6789; //
long scoreHigh[5] = {15785,987,654,321,0};  //
long scoreTemp1 = 0; //
long scoreTemp2 = 0; //
long scoreTemp3 = 0; //
long scoreTemp4 = 0; //

//unsigned long scoreHighList[4] = {0,0,0,0}; //

long scorePrev[5] = {0,0,0,0,0}; //
int scoreTimer = 25; //
unsigned char flagScoreChange = 0; //
unsigned int prevScoreChange = 0; // This is to evaluate which chime fires
unsigned char numPlayers = 0; //
unsigned char bonusMultiplier = 0; //

unsigned char bonusCount = 0; //
unsigned char bumperBonusCount = 0; //

// --------------------------
//    LIGHTS
//    one byte describes pattern
//    0b00000000 == full on, 0b11111111 == full off, etc
//    0b00001111 == slow blink, 0b01010101 == fast blink, etc

unsigned char lightLane[5] = {255,255,255,255,255};	// RETRO
unsigned char lightArrowLane[5] = {0,0,0,0,0};	//
unsigned char lightRollover = 0;	//
unsigned char lightKickerArrow[3] = {0,0,0};	//
unsigned char countKickerArrow = 0;	//
unsigned char lightStarRollover[5] = {0,0,0,0,0};	//
unsigned char countStarRollover = 0;	//
unsigned char lightSpotLow[5] = {0,0,0,0,0};	//	//
unsigned char countSpotLow = 0;
unsigned char lightField[5] = {0,0,0,0,0};    // RETRO
unsigned char lightFieldLow[5] = {0,0,0,0,0};  // SPA ++
unsigned char lightSpinner[2] = {0,0};	//
unsigned char lightLOutLane[4] = {0,0,0,0};	//
unsigned char countLOutLane = 0;
unsigned char flagLOutLane = 0;
unsigned char lightBonusMult[4] = {0,0,0,0};	//
unsigned char lightShootAgain = 0;  //
unsigned char lightBBonus[3] = {0,0,0};	// RETROSPA bumper bonus count, lights lane to spot letter
unsigned char lightSpecial[3] = {0,0,0};	// 20k, extra ball, special
unsigned char countSpecial= 0;

unsigned char lightHunterUp[6] = {255,255,0,0,255,255};	//
unsigned char lightHunterLow[6] = {0,0,0,0,0,0};	//
unsigned char lightPositive = 0;
unsigned char lightNegative = 0;
unsigned char flagReverse = 0;
unsigned char lightThumperLF = 0;
unsigned char lightThumperRT = 0;
unsigned char lightThumperMD = 0;

unsigned char lightLow1Thump = 0;
unsigned char lightLow2Thump = 0; // this is bonus thumper

unsigned char lightRollover0 = 0;
unsigned char lightPlayfield[6] = {0,0,0,0,0,0};
unsigned char lightOtherRollovers = 0;
//unsigned char lightHighScore = 0;

unsigned char lightPlayer1 = 0b10101010; //0;
unsigned char lightPlayer2 = 0b01010101; //0;
unsigned char lightPlayer[4] = {0,0,0,0}; //0b01010101; //0;
unsigned char lightHighScore = 0b11001100; //0;
unsigned char lightCredit = 0b00001111; //0;
unsigned char lightTilt = 0b11001010;
unsigned char lightBall = 0b00111100; //0;
unsigned char lightGame = 0b01001011; //0;
unsigned char lightOver = 0b00101011; //0;

unsigned char testFire = 0;	// SPA lit

unsigned char lightTest = 0;	// SPA lit

//  light indexes
unsigned char indexBBonus = 0;	// bonus spotting letters
unsigned char indexRETRO = 0;	// num lights spotted in RETRO
unsigned char indexSPA = 0;	// num lights spotted in SPA
unsigned char flagRETRO = 0;	// RETRO lit
unsigned char flagSPA = 0;	// SPA lit
unsigned char flagRETROSPA = 0;
unsigned char playerRETRO[5] = {0,0,0,0,0}; // holder for RETRO SPA lights
unsigned char playerSPA[5] = {0,0,0,0,0}; // holder for RETRO SPA lights
unsigned char playerSpecial[5] = {0,0,0,0,0}; // holder for 18k and 36k lights
unsigned char countSpotLowPlayer[5] = {0,0,0,0,0};

unsigned char sendLightOut = 0;	// flag to send light data
unsigned char idCAN = 0;	// id of 8-ch circuit (2 per board, 16 total)
unsigned char x = 1;

int Score100K;
int Score10K;
int ScoreK;
int Score100;
int Score10;
int Score1;
long ScoreX;
long scorePlayer[5] = {0,0,0,0,0}; //

unsigned char wireWriteData[12]; // array for I2C send

unsigned char scoreTransmitSequence = 0;
unsigned char lightTransmitSequence = 0;


unsigned char ShootAgainLongLoop = 0;
unsigned char ShootAgainShortLoop = 0;
unsigned char flagShootAgainStart = 0;

unsigned char KickerLaneLongLoop = 0;
unsigned char KickerLaneShortLoop = 0;
unsigned char flagKickerLaneStart = 0;

unsigned char flagNewSwitch = 0;
boolean flagDiagnostic = 0;
// --------------------------

#define NUM_BALLS 3 // 5
// modes
enum
{
  ATTRACT_INIT, //?? needed ??
  ATTRACT,
  START,
  BALLONE,
  BALLTWO,
  BALLTHREE,
  #if (NUM_BALLS == 3)
    GAMEOVER,
    BALLFOUR, // not used, but needs defining
    BALLFIVE // not used, but needs defining
  #else
    BALLFOUR,
    BALLFIVE,
    GAMEOVER
  #endif
};

// switches

//#if (RETROSPA == 1)

//#else  // scifi
enum
{
  SWITCH_0,
  SWITCH_A, // was 1
  SWITCH_B, // was 2
  SWITCH_C, // was 3
  SWITCH_D, // was 4
  SWITCH_5,
  SWITCH_6,
  SWITCH_7,

  SWITCH_8,
  SWITCH_9,
  SWITCH_10,
  SWITCH_11,
  SWITCH_START,
  SWITCH_COIN,
  SWITCH_KICKER,
  SWITCH_ROLLOVER,

  SWITCH_LF_SPINNER, // 16
  SWITCH_LANE_DROP_1,
  SWITCH_LANE_DROP_2,
  SWITCH_LANE_DROP_3,  // kicker / shoot again
  SWITCH_LANE_TARGET,
  SWITCH_MID_TARGET,
  SWITCH_22,
  SWITCH_RT_SPINNER,

  SWITCH_THUMPER_1,        // BLACK - ORANGE
  SWITCH_THUMPER_2,        // BLACK - GREEN
  SWITCH_THUMPER_3,        // BLACK - BLUE

  SWITCH_LANE_R_1,
  SWITCH_LANE_E,
  SWITCH_LANE_T,
  SWITCH_LANE_R_2,
  SWITCH_LANE_O,

  SWITCH_SHOOT_LANE,
  SWITCH_LF_OUT_LANE,
  SWITCH_LF_RET_LANE,
  SWITCH_LF_SLING,
  SWITCH_OUTHOLE,
  SWITCH_RT_OUT_LANE,
  SWITCH_LOW_1_THUMP, //SHOOT_LANE,
  SWITCH_LOW_2_THUMP // 39
};

/*
enum  // SOLENOIDS BANK ONE
{
  SOLENOID_INLINE_DROP,
  SOLENOID_LF_THUMPER,
  SOLENOID_RT_THUMPER,
  SOLENOID_MD_THUMPER,

  SOLENOID_KICKER,
  SOLENOID_LOW_1_THUMP,
  SOLENOID_LOW_2_THUMP,
  SOLENOID_OUTHOLE,
  // SOLENOIDS BANK TWO
  SOLENOID_LF_SLING,
  SOLENOID_DIVERTER,
  SOLENOID_10,
  //SOLENOID_11,
  SOLENOID_FLIPPER_RELAY, // was 11
  SOLENOID_CHIME_1,
  SOLENOID_CHIME_2,
  SOLENOID_CHIME_3,
  SOLENOID_CHIME_4
};
*/

enum  // SOLENOIDS BANK ONE
{
  // SOLENOIDS BANK TWO
  SOLENOID_LF_SLING,
  SOLENOID_DIVERTER,
  SOLENOID_10,
  //SOLENOID_11,
  SOLENOID_FLIPPER_RELAY, // was 11
  SOLENOID_CHIME_1,
  SOLENOID_CHIME_2,
  SOLENOID_CHIME_3,
  SOLENOID_CHIME_4,

  SOLENOID_INLINE_DROP,
  SOLENOID_LF_THUMPER,
  SOLENOID_RT_THUMPER,
  SOLENOID_MD_THUMPER,
  SOLENOID_KICKER,
  SOLENOID_LOW_1_THUMP,
  SOLENOID_LOW_2_THUMP,
  SOLENOID_OUTHOLE
};

#define SCORE_THUMPER 20
#define SCORE_THUMPER_UNLIT 5
#define SCORE_REV 400
#define SCORE_TARGET 10
#define SCORE_DROP 15
//#define SCORE_INLINE 1 //25
#define SCORE_INLINE 250 // CHANGED 130531 BC
#define SCORE_SLING 3
#define SCORE_ROLLOVER 10
#define SCORE_SPINNER 100
#define SCORE_OUTHOLE 123 //500

#define SCORE_SPINNER_BONUS 1000 //
#define SCORE_SPECIAL 25000 //

//#define NUM_BALLS 5
#define SOLENOID_FIRE_DURATION 100 //50 //5 //10 //50
#define THUMPER_FIRE_DURATION 15 //5 //5 //10
#define UPPER_THUMPER_FIRE_DURATION 20 //15 //10
#define OUTHOLE_FIRE_DURATION 6 //3 //5 //10
#define CHIME_FIRE_DURATION 6 //3 //5 //10
#define KICKER_FIRE_DURATION 40 //20 //5 //10
#define SLING_FIRE_DURATION 4 //3 //5 //10

#define DELAY_OUTHOLE 40 //
#define DELAY_DROP_TARGET 80 //
#define DELAY_MULTI_BALL 300 //
#define DELAY_SWITCH_SCAN 400 //
#define DELAY_BALL_SAVE 75 //

#define SWITCH_DEBOUNCE_DURATION 10 //10 loops

//#endif

unsigned char mode = ATTRACT_INIT;

void setup() {

#if ((LIGHTS_ENABLED == 1) || (DIAGNOSTIC_ENABLED == 1))
  Serial.begin(9600);           // start serial for output
#endif

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Starting Setup Function");
    Serial.print("\r\n");
  #endif

  // initialize the digital pins as outputs.
  //DDRB = B00000000;  // sets Arduino port B pins as outputs
  //DDRD = B00000000;  // sets Arduino port D pins as inputs

  pinMode(14,OUTPUT); // analog in used a row drive out
  pinMode(15,OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(18,OUTPUT);


  pinMode(2,INPUT); // analog in used a row drive out
  pinMode(3,INPUT); // analog in used a row drive out
  pinMode(4,INPUT); // analog in used a row drive out
  pinMode(5,INPUT); // analog in used a row drive out
  pinMode(6,INPUT); // analog in used a row drive out
  pinMode(7,INPUT); // analog in used a row drive out
  pinMode(8,INPUT); // analog in used a row drive out
  pinMode(9,INPUT); // analog in used a row drive out


#if (SCORE_ENABLED == 1)
// wire is for I2C PinScore Arduinos
  Wire.begin(); // join i2c bus (address optional for master)
# endif

  //  this serial for solenoid driver
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Need 8 MHz.  So use DIV2 on a 16 MHz board.

  // this serial for light driver
  //Serial.begin(9600);           // start serial for output
  //mySerial.begin(9600);           // start serial for output



  //for (j = 0; j < 40; j++)
  for (j = 0; j < 16; j++)
  {
    solenoidFireFlag[j] = 0;
  }


// initial read of switches
  // SET DRIVE LINES HERE
  for (j = 1; j < 5; j++)
  {
    // START ALL LOW (no signal)
    digitalWrite(14, LOW); // pins 14-17
    digitalWrite(15, LOW); // pins 14-17
    digitalWrite(16, LOW); // pins 14-17
    digitalWrite(17, LOW); // pins 14-17
    //digitalWrite(18, LOW); // pins 14-17

    // DRIVE ONE LINE HIGH
    digitalWrite((j+13), HIGH); // pins 14-17

    // WAIT HERE FOR RISE TIME
    delayMicroseconds(400) ;

    // START SCAN
    for (i = 0; i < 8; i++)
    {
      switchStateNow[((j*8) + i)] = digitalRead(i + 2); // pins 2-9
      switchStatePrev[((j*8) + i)] = switchStateNow[((j*8) + i)];
      switchNew[((j*8) + i)] = 0; // initialize
    } // end for i
  } // end for j


  mode = ATTRACT_INIT;

} // end setup


void loop()
{


#if (DIAGNOSTIC_ENABLED == 1)
  flagDiagnostic = !flagDiagnostic;
  Serial.print(flagDiagnostic); // TODO check this formatting later
  Serial.print("\r\n");
#endif

  //                                                        *****************************************
  //                                                        -----------------------------------------
  //                                                                   START READ SWITCH
  //                                                        -----------------------------------------
  //                                                        *****************************************

  flagNewSwitch = 0;
  // START SWITCH SCAN HERE
  for (j = 1; j < 5; j++)
  {
    // START ALL HIGH (no signal)
    digitalWrite(14, LOW); // pins 14-17
    digitalWrite(15, LOW); // pins 14-17
    digitalWrite(16, LOW); // pins 14-17
    digitalWrite(17, LOW); // pins 14-17

    digitalWrite((j+13), HIGH); // pins 14-17

    // TO DO look into uS delay?
    //delay(1) ;             // wait one mili-second
    //delayMicroseconds(400) ;
    delayMicroseconds(DELAY_SWITCH_SCAN);

    // START SCAN
    for (i = 0; i < 8; i++)
    {
      switchStatePrev[((j*8) + i)] = switchStateNow[((j*8) + i)]; // rotate variable
      switchStateNow[((j*8) + i)] = digitalRead(i + 2); // pins 2-9

      // check for a "new" state
    #if (DEBOUNCE_MODE)
      if ((switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)]) || (switchDebounce[((j*8) + i)] > 0))
    #else
      if ( switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)])
    #endif
      {
        switchNew[((j*8) + i)] = 0; // same as old
      } // end if


      else // must be new if not old and new equals one
      {
        if (switchStateNow[((j*8) + i)] == 1)
        {
          switchNew[((j*8) + i)] = 1; // new
         flagNewSwitch = 1;

          #if (DEBOUNCE_MODE)
            switchDebounce[((j*8) + i)] = SWITCH_DEBOUNCE_DURATION; // set timer
          #endif

          #if (DIAGNOSTIC_MODE == 1)
            Serial.print("Switch = ");
            Serial.print((j*8) + i); // TODO check this formatting later
            Serial.print("\r\n");
          #endif

        if ( ((j*8) + i) != (SWITCH_SHOOT_LANE))
        {
          flagLOutLane = 0; // reset shoot lane scoring on any other switch activation
        }
        // SWITCH DISPLAY FOR DIAGNOSTIC, REMOVE LATER
        gameCredits = ((j*8) + i);
        //flagScoreChange += 1;
        }
      } // end else
    } // end for i
  } // end for j

  #if (DEBOUNCE_MODE)
    for (j = 0; j < 40; j++)
    {
      if (switchDebounce[j] > 0)
      {
        switchDebounce[j] -= 1; // ramp down to zero
      }
    }
  #endif

  // end read switches

//return;



  //                                               ***************************************************************
  //                                               ***************************************************************
  //                                                                       GAME RULES HERE
  //                                               ***************************************************************
  //                                               ***************************************************************

  switch (mode) {

  default:
  case ATTRACT_INIT:  // -----------------------------------------------------------------------------------------
    // >>> send addresses for light show here

     delay(250); // give everything else a chance to power up
     delay(250); // give everything else a chance to power up
     delay(250); // give everything else a chance to power up
     delay(250); // give everything else a chance to power up

    player = 0;
    scorePlayer[0] = 0;
    scorePlayer[1] = 0;
    scorePlayer[2] = 0;
    scorePlayer[3] = 0;
    scorePlayer[4] = 0;

    #if (SCORE_ENABLED == 1)
      for (j = 0; j < 4; j++)
      {
        sendScoreToDisplay(567 + j, j);
      }
    #endif                                                                      // --------------------------------
                                                                          // SET ATTRACT MODE LIGHTING HERE

    lightLane[0] = 0b11110000;
    lightLane[1] = 0b00111100;
    lightLane[2] = 0b00001111;
    lightLane[3] = 0b11000011;
    lightLane[4] = 0b11110000;

    lightField[0] = 0b01011111;
    lightField[1] = 0b01011110;
    lightField[2] = 0b01011100;
    lightField[3] = 0b01011000;
    lightField[4] = 0b01010000;

    lightFieldLow[0] = 0b01011111;
    lightFieldLow[1] = 0b01011110;
    lightFieldLow[2] = 0b01011100;
    lightFieldLow[3] = 0b01011000;
    lightFieldLow[4] = 0b01010000;

    lightPlayer[0] = 0b01100001;
    lightPlayer[1] = 0b01100010;
    lightPlayer[2] = 0b11000100;
    lightPlayer[3] = 0b11001000;

    lightKickerArrow[0] = 0b00011111;
    lightKickerArrow[1] = 0b00111110;
    lightKickerArrow[2] = 0b01111100;

    lightBonusMult[0] = 0b01010101;
    lightBonusMult[1] = 0b01010100;
    lightBonusMult[2] = 0b01010000;

    lightBBonus[0] = 0b01010101;
    lightBBonus[1] = 0b10101010;
    lightBBonus[2] = 0b01010101;

    lightSpecial[0] = 0b00011001;
    lightSpecial[1] = 0b00110010;
    lightSpecial[2] = 0b01100100;

    lightArrowLane[0] = 0b01010101;
    lightArrowLane[1] = 0b10101010;
    lightArrowLane[2] = 0b10101010;
    lightArrowLane[3] = 0b10101010;
    lightArrowLane[4] = 0b01010101;

    lightHighScore = 0b11001100; //0;
    lightCredit = 0b00001111; //0;
    lightTilt = 0b11001010;
    lightBall = 0b00111100; //0;
    lightGame = 0b01001011; //0;
    lightOver = 0b00101011; //0;

    lightStarRollover[0] = 0b01011111;
    lightStarRollover[1] = 0b01011110;
    lightStarRollover[2] = 0b01011100;
    lightStarRollover[3] = 0b01011000;
    lightStarRollover[4] = 0b01010000;

    lightSpinner[0] = 0b11001100;
    lightSpinner[1] = 0b01100110;

    lightLow2Thump = 0b00001111;

    lightLOutLane[0] = 0b00001111;
    lightLOutLane[1] = 0b00011110;
    lightLOutLane[2] = 0b00111100;
    lightLOutLane[3] = 0b01111000;

    lightSpotLow[0] = 0b01111111;
    lightSpotLow[1] = 0b01111110;
    lightSpotLow[2] = 0b01111100;
    lightSpotLow[3] = 0b01101000;

    numPlayers = 0;
    mode = ATTRACT;

    break;



  case ATTRACT:
    // ----------------------------------------------------------------------------------
    //                                            FLASH HIGH SCORE - PREVIOUS SCORE

    if(scoreTimer == 0)
      {
      // rotate scores high-prev
      if (lightHighScore > 0)
        {
        for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePrev[j],j); //
          #endif
          flagScoreChange = 0;
          }

        lightHighScore = 0b00000000; //0;
        }

      else

        {
        for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scoreHigh[j],j); //
          #endif
          flagScoreChange = 0;
          }

        lightHighScore = 0b11111111; //0;

        //TO DO
        // SET LIGHT FOR HIGH SCORE HERE

        }
      //scoreTimer = 250; // reset score timer
      scoreTimer = 1200; // reset score timer
      }
    else
      {
      scoreTimer -= 1;
      }
// ----------------------------------------------------------------------------------


    // wait for coins, credits
    if (switchStateNow[SWITCH_COIN] == 1) // is on now
    {
      if (switchStatePrev[SWITCH_COIN] == 0) // was off
      {
        if ( gameCredits < 99 )
        {
          gameCredits += 1;
          #if (SCORE_ENABLED == 1)
            sendBallCreditToDisplay(gameCredits, gameBalls);
          #endif
        }// end if credits
      }// end switch on
    } // end switch new


    if(switchNew[SWITCH_START])
    {
      if(switchStateNow[SWITCH_START])
      {
        dataSolenoid = 0;
        dataSolenoidChime = 0;

        switch (numPlayers)
        {
        case 0: // should never get here unless free play
          //gameBalls = 1;
          mode = START;
          gameCredits -= 1;
          numPlayers = 1;
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }

          break;

         case 1: // normal starting case
         case 2: // normal starting case
         case 3: // normal starting case
         case 4: // normal starting case
          mode = START;
          gameCredits -= 1;
          numPlayers += 1;
          if (numPlayers > 4)
            {
              numPlayers = 4;
            }
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }

          break;

        default: // no change for now with two players max
          //gameBalls = 1;
          mode = START;
          numPlayers = 4;
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }

          break;

        } // end switch num plarers

        //OLD
        //mode = START;
        //numPlayers = 1;


      // ---------------------------------------------
      // CHECK FOR LOST BALLS, AND WAIT TILL THEYRE FOUND
      // ---------------------------------------------
       // UPPER right KICKER
       if (switchStateNow[SWITCH_KICKER] == 1) // BALL HIDDEN IN KICKER
       //while (switchStateNow[SWITCH_KICKER] == 1) // BALL HIDDEN IN KICKER
       {
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_KICKER);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_KICKER);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }
       // ---------------------------------------------


        // RESET INLINE DROP AT START OF GAME
        if ( switchStateNow[SWITCH_LANE_DROP_1] || switchStateNow[SWITCH_LANE_DROP_2] ||switchStateNow[SWITCH_LANE_DROP_3] )
        {
          delay(200); //debounce
          fireSolenoidNow(SOLENOID_INLINE_DROP);
          delay(DELAY_DROP_TARGET); //debounce
          unFireSolenoidNow(SOLENOID_INLINE_DROP);
          delay(250); //debounce
          delay(250); //debounce
        }

        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);
        delay(250); //debounce
        delay(250); //debounce
        //END OUTHOLE

        //delay(500); //debounce

      }// end if start now
    }// end if start new

    break;

    // NEW GAME
  case START:  // -----------------------------------------------------------------------------------------

    // ensable flippers with relay
    //dataSolenoidChime = 0;
    //selectSolenoid = SOLENOID_FLIPPER_RELAY; //1;
    //dataSolenoidChime |= 1 << selectSolenoid; // set solenoid
    //sendPDBCommand(0, PDB_COMMAND_WRITE, 0, dataSolenoidChime);
    //solenoidFireFlag[selectSolenoid] = 255; // anything above 253 is constant on
    // no need to time here, should be constant on

    solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 254; // anything above 253 is constant on

    //  RESET LIGHTS
    for (j = 0; j < 5; j++)
    {
      lightLane[j] = 0b11111111;
      lightField[j] = 0b00000000;
    }

    for (j = 0; j < 5; j++)
    {
      //lightSpotLow[j] = 0b11111111;
      lightFieldLow[j] = 0b00000000;
    }

    //SET SPOT SPA
    countSpotLow = 0;
    for (j = 0; j < 4; j++)
    {
      countSpotLowPlayer[j] = 0;
    }
    lightSpotLow[0] = 0b11111111;
    lightSpotLow[1] = 0b00000000;
    lightSpotLow[2] = 0b00000000;
    lightSpotLow[3] = 0b00000000;

    lightKickerArrow[0] = 0b11111111;
    lightKickerArrow[1] = 0b00000000;
    lightKickerArrow[2] = 0b00000000;

    flagLOutLane = 1;
    countLOutLane = 0;
    lightLOutLane[0] = 0b11111111;
    lightLOutLane[1] = 0b00000000;
    lightLOutLane[2] = 0b00000000;
    lightLOutLane[3] = 0b00000000;

    lightBonusMult[0] = 0b00000000;
    lightBonusMult[1] = 0b00000000;
    lightBonusMult[2] = 0b00000000;
    bonusMultiplier = 1;

    lightBBonus[0] = 0b00000000;
    lightBBonus[1] = 0b00000000;
    lightBBonus[2] = 0b00000000;

    lightSpecial[0] = 0b00000000;
    lightSpecial[1] = 0b00000000;
    lightSpecial[2] = 0b00000000;

    lightArrowLane[0] = 0b00000000;
    lightArrowLane[1] = 0b11111111;
    lightArrowLane[2] = 0b00000000;
    lightArrowLane[3] = 0b00000000;
    lightArrowLane[4] = 0b00000000;

    lightHighScore = 0b00000000; //0;
    lightCredit = 0b11111111; //0;
    lightTilt = 0b00000000;
    lightBall = 0b11111111; //0;
    lightGame = 0b00000000; //0;
    lightOver = 0b00000000; //0;

    lightThumperLF = 0b00000000;
    lightThumperRT = 0b00000000;
    lightThumperMD = 0b00000000;

    lightLow2Thump = 0b00000000;

    lightStarRollover[0] = 0b11111111;
    lightStarRollover[1] = 0b00000000;
    lightStarRollover[2] = 0b00000000;
    lightStarRollover[3] = 0b00000000;
    lightStarRollover[4] = 0b00000000;
    countStarRollover = 0;

    lightSpinner[0] = 0b00000000;
    lightSpinner[1] = 0b00000000;

    // RESET ALL SAVED LIGHTS TO ZERO
    for (j = 0; j < 5; j++)
    {
      playerRETRO[j] = 0;
      playerSPA[j] = 0;
      playerSpecial[j] = 0;
    }

    gameBalls =  1; //( mode - 2); // ball in play

    // ADD COIN FOR PLAYER TWO HERE
    if(switchStateNow[SWITCH_START] == 1) // && (gameCredits > 0))
    {
      if((switchStatePrev[SWITCH_START] == 0) && (gameCredits > 0))
      {
        switch (numPlayers)
        {
        case 0: // should never get here unless free play
          //gameBalls = 1;
          mode = START;
          gameCredits -= 1;
          numPlayers = 1;
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }

          break;

         case 1: // normal starting case
         case 2: // normal starting case
         case 3: // normal starting case
         case 4: // normal starting case
          mode = START;
          gameCredits -= 1;
          numPlayers += 1;
          if (numPlayers > 4)
            {
              numPlayers = 4;
            }
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }

          break;

        default: // no change for now with two players max
          //gameBalls = 1;
          mode = START;
          numPlayers = 4;
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;

          for (j = 0; j < 4; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[j],j); //
          #endif
          flagScoreChange = 0;
          }
          break;
        } // end switch num plarers
      } // end if on and credits
    } // end switch start new


    lightPlayer[0] = 0b00000000;
    lightPlayer[1] = 0b00000000;
    lightPlayer[2] = 0b00000000;
    lightPlayer[3] = 0b00000000;
    for (j = 0; j < numPlayers; j++)
    {
      lightPlayer[j] = 0b10101010;
    }

    // TODO possibly make this wait with a while loop
    // OFFICIALLY START BALL ONE ONCE SHOOTER USED
    if (switchStateNow[SWITCH_SHOOT_LANE] == 1) // is on now
    {
      if (switchStatePrev[SWITCH_SHOOT_LANE] == 0) // was off
      {
        mode = BALLONE;
        // enable shoot again for a few seconds after ball comes out
        if ((lightShootAgain > 128) || (lightShootAgain == 0)) // real shoot again, or next ball
        // no multiple ball saves allowed
        {
        lightShootAgain = 0b01010101;
        flagShootAgainStart = 1; //1;
        ShootAgainShortLoop = DELAY_BALL_SAVE; //35; //50;
        ShootAgainLongLoop = DELAY_BALL_SAVE; //35; //50;
        }

        //mode = BALLONE;
        //lightShootAgain = 0b01010101;
        //flagShootAgainStart = 1;
        //ShootAgainShortLoop = DELAY_BALL_SAVE; //60
        //ShootAgainLongLoop = DELAY_BALL_SAVE; //60

        // OPEN DIVERTER GATE
        lightArrowLane[2] = 0b11111111;
        lightArrowLane[1] = 0b00000000;
        lightArrowLane[3] = 0b00000000;

        solenoidFireFlag[SOLENOID_DIVERTER] = 254;

        lightPlayer[0] = 0b11111111;
        lightPlayer[1] = 0b00000000;
        lightPlayer[2] = 0b00000000;
        lightPlayer[3] = 0b00000000;
      }
    }
    #if (SCORE_ENABLED == 1)
      sendBallCreditToDisplay(gameCredits, gameBalls);
    #endif

    break;

  //                                               ***************************************************************
  //                                               ---------------------------------------------------------------
  //                                                           BALL IN PLAY SECTION OF GAME RULES HERE
  //                                               ---------------------------------------------------------------
  //                                               ***************************************************************
  case BALLFIVE:
  case BALLFOUR:
  case BALLTHREE:
  case BALLTWO:
  case BALLONE:


  //                                                              ************************************************
  //                                                              ------------------------------------------------
  //                                                                      *IMEADIATE* SOLENOID ACTIVATION HERE
  //                                                              ------------------------------------------------
  //                                                              ************************************************

  // ---------------------------------------------
  // THUMPER LOW 2
  // ---------------------------------------------
  if (switchNew[SWITCH_LOW_2_THUMP] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_LOW_2_THUMP);
      solenoidFireFlag[SOLENOID_LOW_2_THUMP] = THUMPER_FIRE_DURATION; //50; // thumper one
      rotateBumperBonus();
      rotateArrowLaneBonus();
    }
  // ---------------------------------------------

    // ---------------------------------------------
  //  LEFT SLINGSHOT
  // ---------------------------------------------
  if (switchNew[SWITCH_LF_SLING] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_LF_SLING);
      solenoidFireFlag[SOLENOID_LF_SLING] = THUMPER_FIRE_DURATION; //50; // thumper one
      switchDebounce[SOLENOID_LF_SLING] = 250;
    }
  // ---------------------------------------------

  // ---------------------------------------------
  // THUMPER ONE _MD
  // ---------------------------------------------
  if (switchNew[SWITCH_THUMPER_1] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_MD_THUMPER);
      solenoidFireFlag[SOLENOID_MD_THUMPER] = UPPER_THUMPER_FIRE_DURATION; //50; // thumper one
      rotateBumperBonus();
      rotateArrowLaneBonus();
    }
  // ---------------------------------------------

  // ---------------------------------------------
  // THUMPER TWO LF
  // ---------------------------------------------
  if (switchNew[SWITCH_THUMPER_2] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_LF_THUMPER);
      solenoidFireFlag[SOLENOID_LF_THUMPER] = UPPER_THUMPER_FIRE_DURATION; //50; // thumper one
      rotateBumperBonus();
      rotateArrowLaneBonus();
       // ADD SCORE
      if(lightLow2Thump > 0)
      {
        scorePlayer[player] += SCORE_THUMPER; // should be 1000
        flagScoreChange += 50;
      }
      else
      {
        scorePlayer[player] += SCORE_THUMPER_UNLIT;
        flagScoreChange += 50;
      }
    }
  // ---------------------------------------------

  // ---------------------------------------------
  // THUMPER THREE RT
  // ---------------------------------------------
  if (switchNew[SWITCH_THUMPER_3] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_RT_THUMPER);
      solenoidFireFlag[SOLENOID_RT_THUMPER] = UPPER_THUMPER_FIRE_DURATION; //50; // thumper one
      rotateBumperBonus();
      rotateArrowLaneBonus();
    }
  // ---------------------------------------------

  // ---------------------------------------------
  // THUMPER LOW 1
  // ---------------------------------------------
    if (switchNew[SWITCH_LOW_1_THUMP] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_LOW_1_THUMP);
      solenoidFireFlag[SOLENOID_LOW_1_THUMP] = THUMPER_FIRE_DURATION; //50; // thumper one
      rotateBumperBonus();
      rotateArrowLaneBonus();
    }
  // ---------------------------------------------


//TODO put imediate solenoid activation here
  // each loop between 1 and 2 ms ??
  if (flagShootAgainStart == 1)
    {
      ShootAgainShortLoop -= 1;
      if (ShootAgainShortLoop == 0)
        {
          ShootAgainLongLoop -= 1;
          //ShootAgainShortLoop = 50; // reset short loop
          ShootAgainShortLoop = DELAY_BALL_SAVE; // reset short loop
          if (ShootAgainLongLoop == 0)
            {
              flagShootAgainStart = 0;
              lightShootAgain = 0;
            }
        }
    }


  if (flagKickerLaneStart == 1)
    {
      KickerLaneShortLoop -= 1;
      if (KickerLaneShortLoop == 0)
        {
          KickerLaneLongLoop -= 1;
          //KickerLaneShortLoop = 50; // reset short loop
          KickerLaneShortLoop = DELAY_BALL_SAVE; // reset short loop
          if (KickerLaneLongLoop == 0)
            {
              flagKickerLaneStart = 0;
              lightShootAgain = 0;
            }
        }
    }

    // TODO put switch activation code here


  //                                                                 ****************************************************
  //                                                                 ---------------------------------------------------
  //                                                                     READ SWITCHES, SET LIGHTS and SCORE DATA HERE
  //                                                                 ---------------------------------------------------
  //                                                                 ****************************************************



  if ( switchNew[SWITCH_MID_TARGET] && switchStateNow[SWITCH_MID_TARGET] ) // MID targeT
  {
    // TO DO and switch debounce here
    switchDebounce[SWITCH_MID_TARGET] = 250; // increase debounce from 40 to 250

    switch (countSpotLow)
    {
      default:
      case 0: // first hit
        lightSpotLow[0] = 0b00000000;
        lightSpotLow[1] = 0b11111111;
        lightSpotLow[2] = 0b00000000;
        lightSpotLow[3] = 0b00000000; // bonus

        lightFieldLow[0] = 0b11111111; // "S"

        countSpotLow += 1;
      break;

      case 1:
        lightSpotLow[0] = 0b00000000;
        lightSpotLow[1] = 0b00000000;
        lightSpotLow[2] = 0b11111111;
        lightSpotLow[3] = 0b00000000; // bonus

        lightFieldLow[1] = 0b11111111; // "P"

        countSpotLow += 1;
      break;

      case 2:
        lightSpotLow[0] = 0b00000000;
        lightSpotLow[1] = 0b00000000;
        lightSpotLow[2] = 0b00000000;
        lightSpotLow[3] = 0b11111111; // bonus

        lightFieldLow[2] = 0b11111111; // "A"

        countSpotLow += 1;
      break;

      case 3:
        scorePlayer[player] += (5000); // award bonus
        flagScoreChange += 1;

        lightSpotLow[0] = 0b00000000;
        lightSpotLow[1] = 0b00000000;
        lightSpotLow[2] = 0b00000000;
        lightSpotLow[3] = 0b11111111; // bonus
      break;
    } // end switch
  } // end if new mid target switch

                                                                        // "RETRO" LANES HERE
  if ( switchNew[SWITCH_LANE_R_1] && switchStateNow[SWITCH_LANE_R_1] ) //
  {
     // SCORING HERE
    if (lightArrowLane[0] > 0) // bonus
    {
      scorePlayer[player] += 5000;
      flagScoreChange += 1;
    }

    if (lightLane[0] > 0) //
    {
      scorePlayer[player] += 500;
      lightField[0] = 0b11111111;
      lightLane[0] = 0b00000000;
      flagScoreChange += 1;
    }
  } // end if switch


  if ( switchNew[SWITCH_LANE_E] && switchStateNow[SWITCH_LANE_E] ) //
  {
     // SCORING HERE
    if (lightArrowLane[1] > 0) // outlane diverter on
    {
      lightArrowLane[2] = 0b11111111;
      lightArrowLane[1] = 0b00000000;
      lightArrowLane[3] = 0b00000000;
      solenoidFireFlag[SOLENOID_DIVERTER] = 254;
    }
    if (lightLane[1] > 0) //
    {
      scorePlayer[player] += 500;
      lightField[1] = 0b11111111;
      lightLane[1] = 0b00000000;
      flagScoreChange += 1;
    }
  }


  if ( switchNew[SWITCH_LANE_T] && switchStateNow[SWITCH_LANE_T] ) //
  {
     // SCORING HERE
    if (lightLane[2] > 0) //
    {
      scorePlayer[player] += 500;
      lightField[2] = 0b11111111;
      lightLane[2] = 0b00000000;
      flagScoreChange += 1;
    }
  }


  if ( switchNew[SWITCH_LANE_R_2] && switchStateNow[SWITCH_LANE_R_2] ) //
  {
     // SCORING HERE
    if (lightArrowLane[3] > 0) // outlane diverter on
    {
      lightArrowLane[2] = 0b11111111;
      lightArrowLane[1] = 0b00000000;
      lightArrowLane[3] = 0b00000000;
      solenoidFireFlag[SOLENOID_DIVERTER] = 254;
    }
    if (lightLane[3] > 0) //
    {
      scorePlayer[player] += 500;
      lightField[3] = 0b11111111;
      lightLane[3] = 0b00000000;
      flagScoreChange += 1;
    }
  }


  if ( switchNew[SWITCH_LANE_O] && switchStateNow[SWITCH_LANE_O] ) //
  {
     // SCORING HERE
    if (lightArrowLane[4] > 0) // bonus
    {
      scorePlayer[player] += 5000;
      flagScoreChange += 1;
    }

    if (lightLane[4] > 0) //
    {
      scorePlayer[player] += 500;
      lightField[4] = 0b11111111;
      lightLane[4] = 0b00000000;
      flagScoreChange += 1;
    }
  }


  if ( switchNew[SWITCH_LF_RET_LANE] && switchStateNow[SWITCH_LF_RET_LANE] ) //
  {
    if ( lightArrowLane[2] > 0)
    {
    lightArrowLane[2] = 0b00000000; // extinguish here
    solenoidFireFlag[SOLENOID_DIVERTER] = 250; // set solenoid to un-fire here
    lightArrowLane[3] = 0b11111111; // light upper arrow here
    }
    scorePlayer[player] += 500;
    flagScoreChange += 1;
  }

   if ( switchNew[SWITCH_LF_OUT_LANE] && switchStateNow[SWITCH_LF_OUT_LANE] ) //
  {
    scorePlayer[player] += 500;
    flagScoreChange += 1;
  }

  if ( switchNew[SWITCH_SHOOT_LANE] && switchStateNow[SWITCH_SHOOT_LANE] ) //
  {
    lightPlayer[player] = 0b11111111;
  if (flagLOutLane == 0)
  {
    if (lightBBonus[2] > 0) // spot letter if lit
    {
    indexRETRO = 1;
    j = 0;
      while ((indexRETRO > 0) && (j < 5))
      {
        if (lightLane[j] > 0)
        {
          lightLane[j] = 0b00000000;
          lightField[j] = 0b11111111;
          indexRETRO = 0;
        }
        j += 1;
      }
    }


  switch (countLOutLane)
      {
        default:
        case 0: // first hit
          scorePlayer[player] += ((countLOutLane + 1) * 2500);
          flagScoreChange += 1;

          lightLOutLane[0] = 0b00000000;
          lightLOutLane[1] = 0b11111111;
          lightLOutLane[2] = 0b00000000;
          lightLOutLane[3] = 0b00000000;

          countLOutLane += 1;
          flagLOutLane = 1;
        break;

        case 1:
          scorePlayer[player] += ((countLOutLane + 1) * 2500);
          flagScoreChange += 1;

          lightLOutLane[0] = 0b00000000;
          lightLOutLane[1] = 0b00000000;
          lightLOutLane[2] = 0b11111111;
          lightLOutLane[3] = 0b00000000;

          countLOutLane += 1;
          flagLOutLane = 1;
        break;

        case 2:
          scorePlayer[player] += ((countLOutLane + 1) * 2500);
          flagScoreChange += 1;

          lightLOutLane[0] = 0b00000000;
          lightLOutLane[1] = 0b00000000;
          lightLOutLane[2] = 0b00000000;
          lightLOutLane[3] = 0b11111111;

          countLOutLane += 1;
          flagLOutLane = 1;
        break;

        case 3:
          scorePlayer[player] += ((countLOutLane + 1) * 2500);
          flagScoreChange += 1;

          lightLOutLane[0] = 0b00000000;
          lightLOutLane[1] = 0b00000000;
          lightLOutLane[2] = 0b00000000;
          lightLOutLane[3] = 0b11111111;

          flagLOutLane = 1;
        break;
      } // end switch
    } // end if
    // TO DO
    // SPOT RETRO LETTERS HERE
  }


                                                                            // ------------ SPINNERS ------------
  if ( switchNew[SWITCH_RT_SPINNER] && switchStateNow[SWITCH_RT_SPINNER] ) //
  {
    if(lightSpinner[1] > 0)
    {
      scorePlayer[player] += SCORE_SPINNER_BONUS;
      flagScoreChange += 1;
    }
    else
    {
      scorePlayer[player] += SCORE_SPINNER;
      flagScoreChange += 1;
    }
  }

   if ( switchNew[SWITCH_LF_SPINNER] && switchStateNow[SWITCH_LF_SPINNER] ) //
  {
    if(lightSpinner[0] > 0)
    {
      scorePlayer[player] += SCORE_SPINNER_BONUS;
      flagScoreChange += 1;
    }
    else
    {
      scorePlayer[player] += SCORE_SPINNER;
      flagScoreChange += 1;
    }
  }

                                                                              // -------- INLINE TARGET BANK ------------
  if ( switchNew[SWITCH_LANE_DROP_1] && switchStateNow[SWITCH_LANE_DROP_1] ) //
  {
    lightSpinner[0] = 0b11111111;
    lightSpinner[1] = 0b11111111;

    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;

    if (lightBBonus[1] > 0) // spot letter if lit
    {
      switch (countSpotLow)
      {
        default:
        case 0: // first hit
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b11111111;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[0] = 0b11111111; // "S"

          countSpotLow += 1;
        break;

        case 1:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b11111111;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[1] = 0b11111111; // "P"

          countSpotLow += 1;
        break;

        case 2:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus

          lightFieldLow[2] = 0b11111111; // "A"

          countSpotLow += 1;
        break;

        case 3:
          scorePlayer[player] += (5000); // award bonus
          flagScoreChange += 1;

          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus
        break;
      } // end switch
    }

  }

  if ( switchNew[SWITCH_LANE_DROP_2] && switchStateNow[SWITCH_LANE_DROP_2] ) //
  {
    lightBonusMult[0] = 0b11111111;
    lightBonusMult[1] = 0b00000000;
    lightBonusMult[2] = 0b00000000;
    bonusMultiplier = 2;

    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;

    if (lightBBonus[1] > 0) // spot letter if lit
    {
      switch (countSpotLow)
      {
        default:
        case 0: // first hit
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b11111111;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[0] = 0b11111111; // "S"

          countSpotLow += 1;
        break;

        case 1:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b11111111;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[1] = 0b11111111; // "P"

          countSpotLow += 1;
        break;

        case 2:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus

          lightFieldLow[2] = 0b11111111; // "A"

          countSpotLow += 1;
        break;

        case 3:
          scorePlayer[player] += (5000); // award bonus
          flagScoreChange += 1;

          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus
        break;
      } // end case switch
    }
  }


  if ( switchNew[SWITCH_LANE_DROP_3] && switchStateNow[SWITCH_LANE_DROP_3] ) //
  {
    lightBonusMult[0] = 0b00000000;
    lightBonusMult[1] = 0b11111111;
    lightBonusMult[2] = 0b00000000;
    bonusMultiplier = 4;

    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;

    if (lightBBonus[1] > 0 ) // spot letter if lit
    {
      switch (countSpotLow)
      {
        default:
        case 0: // first hit
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b11111111;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[0] = 0b11111111; // "S"

          countSpotLow += 1;
        break;

        case 1:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b11111111;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[1] = 0b11111111; // "P"

          countSpotLow += 1;
        break;

        case 2:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus

          lightFieldLow[2] = 0b11111111; // "A"

          countSpotLow += 1;
        break;

        case 3:
          scorePlayer[player] += (5000); // award bonus
          flagScoreChange += 1;

          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus
        break;
      } // end switch
    }
  }



  if ( switchNew[SWITCH_LANE_TARGET] && switchStateNow[SWITCH_LANE_TARGET] ) //
  {
    //TO DO
    // ADD CHECKS FOR DROP TARGETS DOWN
    // ROTATE VARIABLE FOR MULTIPLE TAGET HITS
    // ADVANCE HITS TO EXTRA BALL, SPECIAL, ETC

    switch (countSpecial)
    {
      default:
      case 0: // first hit
        lightBonusMult[0] = 0b00000000;
        lightBonusMult[1] = 0b00000000;
        lightBonusMult[2] = 0b11111111;
        bonusMultiplier = 6;

        countSpecial += 1;

        lightSpecial[0] = 0b11111111; // light 20K
        lightSpecial[1] = 0b00000000; // light extra ball
        lightSpecial[2] = 0b00000000; // light special
      break;

      case 1:
        countSpecial += 1;

        lightSpecial[0] = 0b00000000; // light 20K
        lightSpecial[1] = 0b11111111; // light extra ball
        lightSpecial[2] = 0b00000000; // light special

        scorePlayer[player] += 20000; // award 20K
      break;

      case 2:
        countSpecial += 1;

        lightSpecial[0] = 0b00000000; // light 20K
        lightSpecial[1] = 0b00000000; // light extra ball
        lightSpecial[2] = 0b11111111; // light special

        lightShootAgain = 0b11111111; // extra ball awarded
      break;

      case 3:
        lightSpecial[0] = 0b00000000; // light 20K
        lightSpecial[1] = 0b00000000; // light extra ball
        lightSpecial[2] = 0b11111111; // light special

        scorePlayer[player] += SCORE_SPECIAL; // award special
      break;
    }

    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;

    if (lightBBonus[1] > 0) // spot letter if lit
    {
      switch (countSpotLow)
      {
        default:
        case 0: // first hit
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b11111111;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[0] = 0b11111111; // "S"

          countSpotLow += 1;
        break;

        case 1:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b11111111;
          lightSpotLow[3] = 0b00000000; // bonus

          lightFieldLow[1] = 0b11111111; // "P"

          countSpotLow += 1;
        break;

        case 2:
          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus

          lightFieldLow[2] = 0b11111111; // "A"

          countSpotLow += 1;
        break;

        case 3:
          scorePlayer[player] += (5000); // award bonus
          flagScoreChange += 1;

          lightSpotLow[0] = 0b00000000;
          lightSpotLow[1] = 0b00000000;
          lightSpotLow[2] = 0b00000000;
          lightSpotLow[3] = 0b11111111; // bonus
        break;
      } // end switch
    }
  }


  // ---------------------------------------------
  //  ROLLOVER SWITCH
  if ( switchNew[SWITCH_ROLLOVER] ) // change
  {
    if (switchStateNow[SWITCH_ROLLOVER]) // on
    {
      switch (countStarRollover)
      {
        default:
        case 0: // first hit
          countStarRollover += 1;
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b11111111;
          lightStarRollover[2] = 0b00000000;
          lightStarRollover[3] = 0b00000000;
          lightStarRollover[4] = 0b00000000;
        break;

        case 1:
          countStarRollover += 1;
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b00000000;
          lightStarRollover[2] = 0b11111111;
          lightStarRollover[3] = 0b00000000;
          lightStarRollover[4] = 0b00000000;
        break;

        case 2:
          countStarRollover += 1;
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b00000000;
          lightStarRollover[2] = 0b00000000;
          lightStarRollover[3] = 0b11111111;
          lightStarRollover[4] = 0b00000000;
        break;

        case 3:
          countStarRollover += 1;
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b00000000;
          lightStarRollover[2] = 0b00000000;
          lightStarRollover[3] = 0b00000000;
          lightStarRollover[4] = 0b11111111;
        break;

        case 4:
          countStarRollover += 1;
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b00000000;
          lightStarRollover[2] = 0b00000000;
          lightStarRollover[3] = 0b00000000;
          lightStarRollover[4] = 0b11111111;
        break;

        case 5:
          //no change here, just add score
          scorePlayer[player] += (countStarRollover * 2000);

          lightStarRollover[0] = 0b00000000;
          lightStarRollover[1] = 0b00000000;
          lightStarRollover[2] = 0b00000000;
          lightStarRollover[3] = 0b00000000;
          lightStarRollover[4] = 0b11111111;
        break;
      }
    }
  }
  // ---------------------------------------------

  // ---------------------------------------------
  //  ROLLOVER SWITCH

  if ( switchNew[SWITCH_KICKER] ) // change
  {
    if (switchStateNow[SWITCH_KICKER]) // on
    {
    delay(250); // pause for effect
    delay(250); // pause for effect

   fireSolenoidNow(SOLENOID_KICKER);
    delay(50); // activation fire time
   unFireSolenoidNow(SOLENOID_KICKER);

    delay(100); // to debounce switch

    switch (countKickerArrow)
      {
        default:
        case 0: // first hit
          countKickerArrow += 1;
          scorePlayer[player] += (countKickerArrow * 3000);
          flagScoreChange += 1;
          lightKickerArrow[0] = 0b00000000;
          lightKickerArrow[1] = 0b11111111;
          lightKickerArrow[2] = 0b00000000;
        break;

        case 1:
          countKickerArrow += 1;
          scorePlayer[player] += (countKickerArrow * 3000);
          flagScoreChange += 1;
          lightKickerArrow[0] = 0b00000000;
          lightKickerArrow[1] = 0b00000000;
          lightKickerArrow[2] = 0b11111111;
        break;

        case 2:
          countKickerArrow += 1;
          scorePlayer[player] += (countKickerArrow * 3000);
          flagScoreChange += 1;
          lightKickerArrow[0] = 0b00000000;
          lightKickerArrow[1] = 0b00000000;
          lightKickerArrow[2] = 0b11111111;
        break;

        case 3:
          // no change, just add score
          scorePlayer[player] += (countKickerArrow * 3000);
          flagScoreChange += 1;
          lightKickerArrow[0] = 0b00000000;
          lightKickerArrow[1] = 0b00000000;
          lightKickerArrow[2] = 0b11111111;
        break;
      }
    }
  }




  //                                                                        ***********************************
  //                                                                        -----------------------------------
  //                                                              check for RETRO, SPA, and RETROSPA here after each loop
  //                                                                        -----------------------------------
  //                                                                        ***********************************

  // CHECK "SPA"
  indexSPA = 0;
  for (j = 0; j < 3; j++) // reset RETRO lights
  {
    //if (lightSpotLow[j] == 0)
    if (lightFieldLow[j] > 0)
    {
      indexSPA += 1;
    }
  }

  if (indexSPA > 2)
  {
    lightLow2Thump = 0b11111111;
    flagSPA = 1;
  }

// CHECK "RETRO"
  indexRETRO = 0;
  for (j = 0; j < 5; j++) // reset RETRO lights
  {
    if (lightLane[j] == 0)
    {
      indexRETRO += 1;
    }
  }

  if (indexRETRO > 4)
  {
    lightArrowLane[4] = 0b11111111;
    lightArrowLane[0] = 0b00000000;
    flagRETRO = 1;
  }

  if (flagSPA && flagRETRO)
  {
    flagSPA = 0;
    flagRETRO = 0;

    // -----------------------------
    //    reset lights
    //------------------------------
    for (j = 0; j < 5; j++) // reset RETRO lights
    {
      lightLane[j] = 0b11111111;
      lightField[j] = 0b00000000;
    }

    for (j = 0; j < 3; j++) // reset SPA lights
    {
      //lightSpotLow[j] = 0b11111111;
      lightFieldLow[j] = 0b00000000;
    }

    countSpotLow = 0;
    lightSpotLow[0] = 0b11111111;
    lightSpotLow[1] = 0b00000000;
    lightSpotLow[2] = 0b00000000;
    lightSpotLow[3] = 0b00000000; // bonus

    lightArrowLane[0] = 0b00000000; // bonus lanes
    lightArrowLane[4] = 0b00000000;
    lightLow2Thump = 0b00000000; // bonus thumper

    // -----------------------------
    //    check and set 18K and 36K bonus lights
    //------------------------------
    if ((lightFieldLow[3] == 0) && (lightFieldLow[4] == 0))
    {
      lightFieldLow[3] = 0b11111111;
      lightFieldLow[4] = 0b00000000;
    }
    else
    {
      //if (lightFieldLow[3] == 1)
      if (lightFieldLow[3] > 0)
      {
        lightFieldLow[3] = 0b00000000;
        lightFieldLow[4] = 0b11111111;
      }
      else
      {
        gameCredits += 1; // FREE GAME
      }
    }
  }


  //                                                                    *****************************************
  //                                                                    ------------------------------------------
  //                                                                      OUTHOLE CODE HERE
  //                                                                    ------------------------------------------
  //                                                                    *****************************************


  //TODO finish updating
  // NEW CODE HERE
   // ---------------------------------------------
  //  OUTHOLE
  //if (switchStateNow[SWITCH_OUTHOLE] == 1) // is on now
  while ((switchStateNow[SWITCH_OUTHOLE] == 1) && (mode != GAMEOVER)) // is on now
  {

    #if (DIAGNOSTIC_ENABLED == 1)
      Serial.print("Outhole = 1");
      Serial.print("\r\n");
    #endif

      // DEACTIVATE ALL COILS
      delay(100);
      for (j = 0; j < 16; j++)
      {
        unFireSolenoidNow(j);
        delay(100);
      }
      dataSolenoid = 0;
      dataSolenoidChime = 0;

    #if (DIAGNOSTIC_ENABLED == 1)
      Serial.print("Outhole = 2");
      Serial.print("\r\n");
    #endif

      // ------------------------------------------
      // SAVE STATE OF "RETRO SPA" LIGHTS
      // TODO convert this from array to automatic matrix
      playerRETRO[player] = 0; // reset individual players saved lights
      for (j = 0; j < 5; j++)
      {
        if( lightField[j] > 0)
        {
          playerRETRO[player] |= 1 << j; // save RETRO
          lightField[j] = 0;
          scorePlayer[player] += (bonusMultiplier * 2000.0);
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[player], player);
          #endif
        }
      }

      playerSPA[player] = 0; // reset individual players saved lights
      for (j = 0; j < 3; j++)
      {
        if( lightFieldLow[j] > 0)
        {
          playerSPA[player] |= 1 << j; // save SPA
          lightFieldLow[j] = 0;
          scorePlayer[player] += (bonusMultiplier * 2000.0);
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[player], player);
          #endif
        }
      }

      countSpotLowPlayer[player] = countSpotLow;

      playerSpecial[player] = 0; // reset individual players saved lights
      for (j = 0; j < 2; j++)
      {
        if( lightFieldLow[j+3] > 0)
        {
          playerSpecial[player] |= 1 << j; // save Specials
          lightFieldLow[j+3] = 0;
          scorePlayer[player] += ((j+1) * 18000.0 * bonusMultiplier);
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePlayer[player], player);
          #endif
        }
      }
       //  END SAVE STATE OF "RETRO SPA" LIGHTS
       // ------------------------------------------

    // --------------------------------------------------
    //    CHECK FOR SAME PLAYER SHOOT AGAIN
    if (lightShootAgain == 0)
    {
      player += 1;
      if (player > (numPlayers-1))
      {
        player = 0; //1;
        mode += 1; // ADVANCE mode here, all the way to game over
        gameBalls =  ( mode - 2); // ball in play
        flagScoreChange = 1;
      } // end if advance balls

    } // end if NOT shoot again

    else // shoot again
    {
      flagScoreChange = 1;
      //lightShootAgain = 0;
    }
    // --------------------------------------------------

    #if (DIAGNOSTIC_ENABLED == 1)
      Serial.print("Outhole = 3");
      Serial.print("\r\n");
    #endif

    // ------------------------------------------
      // LOAD STATE OF "RETRO SPA" LIGHTS

      // RESET LANE LIGHTS
      for (j = 0; j < 5; j++)
      {
        lightLane[j] = 0b11111111; // set upper RETRO lanes
      }

      for (j = 0; j < 5; j++)
      {
        indexRETRO = 0;
        indexRETRO |= 1 << j;
        if( (playerRETRO[player] & indexRETRO ) > 0)
        {
          lightLane[j] = 0b00000000; // clear upper RETRO lanes
          lightField[j] = 0b11111111; // set lower RETRO field
        }
      }

      lightSpotLow[0] = 0b11111111; // set spot S
      lightSpotLow[1] = 0b00000000; // clear spot P
      lightSpotLow[2] = 0b00000000; // clear spot A
      lightSpotLow[3] = 0b00000000; // clear spot 5k

      for (j = 0; j < 3; j++)
      {
        indexSPA = 0;
        indexSPA |= 1 << j;
        if( (playerSPA[player] & indexSPA ) > 0)
        {
          lightSpotLow[j] = 0b00000000; // clear spot SPA
          lightSpotLow[j+1] = 0b11111111; // set next spot SPA
          lightFieldLow[j] = 0b11111111; // set lower SPA field
        }
      }

      countSpotLow = countSpotLowPlayer[player];

      lightFieldLow[3] = 0b00000000;
      lightFieldLow[4] = 0b00000000;
      for (j = 0; j < 2; j++)
      {
        indexRETRO = 0;
        indexRETRO |= 1 << j;
        if( (playerSpecial[player] & indexRETRO ) > 0)
        {
          lightFieldLow[j+3] = 0b11111111; // set lower Special field
        }
      }
       //  END LOAD STATE OF "RETRO SPA" LIGHTS
       // ------------------------------------------

      // ------------------------------------------
      // reset lights for new ball
      countKickerArrow = 0;
      lightKickerArrow[0] = 0b11111111;
      lightKickerArrow[1] = 0b00000000;
      lightKickerArrow[2] = 0b00000000;

      flagLOutLane = 1;
      countLOutLane = 0;
      lightLOutLane[0] = 0b11111111;
      lightLOutLane[1] = 0b00000000;
      lightLOutLane[2] = 0b00000000;
      lightLOutLane[3] = 0b00000000;

      lightBonusMult[0] = 0b00000000;
      lightBonusMult[1] = 0b00000000;
      lightBonusMult[2] = 0b00000000;
      bonusMultiplier = 1;

      lightBBonus[0] = 0b00000000;
      lightBBonus[1] = 0b00000000;
      lightBBonus[2] = 0b00000000;

      lightLow2Thump = 0b00000000;

      lightArrowLane[2] = 0b00000000;
      // ------------------------------------------

    if (mode == GAMEOVER)
      {
      //mode += 1; // ADVANCE mode here, all the way to game over

        gameBalls =  0; // ball in play
        flagScoreChange = 1;

        for (j = 0; j < 4; j++)
        {
        // CHECK FOR HIGH SCORE
          player = j;
          scorePrev[player] = scorePlayer[player];
          if (scorePrev[player] > scoreHigh[0])
          {
            scoreHigh[3] = scoreHigh[2];
            scoreHigh[2] = scoreHigh[1];
            scoreHigh[1] = scoreHigh[0];
            scoreHigh[0] = scorePrev[player];
            // TO DO
            // ADD WRITE TO EEPROM HERE
          }
          else if (scorePrev[player] > scoreHigh[1])
          {
            scoreHigh[3] = scoreHigh[2];
            scoreHigh[2] = scoreHigh[1];
            scoreHigh[1] = scorePrev[player];
            // TO DO
            // ADD WRITE TO EEPROM HERE
          }
          else if (scorePrev[player] > scoreHigh[2])
          {
            scoreHigh[3] = scoreHigh[2];
            scoreHigh[2] = scorePrev[player];
            // TO DO
            // ADD WRITE TO EEPROM HERE
          }
          else if (scorePrev[player] > scoreHigh[3])
          {
            scoreHigh[3] = scorePrev[player];
            // TO DO
            // ADD WRITE TO EEPROM HERE
          }
        } // end for j
        #if (SCORE_ENABLED == 1)
          sendBallCreditToDisplay(gameCredits, gameBalls);
        #endif
      } // end if GAME OVER
      //------------------------------------------------------------

      else // not GAME OVER
      {
        #if (SCORE_ENABLED == 1)
          sendBallCreditToDisplay(gameCredits, gameBalls);
        #endif

        delay(250);
        delay(250); // increased delay x4
        delay(250);
        delay(250);

        if ( switchStateNow[SWITCH_LANE_DROP_1] || switchStateNow[SWITCH_LANE_DROP_2] ||switchStateNow[SWITCH_LANE_DROP_3] )
        {
          // RESET Upper Drop Target
          //
          //delay(200); //debounce
          fireSolenoidNow(SOLENOID_INLINE_DROP);
          delay(DELAY_DROP_TARGET); //debounce
          unFireSolenoidNow(SOLENOID_INLINE_DROP);
          delay(250);
          delay(250);
          // END RESET DROP TARGETS
        }

        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);

        // enable shoot again for a few seconds after ball comes out
        if ((lightShootAgain > 128) || (lightShootAgain == 0)) // real shoot again, or next ball
        // no multiple ball saves allowed
        {
        lightShootAgain = 0b01010101;
        flagShootAgainStart = 1; //1;
        ShootAgainShortLoop = DELAY_BALL_SAVE; //35; //50;
        ShootAgainLongLoop = DELAY_BALL_SAVE; //35; //50;
        }

        solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 254; // anything above 253 is constant on

        // RESET LIGHTS FOR NEW BALL
        lightPlayer[0] = 0b00000000;
        lightPlayer[1] = 0b00000000;
        lightPlayer[2] = 0b00000000;
        lightPlayer[3] = 0b00000000;
        lightPlayer[player] = 0b10101010;

        lightArrowLane[2] = 0b11111111;
        lightArrowLane[1] = 0b00000000;
        lightArrowLane[3] = 0b00000000;

        solenoidFireFlag[SOLENOID_DIVERTER] = 254;

        lightStarRollover[0] = 0b11111111;
        lightStarRollover[1] = 0b00000000;
        lightStarRollover[2] = 0b00000000;
        lightStarRollover[3] = 0b00000000;
        lightStarRollover[4] = 0b00000000;
        countStarRollover = 0;

        lightSpinner[0] = 0b00000000;
        lightSpinner[1] = 0b00000000;

      } // end else if mode gameover

      #if (DIAGNOSTIC_ENABLED == 1)
        Serial.print("Outhole = 4");
        Serial.print("\r\n");
      #endif

        // todo - call sub here
        // START SCAN

        scanPlayfieldSwitches(); // this is to check outhole switch, escape while loop

    delay (100);

  } // end while outhole now



  //                                                                 *****************************************
  //                                                                 ---------------------------------------------
  //                                                                          END OUTHOLE / BONUS CALC
  //                                                                 ---------------------------------------------
  //                                                                 *****************************************


    break;  // end case: BALLONE through BALLFIVE

  case GAMEOVER:  // -----------------------------------------------------------------------------------------

    // TURN OFF SOLENOIDS
    dataSolenoid = 0; // clear all
    sendPDBCommand(0, PDB_COMMAND_WRITE, 0, dataSolenoid);
    dataSolenoidChime = 0; // clear all
    sendPDBCommand(0, PDB_COMMAND_WRITE, 1, dataSolenoidChime);

    // DISABLE FLIPPER RELAY
    selectSolenoid = SOLENOID_FLIPPER_RELAY; //1;
    solenoidFireFlag[selectSolenoid] = 0; // anything above 253 is constant on
    // no need to time here, should be constant on

    // >>> output lights for game over

    gameBalls =  0; //( mode - 2); // ball in play
    flagScoreChange = 1;

    #if (SCORE_ENABLED == 1)
      sendBallCreditToDisplay(gameCredits, gameBalls);
    #endif

    // -------------------------------------
    //  GAME OVER LIGHTS
    lightPlayer1 = 0b00000000; //0;
    lightPlayer2 = 0b00000000; //0;
    lightHighScore = 0b00000000; //0;
    lightCredit = 0b11111111; //0;
    lightTilt = 0b00000000;
    lightBall = 0b11111111; //0;
    lightGame = 0b11111111; //0;
    lightOver = 0b11111111; //0;
    // -----------------------------------
    // -----------------------------------

    // PAUSE
    // TO DO add-a-ball ???

    // RESET LITE MEMORY BACK TO ZEROS
    for (j = 0; j < 5; j++)
    {
      playerRETRO[j] = 0;
      playerSPA[j] = 0;
      playerSpecial[j] = 0;
    }

    mode = ATTRACT_INIT;
    break;

  } // end switch game mode





  //                                                                        ***********************************
  //                                                                        -----------------------------------
  //                                                                            SEND LIGHTS OUT
  //                                                                        -----------------------------------
  //                                                                        ***********************************

#if ((LIGHTS_ENABLED == 1) && (DIAGNOSTIC_ENABLED == 0))
//#if ((LIGHTS_ENABLED == 1))
    switch (lightTransmitSequence)
    {
    default:
      break;

    case 1:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 2:
      Serial.write(1);
      break;
    case 3:
      Serial.write(lightLane[0]);
      break;
    case 4:
      Serial.write(lightLane[1]);
      break;
    case 5:
      Serial.write(lightLane[2]);
      break;
    case 6:
      Serial.write(lightLane[3]);
      break;
    case 7:
      Serial.write(lightLane[4]);
      break;
    case 8:
      Serial.write(lightKickerArrow[0]);
      break;
    case 9:
      Serial.write(lightKickerArrow[1]);
      break;
    case 10:
      Serial.write(lightKickerArrow[2]);
      break;
    case 11:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 12:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 13:
      Serial.write(2);
      break;
    case 14:
      Serial.write(lightArrowLane[0]);
      break;
    case 15:
      Serial.write(lightArrowLane[1]);
      break;
    case 16:
      Serial.write(lightArrowLane[3]);
      break;
    case 17:
      Serial.write(lightArrowLane[4]);
      break;
    case 18:
      Serial.write(lightSpotLow[0]);
      break;
    case 19:
      Serial.write(lightSpotLow[1]);
      break;
    case 20:
      Serial.write(lightSpotLow[2]);
      break;
    case 21:
      Serial.write(lightSpotLow[3]);
      break;
    case 22:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 23:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 24:
      Serial.write(3);
      break;
    case 25:
      Serial.write(lightStarRollover[0]);
      break;
    case 26:
      Serial.write(lightStarRollover[1]);
      break;
    case 27:
      Serial.write(lightStarRollover[2]);
      break;
    case 28:
      Serial.write(lightStarRollover[3]);
      break;
    case 29:
      Serial.write(lightStarRollover[4]);
      break;
    case 30:
      Serial.write(lightSpinner[1]);
      break;
    case 31:
      Serial.write(lightFieldLow[3]);
      break;
    case 32:
      Serial.write(lightFieldLow[4]);
      break;
    case 33:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 34:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 35:
      Serial.write(4);
      break;
    case 36:
      Serial.write(lightField[0]);
      break;
    case 37:
      Serial.write(lightField[1]);
      break;
    case 38:
      Serial.write(lightField[2]);
      break;
    case 39:
      Serial.write(lightField[3]);
      break;
    case 40:
      Serial.write(lightField[4]);
      break;
    case 41:
      Serial.write(lightFieldLow[0]);
      break;
    case 42:
      Serial.write(lightFieldLow[1]);
      break;
    case 43:
      Serial.write(lightFieldLow[2]);
      break;
    case 44:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 45:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 46:
      Serial.write(5);
      break;
    case 47:
      Serial.write(lightSpecial[0]);
      break;
    case 48:
      Serial.write(lightSpecial[1]);
      break;
    case 49:
      Serial.write(lightSpecial[2]);
      break;
    case 50:
      Serial.write(lightBBonus[1]);
      break;
    case 51:
      Serial.write(lightBonusMult[0]); // WIRED TO 0
      break;
    case 52:
      Serial.write(lightBonusMult[2]); // WIRED TO 2
      break;
    case 53:
      Serial.write(lightBonusMult[1]);  //WIRED TO 1
      break;
    case 54:
      Serial.write(lightShootAgain);
      break;
    case 55:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 56:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 57:
      Serial.write(6);
      break;
    case 58:
      Serial.write(lightLOutLane[0]);
      break;
    case 59:
      Serial.write(lightLOutLane[1]);
      break;
    case 60:
      Serial.write(lightLOutLane[2]);
      break;
    case 61:
      Serial.write(lightLOutLane[3]);
      break;
    case 62:
      Serial.write(lightBBonus[2]);
      break;
    case 63:
      Serial.write(lightArrowLane[2]);
      break;
    case 64:
      Serial.write(lightSpinner[0]);
      break;
    case 65:
      Serial.write(lightLow2Thump);
      break;
    case 66:
      Serial.write("\r\n");        // sends five bytes
      break;


    case 67:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 68:
      Serial.write(7);
      break;
    case 69:
      Serial.write(lightOver);
      break;
    case 70:
      Serial.write(lightGame);
      break;
    case 71:
      Serial.write(lightTilt);
      break;
    case 72:
      Serial.write(lightHighScore);
      break;
    case 73:
      Serial.write(lightPlayer[3]);
      break;
    case 74:
      Serial.write(lightPlayer[2]);
      break;
    case 75:
      Serial.write(lightPlayer[1]);
      break;
    case 76:
      Serial.write(lightPlayer[0]);
      break;
    case 77:
      Serial.write("\r\n");        // sends five bytes

      lightTransmitSequence = 0; // start over from beginning
      sendLightOut = 0; // reset flag

      break;

    } // END SWITCH

    lightTransmitSequence += 1; // start over from beginning

#endif // #if (LIGHTS_ENABLED == 1)

  //                                                                      *******************************
  //                                                                      -------------------------------
  //                                                                          FIRE SOLENOIDS HERE
  //                                                                      -------------------------------
  //                                                                      *******************************

// TODO - check to see if this can be skipped when no data changed, without timing out the controller
//---------------------------------------
//  SELECT BANK zero (really one)
//---------------------------------------
  dataSolenoid = 0;
  for (j = 0; j < 8; j++)
  {
    selectSolenoid = j;
    if (solenoidFireFlag[j] > 0)
    {
      dataSolenoid |= 1 << selectSolenoid; // set solenoid
      if ( solenoidFireFlag[j] < 253 )
      {
        solenoidFireFlag[j] -= 1; // ramp down to zero
      }
    }
  }
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, dataSolenoid);  // 1 == MAIN BANK

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Solenoid Bank 1 Data = ");
    Serial.print(dataSolenoid); // TODO check this formatting later
    Serial.print("\r\n");
  #endif
//---------------------------------------


//---------------------------------------
//  SELECT BANK one (really mapped to zero)
//---------------------------------------
  dataSolenoidChime = 0;
  for (j = 0; j < 8; j++)
  {
    selectSolenoid = j;
    if (solenoidFireFlag[j + 8] > 0)
    {
      dataSolenoidChime |= 1 << selectSolenoid; // set solenoid
      if ( solenoidFireFlag[j + 8] < 253 )
      {
      solenoidFireFlag[j + 8] -= 1; // ramp down to zero
      }
    }
  }
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, dataSolenoidChime);  // 0 == AUX BANK

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Solenoid Bank 0 Data = ");
    Serial.print(dataSolenoidChime); // TODO check this formatting later
    Serial.print("\r\n");
  #endif
//---------------------------------------





  //                                                                      ****************************************
  //                                                                      ----------------------------------------
  //                                                                          Send PinScore test data
  //                                                                      ----------------------------------------
  //                                                                      ****************************************

#if (SCORE_ENABLED == 1)

  if (flagScoreChange > 0)
  {
    // new 121207 BC
    if (flagScoreChange == 1) // only send out score when all is quiet for a minium number of cycles
    {
      sendScoreToDisplay(scorePlayer[player],player);
      //playScoreChime(scorePlayer[player], prevScoreChange); // TO DO : reset this to current score each player change
      prevScoreChange = scorePlayer[player]; // reset delta to zero
      //sendBallCreditToDisplay(gameCredits,gameBalls); // enable if balls or credits change only
      flagScoreChange = 0;
    } // end if flagscorechange == 1
    else
    {
      flagScoreChange -= 1; // count down based on how fast things are moving
    }
  } // end if flagScoreCHange > 0

#endif // score enable



} // end MAIN LOOP


//-------------------------------
//  start sub-routines
//-------------------------------

void sendPDBCommand(byte addr, byte command, byte bankAddr, byte data)
{
  byte cmdWord[5];
  cmdWord[0] = addr;
  cmdWord[1] = command;
  cmdWord[2] = bankAddr;
  cmdWord[3] = (data >> 4) & 0xF;
  cmdWord[4] = data & 0xF;

  // Turn off interrupts so the transfer doesn't get interrupted.
  noInterrupts();
  // Hardcode transfers to minimize IDLEs between transfers.
  // Using a for-loop adds 5 extra IDLEs between transfers.
  SPI.transfer(cmdWord[0]);
  SPI.transfer(cmdWord[1]);
  SPI.transfer(cmdWord[2]);
  SPI.transfer(cmdWord[3]);
  SPI.transfer(cmdWord[4]);
  // Re-enable interrupts
  interrupts();

  return;
}

// not used???
void sendBonusScore(long ScoreX_in, byte player_in)
{
  Score100K = (ScoreX_in/100000.0);
  Score10K = ((ScoreX_in-(Score100K*100000.0))/10000.0);
  ScoreK = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0))/1000.0);
  Score100 = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0))/100.0);
  Score10 = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0)-(Score100*100.0))/10.0);
  Score1 = (ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0)-(Score100*100.0)-(Score10*10.0));
  wireWriteData[0] = ASCII_DOLLAR;
  wireWriteData[1] = ASCII_C;
  wireWriteData[2] = ASCII_A;
  wireWriteData[3] = ASCII_N;
  wireWriteData[4] = (player_in + 4); // PinScore Unit IDs, 4 thru 7 //(player + 4); // PinScore ID
  wireWriteData[5] = Score100K;
  wireWriteData[6] = Score10K;
  wireWriteData[7] = ScoreK;
  wireWriteData[8] = Score100;
  wireWriteData[9] = Score10;
  wireWriteData[10] = Score1;
  wireWriteData[11] = ASCII_CR;

  Wire.beginTransmission(4); // transmit to player one Arduino LED
  Wire.write(wireWriteData,12);
  Wire.endTransmission();    // stop transmitting

  return;
}

void sendScoreToDisplay(long ScoreX_in, byte player_in)
{
  Score100K = (ScoreX_in/100000.0);
  Score10K = ((ScoreX_in-(Score100K*100000.0))/10000.0);
  ScoreK = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0))/1000.0);
  Score100 = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0))/100.0);
  Score10 = ((ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0)-(Score100*100.0))/10.0);
  Score1 = (ScoreX_in-(Score100K*100000.0)-(Score10K*10000.0)-(ScoreK*1000.0)-(Score100*100.0)-(Score10*10.0));
  wireWriteData[0] = ASCII_DOLLAR;
  wireWriteData[1] = ASCII_C;
  wireWriteData[2] = ASCII_A;
  wireWriteData[3] = ASCII_N;
  wireWriteData[4] = (player_in + 4); // PinScore Unit IDs, 4 thru 7 //(player + 4); // PinScore ID
  wireWriteData[5] = Score100K;
  wireWriteData[6] = Score10K;
  wireWriteData[7] = ScoreK;
  wireWriteData[8] = Score100;
  wireWriteData[9] = Score10;
  wireWriteData[10] = Score1;
  wireWriteData[11] = ASCII_CR;

  Wire.beginTransmission(4); // transmit to player one Arduino LED
  Wire.write(wireWriteData,12);
  Wire.endTransmission();    // stop transmitting

  return;
}

void sendBallCreditToDisplay(byte gameCredits_in, byte gameBalls_in)
{
  wireWriteData[0] = ASCII_DOLLAR;
  wireWriteData[1] = ASCII_C;
  wireWriteData[2] = ASCII_A;
  wireWriteData[3] = ASCII_N;
  wireWriteData[4] = (8); // this is ID for Credit-Ball PinScore display
  wireWriteData[5] = 10; // 10 displays as blank
  wireWriteData[6] = (gameCredits_in/10);
  wireWriteData[7] = (gameCredits_in-(wireWriteData[6]*10));
  wireWriteData[8] = 10; //10 displays as blank
  wireWriteData[9] = 0;
  wireWriteData[10] = gameBalls_in;
  wireWriteData[11] = ASCII_CR;

  Wire.beginTransmission(4); // transmit to player one Arduino LED
  Wire.write(wireWriteData,12);
  Wire.endTransmission();    // stop transmitting

  return;
}

void rotateBumperBonus(void)
{
  bumperBonusCount += 1;
  if (bumperBonusCount > 2)
    {
      bumperBonusCount = 0;
    }
  for (k = 0; k < 3; k++)
  {
    lightBBonus[k] = 0b00000000;
  }
  lightBBonus[bumperBonusCount] = 0b11111111;

  return;
}

void rotateArrowLaneBonus(void)
{
  if (lightArrowLane[2] == 0) // outlane diverter
  {
    if (lightArrowLane[3] > 0)
    {
      lightArrowLane[3] = 0b00000000;
      lightArrowLane[1] = 0b11111111;
    }
    else
    {
      lightArrowLane[1] = 0b00000000;
      lightArrowLane[3] = 0b11111111;
    }
  }
  if (flagRETRO == 1) // was 0
  {
    if (lightArrowLane[4] > 0)
    {
      lightArrowLane[4] = 0b00000000;
      lightArrowLane[0] = 0b11111111;
    }
    else
    {
      lightArrowLane[0] = 0b00000000;
      lightArrowLane[4] = 0b11111111;
    }
  }
  else // added case where no flag = no lights
  {
    lightArrowLane[0] = 0b00000000;
    lightArrowLane[4] = 0b00000000;
  }

  return;
}

void fireSolenoidNow(byte selectSolenoid_in)
{
    switch(selectSolenoid_in)
  {
    case (0):
    case (1):
    case (2):
    case (3):
    case (4):
    case (5):
    case (6):
    case (7):
      selectBank = 0; //
      selectSolenoid = selectSolenoid_in; //1;
      selectSolenoid -= (selectBank*8); //1;
      dataSolenoid |= 1 << selectSolenoid; // set solenoid
      sendPDBCommand(board, PDB_COMMAND_WRITE, selectBank, dataSolenoid);
    break;

    case (8):
    case (9):
    case (10):
    case (11):
    case (12):
    case (13):
    case (14):
    case (15):
      selectBank = 1; //
      selectSolenoid = selectSolenoid_in; //1;
      selectSolenoid -= (selectBank*8); //1;
      dataSolenoidChime |= 1 << selectSolenoid; // set solenoid
      sendPDBCommand(board, PDB_COMMAND_WRITE, selectBank, dataSolenoidChime);
    break;

    default:
    break;
  }

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Solenoid Fire Now =");
    Serial.print(selectSolenoid_in); // TODO check this formatting later
    Serial.print("\r\n");
  #endif

  return;
}

void unFireSolenoidNow(byte selectSolenoid_in)
{
  switch(selectSolenoid_in)
  {
    case (0):
    case (1):
    case (2):
    case (3):
    case (4):
    case (5):
    case (6):
    case (7):
      selectBank = 0; // 0 or 1
      selectSolenoid = selectSolenoid_in; //1;
      selectSolenoid -= (selectBank*8); //1;
      dataSolenoid &= ~(1 << selectSolenoid); // set solenoid
      sendPDBCommand(board, PDB_COMMAND_WRITE, selectBank, dataSolenoid);
    break;

    case (8):
    case (9):
    case (10):
    case (11):
    case (12):
    case (13):
    case (14):
    case (15):
      selectBank = 1; // 0 or 1
      selectSolenoid = selectSolenoid_in; //1;
      selectSolenoid -= (selectBank*8); //1;
      dataSolenoidChime &= ~(1 << selectSolenoid); // set solenoid
      sendPDBCommand(board, PDB_COMMAND_WRITE, selectBank, dataSolenoidChime);
    break;

    default:
    break;
  }

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Solenoid Fire Now =");
    Serial.print(selectSolenoid_in); // TODO check this formatting later
    Serial.print("\r\n");
  #endif

  return;
}

void scanPlayfieldSwitches(void)
{
  flagNewSwitch = 0;
  // SET DRIVE LINES HERE
  for (j = 0; j < 5; j++)
  {
    // START ALL LOW (no signal)
    digitalWrite(14, LOW); // pins 14-17
    digitalWrite(15, LOW); // pins 14-17
    digitalWrite(16, LOW); // pins 14-17
    digitalWrite(17, LOW); // pins 14-17
    digitalWrite(18, LOW); // pins 14-17

    // DRIVE ONE LINE HIGH
    digitalWrite((j+14), HIGH); // pins 14-17

    // WAIT HERE FOR RISE TIME
    delayMicroseconds(DELAY_SWITCH_SCAN) ;

    // START SCAN
    for (i = 0; i < 8; i++)
    {
      switchStatePrev[((j*8) + i)] = switchStateNow[((j*8) + i)]; // rotate variable
      switchStateNow[((j*8) + i)] = digitalRead(i + 2); // pins 2-9

      // check for a "new" state
    #if (DEBOUNCE_MODE)
      if ((switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)]) || (switchDebounce[((j*8) + i)] > 0))
    #else
      if ( switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)])
    #endif
      {
        switchNew[((j*8) + i)] = 0; // same as old
      } // end if

      else // must be new if not old and new equals one
      {
        if (switchStateNow[((j*8) + i)] == 1)
        {
          switchNew[((j*8) + i)] = 1; // new
          flagNewSwitch = 1;

          #if (DEBOUNCE_MODE)
            switchDebounce[((j*8) + i)] = SWITCH_DEBOUNCE_DURATION; // set timer
          #endif

          #if (DIAGNOSTIC_ENABLED == 1)
            Serial.print("Switch = ");
            Serial.print((j*8) + i); // TODO check this formatting later
            Serial.print("\r\n");
          #endif

          // DISPLAY SWITCH NUMBER FOR DIAGNOSTIC, REMOVE LATER
          gameCredits = ((j*8) + i);

        }
      } // end else
    } // end for i
  } // end for j

  #if (DEBOUNCE_MODE)
    for (j = 0; j < 40; j++)
    {
      if (switchDebounce[j] > 0)
      {
        switchDebounce[j] -= 1; // ramp down to zero
      }
    }
  #endif
}
