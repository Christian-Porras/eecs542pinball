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

byte lastData = 0;
byte board = 0;

// --------------------------
//    SWITCHES
unsigned char switchStateNow[40];	// start no digit selected
unsigned char switchStatePrev[40];	// start no digit selected
unsigned char switchNew[40];	//
unsigned char switchDebounce[40];

unsigned char solenoidFireFlag[16];	// start no digit selected

unsigned char gameCredits = 99; //5;	//
unsigned char gameCreditsX10 = 9; //5;	//
unsigned char gameCreditsX1 = 9; //5;	//
unsigned char gameBalls = 0;	//
unsigned char gameTilted = 0;	//

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
unsigned int scoreTimer = 25; //
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

unsigned char lightLane[5] = {0,0,0,0,0};	//
unsigned char lightArrowLane[5] = {0,0,0,0,0};	//
unsigned char lightRollover = 0;	//
unsigned char lightStarRollover[5] = {0,0,0,0,0};	//
unsigned char lightSpotLow[4] = {0,0,0,0};	//
unsigned char lightField[5] = {0,0,0,0,0};    // RETRO
unsigned char lightFieldLow[5] = {0,0,0,0,0};  // SPA ++
unsigned char lightSpinner[2] = {0,0};	//
unsigned char lightLOutLane[4] = {0,0,0,0};	//
unsigned char lightBonusMult[4] = {0,0,0,0};	//
unsigned char lightShootAgain = 0;  //
unsigned char lightBBonus[3] = {0,0,0};	//
unsigned char lightSpecial[3] = {0,0,0};	//

unsigned char lightHunterUp[6] = {255,255,0,0,255,255};	//
unsigned char lightHunterLow[6] = {0,0,0,0,0,0};	//
unsigned char lightPositive = 0;
unsigned char lightNegative = 0;
unsigned char flagReverse = 0;
unsigned char lightThumperLF = 0;
unsigned char lightThumperRT = 0;
unsigned char lightThumperMD = 0;
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
  SWITCH_LEFT_DROP_1,
  SWITCH_LEFT_DROP_2,
  SWITCH_LEFT_DROP_3,
  SWITCH_ROLLOVER_1,
  SWITCH_ROLLOVER_3,
  SWITCH_TARGET_H,
  SWITCH_TARGET_U,
  SWITCH_LF_TOP_LANE,
  SWITCH_LANE_DROP_1, // 16
  SWITCH_LANE_DROP_2,
  SWITCH_LANE_DROP_3,
  SWITCH_ROLLOVER_2,  // kicker / shoot again
  SWITCH_RT_TOP_LANE,
  SWITCH_KICKER,
  SWITCH_TARGET_E,
  SWITCH_TARGET_R,
  SWITCH_THUMPER_1,        // BLACK - ORANGE
  SWITCH_THUMPER_2,        // BLACK - GREEN
  SWITCH_THUMPER_3,        // BLACK - BLUE
  SWITCH_27,
  SWITCH_SPINNER,
  SWITCH_TARGET_REV,
  SWITCH_COIN,
  SWITCH_START,
  SWITCH_LF_OUT_LANE,
  SWITCH_LF_RET_LANE,
  SWITCH_RT_RET_LANE,
  SWITCH_RT_OUT_LANE,
  SWITCH_LF_SLING,
  SWITCH_RT_SLING,
  SWITCH_ROLLOVER_4, //SHOOT_LANE,
  SWITCH_OUTHOLE // 39
};

/*
enum  // SOLENOIDS BANK ONE
{
  SOLENOID_KICKER,
  SOLENOID_OUTHOLE,
  SOLENOID_2,
  SOLENOID_RT_THUMPER, // switched mid and right
  SOLENOID_LF_THUMPER,
  SOLENOID_MD_THUMPER, // switched mid and right
  SOLENOID_INLINE_DROP,
  SOLENOID_DROP_RESET
};

enum    // SOLENOIDS BANK TWO
{
  SOLENOID_8,
  SOLENOID_9,
  SOLENOID_10,
  SOLENOID_11,
  SOLENOID_CHIME_1,
  SOLENOID_CHIME_2,
  SOLENOID_CHIME_3,
  SOLENOID_CHIME_4
};
*/

enum  // SOLENOIDS BANK ONE
{
  SOLENOID_8,
  SOLENOID_9,
  SOLENOID_10,
  SOLENOID_FLIPPER_RELAY, //SOLENOID_11,
  SOLENOID_CHIME_1,
  SOLENOID_CHIME_2,
  SOLENOID_CHIME_3,
  SOLENOID_CHIME_4,

  SOLENOID_KICKER,
  SOLENOID_OUTHOLE,
  SOLENOID_2,
  SOLENOID_RT_THUMPER, // switched mid and right
  SOLENOID_LF_THUMPER,
  SOLENOID_MD_THUMPER, // switched mid and right
  SOLENOID_INLINE_DROP,
  SOLENOID_DROP_RESET
};

#define SCORE_THUMPER 20
#define SCORE_THUMPER_UNLIT 5
#define SCORE_REV 400
#define SCORE_TARGET 10
#define SCORE_DROP 15
#define SCORE_INLINE 1 //25
#define SCORE_SLING 3
#define SCORE_ROLLOVER 10
#define SCORE_SPINNER 100
#define SCORE_OUTHOLE 123 //500

//#define NUM_BALLS 5
#define SOLENOID_FIRE_DURATION 100 //50 //5 //10 //50
#define THUMPER_FIRE_DURATION 10 //5 //10
#define UPPER_THUMPER_FIRE_DURATION 10 //15 //10
#define OUTHOLE_FIRE_DURATION 6 //3 //5 //10
#define CHIME_FIRE_DURATION 6 //3 //5 //10
#define KICKER_FIRE_DURATION 20 //5 //10
#define SLING_FIRE_DURATION 4 //3 //5 //10

#define DELAY_CHIME 20//ms
#define DELAY_OUTHOLE 40//
#define DELAY_DROP_TARGET 80//
#define DELAY_MULTI_BALL 300//
#define DELAY_SWITCH_SCAN 400//
#define DELAY_BALL_SAVE 75//

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
  //pinMode(18,OUTPUT);


  pinMode(2,INPUT); // analog in used a row drive out
  pinMode(3,INPUT); // analog in used a row drive out
  pinMode(4,INPUT); // analog in used a row drive out
  pinMode(5,INPUT); // analog in used a row drive out
  pinMode(6,INPUT); // analog in used a row drive out
  pinMode(7,INPUT); // analog in used a row drive out
  pinMode(8,INPUT); // analog in used a row drive out
  pinMode(9,INPUT); // analog in used a row drive out


//#if (SCORE_ENABLED == 1)
// wire is for I2C PinScore Arduinos
  Wire.begin(); // join i2c bus (address optional for master)
//# endif

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

}


void loop()
{

#if (DIAGNOSTIC_ENABLED == 1)
  flagDiagnostic = !flagDiagnostic;
  Serial.print(flagDiagnostic); //
  Serial.print("\r\n");
#endif

  //                                                        *****************************************
  //                                                        -----------------------------------------
  //                                                                   START READ SWITCH
  //                                                        -----------------------------------------
  //                                                        *****************************************

  flagNewSwitch = 0;

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
            Serial.print((j*8) + i); //
            Serial.print("\r\n");
          #endif

        // SWITCH DISPLAY FOR DIAGNOSTIC, REMOVE LATER
        gameCredits = ((j*8) + i);
        //flagScoreChange += 1;
        }
      } // end else
    } // end for i
  } // end for j

  // end read switches

  #if (DEBOUNCE_MODE)
    for (j = 0; j < 40; j++)
    {
      if (switchDebounce[j] > 0)
      {
        switchDebounce[j] -= 1; // ramp down to zero
      }
    }
  #endif



  //                                               ***************************************************************
  //                                               ***************************************************************
  //                                                                       GAME RULES HERE
  //                                               ***************************************************************
  //                                               ***************************************************************



  switch (mode) {

  default:
  case ATTRACT_INIT:  // -----------------------------------------------------------------------------------------
    // >>> send addresses for light show here

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Starting Attract Init Mode");
    Serial.print("\r\n");
  #endif

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


   //#if (SCORE_ENABLED == 1)
      for (j = 0; j < 4; j++)
      {
        sendScoreToDisplay(567 + j, j);
      }
    //#endif


    //#if (SCIFI == 1)
    lightHunterUp[5] = 0b10111110;
    lightHunterLow[5] = 0b10111110;

    lightHunterUp[4] = 0b11011110;
    lightHunterLow[4] = 0b11011110;

    lightHunterUp[3] = 0b11101110;
    lightHunterLow[3] = 0b11101110;

    lightHunterUp[2] = 0b11110110;
    lightHunterLow[2] = 0b11110110;

    lightHunterUp[1] = 0b11111010;
    lightHunterLow[1] = 0b11111010;

    lightHunterUp[0] = 0b11111110;
    lightHunterLow[0] = 0b11111100;

    lightPositive = 0b11001100;
    lightNegative = 0b00110011;

    lightBonusMult[0] = 0b00000001;
    lightBonusMult[1] = 0b00000010;
    lightBonusMult[2] = 0b00000100;
    lightBonusMult[3] = 0b10101000;

    lightThumperLF = 0b10101000;
    lightThumperRT = 0b10100100;
    lightThumperMD = 0b10100010;

    lightPlayer1 = 0b10101010; //0;
    lightPlayer2 = 0b01010101; //0;
    lightHighScore = 0b11001100; //0;
    lightCredit = 0b00001111; //0;
    lightTilt = 0b11001010;
    lightBall = 0b00111100; //0;
    lightGame = 0b01001011; //0;
    lightOver = 0b00101011; //0;

//TODO convert this chime to a sub??
                                                                    // --------------------------------
                                                                    // SOUND CHIMES ATTRACT INIT


      //selectSolenoid = SOLENOID_CHIME_1; //1;
      fireSolenoidNow(SOLENOID_CHIME_1);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_1);
      delay(250);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_2;
      fireSolenoidNow(SOLENOID_CHIME_2);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_2);
      delay(250);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_3; //1;
      fireSolenoidNow(SOLENOID_CHIME_3);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_3);
      delay(250);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_4; //1;
      fireSolenoidNow(SOLENOID_CHIME_4);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_4);
      delay(250);
      delay(250);

      delay(250);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_4; //1;
      fireSolenoidNow(SOLENOID_CHIME_4);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_4);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_2; //1;
      fireSolenoidNow(SOLENOID_CHIME_2);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_2);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_3; //1;
      fireSolenoidNow(SOLENOID_CHIME_3);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_3);
      delay(250);

      //selectSolenoid = SOLENOID_CHIME_1; //1;
      fireSolenoidNow(SOLENOID_CHIME_1);
      delay(DELAY_CHIME);
      unFireSolenoidNow(SOLENOID_CHIME_1);
      delay(250);



                                                                    // END SOUND CHIMES
                                                                    // -------------------------------

    //numPlayers = 0;
    mode = ATTRACT;

    break;

  case ATTRACT:  // -----------------------------------------------------------------------------------------

  #if (DIAGNOSTIC_ENABLED == 1)
    Serial.print("Starting Attract Mode");
    Serial.print("\r\n");
  #endif

// ----------------------------------------------------------------------------------
//                                            FLASH HIGH SCORE - PREVIOUS SCORE
    if(scoreTimer == 0)
      {
      // rotate scores high-prev
      if (lightHighScore > 0)
        {
        for (j = 0; j < 2; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scorePrev[j],j); //
            //sendScoreToDisplay(501,j); //
            //sendBallCreditToDisplay(gameCredits, gameBalls);
            //sendBallCreditToDisplay(12, 34);
          #endif
          flagScoreChange = 0;
          }
        lightHighScore = 0b00000000; //0;
        }

      else
        {
        for (j = 0; j < 2; j++)
          {
          #if (SCORE_ENABLED == 1)
            sendScoreToDisplay(scoreHigh[j],j); //
            //sendScoreToDisplay(502,j); //
            //sendBallCreditToDisplay(gameCredits, gameBalls);
            //sendBallCreditToDisplay(56, 78);
          #endif
          flagScoreChange = 0;
          }

        lightHighScore = 0b11111111; //0;

        //TO DO
        // SET LIGHT FOR HIGH SCORE HERE

        }
      scoreTimer = 1200; // reset score timer
      }
    else
      {
      scoreTimer -= 1;
      }
// ----------------------------------------------------------------------------------


    // wait for coins, credits
    if(switchNew[SWITCH_COIN])
    {
      if(switchStateNow[SWITCH_COIN])
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

    if(switchNew[SWITCH_START]) // && (gameCredits > 0))
    {
      // todo add this back later
      //if(switchStateNow[SWITCH_START] && (gameCredits > 0))
      if(switchStateNow[SWITCH_START])
      {
        dataSolenoid = 0;
        dataSolenoidChime = 0;

        mode = START;
        numPlayers = 1;

       // ---------------------------------------------
      // CHECK FOR LOST BALLS, AND WAIT TILL THEYRE FOUND
      // ---------------------------------------------
       // UPPER right KICKER
       if (switchStateNow[SWITCH_KICKER] == 1) // BALL HIDDEN IN KICKER
       //while (switchStateNow[SWITCH_KICKER] == 1) // BALL HIDDEN IN KICKER
       {
        //delay(200); //debounce
        fireSolenoidNow(SOLENOID_KICKER);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_KICKER);
        //delay(250); //debounce
        //delay(250); //debounce

        //while (switchStateNow[SWITCH_OUTHOLE] == 0) // WAIT FOR KICKER BALL TO FALL INTO OUTHOLE
        //{
        //  scanPlayfieldSwitches();
        //  delay(250); // NO NEED TO GO TOO FAST HERE
        //}

       }
       // ---------------------------------------------

       // testing only

       if (switchStateNow[SWITCH_THUMPER_2] == 1) // on
        {
          //selectSolenoid = SOLENOID_LF_THUMPER;
          fireSolenoidNow(SOLENOID_LF_THUMPER);
          delay(DELAY_OUTHOLE); //debounce
          unFireSolenoidNow(SOLENOID_LF_THUMPER);
        }


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

        // RESET INLINE DROP AT START OF GAME
        if ( switchStateNow[SWITCH_LEFT_DROP_1] || switchStateNow[SWITCH_LEFT_DROP_2] ||switchStateNow[SWITCH_LEFT_DROP_3] )
        {
          delay(200); //debounce
          fireSolenoidNow(SOLENOID_DROP_RESET);
          delay(DELAY_DROP_TARGET); //debounce
          unFireSolenoidNow(SOLENOID_DROP_RESET);
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

      } // end if on and credits
    } // end if start button

    break;

    // NEW GAME
  case START:  // -----------------------------------------------------------------------------------------

    solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 254; // anything above 253 is constant on

      // -------------------------------------
      //  RESET LIGHTS
      for (j = 0; j < 6; j++)
      {
        lightHunterUp[j] = 0b11111111;
        lightHunterLow[j] = 0b00000000;
      }

      lightPositive = 0b11111111;
      lightNegative = 0b00000000;

      bonusCount = 0;
      lightBonusMult[0] = 0b00000000;
      lightBonusMult[1] = 0b00000000;
      lightBonusMult[2] = 0b00000000;
      lightBonusMult[3] = 0b00000000;

      lightThumperLF = 0b00000000;
      lightThumperRT = 0b00000000;
      lightThumperMD = 0b00000000;

    lightPlayer1 = 0b11111111; //0;
    lightPlayer2 = 0b00000000; //0;
    lightHighScore = 0b00000000; //0;
    lightCredit = 0b11111111; //0;
    lightTilt = 0b00000000;
    lightBall = 0b11111111; //0;
    lightGame = 0b00000000; //0;
    lightOver = 0b00000000; //0;

      // -----------------------------------

    gameBalls =  1; //( mode - 2); // ball in play

    // ADD COIN FOR PLAYER TWO HERE
    if(switchNew[SWITCH_START] == 1) // && (gameCredits > 0))
    {
      //if((switchStatePrev[SWITCH_START] == 0) && (gameCredits > 0))
      if(switchStateNow[SWITCH_START] == 1)
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
    if (switchStateNow[SWITCH_ROLLOVER_4] == 1) // is on now
    {
      if (switchStatePrev[SWITCH_ROLLOVER_4] == 0) // was off
      {
        mode = BALLONE;
        lightShootAgain = 0b10101010;
        flagShootAgainStart = 1;
        ShootAgainShortLoop = DELAY_BALL_SAVE; //60
        ShootAgainLongLoop = DELAY_BALL_SAVE; //60

        lightPlayer[0] = 0b11111111;
        lightPlayer[1] = 0b00000000;
        lightPlayer[2] = 0b00000000;
        lightPlayer[3] = 0b00000000;
      }
    }

    // todo, remove later, until shooter lane is wired in, just jump to ball one
    mode = BALLONE;
    lightShootAgain = 0b10101010;
    flagShootAgainStart = 1;
    ShootAgainShortLoop = DELAY_BALL_SAVE; //60
    ShootAgainLongLoop = DELAY_BALL_SAVE; //60

    lightPlayer[0] = 0b11111111;
    lightPlayer[1] = 0b00000000;
    lightPlayer[2] = 0b00000000;
    lightPlayer[3] = 0b00000000;


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
  // THUMPER ONE _MD

  if (switchNew[SWITCH_THUMPER_1] == 1) // change
  {
    if (switchStateNow[SWITCH_THUMPER_1] == 1) // on
    {
      //selectSolenoid = SOLENOID_MD_THUMPER;
      fireSolenoidNow(SOLENOID_MD_THUMPER);
      solenoidFireFlag[SOLENOID_MD_THUMPER] = THUMPER_FIRE_DURATION;

      if(lightThumperMD > 0)
      {
        if (scorePlayer[player] > SCORE_REV)
          {
           scorePlayer[player] -= SCORE_REV;
          }
         else
         {
           scorePlayer[player] = 0;
         }
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }
      else
      {
        scorePlayer[player] += SCORE_THUMPER_UNLIT;
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }
    }
  }
  // ---------------------------------------------

  // ---------------------------------------------
  // THUMPER TWO LF

  if (switchNew[SWITCH_THUMPER_2] == 1) // change
  {
    if (switchStateNow[SWITCH_THUMPER_2] == 1) // on
    {
      //selectSolenoid = SOLENOID_LF_THUMPER;
      fireSolenoidNow(SOLENOID_LF_THUMPER);
      solenoidFireFlag[SOLENOID_LF_THUMPER] = UPPER_THUMPER_FIRE_DURATION;

      if(lightThumperLF > 0)
      {
        scorePlayerOne += SCORE_THUMPER;
        scorePlayer[player] += SCORE_THUMPER;
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }
      else
      {
        scorePlayer[player] += SCORE_THUMPER_UNLIT;
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }

    }
  }
  // ---------------------------------------------


  // ---------------------------------------------
  // THUMPER THREE RT
  if (switchNew[SWITCH_THUMPER_3] == 1) // change
  {
    if (switchStateNow[SWITCH_THUMPER_3] == 1) // on
    {
      fireSolenoidNow(SOLENOID_RT_THUMPER);
      solenoidFireFlag[SOLENOID_RT_THUMPER] = UPPER_THUMPER_FIRE_DURATION;

      if(lightThumperRT > 0)
      {
        scorePlayerOne += SCORE_THUMPER;
        scorePlayer[player] += SCORE_THUMPER;
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }
      else
      {
        scorePlayer[player] += SCORE_THUMPER_UNLIT;
        flagScoreChange += (THUMPER_FIRE_DURATION + 15); // only add score after thumper turns off
      }
    }
  }
  // ---------------------------------------------

  // ---------------------------------------------
  //  LEFT DROP TARGET BANK
  if ( switchNew[SWITCH_LEFT_DROP_1] || switchNew[SWITCH_LEFT_DROP_2] || switchNew[SWITCH_LEFT_DROP_3] ) // change
  {
    if (switchStateNow[SWITCH_LEFT_DROP_1] && switchStateNow[SWITCH_LEFT_DROP_2] && switchStateNow[SWITCH_LEFT_DROP_3]) // on
    {

      delay(50);
      delay(250); //debounce
      delay(250); //debounce

      //selectSolenoid = SOLENOID_DROP_RESET; //0;
      fireSolenoidNow(SOLENOID_DROP_RESET);
      solenoidFireFlag[SOLENOID_DROP_RESET] = SOLENOID_FIRE_DURATION;

      bonusCount += 1;
      if (bonusCount > 4)
        {
         bonusCount = 4;
        }
      for (j = 0; j < 4; j++)
        {
         lightBonusMult[j] = 0b00000000;
        }
     lightBonusMult[bonusCount - 1] = 0b11111111;
    }
  }
  // ---------------------------------------------

  // ---------------------------------------------
  //  RIGHT INLINE DROP TARGET BANK
  if ( switchNew[SWITCH_ROLLOVER_1] ) // change
  {
    if (switchStateNow[SWITCH_ROLLOVER_1] && (switchStateNow[SWITCH_LANE_DROP_1] || switchStateNow[SWITCH_LANE_DROP_2] || switchStateNow[SWITCH_LANE_DROP_3])) // on
    {
      //selectSolenoid = SOLENOID_INLINE_DROP; //1;
      fireSolenoidNow(SOLENOID_INLINE_DROP);
      solenoidFireFlag[SOLENOID_INLINE_DROP] = SOLENOID_FIRE_DURATION;
    }
  }
  // ---------------------------------------------

  // ---------------------------------------------
  //  ball cap kicker
  if ( switchNew[SWITCH_KICKER] ) // change
  {
    if (switchStateNow[SWITCH_KICKER]) // on
    {

      if (flagKickerLaneStart == 1) // timer for getting free ball
        {
        lightShootAgain = 0b11111111;
        flagKickerLaneStart = 0; // stop countdown
        }

      delay(250);

      calcHunterBonus(); // todo check this

      delay(250);

      //selectSolenoid = SOLENOID_KICKER; //1;
      fireSolenoidNow(SOLENOID_KICKER);
      solenoidFireFlag[SOLENOID_KICKER] = KICKER_FIRE_DURATION;
      // todo add while loop here
    }
  }
  // ---------------------------------------------


  // ---------------------------------------------
  //     SET LOOPS FOR SHOOT AGAIN
  // ---------------------------------------------

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


  if (flagKickerLaneStart == 1)
    {
      KickerLaneShortLoop -= 1;
      if (KickerLaneShortLoop == 0)
        {
          KickerLaneLongLoop -= 1;
          KickerLaneShortLoop = 50; // reset short loop
          if (KickerLaneLongLoop == 0)
            {
              flagKickerLaneStart = 0;
              lightShootAgain = 0;
            }
        }
    }



  //                                                                 ****************************************************
  //                                                                 ---------------------------------------------------
  //                                                                     READ SWITCHES, SET LIGHTS and SCORE DATA HERE
  //                                                                 ---------------------------------------------------
  //                                                                 ****************************************************


  if ( switchNew[SWITCH_TARGET_R] && switchStateNow[SWITCH_TARGET_R] ) // target "R"
  {
    if (lightHunterUp[5] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;
    }
    lightHunterUp[5] = 0b00000000;
    lightHunterLow[5] = 0b11111111;
  }

  if ( switchNew[SWITCH_TARGET_E] && switchStateNow[SWITCH_TARGET_E] ) //
  {
    if (lightHunterUp[4] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;
          lightHunterUp[4] = 0b00000000;
    lightHunterLow[4] = 0b11111111;
    }
  }

  if ( switchNew[SWITCH_RT_TOP_LANE] && switchStateNow[SWITCH_RT_TOP_LANE] ) //
  {
    if (lightHunterUp[3] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;
          lightHunterUp[3] = 0b00000000;
    lightHunterLow[3] = 0b11111111;
    }
    lightThumperRT = 0b11111111;
  }

  if ( switchNew[SWITCH_LF_TOP_LANE] && switchStateNow[SWITCH_LF_TOP_LANE] ) //
  {
    if (lightHunterUp[2] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;
         lightHunterUp[2] = 0b00000000;
    lightHunterLow[2] = 0b11111111;
    }
    lightThumperLF = 0b11111111;
  }

  if ( switchNew[SWITCH_TARGET_U] && switchStateNow[SWITCH_TARGET_U] ) //
  {
    if (lightHunterUp[1] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;

          lightHunterUp[1] = 0b00000000;
    lightHunterLow[1] = 0b11111111;
    }
  }

  if ( switchNew[SWITCH_TARGET_H] && switchStateNow[SWITCH_TARGET_H] ) //
  {
    if (lightHunterUp[0] > 1)
    {
      scorePlayer[player] += SCORE_TARGET;
      flagScoreChange += 1;

    lightHunterUp[0] = 0b00000000;
    lightHunterLow[0] = 0b11111111;
    }
  }

  if ( switchNew[SWITCH_SPINNER] && switchStateNow[SWITCH_SPINNER] ) //
  {
    scorePlayer[player] += SCORE_SPINNER;
    flagScoreChange += 1;
  }


  if ( switchNew[SWITCH_LEFT_DROP_1] && switchStateNow[SWITCH_LEFT_DROP_1] ) //
  {
    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;
  }

    if ( switchNew[SWITCH_LEFT_DROP_2] && switchStateNow[SWITCH_LEFT_DROP_2] ) //
  {
    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;
  }

    if ( switchNew[SWITCH_LEFT_DROP_3] && switchStateNow[SWITCH_LEFT_DROP_3] ) //
  {
    scorePlayer[player] += SCORE_DROP;
    flagScoreChange += 1;
  }

  if ( switchNew[SWITCH_LANE_DROP_1] && switchStateNow[SWITCH_LANE_DROP_1] ) //
  {
    //scorePlayer[player] += SCORE_INLINE + 1;
    //flagScoreChange += 1;
  }

    if ( switchNew[SWITCH_LANE_DROP_2] && switchStateNow[SWITCH_LANE_DROP_2] ) //
  {
    scorePlayer[player] += SCORE_INLINE + 2;
    flagScoreChange += 1;
  }

    if ( switchNew[SWITCH_LANE_DROP_3] && switchStateNow[SWITCH_LANE_DROP_3] ) //
  {
    scorePlayer[player] += SCORE_INLINE + 3;
    flagScoreChange += 1;
  }

  // --------------------------
  //  NEGATIVE THUMPER
  // ---------------------------
  if ( switchNew[SWITCH_TARGET_REV] && switchStateNow[SWITCH_TARGET_REV] ) //
  {
    // >>> ADD LIGHT THUMPER

    scorePlayer[player] += SCORE_TARGET;
    flagScoreChange += 1;

    lightThumperMD = 0b11111111;
    lightPositive = 0b00000000;
    lightNegative = 0b10101010;
  }

  if ( switchNew[SWITCH_ROLLOVER_1] && switchStateNow[SWITCH_ROLLOVER_1] ) //
  {
    scorePlayer[player] += SCORE_ROLLOVER;
    flagScoreChange += 1;

    // >>> ADD LIGHT THUMPER
    lightThumperMD = 0b00000000;
    lightPositive = 0b11111111;
    lightNegative = 0b00000000;
  }


  // KICKER TIMER ROLLOVER
  if ( switchNew[SWITCH_ROLLOVER_2] && switchStateNow[SWITCH_ROLLOVER_2] ) //
  {
    scorePlayer[player] += SCORE_ROLLOVER;
    flagScoreChange += 1;

    KickerLaneLongLoop = 150; // reset long loop timer
    KickerLaneShortLoop = 25; // reset short loop timer
    flagKickerLaneStart = 1; // set flag that activates countdown timer

    lightShootAgain = 0b00010001;

    //START SHOOT AGAIN KICKER TIMER HERE
  }


  if ( switchNew[SWITCH_ROLLOVER_3] && switchStateNow[SWITCH_ROLLOVER_3] ) //
  {
    scorePlayer[player] += SCORE_ROLLOVER;
    flagScoreChange += 1;
  }

  // SHOOTER LANE ROLLOVER ?
  if ( switchNew[SWITCH_ROLLOVER_4] && switchStateNow[SWITCH_ROLLOVER_4] ) //
  {
    scorePlayer[player] += SCORE_ROLLOVER;
    flagScoreChange += 1;
  }
  //#endif


  //                                                                    *****************************************
  //                                                                    ------------------------------------------
  //                                                                      OUTHOLE CODE HERE
  //                                                                    ------------------------------------------
  //                                                                    *****************************************

  // ---------------------------------------------
  //  OUTHOLE
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

    // --------------------------------------------------
    //    AWARD BONUS HERE
    calcHunterBonus(); // todo check this

    // --------------------------------------------------
    //    CHECK FOR SAME PLAYER SHOOT AGAIN
    //if (lightShootAgain == 0)
    if (lightShootAgain < 20) // so that we can activate S.A. light for kicker timer
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
        //mode += 1; // testing only, remove later
        flagScoreChange = 1;
        lightShootAgain = 0;
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

        if ( switchStateNow[SWITCH_LEFT_DROP_1] || switchStateNow[SWITCH_LEFT_DROP_2] ||switchStateNow[SWITCH_LEFT_DROP_3] )
        {
          // RESET Upper Drop Target
          //
          delay(250); //debounce
          delay(250); //debounce
          fireSolenoidNow(SOLENOID_DROP_RESET);
          delay(DELAY_DROP_TARGET); //debounce
          unFireSolenoidNow(SOLENOID_DROP_RESET);
          delay(250);
          delay(250);
          // END RESET DROP TARGETS
        }

        // LOAD FIRST BALL INTO SHOOT LANE
        fireSolenoidNow(SOLENOID_OUTHOLE);
        delay(DELAY_OUTHOLE); //debounce
        unFireSolenoidNow(SOLENOID_OUTHOLE);

        // enable shoot again for a few seconds after ball comes out
        lightShootAgain = 0b10101010;
        flagShootAgainStart = 1; //1;
        ShootAgainShortLoop = DELAY_BALL_SAVE; //35; //50;
        ShootAgainLongLoop = DELAY_BALL_SAVE; //35; //50;

        solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 254; // anything above 253 is constant on

      // RESET LIGHTS FOR NEW BALL
      // -------------------------------------
      //  RESET LIGHTS
      for (j = 0; j < 6; j++)
      {
        lightHunterUp[j] = 0b11111111;
        lightHunterLow[j] = 0b00000000;
      }

      lightPositive = 0b11111111;
      lightNegative = 0b00000000;

      bonusCount = 0;
      lightBonusMult[0] = 0b00000000;
      lightBonusMult[1] = 0b00000000;
      lightBonusMult[2] = 0b00000000;
      lightBonusMult[3] = 0b00000000;

      lightThumperLF = 0b00000000;
      lightThumperRT = 0b00000000;
      lightThumperMD = 0b00000000;
      // -----------------------------------

      } // end else if mode gameover

      #if (DIAGNOSTIC_ENABLED == 1)
        Serial.print("Outhole = 4");
        Serial.print("\r\n");
      #endif

        scanPlayfieldSwitches(); // this is to check outhole switch, escape while loop

    delay (100);

  }// end while outhole

  //                                                                 *****************************************
  //                                                                 ---------------------------------------------
  //                                                                          END OUTHOLE / BONUS CALC
  //                                                                 ---------------------------------------------
  //                                                                 *****************************************



    break; // END CASE BALLONE THRU BALLFIVE
    // -----------------------------------------------------------------------------------------


  // -----------------------------------------------------------------------------------------
  case GAMEOVER:

    // TURN OFF SOLENOIDS
    dataSolenoid = 0; // clear all
    sendPDBCommand(0, PDB_COMMAND_WRITE, 0, dataSolenoid);
    dataSolenoidChime = 0; // clear all
    sendPDBCommand(0, PDB_COMMAND_WRITE, 1, dataSolenoidChime);

    // DISABLE FLIPPER RELAY
    //selectSolenoid = SOLENOID_FLIPPER_RELAY; //1;
    solenoidFireFlag[SOLENOID_FLIPPER_RELAY] = 0; // anything above 253 is constant on
    // no need to time here, should be constant on

    // >>> output lights for game over

    gameBalls =  0; //( mode - 2); // ball in play
    flagScoreChange = 1;

    #if (SCORE_ENABLED == 1)
      sendBallCreditToDisplay(gameCredits, gameBalls);
    #endif

    // -------------------------------------
    //  GAME OVER LIGHTS
    for (j = 0; j < 6; j++)
    {
      lightHunterUp[j] = 0b00000000;
      lightHunterLow[j] = 0b00000000;
    }

    lightPositive = 0b00000000;
    lightNegative = 0b00000000;

    lightBonusMult[0] = 0b00000000;
    lightBonusMult[1] = 0b00000000;
    lightBonusMult[2] = 0b00000000;
    lightBonusMult[3] = 0b00000000;

    lightThumperLF = 0b00000000;
    lightThumperRT = 0b00000000;
    lightThumperMD = 0b00000000;

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
    // add-a-ball ???
    mode = ATTRACT_INIT;
    break;

  }
  //   END GAME MODE CASE SWITCH
  // ----------------------------------------------------------------------



  //                                                                        ***********************************
  //                                                                        -----------------------------------
  //                                                                            SEND LIGHTS OUT
  //                                                                        -----------------------------------
  //                                                                        ***********************************

//#if (LIGHTS_ENABLED == 1)
#if ((LIGHTS_ENABLED == 1) && (DIAGNOSTIC_ENABLED == 0))
  //testing
  //sendLightOut = 1;

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
      Serial.write(lightHunterUp[0]);
      break;
    case 4:
      Serial.write(lightHunterUp[1]);
      break;
    case 5:
      Serial.write(lightHunterUp[2]);
      break;
    case 6:
      Serial.write(lightHunterUp[3]);
      break;
    case 7:
      Serial.write(lightHunterUp[4]);
      break;
    case 8:
      Serial.write(lightHunterUp[5]);
      break;
    case 9:
      Serial.write(lightThumperLF);
      break;
    case 10:
      Serial.write(lightThumperRT);
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
      Serial.write(lightHunterLow[0]);
      break;
    case 15:
      Serial.write(lightHunterLow[1]);
      break;
    case 16:
      Serial.write(lightHunterLow[2]);
      break;
    case 17:
      Serial.write(lightHunterLow[3]);
      break;
    case 18:
      Serial.write(lightHunterLow[4]);
      break;
    case 19:
      Serial.write(lightHunterLow[5]);
      break;
    case 20:
      Serial.write(lightThumperMD);
      break;
    case 21:
      Serial.write(lightShootAgain);
      break;
    case 22:
      Serial.write("\r\n");        // sends five bytes
      break;

    case 23:
      Serial.write("$CAN");        // sends five bytes
      break;
    case 24:
      Serial.write(5);
      break;
    case 25:
      Serial.write(lightPositive);
      break;
    case 26:
      Serial.write(lightNegative);
      break;
    case 27:
      Serial.write(lightTest);
      break;
    case 28:
      Serial.write(lightTest);
      break;
    case 29:
      Serial.write(lightBonusMult[0]);
      break;
    case 30:
      Serial.write(lightBonusMult[1]);
      break;
    case 31:
      Serial.write(lightBonusMult[2]);
      break;
    case 32:
      Serial.write(lightBonusMult[3]);
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
      Serial.write(lightPlayer1);
      break;
    case 37:
      Serial.write(lightPlayer2);
      break;
    case 38:
      Serial.write(lightHighScore);
      break;
    case 39:
      Serial.write(lightTilt);
      break;
    case 40:
      Serial.write(lightCredit);
      break;
    case 41:
      Serial.write(lightBall);
      break;
    case 42:
      Serial.write(lightGame);
      break;
    case 43:
      Serial.write(lightOver);
      break;
    case 44:
      Serial.write("\r\n");        // sends five bytes

      lightTransmitSequence = 0; // start over from beginning
      sendLightOut = 0; // reset flag

      break;

    } // END SWITCH

    lightTransmitSequence += 1; // start over from beginning

  //------------------------------

#endif // LIGHTS ENABLE

  //                                                                      *******************************
  //                                                                      -------------------------------
  //                                                                          FIRE SOLENOIDS HERE
  //                                                                      -------------------------------
  //                                                                      *******************************

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


} // end main loop


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

  // todo change this to unit specific i2c address, ie player_in + 4
  //Wire.beginTransmission(4); // transmit to player one Arduino LED
  Wire.beginTransmission(player_in + 4); // transmit to player one Arduino LED
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
  wireWriteData[5] = (gameCredits_in/10);// 10; // 10 displays as blank
  wireWriteData[6] = (gameCredits_in-(wireWriteData[5]*10)); //(gameCredits_in/10);
  wireWriteData[7] = 10; //(gameCredits_in-(wireWriteData[6]*10));
  wireWriteData[8] = 0; //10; //10 displays as blank
  wireWriteData[9] = gameBalls_in; //0;
  wireWriteData[10] = 10; //gameBalls_in;
  wireWriteData[11] = ASCII_CR;

 // todo change this to unit specific i2c address, credit_balls == 8
  //Wire.beginTransmission(4); // transmit to player one Arduino LED
  Wire.beginTransmission(8); // transmit to player one Arduino LED credit-balls
  Wire.write(wireWriteData,12);
  Wire.endTransmission();    // stop transmitting

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
    Serial.print(selectSolenoid_in);
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
    Serial.print(selectSolenoid_in);
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
            Serial.print((j*8) + i); //
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

void calcHunterBonus(void)
{
  unsigned char j_BonusCalc;
  scoreBonus = 25;

  for (j_BonusCalc = 0; j_BonusCalc < 6; j_BonusCalc++)
  {
    if (lightHunterLow[j_BonusCalc] > 0)
    {
      scoreBonus *= 2;

      scorePlayer[player] += scoreBonus;
      // todo
      //sendScoreToDisplay(scorePlayer[player], player);

      fireSolenoidNow(SOLENOID_CHIME_1);
      delay(10);
      unFireSolenoidNow(SOLENOID_CHIME_1);
      delay(90);

      lightHunterLow[j_BonusCalc] = 0b00000000;
      lightHunterUp[j_BonusCalc] = 0b11111111;

      Serial.write("$CAN");        // sends five bytes
      Serial.write(1);
      Serial.write(lightHunterUp[0]);
      Serial.write(lightHunterUp[1]);
      Serial.write(lightHunterUp[2]);
      Serial.write(lightHunterUp[3]);
      Serial.write(lightHunterUp[4]);
      Serial.write(lightHunterUp[5]);
      Serial.write(lightThumperLF);
      Serial.write(lightThumperRT);
      Serial.write("\r\n");        // sends five bytes

      Serial.write("$CAN");        // sends five bytes
      Serial.write(2);
      Serial.write(lightHunterLow[0]);
      Serial.write(lightHunterLow[1]);
      Serial.write(lightHunterLow[2]);
      Serial.write(lightHunterLow[3]);
      Serial.write(lightHunterLow[4]);
      Serial.write(lightHunterLow[5]);
      Serial.write(lightThumperMD);
      Serial.write(lightShootAgain);
      Serial.write("\r\n");        // sends five bytes

      delay(250);
      delay(250);
    } // end if light add bonus
  } // end check of lower hunter lights to add bonus
}
