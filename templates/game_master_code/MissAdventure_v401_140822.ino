// TO DO
// add score displays - move credits to unit 8, player score units 4 thru 7.
// add high score rotation - top four on displays 4 thru 7.
// mod the enumuration of switches, solemoids, based on as-built chart
//

#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#define pinMOSI 11
#define PDB_COMMAND_WRITE 1

#define DEBOUNCE_MODE 1 //1

#define FREEPLAY 1

#define LIGHTS_ENABLED 1
#define SCORE_ENABLED 1
#define GAMERULES_ENABLED 1
#define DIAGNOSTIC_ENABLED 0 // uses same serial port as lights
#define MULTIBALL_ENABLED 1

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

unsigned char upDown=1;           // start off going UP
unsigned char cylon=0;  // determines which LED is on 0 to 4

unsigned char selectDigit = 0;     // start no digit selected
unsigned char digitBCD = 0;         // start no digit selected
const unsigned char selectLatch = 12;     // start no digit selected
const unsigned char selectBlank = 13;     // start no digit selected
const unsigned char selectOffset = 0;     // start no digit selected

unsigned char selectSolenoid = 0;             //
unsigned char dataSolenoid = 0;                //
unsigned char dataSolenoidChime = 0;   //
unsigned char selectBank = 1;    //

unsigned char j = 0;         //
unsigned char i = 0;         //
unsigned char k = 0;        //

byte lastData = 0;
byte board = 0;

// --------------------------
//    SWITCHES
unsigned char switchStateNow[40];        // start no digit selected
unsigned char switchStatePrev[40];        // start no digit selected
unsigned char switchNew[40];   //
unsigned char switchDebounce[40];

unsigned char solenoidFireFlag[16];        // start no digit selected

unsigned char gameCredits = 99; //5;      //
unsigned char gameCreditsX10 = 9; //5; //
unsigned char gameCreditsX1 = 9; //5;   //
unsigned char gameBalls = 0;      //
unsigned char gameTilted = 0;    //

unsigned long scorePlayerOne = 0; //
unsigned long scorePlayerTwo = 0; //

unsigned char player = 0; // 0== player one, 1 == player two
unsigned int scoreBonus = 0; //

long scoreHigh[5] = {15785,987,654,321,0};  //
long scoreTemp1 = 0; //
long scoreTemp2 = 0; //
long scoreTemp3 = 0; //
long scoreTemp4 = 0; //

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

unsigned char lightLane[5] = {255,255,255,255,255};         // RETRO
unsigned char lightArrowLane[5] = {0,0,0,0,0};    //
unsigned char lightKickerArrow[3] = {0,0,0};        //
unsigned char countKickerArrow = 0;      //
unsigned char lightStarRollover[5] = {0,0,0,0,0}; //
unsigned char countStarRollover = 0;      //
unsigned char lightSpotLow[5] = {0,0,0,0,0};        //            //
unsigned char countSpotLow = 0;
unsigned char lightField[5] = {0,0,0,0,0};    // RETRO
unsigned char lightFieldLow[5] = {0,0,0,0,0};  // SPA ++
unsigned char lightSpinner[2] = {0,0};     //
unsigned char lightLOutLane[4] = {0,0,0,0};          //
unsigned char countLOutLane = 0;
unsigned char flagLOutLane = 0;
unsigned char lightBonusMult[4] = {0,0,0,0};       //
unsigned char lightBBonus[3] = {0,0,0};  // RETROSPA bumper bonus count, lights lane to spot letter
unsigned char lightSpecial[3] = {0,0,0};   // 20k, extra ball, special
unsigned char countSpecial= 0;

unsigned char lightHunterUp[6] = {255,255,0,0,255,255};               //
unsigned char lightHunterLow[6] = {0,0,0,0,0,0};                //
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

//MISS ADVENTURE LIGHTS
unsigned char lightUpTarget[9] = {0,0,0,0,0,0,0,0,0};
unsigned char lightLowTarget[7] = {0,0,0,0,0,0,0};
//unsigned char lightMissAdventure[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char lightMissAdventure[4][13];
unsigned char lightMissAdventureBonusCount[13];
unsigned char lightStar[3] = {0,0,0};
unsigned char lightRollover[3] = {0,0,0};
//unsigned char lightBonus[4] = {0,0,0,0};
unsigned char lightBonus[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char lightBonusCount[4] = {0,0,0,0};
unsigned char lightUpLfArch = 0;
unsigned char lightUpRtArch = 0;
unsigned char lightLowLfSling = 0;
unsigned char lightLowRtSling = 0;
unsigned char lightUpLfThump = 0;
unsigned char lightUpRtThump = 0;
unsigned char lightShootAgain = 0;
unsigned char lightLowThump = 0;
unsigned char lightLfRamp = 0;
unsigned char lightRtRamp = 0;
unsigned char lightRtOutLane = 0;
unsigned char lightLfOutLane = 0;

// BACKGLASS LIGHTS
unsigned char lightPlayer1 = 0b10101010; //0;
unsigned char lightPlayer2 = 0b01010101; //0;
unsigned char lightPlayer[4] = {0,0,0,0}; //0b01010101; //0;
unsigned char lightHighScore = 0b11001100; //0;
unsigned char lightCredit = 0b00001111; //0;
unsigned char lightTilt = 0b11001010;
unsigned char lightBall = 0b00111100; //0;
unsigned char lightGame = 0b01001011; //0;
unsigned char lightOver = 0b00101011; //0;

unsigned char testFire = 0;           // SPA lit

unsigned char lightTest = 0;         // SPA lit

//  light indexes
unsigned char indexBBonus = 0;                // bonus spotting letters
unsigned char indexRETRO = 0;  // num lights spotted in RETRO
unsigned char indexSPA = 0;       // num lights spotted in SPA
unsigned char flagRETRO = 0;      // RETRO lit
unsigned char flagSPA = 0;           // SPA lit
unsigned char flagRETROSPA = 0;
unsigned char playerRETRO[5] = {0,0,0,0,0}; // holder for RETRO SPA lights
unsigned char playerSPA[5] = {0,0,0,0,0}; // holder for RETRO SPA lights
unsigned char playerSpecial[5] = {0,0,0,0,0}; // holder for 18k and 36k lights
unsigned char countSpotLowPlayer[5] = {0,0,0,0,0};

unsigned char sendLightOut = 0;               // flag to send light data
unsigned char idCAN = 0;              // id of 8-ch circuit (2 per board, 16 total)
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

unsigned char flagLoopRollover = 0;
unsigned char flagCheckMissAdventure = 0;
unsigned int flagLoopBallSave = 0;
unsigned char indexBonus = 0;
unsigned char indexMissAdventure = 0;
unsigned char flagNewSwitch = 0;
boolean flagDiagnostic = 0;

unsigned char indexPlayerStateMiss[4] = {0,0,0,0};
unsigned char indexPlayerStateAdventure[4] = {0,0,0,0};

unsigned char multiballCount = 1;
unsigned int launcherCountdown = 0;
// --------------------------


// modes
/*
enum
{
  ATTRACT_INIT, //?? needed ??
  ATTRACT,
  START,
  BALLONE,
  BALLTWO,
  BALLTHREE,
c
  GAMEOVER
};
*/

// modes
enum
{
  ATTRACT_INIT, //?? needed ??
  ATTRACT,
  START,
  BALLONE,
  BALLTWO,
  BALLTHREE,
  GAMEOVER,
  BALLFOUR,
  BALLFIVE
};

// flagLoopRollover
enum
{
  LEFT_LOOP,
  RIGHT_LOOP
};

// switches
enum
{
  SWITCH_UP_LF_KICK,
  SWITCH_UP_TARGET_1,
  SWITCH_UP_TARGET_2,
  SWITCH_UP_TARGET_3,
  SWITCH_UP_TARGET_7,
  SWITCH_UP_TARGET_8,
  SWITCH_UP_TARGET_9,
  SWITCH_UP_RT_KICK,

  SWITCH_DROP_1,
  SWITCH_DROP_2,
  SWITCH_DROP_3,
  SWITCH_DROP_4,
  SWITCH_UP_TARGET_4,
  SWITCH_UP_TARGET_5,
  SWITCH_UP_TARGET_6,
  SWITCH_15,

  SWITCH_LF_OUT_LANE,
  SWITCH_OUTHOLE,
  SWITCH_SHOOT_LANE,//SWITCH_MULTI_BALL,
  SWITCH_LOW_THUMP,
  SWITCH_RT_OUT_LANE,
  SWITCH_LOW_TARGET_5,
  SWITCH_LOW_TARGET_6,
  SWITCH_LOW_TARGET_7,

  SWITCH_LOW_TARGET_1,
  SWITCH_LOW_TARGET_2,
  SWITCH_LOW_TARGET_3,
  SWITCH_LOW_TARGET_4,
  SWITCH_ROLL_OVER_1,
  SWITCH_ROLL_OVER_2,
  SWITCH_ROLL_OVER_3,
  SWITCH_LOW_KICK,

  SWITCH_UP_LF_THUMP,
  SWITCH_UP_RT_THUMP,
  SWITCH_34,//SWITCH_SHOOT_LANE,
  SWITCH_35,
  SWITCH_COIN,
  SWITCH_START,
  SWITCH_MULTI_BALL_1,//SWITCH_38,
  SWITCH_MULTI_BALL_2//SWITCH_39
};

enum
{
  // SOLENOIDS BANK zero
  SOLENOID_UP_LF_KICK,
  SOLENOID_UPPER_DROP,
  SOLENOID_UP_RT_KICK,
  SOLENOID_UP_RT_THUMP,
  SOLENOID_UP_LF_THUMP,
  SOLENOID_LOW_LF_KICK,
  SOLENOID_LOW_RT_KICK,
  SOLENOID_FLIPPER_RELAY,

  // SOLENOIDS BANK one
  SOLENOID_BALL_SAVE,
  SOLENOID_OUTHOLE,
  SOLENOID_MULTI_BALL,
  SOLENOID_DIVERTER,
  SOLENOID_LOW_THUMP,
  SOLENOID_CHIME_1,
  SOLENOID_CHIME_2,
  SOLENOID_CHIME_3
};

#define SCORE_THUMPER 100
#define SCORE_THUMPER_UNLIT 10
#define SCORE_REV 400
#define SCORE_TARGET 1000
#define SCORE_TARGET_UNLIT 1
#define SCORE_DROP 250
#define SCORE_KICKER 3000
#define SCORE_BONUS_INIT 1000

#define SCORE_INLINE 250 // CHANGED 130531 BC
#define SCORE_SLING 3
#define SCORE_ROLLOVER 50
#define SCORE_SPINNER 100
#define SCORE_OUTHOLE 1000 //500
#define SCORE_OUTLANE 5000

#define SCORE_SPINNER_BONUS 1000 //
#define SCORE_SPECIAL 15000 //

#define NUM_BALLS 3 //5
#define SOLENOID_FIRE_DURATION 100 //50 //5 //10 //50
#define THUMPER_FIRE_DURATION 7 //10 //20 //5 //5 //10
#define UPPER_THUMP_FIRE_DURATION 10 //20 //10
#define OUTHOLE_FIRE_DURATION 6 //3 //5 //10
#define CHIME_FIRE_DURATION 6 //3 //5 //10
#define KICKER_FIRE_DURATION 40 //20 //5 //10
#define SLING_FIRE_DURATION 4 //3 //5 //10
#define MULTIBALL_FIRE_DURATION 250 //50 //5 //10 //50

#define DELAY_OUTHOLE 40 //
#define DELAY_DROP_TARGET 80 //
#define DELAY_MULTI_BALL 500 //300 //
#define DELAY_SWITCH_SCAN 400 //300 //400 //

#define SWITCH_DEBOUNCE_DURATION 10 //10 loops

unsigned char mode = ATTRACT_INIT;

void setup() {

#if ((LIGHTS_ENABLED == 1) || (DIAGNOSTIC_ENABLED == 1))
  Serial.begin(9600);           // start serial for output
#endif

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Starting Setup Function");
    Serial.print("\r\n");
  #endif

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



  //for (j = 0; j < 40; j++)
  for (j = 0; j < 16; j++)
  {
    solenoidFireFlag[j] = 0;
  }

// initial read of switches
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
    //delay(2) ;

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
    delayMicroseconds(DELAY_SWITCH_SCAN);
    //delayMicroseconds(200);
    //delay(2);
    //delay(1);

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

   // TODO check this delay later
  //delay(10) ; // 10ms loop time // TODO set correct time here

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
    #endif

    // --------------------------------
    // SET ATTRACT MODE LIGHTING HERE

    lightHighScore = 0b11001100; //0;
    lightCredit = 0b00001111; //0;
    lightTilt = 0b11001010;
    lightBall = 0b00111100; //0;
    lightGame = 0b01001011; //0;
    lightOver = 0b00101011; //0;

   // CHASER UNIT ONE AND TWO
   lightUpTarget[0] = 0b01010101;
   lightUpTarget[1] = 0b10101010;
   lightUpTarget[2] = 0b01010101;
   lightUpTarget[3] = 0b10101010;
   lightUpTarget[4] = 0b01010101;
   lightUpTarget[5] = 0b10101010;
   lightUpTarget[6] = 0b01010101;
   lightUpTarget[7] = 0b10101010;
   lightUpTarget[8] = 0b01010101;

   lightUpLfThump = 0b11001100;
   lightUpRtThump = 0b00110011;

   // CHASER UNIT THREE
   lightRollover[0] = 0b11000111; //0;
   lightRollover[1] = 0b01101110; //0;
   lightRollover[2] = 0b00111100; //0;

   lightLowTarget[3] = 0b00110011;

   //lightMissAdventure[0] = 0b00011111; //0;
   //lightMissAdventure[1] = 0b00111110; //0;
   //lightMissAdventure[2] = 0b01111100; //0;
   //lightMissAdventure[3] = 0b11111000; //0;

   for (i = 0; i < 4; i++)
   {
     lightMissAdventure[i][0] = 0b00011111; //0;
     lightMissAdventure[i][1] = 0b00111110; //0;
     lightMissAdventure[i][2] = 0b01111100; //0;
     lightMissAdventure[i][3] = 0b11111000; //0;
   }

   // CHASER UNIT FOUR
   lightShootAgain = 0b01100110;
   lightLowThump = 0b00110011;
   lightStar[0] = 0b00011111;
   lightStar[1] = 0b00111110;
   lightStar[2] = 0b01111100;
   lightLfOutLane = 0b11001100;
   lightLowTarget[0] = 0b11110000;
   lightLowTarget[1] = 0b00111100;

   // CHASER UNIT Five
   lightLowTarget[2] = 0b00001111;
   lightLfRamp = 0b01110111;

   //lightMissAdventure[4] = 0b10101010; //0;
   //lightMissAdventure[5] = 0b01010101; //0;
   //lightMissAdventure[6] = 0b10101010; //0;
   //lightMissAdventure[7] = 0b01010101; //0;
   //lightMissAdventure[8] = 0b10101010; //0;
   //lightMissAdventure[9] = 0b01010101; //0;

   for (i = 0; i < 4; i++)
   {
     lightMissAdventure[i][4] = 0b10101010; //0;
     lightMissAdventure[i][5] = 0b01010101; //0;
     lightMissAdventure[i][6] = 0b10101010; //0;
     lightMissAdventure[i][7] = 0b01010101; //0;
     lightMissAdventure[i][8] = 0b10101010; //0;
     lightMissAdventure[i][9] = 0b01010101; //0;
   }

   // CHASER UNIT SIX
   //lightMissAdventure[10] = 0b10101010; //0;
   //lightMissAdventure[11] = 0b01010101; //0;
   //lightMissAdventure[12] = 0b10101010; //0;

   for (i = 0; i < 4; i++)
   {
     lightMissAdventure[i][10] = 0b10101010; //0;
     lightMissAdventure[i][11] = 0b01010101; //0;
     lightMissAdventure[i][12] = 0b10101010; //0;
   }

   lightRtRamp = 0b10111011; //0;
   lightLowTarget[4] = 0b01111000;
   lightLowTarget[5] = 0b00111100;
   lightLowTarget[6] = 0b00011110;

   // CHASER UNIT SEVEN
   for (i = 0; i < 4; i++)
   {
     lightBonus[i][0] = 0b00110011; //0;
     lightBonus[i][1] = 0b01100110; //0;
     lightBonus[i][2] = 0b11001100; //0;
     lightBonus[i][3] = 0b10011000; //0;
   }

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


    if(switchNew[SWITCH_START]) // SHOOTER_LANE
    {
      if(switchStateNow[SWITCH_START])
      {
        dataSolenoid = 0;
        dataSolenoidChime = 0;

        mode = START;
        numPlayers = 1;

      // ---------------------------------------------
      // CHECK FOR LOST BALLS, AND WAIT TILL THEYRE FOUND
      // ---------------------------------------------

        // UPPER LEFT KICKER
       //if (switchStateNow[SWITCH_UP_LF_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_UP_LF_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UP_LF_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_UP_LF_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

       // UPPER RIGHT KICKER
       // todo change if to while if previous code works
       //if (switchStateNow[SWITCH_UP_RT_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_UP_RT_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

       // LOWER KICKER
       // todo change if to while if previous code works
       //if (switchStateNow[SWITCH_LOW_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_LOW_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

       /*
        if (switchStateNow[SWITCH_UP_RT_KICK])
       {
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce
       }

        if (switchStateNow[SWITCH_LOW_KICK])
       {
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce
       }
       */
      // END RESET KICKERS

  // ---------------------------------------------
  // NORMAL STARTUP - NO BALLS LOST
  // ---------------------------------------------

       // RESET Upper Drop Target
        //
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UPPER_DROP);
        delay(DELAY_DROP_TARGET); //debounce
        unFireSolenoidNow(SOLENOID_UPPER_DROP);
        delay(250); //debounce
        delay(250); //debounce
        // END RESET DROP TARGETS

        //todo MULTIBALL serve up ball w/ second solenoid
        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(DELAY_MULTI_BALL); //debounce
        unFireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(250); //debounce
        delay(250); //debounce
        //END MULTIBALL

        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);
        delay(250); //debounce
        delay(250); //debounce
        //END OUTHOLE

       // todo turn this into a real check of balls
       multiballCount = 1;

      }// end if start now
    }// end if start new

    break;

    // NEW GAME
  case START:  // -----------------------------------------------------------------------------------------


    solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 255; // anything above 253 is constant on

    // --------------------------
    //  RESET LIGHTS

    lightHighScore = 0b00000000; //0;
    lightCredit = 0b11111111; //0;
    lightTilt = 0b00000000;
    lightBall = 0b11111111; //0;
    lightGame = 0b00000000; //0;
    lightOver = 0b00000000; //0;

   // TODO add start lights here
    for (j = 0; j < 9; j++)
    {
      lightUpTarget[j] = 0b11111111;
    }
    for (j = 0; j < 7; j++)
    {
      lightLowTarget[j] = 0b11111111;
    }

    for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 14; j++)
      {
        lightMissAdventure[i][j] = 0b00000000;
      }
      for (j = 0; j < 4; j++)
      {
        lightBonus[i][j] = 0b00000000;
      }
    }

    //for (j = 0; j < 4; j++)
    //{
    //  lightBonus[j] = 0b00000000;
    //}

   lightRollover[0] = 0b11111111; //0;
   lightRollover[1] = 0b11111111; //0;
   lightRollover[2] = 0b11111111; //0;

   lightStar[0] = 0b00000000;
   lightStar[1] = 0b00000000;
   lightStar[2] = 0b00000000;

    lightLfRamp = 0;
    lightRtRamp = 0;
    lightLfOutLane = 0b11111111;
    lightRtOutLane = 0b11111111;

   lightUpLfThump = 0b00000000;
   lightUpRtThump = 0b00000000;
   lightLowThump = 0b00000000;

    gameBalls =  1; //( mode - 2); // ball in play


    // REMOVED FOR SINGLE PLAYER, ADD BACK FOR MULTI-PLAYER

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
          break;

         case 1: // normal starting case
         case 2: // normal starting case
         //case 3: // normal starting case
         //case 4: // normal starting case
         // TWO PLAYER ONLY
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
          break;

        default: // no change for now with two players max
          //gameBalls = 1;
          mode = START;
          numPlayers = 2;
          //numPlayers = 4;
          player = 0;
          scorePlayer[0] = 0;
          scorePlayer[1] = 0;
          scorePlayer[2] = 0;
          scorePlayer[3] = 0;
          break;

        } // end switch num plarers

      } // end if on and credits

    } // end switch start new


   // TODO check this, maybe just lightPlayer[player] = 0b10101010; ??
    lightPlayer[0] = 0b00000000;
    lightPlayer[1] = 0b00000000;
    lightPlayer[2] = 0b00000000;
    lightPlayer[3] = 0b00000000;
    for (j = 0; j < numPlayers; j++)
    {
      lightPlayer[j] = 0b10101010;
    }


    // OFFICIALLY START BALL ONE ONCE SHOOTER USED
    //  CHANGED TO WHEN FIRST SWITCH IS HIT...
    //if (switchStateNow[SWITCH_SHOOT_LANE] == 1) // is on now
    //{
     // if (switchStatePrev[SWITCH_SHOOT_LANE] == 0) // was off
     // {

      if (flagNewSwitch > 0) // indicates something was hit
      {
        mode = BALLONE;
        lightShootAgain = 0b10101010;
        flagShootAgainStart = 1;
        ShootAgainShortLoop = 60;
        ShootAgainLongLoop = 60;

        //solenoidFireFlag[SOLENOID_DIVERTER] = 255;

        lightPlayer[0] = 0b11111111;
        lightPlayer[1] = 0b00000000;
        lightPlayer[2] = 0b00000000;
        lightPlayer[3] = 0b00000000;
      }

    //}

   /*
   // -------------------------------------------
    // for debug, just start countdown and go to ball one
    mode = BALLONE;
    lightShootAgain = 0b10101010;
    flagShootAgainStart = 1;
    ShootAgainShortLoop = 60;
    ShootAgainLongLoop = 60;

    lightPlayer[0] = 0b11111111;
    lightPlayer[1] = 0b00000000;
    lightPlayer[2] = 0b00000000;
    lightPlayer[3] = 0b00000000;
    // -------------------------------------------
    */

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
  // BALL SAVE
  // ---------------------------------------------
  //if (switchStateNow[SWITCH_LF_OUT_LANE] == 1) // is on now
  //{
    //if (switchStatePrev[SWITCH_LF_OUT_LANE] == 0) // was off
    if (switchNew[SWITCH_LF_OUT_LANE] == 1) // was off
    {
    if (lightLfOutLane > 0)
      {
        lightLfOutLane = 0; // reset

        delay(50); //debounce

        // WAIT FOR SWITCH TO LIFT
        flagLoopBallSave = 0;
        //todo fix this so it doesnt stick
        //while ( (flagLoopBallSave < 30000) && (switchStateNow[SWITCH_LF_OUT_LANE] == 1))
        while ((switchStateNow[SWITCH_LF_OUT_LANE] == 1))
        {
          /*
          j = 2;
          i = 0;
          digitalWrite((j+14), HIGH); // pins 14-17
          switchStateNow[SWITCH_LF_OUT_LANE] = digitalRead(i + 2); // pins 2-9
          */
          scanPlayfieldSwitches();

          flagLoopBallSave += 1;

          //delay(8); //debounce


          delayMicroseconds(250) ; // was 250
          delayMicroseconds(250) ; // was 250
          delayMicroseconds(250) ; // was 250
          delayMicroseconds(250) ; // was 250

        }

        switchDebounce[SWITCH_LF_OUT_LANE] = 250;

        //delay(30) ; // sets fire delay
        delay(25) ; // sets fire delay

        //delay(flagLoopBallSave/10) ; // sets fire delay

        // THEN FIRE!!!
        fireSolenoidNow(SOLENOID_BALL_SAVE);
        solenoidFireFlag[SOLENOID_BALL_SAVE] = THUMPER_FIRE_DURATION; //50; // thumper one

        #if (DIAGNOSTIC_ENABLED == 1)
          Serial.print("Solenoid Fire Now = ");
          Serial.print(SOLENOID_BALL_SAVE); // TODO check this formatting later
          Serial.print(" SOLENOID_BALL_SAVE"); // TODO check this formatting later
          Serial.print("\r\n");
        #endif

      } // end if outlane lit
    } // end if prev 0
  //} // end if now 1


  // ---------------------------------------------
  // THUMPER LOW
  // ---------------------------------------------
    if (switchNew[SWITCH_LOW_THUMP] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_LOW_THUMP);
      solenoidFireFlag[SOLENOID_LOW_THUMP] = THUMPER_FIRE_DURATION; //50; // thumper one
      if (lightLowThump > 0)
      {
        scorePlayer[player] += SCORE_THUMPER;
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
  // THUMPER UP_LF
  // ---------------------------------------------
    if (switchNew[SWITCH_UP_LF_THUMP] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_UP_LF_THUMP);
      solenoidFireFlag[SOLENOID_UP_LF_THUMP] = UPPER_THUMP_FIRE_DURATION; //50; // thumper one
      if (lightUpLfThump > 0)
      {
        scorePlayer[player] += SCORE_THUMPER;
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
  // THUMPER UP_RT
  // ---------------------------------------------

    if (switchNew[SWITCH_UP_RT_THUMP] == 1) // was off
    {
      fireSolenoidNow(SOLENOID_UP_RT_THUMP);
      solenoidFireFlag[SOLENOID_UP_RT_THUMP] = UPPER_THUMP_FIRE_DURATION; //50; // thumper one
      if (lightUpRtThump > 0)
      {
        scorePlayer[player] += SCORE_THUMPER;
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
  // LOOP KICKER
  // ---------------------------------------------

    if (switchNew[SWITCH_ROLL_OVER_1] == 1) // was off
    {
      flagLoopRollover = RIGHT_LOOP;
      if (lightLfRamp > 0)
      {
        scorePlayer[player] += SCORE_ROLLOVER;
        flagScoreChange += 1;
      }
    }

    if (switchNew[SWITCH_ROLL_OVER_3] == 1) // was off
    {
      flagLoopRollover = LEFT_LOOP;
      if (lightRtRamp > 0)
      {
        scorePlayer[player] += SCORE_ROLLOVER;
        flagScoreChange += 1;
      }
    }

    if (switchNew[SWITCH_LOW_KICK] == 1) // was off
    {
      if (flagLoopRollover == RIGHT_LOOP)
      {
        fireSolenoidNow(SOLENOID_LOW_RT_KICK);
        solenoidFireFlag[SOLENOID_LOW_RT_KICK] = UPPER_THUMP_FIRE_DURATION; //50; // thumper one
      }
      else
      {
        fireSolenoidNow(SOLENOID_LOW_LF_KICK);
        solenoidFireFlag[SOLENOID_LOW_LF_KICK] = UPPER_THUMP_FIRE_DURATION; //50; // thumper one
      }
    }

  // ---------------------------------------------
  // UPPER LEFT KICKER
  // ---------------------------------------------
  if (switchStateNow[SWITCH_UP_LF_KICK] == 1) // is on now
  {
    if (switchNew[SWITCH_UP_LF_KICK] == 1) // was off
    {
      switchDebounce[SWITCH_UP_LF_KICK] = 250; // avoid new on release

      scorePlayer[player] += SCORE_KICKER;
      flagScoreChange += 1;
      #if (MULTIBALL_ENABLED == 1)
        // todo add check of ball availability here
        if (multiballCount < 3) // eject new ball
        {
          multiballCount += 1;

        delay(200); //debounce
        fireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(DELAY_MULTI_BALL); //debounce
        unFireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(250); //debounce
        delay(250); //debounce

          //fireSolenoidNow(SOLENOID_MULTI_BALL);
          //solenoidFireFlag[SOLENOID_MULTI_BALL] = MULTIBALL_FIRE_DURATION; //50; // thumper one
          launcherCountdown = 500; // todo figure out this time
        } // end if multiballs are available

        else  // multiballs at max, just kick out
        {
        launcherCountdown = 500; // todo figure out this time
        }

      #else

        launcherCountdown = 500; // todo figure out this time

      #endif
    } // end if kicker new, only one time through this loop

    if (launcherCountdown == 0)
    {
      if (switchStateNow[SWITCH_SHOOT_LANE] == 0) // wait til ball launched
      // when not multiball, shoot lane should always be zero if ball is in kicker
      {
        // todo add timeout
        fireSolenoidNow(SOLENOID_UP_LF_KICK);
        delay(250); //debounce
        unFireSolenoidNow(SOLENOID_UP_LF_KICK);
        //switchDebounce[SWITCH_UP_LF_KICK] = 250; // avoid new on release
        delay(250); //debounce
      }
    }
    else // launcherCountdown not zero
    {
      launcherCountdown -= 1; // ramp down to zero
    }
  } // end kicker now


  // ---------------------------------------------
  // UPPER right KICKER
  // ---------------------------------------------

  if (switchStateNow[SWITCH_UP_RT_KICK] == 1) // is on now
  {
    if (switchNew[SWITCH_UP_RT_KICK] == 1) // was off
    {
      switchDebounce[SWITCH_UP_RT_KICK] = 250; // avoid new on release

      scorePlayer[player] += SCORE_KICKER;
      flagScoreChange += 1;
      #if (MULTIBALL_ENABLED == 1)
        // todo add check of ball availability here
        if (multiballCount < 3) // eject new ball
        {
          multiballCount += 1;
          delay(200); //debounce
          fireSolenoidNow(SOLENOID_MULTI_BALL);
          delay(DELAY_MULTI_BALL); //debounce
          unFireSolenoidNow(SOLENOID_MULTI_BALL);
          delay(250); //debounce
          delay(250); //debounce
          launcherCountdown = 500; // todo figure out this time
        } // end if multiballs are available

        else  // multiballs at max, just kick out
        {
        launcherCountdown = 500; // todo figure out this time
        }

      #else
        launcherCountdown = 500; // todo figure out this time
      #endif
    } // end if kicker new, only one time through this loop

    if (launcherCountdown == 0)
    {
      if (switchStateNow[SWITCH_SHOOT_LANE] == 0) // wait til ball launched
      // when not multiball, shoot lane should always be zero if ball is in kicker
      {
        // todo add timeout
        fireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(250); //debounce
        unFireSolenoidNow(SOLENOID_UP_RT_KICK);
        //switchDebounce[SWITCH_UP_RT_KICK] = 250; // avoid new on release
        delay(250); //debounce
      }
    }
    else // launcherCountdown not zero
    {
      launcherCountdown -= 1; // ramp down to zero
    }
  } // end kicker now


  // END IMEADIATE ACTIVATION BLOCK
  // ---------------------------------------------



  // each loop between 1 and 2 ms ??
  if (flagShootAgainStart == 1)
    {
      ShootAgainShortLoop -= 1;
      if (ShootAgainShortLoop == 0)
        {
          ShootAgainLongLoop -= 1;
          ShootAgainShortLoop = 50; // reset short loop
          if (ShootAgainLongLoop == 0)
            {
              flagShootAgainStart = 0;
              lightShootAgain = 0;
            }
        }
    }






  //                                                                    *****************************************
  //                                                                    ------------------------------------------
  //                                                                      STANDUP TARGETS
 //                                                                    ------------------------------------------
  //                                                                    *****************************************

    // ---------------------------------------------
    //  UP TARGET ONE
    //if (switchStateNow[SWITCH_UP_TARGET_1] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_1] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_1] == 1) // was off
      {
        if (lightUpTarget[0] > 0)
        {
          lightUpTarget[0] = 0b00000000;
          lightMissAdventure[player][4] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET TWO
    //if (switchStateNow[SWITCH_UP_TARGET_2] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_2] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_2] == 1) // was off
      {
        if (lightUpTarget[1] > 0)
        {
          lightUpTarget[1] = 0b00000000;
          lightMissAdventure[player][5] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET Three
    //if (switchStateNow[SWITCH_UP_TARGET_3] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_3] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_3] == 1) // was off
      {
        if (lightUpTarget[2] > 0)
        {
          lightUpTarget[2] = 0b00000000;
          lightMissAdventure[player][6] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET FOUR
    //if (switchStateNow[SWITCH_UP_TARGET_4] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_4] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_4] == 1) // was off
      {
        if (lightUpTarget[3] > 0)
        {
          lightUpTarget[3] = 0b00000000;
          lightMissAdventure[player][7] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET FIVE
    //if (switchStateNow[SWITCH_UP_TARGET_5] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_5] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_5] == 1) // was off
      {
        if (lightUpTarget[4] > 0)
        {
          lightUpTarget[4] = 0b00000000;
          lightMissAdventure[player][8] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET SIX
    //if (switchStateNow[SWITCH_UP_TARGET_6] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_6] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_6] == 1) // was off
      {
        if (lightUpTarget[5] > 0)
        {
          lightUpTarget[5] = 0b00000000;
          lightMissAdventure[player][9] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  UP TARGET SEVEN
    //if (switchStateNow[SWITCH_UP_TARGET_7] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_7] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_7] == 1) // was off
      {
        if (lightUpTarget[6] > 0)
        {
          lightUpTarget[6] = 0b00000000;
          lightMissAdventure[player][10] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  UP TARGET EIGHT
    //if (switchStateNow[SWITCH_UP_TARGET_8] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_8] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_8] == 1) // was off
      {
        if (lightUpTarget[7] > 0)
        {
          lightUpTarget[7] = 0b00000000;
          lightMissAdventure[player][11] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  UP TARGET NINE
    //if (switchStateNow[SWITCH_UP_TARGET_9] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_UP_TARGET_9] == 0) // was off
      if (switchNew[SWITCH_UP_TARGET_9] == 1) // was off
      {
        if (lightUpTarget[8] > 0)
        {
          lightUpTarget[8] = 0b00000000;
          lightMissAdventure[player][12] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}


     // ---------------------------------------------
    //  LOW TARGET ONE
    //if (switchStateNow[SWITCH_LOW_TARGET_1] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_1] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_1] == 1) // was off
      {
        if (lightLowTarget[0] > 0)
        {
          lightLowTarget[0] = 0b00000000;
          lightMissAdventure[player][0] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  LOW TARGET TWO
    //if (switchStateNow[SWITCH_LOW_TARGET_2] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_2] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_2] == 1) // was off
      {
        if (lightLowTarget[1] > 0)
        {
          lightLowTarget[1] = 0b00000000;
          lightRtRamp = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          //flagCheckMissAdventure = 1;

          lightUpLfThump = 0b11111111;
          //lightUpRtThump = 0b00000000;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  LOW TARGET THREE
    //if (switchStateNow[SWITCH_LOW_TARGET_3] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_3] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_3] == 1) // was off
      {
        if (lightLowTarget[2] > 0)
        {
          lightLowTarget[2] = 0b00000000;
          lightMissAdventure[player][1] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  LOW TARGET FOUR
    //if (switchStateNow[SWITCH_LOW_TARGET_4] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_4] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_4] == 1) // was off
      {
        if (lightLowTarget[3] > 0)
        {
          //lightLowTarget[3] = 0b00000000;
          //lightMissAdventure[1] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          //flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }


        if (lightLfOutLane  == 0)
        {
          lightLfOutLane = 0b11111111;
        }
        else
        {
          if (lightLowThump == 0)
          {
            lightLowThump = 0b11111111;
            lightLowTarget[3] = 0;
          }
        }


      }
    //}

     // ---------------------------------------------
    //  LOW TARGET FIVE
    //if (switchStateNow[SWITCH_LOW_TARGET_5] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_5] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_5] == 1) // was off
      {
        if (lightLowTarget[4] > 0)
        {
          lightLowTarget[4] = 0b00000000;
          lightMissAdventure[player][2] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

     // ---------------------------------------------
    //  LOW TARGET SIX
    //if (switchStateNow[SWITCH_LOW_TARGET_6] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_6] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_6] == 1) // was off
      {
        if (lightLowTarget[5] > 0)
        {
          lightLowTarget[5] = 0b00000000;
          lightLfRamp = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          //flagCheckMissAdventure = 1;
          //lightUpLfThump = 0b00000000;
          lightUpRtThump = 0b11111111;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

    // ---------------------------------------------
    //  LOW TARGET SEVEN
    //if (switchStateNow[SWITCH_LOW_TARGET_7] == 1) // is on now
    //{
      //if (switchStatePrev[SWITCH_LOW_TARGET_7] == 0) // was off
      if (switchNew[SWITCH_LOW_TARGET_7] == 1) // was off
      {
        if (lightLowTarget[6] > 0)
        {
          lightLowTarget[6] = 0b00000000;
          lightMissAdventure[player][3] = 0b11111111;
          scorePlayer[player] += SCORE_TARGET;
          flagScoreChange += 1;
          flagCheckMissAdventure = 1;
        }
        else
        {
          scorePlayer[player] += SCORE_TARGET_UNLIT;
          flagScoreChange += 1;
        }
      }
    //}

                                                                        // ---------------------------------------------
                                                                        //  DROP TARGETS
                                                                        // ---------------------------------------------
    // ---------------------------------------------
    //  DROP TARGET ONE
      if (switchNew[SWITCH_DROP_1] == 1) // was off
      {
        scorePlayer[player] += SCORE_DROP;
        flagScoreChange += 1;
        checkDropTargets();
      }

    // ---------------------------------------------
    //  DROP TARGET TWO
      if (switchNew[SWITCH_DROP_2] == 1) // was off
      {
        scorePlayer[player] += SCORE_DROP;
        flagScoreChange += 1;
        checkDropTargets();
      }

    // ---------------------------------------------
    //  DROP TARGET THREE
      if (switchNew[SWITCH_DROP_3] == 1) // was off
      {
        scorePlayer[player] += SCORE_DROP;
        flagScoreChange += 1;
        checkDropTargets();
      }

    // ---------------------------------------------
    //  DROP TARGET FOUR
      if (switchNew[SWITCH_DROP_4] == 1) // was off
      {
        scorePlayer[player] += SCORE_DROP;
        flagScoreChange += 1;
        checkDropTargets();
      }

                                                                      // ---------------------------------------------
                                                                      //  DIVERTER
                                                                      // ---------------------------------------------
    // ---------------------------------------------
      if (switchNew[SWITCH_RT_OUT_LANE] == 1) // was off
      {
        scorePlayer[player] += SCORE_OUTLANE;
        flagScoreChange += 1;
        if (lightRtOutLane > 0)
        {
          solenoidFireFlag[SOLENOID_DIVERTER] = 250;
          lightRtOutLane = 0;
        }
      }
    // ---------------------------------------------

    // ---------------------------------------------
    // SHORT LOOP ROLLOVER
    // ---------------------------------------------

      if (switchNew[SWITCH_ROLL_OVER_2] == 1) // was off
      {
        switchDebounce[SWITCH_ROLL_OVER_2] = 250;
        scorePlayer[player] += SCORE_ROLLOVER;
        flagScoreChange += 1;
        lightRtOutLane = 0b11111111;

        flagCheckMissAdventure = 0;
        for (j = 0; ((j < 13) && (flagCheckMissAdventure == 0)); j++) // check all lights
        {
          if (lightMissAdventure[player][j] == 0)
          {
            lightMissAdventure[player][j] = 0b11111111;

            switch (j)
            {
              case (0):
              case (1):
              case (2):
              case (3):
                lightLowTarget[j*2] = 0b00000000;
              break;

              case (4):
              case (5):
              case (6):
              case (7):
              case (8):
              case (9):
              case (10):
              case (11):
              case (12):
                lightUpTarget[j-4] = 0b00000000;
              break;

              default:
              break;
            }
            flagCheckMissAdventure = 1;
          }
        }
      }




  //                                                                    *****************************************
  //                                                                    ------------------------------------------
  //                                                                      CHECK MISS ADVENTURE HERE
 //                                                                    ------------------------------------------
  //                                                                    *****************************************

    if (flagCheckMissAdventure == 1)
    {
      indexMissAdventure = 0;
      for (j = 0; j < 13; j++) // check all lights
      {
        if (lightMissAdventure[player][j] > 0)
        {
          indexMissAdventure += 1;
        }
      }

      if (indexMissAdventure == 13) // reset light, increment bonu light
      {
        for (j = 0; j < 13; j++) // check all lights
        {
          lightMissAdventure[player][j] = 0;
        }

        // check and reset bonus lights
        indexBonus += 1;
        if (indexBonus > 4)
        {
          indexBonus = 4;
        }
        for (j = 0; j < 4; j++) // clear bonus lights
        {
          lightBonus[player][j] = 0;
        }
        for (j = 0; j < indexBonus; j++) // set bonus lights
        {
          lightBonus[player][j] = 0b11111111;
        }
        for (j = 0; j < 9; j++) // check all lights
        {
          lightUpTarget[j] = 0b11111111;
        }
        lightLowTarget[0] = 0b11111111;
        lightLowTarget[2] = 0b11111111;
        lightLowTarget[4] = 0b11111111;
        lightLowTarget[6] = 0b11111111;

      } // end if all lit
      flagCheckMissAdventure = 0;
    } // end if flag



  //                                                                    *****************************************
  //                                                                    ------------------------------------------
  //                                                                      OUTHOLE CODE HERE
  //                                                                    ------------------------------------------
  //                                                                    *****************************************

   // ---------------------------------------------
  //  OUTHOLE



  while ((switchStateNow[SWITCH_OUTHOLE] == 1) && (mode != GAMEOVER)) // is on now
  {

    #if (MULTIBALL_ENABLED == 1)
      if (switchStateNow[SWITCH_MULTI_BALL_1] == 0)
      {
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);
        delay(250);
        delay(250);
        //delay(250);
        //delay(250);
        // todo reduce delay
        scanPlayfieldSwitches(); // to get out of while loop
        multiballCount -= 1;
        if (multiballCount < 1)
        {
          multiballCount = 1;
        }
      }
        //todo finish this

      else
      {
    #endif


    // TO DO add timer reset here if ball stuck in outhole but not "new"
    //if (switchStatePrev[SWITCH_OUTHOLE] == 0) // was off
    //{

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

      // --------------------------------------------------
      //    BONUS CALCULATION HERE
      calcMissAdventureBonus();

      /*
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
          flagScoreChange += 1;
        } // end if advance balls

      } // end if NOT shoot again

      else // shoot again
        {
          //todo
          //mode += 1; // testing only, remove later
          flagScoreChange += 1;
          lightShootAgain = 0;
        }
      // --------------------------------------------------
     */

       // --------------------------------------------------
      //    CHECK FOR SAME PLAYER SHOOT AGAIN
      if (lightShootAgain > 0)
      {
        //todo
        //mode += 1; // testing only, remove later
        flagScoreChange += 1;
        //lightShootAgain = 0;
        if (lightShootAgain > 128) // this is a genuine extra ball won
        {
          // enable shoot again for a few seconds after ball comes out
          lightShootAgain = 0b01010101;
          flagShootAgainStart = 1; //1;
          ShootAgainShortLoop = 50; //35; //50;
          ShootAgainLongLoop = 50; //35; //50;
        }
      } // end if shoot again

      else // don't shoot again
        {
        // enable shoot again for a few seconds after ball comes out
        lightShootAgain = 0b01010101; // less than 128
        flagShootAgainStart = 1; //1;
        ShootAgainShortLoop = 50; //35; //50;
        ShootAgainLongLoop = 50; //35; //50;

        player += 1;
        if (player > (numPlayers-1))
          {
            player = 0; //1;
            mode += 1; // ADVANCE mode here, all the way to game over
            gameBalls =  ( mode - 2); // ball in play
            flagScoreChange += 1;
          } // end if advance balls
        }
      // --------------------------------------------------

    #if (DIAGNOSTIC_ENABLED == 1)
      Serial.print("Outhole = 3");
      Serial.print("\r\n");
    #endif

      if (mode == GAMEOVER)
      {
      //mode += 1; // ADVANCE mode here, all the way to game over

        gameBalls =  0; // ball in play
        flagScoreChange += 1;

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

      else // not GAME OVER
      {
        #if (SCORE_ENABLED == 1)
          sendBallCreditToDisplay(gameCredits, gameBalls);
        #endif

        // todo reset up drop targets here
        if ( 0 )
        {

        }

        delay(250);
        delay(250); // increased delay x4
        delay(250);
        delay(250);


              // RESET Upper Drop Target
        //
        //delay(200); //debounce
        fireSolenoidNow(SOLENOID_UPPER_DROP);
        delay(DELAY_DROP_TARGET); //debounce
        unFireSolenoidNow(SOLENOID_UPPER_DROP);
        delay(250);
        delay(250);
        // END RESET DROP TARGETS

        //todo MULTIBALL serve up ball w/ second solenoid
        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        //delay(200); //debounce
        fireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(DELAY_MULTI_BALL); //debounce
        unFireSolenoidNow(SOLENOID_MULTI_BALL);
        delay(250);
        delay(250);
        //END MULTIBALL

        // todo check switch to see if ball is available
        // LOAD FIRST BALL INTO SHOOT LANE
        //delay(200); //debounce
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);
        //delay(250);
        //delay(250);
        //END OUTHOLE

       /* // moved up earlier in the code
        // enable shoot again for a few seconds after ball comes out
        lightShootAgain = 0b10101010;
        flagShootAgainStart = 1; //1;
        ShootAgainShortLoop = 50; //35; //50;
        ShootAgainLongLoop = 50; //35; //50;
       */

        // todo change rules once new hold coil is found
        //solenoidFireFlag[SOLENOID_DIVERTER] = 255;
        solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 255; // anything above 253 is constant on

        // RESET LIGHTS FOR NEW BALL
        lightPlayer[0] = 0b00000000;
        lightPlayer[1] = 0b00000000;
        lightPlayer[2] = 0b00000000;
        lightPlayer[3] = 0b00000000;

        lightPlayer[player] = 0b10101010;

        lightLfRamp = 0;
        lightRtRamp = 0;
        lightLfOutLane = 0b11111111;
        lightRtOutLane = 0b11111111;

        lightUpLfThump = 0b00000000;
        lightUpRtThump = 0b00000000;
        lightLowThump = 0b00000000;
        lightLowTarget[5] = 0b11111111;
        lightLowTarget[1] = 0b11111111;

        checkMissAdventureLights(); // THIS SETS UN-HIT TARGETS BASED ON "MISS ADVENTURE"

        // delay three seconds in case outhole doesnt get it out
        //delay(250);
        //delay(250);
        //delay(250);
        //delay(250);
        delay(250);
        delay(250);
        delay(250);
        delay(250);

      } // end if mode gameover

      #if (DIAGNOSTIC_ENABLED == 1)
        Serial.print("Outhole = 4");
        Serial.print("\r\n");
      #endif

      // read switches, wait for outhole to clear
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
        //delayMicroseconds(400) ;
        delay(2);

        // todo - call sub here
        // START SCAN
        for (i = 0; i < 8; i++)
        {
          switchStatePrev[((j*8) + i)] = switchStateNow[((j*8) + i)]; // rotate variable
          switchStateNow[((j*8) + i)] = digitalRead(i + 2); // pins 2-9

          // check for a "new" state
          if ( switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)])
          {
            switchNew[((j*8) + i)] = 0; // same as old
          } // end if

          else // must be new if not old and new equals one
          {
            if (switchStateNow[((j*8) + i)] == 1)
            {
              switchNew[((j*8) + i)] = 1; // new
              flagNewSwitch = 1;

              #if (DIAGNOSTIC_ENABLED == 1)
                Serial.print("Switch = ");
                Serial.print((j*8) + i); // TODO check this formatting later
                Serial.print("\r\n");
              #endif
            }
          } // end else
        } // end for i
      } // end for j

  // add delay to match diagnostic
  delay (100);

    //} // end if outhole prev

 #if (MULTIBALL_ENABLED == 1)
    } // end else multiball > 1
 #endif

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
    flagScoreChange += 1;

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


         // CHECK FOR LOST BALLS, AND WAIT TILL THEYRE FOUND
      // --------------------------------------------
        // UPPER LEFT KICKER
       //if (switchStateNow[SWITCH_UP_LF_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_UP_LF_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UP_LF_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_UP_LF_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

          // UPPER RIGHT KICKER
       // todo change if to while if previous code works
       //if (switchStateNow[SWITCH_UP_RT_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_UP_RT_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_UP_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

       // LOWER KICKER
       // todo change if to while if previous code works
       //if (switchStateNow[SWITCH_LOW_KICK] == 1) // BALL HIDDEN IN KICKER
       while (switchStateNow[SWITCH_LOW_KICK] == 1) // BALL HIDDEN IN KICKER
       {
         if (switchStateNow[SWITCH_OUTHOLE] == 1) // MAKE SPACE IN THE OUTHOLE FOR THE NEW BALL
         {
           fireSolenoidNow(SOLENOID_OUTHOLE);
           delay(DELAY_OUTHOLE); //debounce
           unFireSolenoidNow(SOLENOID_OUTHOLE);
         }
        delay(200); //debounce
        fireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(30); //debounce
        unFireSolenoidNow(SOLENOID_LOW_RT_KICK);
        delay(250); //debounce
        delay(250); //debounce

        while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        {
          scanPlayfieldSwitches();
          delay(250); // NO NEED TO GO TOO FAST HERE
        }
       }

    mode = ATTRACT_INIT;
    break;

  } // end switch game mode


 // todo , move this closer to the switch scanning code


  //                                                                 ****************************************************
  //                                                                 ---------------------------------------------------
  //                                                                     READ SWITCHES, SET LIGHTS and SCORE DATA HERE
  //                                                                 ---------------------------------------------------
  //                                                                 ****************************************************



  //                                                                        ***********************************
  //                                                                        -----------------------------------
  //                                                                            SEND LIGHTS OUT
  //                                                                        -----------------------------------
  //                                                                        ***********************************

#if ((LIGHTS_ENABLED == 1) && (DIAGNOSTIC_ENABLED == 0))

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
      Serial.write(lightLowLfSling);
      break;
    case 4:
      Serial.write(lightUpLfArch);
      break;
    case 5:
      Serial.write(lightUpTarget[0]);
      break;
    case 6:
      Serial.write(lightUpTarget[1]);
      break;
    case 7:
      Serial.write(lightUpTarget[2]);
      break;
    case 8:
      Serial.write(lightUpTarget[3]);
      break;
    case 9:
      Serial.write(lightUpTarget[4]);
      break;
    case 10:
      Serial.write(lightUpTarget[5]);
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
      Serial.write(lightUpTarget[6]);
      break;
    case 15:
      Serial.write(lightUpTarget[7]);
      break;
    case 16:
      Serial.write(lightUpTarget[8]);
      break;
    case 17:
      Serial.write(lightUpRtThump);
      break;
    case 18:
      Serial.write(lightUpLfThump);
      break;
    case 19:
      Serial.write(lightUpRtArch);
      break;
    case 20:
      Serial.write(lightLowRtSling);
      break;
    case 21:
      Serial.write(lightLowRtSling);
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
      Serial.write(lightRollover[0]);
      break;
    case 26:
      Serial.write(lightRollover[1]);
      break;
    case 27:
      Serial.write(lightRollover[2]);
      break;
    case 28:
      Serial.write(lightLowTarget[3]);
      break;
    case 29:
      Serial.write(lightMissAdventure[player][0]);
      break;
    case 30:
      Serial.write(lightMissAdventure[player][1]);
      break;
    case 31:
      Serial.write(lightMissAdventure[player][2]);
      break;
    case 32:
      Serial.write(lightMissAdventure[player][3]);
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
      Serial.write(lightShootAgain);
      break;
    case 37:
      Serial.write(lightLowThump);
      break;
    case 38:
      Serial.write(lightStar[0]);
      break;
    case 39:
      Serial.write(lightStar[1]);
      break;
    case 40:
      Serial.write(lightStar[2]);
      break;
    case 41:
      Serial.write(lightLfOutLane);
      break;
    case 42:
      Serial.write(lightLowTarget[0]);
      break;
    case 43:
      Serial.write(lightLowTarget[1]);
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
      Serial.write(lightLowTarget[2]);
      break;
    case 48:
      Serial.write(lightLfRamp);
      break;
    case 49:
      Serial.write(lightMissAdventure[player][4]);
      break;
    case 50:
      Serial.write(lightMissAdventure[player][5]);
      break;
    case 51:
      Serial.write(lightMissAdventure[player][6]); // WIRED TO 0
      break;
    case 52:
      Serial.write(lightMissAdventure[player][7]); // WIRED TO 2
      break;
    case 53:
      Serial.write(lightMissAdventure[player][8]);  //WIRED TO 1
      break;
    case 54:
      Serial.write(lightMissAdventure[player][9]);
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
      Serial.write(lightMissAdventure[player][10]);
      break;
    case 59:
      Serial.write(lightMissAdventure[player][11]);
      break;
    case 60:
      Serial.write(lightMissAdventure[player][12]);
      break;
    case 61:
      Serial.write(lightRtRamp);
      break;
    case 62:
      Serial.write(lightLowTarget[4]);
      break;
    case 63:
      Serial.write(lightLowTarget[5]);
      break;
    case 64:
      Serial.write(lightLowTarget[6]);
      break;
    case 65:
      Serial.write(lightRtOutLane);
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
      Serial.write(lightBonus[player][0]);
      break;
    case 70:
      Serial.write(lightBonus[player][1]);
      break;
    case 71:
      Serial.write(lightBonus[player][2]);
      break;
    case 72:
      Serial.write(lightBonus[player][3]);
      break;
    case 73:
      Serial.write(lightBonus[player][0]);
      break;
    case 74:
      Serial.write(lightBonus[player][1]);
      break;
    case 75:
      Serial.write(lightBonus[player][2]);
      break;
    case 76:
      Serial.write(lightBonus[player][3]);
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
//  SELECT BANK zero
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
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, dataSolenoid);  // 0 == MAIN BANK

  #if (DIAGNOSTIC_ENABLED == 1)
  if(dataSolenoid > 0)
  {
    // todo add back later
    //Serial.print("Solenoid Bank 0 Data = ");
    //Serial.print(dataSolenoid); // TODO check this formatting later
    //Serial.print("\r\n");
  }
  #endif
//---------------------------------------


//---------------------------------------
//  SELECT BANK one
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
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, dataSolenoidChime);  // 1 == AUX BANK

  #if (DIAGNOSTIC_ENABLED == 1)
  if(dataSolenoidChime > 0)
  {
    // todo add back later
    //Serial.print("Solenoid Bank 1 Data = ");
    //Serial.print(dataSolenoidChime); // TODO check this formatting later
    //Serial.print("\r\n");
  }
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

void playScoreChime(long scorePlayer_in, long prevScore_in)
{
  if ((scorePlayer_in - prevScore_in) > 1000)
    {
    selectSolenoid = SOLENOID_CHIME_3;
    }
  else if ((scorePlayer_in - prevScore_in) > 500)
    {
    selectSolenoid = SOLENOID_CHIME_2;
    }
  else
    {
    selectSolenoid = SOLENOID_CHIME_1;
    }

  solenoidFireFlag[selectSolenoid + 8] = CHIME_FIRE_DURATION; //50; //
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
      selectBank = 0; // 0 or 1
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
      selectBank = 1; // 0 or 1
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

void scanPlayfieldSwitchesOld(void)
{
      // read switches, wait for outhole to clear
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
        //delay(2);


        // START SCAN
        for (i = 0; i < 8; i++)
        {
          switchStatePrev[((j*8) + i)] = switchStateNow[((j*8) + i)]; // rotate variable
          switchStateNow[((j*8) + i)] = digitalRead(i + 2); // pins 2-9

          // check for a "new" state
          if ( switchStateNow[((j*8) + i)] == switchStatePrev[((j*8) + i)])
          {
            switchNew[((j*8) + i)] = 0; // same as old
          } // end if

          else // must be new if not old and new equals one
          {
            if (switchStateNow[((j*8) + i)] == 1)
            {
              switchNew[((j*8) + i)] = 1; // new
              flagNewSwitch = 1;

              #if (DIAGNOSTIC_ENABLED == 1)
                Serial.print("Switch = ");
                Serial.print((j*8) + i); // TODO check this formatting later
                Serial.print("\r\n");
              #endif
            }
          } // end else
        } // end for i
      } // end for j
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

void checkMissAdventureLights(void)
{
  for (j = 0; j < 13; j++) // check all lights
  {
    if (lightMissAdventure[player][j] > 0)
    {
      switch (j)
      {
        case (0):
        case (1):
        case (2):
        case (3):
          lightLowTarget[j*2] = 0b00000000;
        break;

        case (4):
        case (5):
        case (6):
        case (7):
        case (8):
        case (9):
        case (10):
        case (11):
        case (12):
          lightUpTarget[j-4] = 0b00000000;
        break;

        default:
        break;
      } // end switch
    } // end light is off

    else // light is on
    {
      switch (j)
      {
        case (0):
        case (1):
        case (2):
        case (3):
          lightLowTarget[j*2] = 0b11111111;
        break;

        case (4):
        case (5):
        case (6):
        case (7):
        case (8):
        case (9):
        case (10):
        case (11):
        case (12):
          lightUpTarget[j-4] = 0b11111111;
        break;

        default:
        break;
      } // end switch
    } // end else on
  } // end for j
}// end checkLights

void rampdownSolenoids(void)
{
  // TODO - check to see if this can be skipped when no data changed, without timing out the controller
//---------------------------------------
//  SELECT BANK zero
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
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, dataSolenoid);  // 0 == MAIN BANK

  #if (DIAGNOSTIC_ENABLED == 1)
  if(dataSolenoid > 0)
  {
    // todo add back later
    //Serial.print("Solenoid Bank 0 Data = ");
    //Serial.print(dataSolenoid); // TODO check this formatting later
    //Serial.print("\r\n");
  }
  #endif
//---------------------------------------

//---------------------------------------
//  SELECT BANK one
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
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, dataSolenoidChime);  // 1 == AUX BANK

  #if (DIAGNOSTIC_ENABLED == 1)
  if(dataSolenoidChime > 0)
  {
    // todo add back later
    //Serial.print("Solenoid Bank 1 Data = ");
    //Serial.print(dataSolenoidChime); // TODO check this formatting later
    //Serial.print("\r\n");
  }
  #endif
//---------------------------------------
} // end rampdownSolenoid


void calcMissAdventureBonus(void)
{
  unsigned char j_BonusCalc;
  // ---------------------------------
  //  SAVE LIGHT STATE
  for (j_BonusCalc = 0; j_BonusCalc < 13; j_BonusCalc++)
  {
    lightMissAdventureBonusCount[j_BonusCalc] = lightMissAdventure[player][j_BonusCalc];
  }

  for (j_BonusCalc = 0; j_BonusCalc < 4; j_BonusCalc++)
  {
    lightBonusCount[j_BonusCalc] = lightBonus[player][j_BonusCalc];
  }

  // ---------------------------------
  //  EVERYTHING ELSE HAPPENS HERE
  for (j_BonusCalc = 0; j_BonusCalc < 4; j_BonusCalc++)
  {
    if (lightBonus[player][j_BonusCalc] > 0)
    {
      lightBonus[player][j_BonusCalc] = 0; // turn light off
      scorePlayer[player] += SCORE_SPECIAL; //scoreBonus; // add to score
      sendScoreToDisplay(scorePlayer[player], player); // send out score

      Serial.write("$CAN");        // sends five bytes
      Serial.write(7);
      Serial.write(lightBonus[player][0]);
      Serial.write(lightBonus[player][1]);
      Serial.write(lightBonus[player][2]);
      Serial.write(lightBonus[player][3]);
      Serial.write(lightBonus[player][0]);
      Serial.write(lightBonus[player][1]);
      Serial.write(lightBonus[player][2]);
      Serial.write(lightBonus[player][3]);
      Serial.write("\r\n");        // sends five bytes
    }
  }

  for (j_BonusCalc = 0; j_BonusCalc < 13; j_BonusCalc++)
  {
    if (lightMissAdventure[player][j_BonusCalc] > 0)
    {
      lightMissAdventure[player][j_BonusCalc] = 0; // turn light off
      scorePlayer[player] += SCORE_TARGET; //scoreBonus; // add to score
      sendScoreToDisplay(scorePlayer[player], player); // send out score
      // send out lights
      Serial.write("$CAN");        // sends five bytes
      Serial.write(3);
      Serial.write(lightRollover[0]);
      Serial.write(lightRollover[1]);
      Serial.write(lightRollover[2]);
      Serial.write(lightLowTarget[3]);
      Serial.write(lightMissAdventure[player][0]);
      Serial.write(lightMissAdventure[player][1]);
      Serial.write(lightMissAdventure[player][2]);
      Serial.write(lightMissAdventure[player][3]);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(5);
      Serial.write(lightLowTarget[2]);
      Serial.write(lightLfRamp);
      Serial.write(lightMissAdventure[player][4]);
      Serial.write(lightMissAdventure[player][5]);
      Serial.write(lightMissAdventure[player][6]); // WIRED TO 0
      Serial.write(lightMissAdventure[player][7]); // WIRED TO 2
      Serial.write(lightMissAdventure[player][8]);  //WIRED TO 1
      Serial.write(lightMissAdventure[player][9]);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(6);
      Serial.write(lightMissAdventure[player][10]);
      Serial.write(lightMissAdventure[player][11]);
      Serial.write(lightMissAdventure[player][12]);
      Serial.write(lightRtRamp);
      Serial.write(lightLowTarget[4]);
      Serial.write(lightLowTarget[5]);
      Serial.write(lightLowTarget[6]);
      Serial.write(lightRtOutLane);
      Serial.write("\r\n");        // sends five bytes

      fireSolenoidNow(SOLENOID_CHIME_1);
      delay(30);
      unFireSolenoidNow(SOLENOID_CHIME_1);

      //delay(250);
      //delay(250);
      delay(250);
      delay(250);
    }
  }

  // ---------------------------------
  //  RESTORE LIGHT STATE

  for (j_BonusCalc = 0; j_BonusCalc < 4; j_BonusCalc++)
  {
    lightBonus[player][j_BonusCalc] = lightBonusCount[j_BonusCalc];
  }

  for (j_BonusCalc = 0; j_BonusCalc < 13; j_BonusCalc++)
  {
    lightMissAdventure[player][j_BonusCalc] = lightMissAdventureBonusCount[j_BonusCalc];
  }

      // send out lights
      Serial.write("$CAN");        // sends five bytes
      Serial.write(3);
      Serial.write(lightRollover[0]);
      Serial.write(lightRollover[1]);
      Serial.write(lightRollover[2]);
      Serial.write(lightLowTarget[3]);
      Serial.write(lightMissAdventure[player][0]);
      Serial.write(lightMissAdventure[player][1]);
      Serial.write(lightMissAdventure[player][2]);
      Serial.write(lightMissAdventure[player][3]);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(5);
      Serial.write(lightLowTarget[2]);
      Serial.write(lightLfRamp);
      Serial.write(lightMissAdventure[player][4]);
      Serial.write(lightMissAdventure[player][5]);
      Serial.write(lightMissAdventure[player][6]); // WIRED TO 0
      Serial.write(lightMissAdventure[player][7]); // WIRED TO 2
      Serial.write(lightMissAdventure[player][8]);  //WIRED TO 1
      Serial.write(lightMissAdventure[player][9]);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(6);
      Serial.write(lightMissAdventure[player][10]);
      Serial.write(lightMissAdventure[player][11]);
      Serial.write(lightMissAdventure[player][12]);
      Serial.write(lightRtRamp);
      Serial.write(lightLowTarget[4]);
      Serial.write(lightLowTarget[5]);
      Serial.write(lightLowTarget[6]);
      Serial.write(lightRtOutLane);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(7);
      Serial.write(lightBonus[player][0]);
      Serial.write(lightBonus[player][1]);
      Serial.write(lightBonus[player][2]);
      Serial.write(lightBonus[player][3]);
      Serial.write(lightBonus[player][0]);
      Serial.write(lightBonus[player][1]);
      Serial.write(lightBonus[player][2]);
      Serial.write(lightBonus[player][3]);
      Serial.write("\r\n");        // sends five bytes

      //delay(250);
      //delay(250);
      //delay(250);
      //delay(250);
}

void checkDropTargets(void)
{
 if ((switchStateNow[SWITCH_DROP_1] &&
       switchStateNow[SWITCH_DROP_2] &&
       switchStateNow[SWITCH_DROP_3] &&
       switchStateNow[SWITCH_DROP_4]) == 1)
  {
     lightLowThump = 0b11111111;
  }
}
