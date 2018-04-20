#include <SPI.h>
#include <MuxShield.h>
#include <wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define SOLENOIDS_ON true
#define ENABLE_DEBUG true

#define PDB_COMMAND_WRITE 1

#define num_coils 7
#define board 0

//-------------------------
//      SWITCHES
//-------------------------
unsigned char gameBalls = 0;      //tracks which ball is currently in play
unsigned char player = 0;         //tracks the current player playing
//-------------------------
//      SCOREBOARD
//-------------------------
uint16_t scorePlayer[5] = {0,0,0,0,0}; // array to hold the current players' scores
Adafruit_7segment scoreDisplay = Adafruit_7segment(); //7segment display

//-------------------------
//      GAMEMODES
//-------------------------
#define NUM_BALLS 3
#define MAX_SCORE 9999

enum{
  INIT,
  IDLE,
  START,
  PLAY,
  GAMEOVER
};

//-------------------------
//      COILS
//-------------------------
typedef enum e_coils
{
  left_flipper,
  right_flipper,
  left_bumper,
  right_bumper,
  top_bumper,
  left_slingshot,
  right_slingshot
} coil_t;

struct s_coils
{
  coil_t coil_enum;
  int switch_num;
  int coil_num;
  char* coil_name;
  uint16_t score;
};

struct s_coils coils [num_coils] = {

//  Coil Enum         Switch Num    Coil Num     Coil String        score
  { left_flipper,     0,            2,           "Left Flipper"     , 0},
  { right_flipper,    0,            1,           "Right Flipper"    , 0},
  { left_bumper,      1,            6,           "Left Bumper"      , 50},
  { right_bumper,     2,            5,           "Right Bumper"     , 50},
  { top_bumper,       3,            3,           "Top Bumper"       , 100},
  { left_slingshot,   4,            7,           "Left Slingshot"   , 25},
  { right_slingshot,  7,            7,           "Right Slingshot"  , 25}
};

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

//Initialize the Mux Shield
MuxShield muxShield;
#define input_port 2

unsigned char mode = IDLE;

void setup() {

  // Initialize SPI for Solenoid Driver Board
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Need 8 MHz.  So use DIV2 on a 16 MHz board.

  // Start Serial for communication with Serial Monitor
  Serial.begin(9600);

  // Initialize the input port on the mux shield as digital input
  muxShield.setMode( input_port, DIGITAL_IN );

  // Write all the solenoids low intially
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0b00000000);

  mode = INIT;
}

void loop() {

  switch (mode){

    default:

    case INIT: //--------------------------------------------------------------
    delay(250);
    delay(250);
    delay(250);
    delay(250);

    player = 0;
    scorePlayer[player] = 0;

    mode = IDLE;
    break;

    case IDLE: //--------------------------------------------------------------
    //light show

    //once start sensor detected, switch to start state
    mode = START;
    break;

    case START: //-------------------------------------------------------------
    gameBalls = 1;

    //set score display to 0
    scorePlayer[player] = 0;
    updateScore(0, player);
    break;

    case PLAY: //--------------------------------------------------------------
    byte solenoids = 0;

    for( int i = 0; i < num_coils; i++ )
    {
      if( muxShield.digitalReadMS( input_port, coils[i].switch_num ) )
      {
        solenoids += ( 1 << ( coils[i].coil_num - 1 ) );

        #if ENABLE_DEBUG
        Serial.print( coils[i].coil_name );
        Serial.print( " Activated." );
        Serial.println();
        #endif
      }
    }

    #if SOLENOIDS_ON
    sendPDBCommand(board, PDB_COMMAND_WRITE, 1, solenoids);
    #endif

    delay(50); // Amount of time the solenoids are powered
    sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0);
    delay(20); // Amount of time the solenoid is off

    //check switches and update scores


    //if ball reaches outhole
    if(outHole == /* high */){
      // 1) turn off coils
      // 2) increase gameBalls
      gameBalls ++;
      // 3) check that gameBalls != maxBalls
      if(gameBalls > NUM_BALLS){
        // 4) if gameBalls == maxBalls -> GAMEOVER
        mode = GAMEOVER;
      }
    }

    case GAMEOVER:
    //turn off solenoids

    //send score to display
    updateScore(0.0, player);
    //reset all values to defaults
    player = 0;
    for(int i = 0; i < numPlayers; i++){
      scorePlayer[i] = 0.0;
    }

    mode = INIT;
    break;
  }



  // Used to store which coils to be activated
  //byte solenoids = 0;

/*  for( int i = 0; i < num_coils; i++ )
  {
    if( muxShield.digitalReadMS( input_port, coils[i].switch_num ) )
    {
      solenoids += ( 1 << ( coils[i].coil_num - 1 ) );

      #if ENABLE_DEBUG
      Serial.print( coils[i].coil_name );
      Serial.print( " Activated." );
      Serial.println();
      #endif
    }
  }

  #if SOLENOIDS_ON
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, solenoids);
  #endif

  delay(50); // Amount of time the solenoids are powered
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0);
  delay(20); // Amount of time the solenoid is off
  */
}

void updateScore(uint16_t score, int player){
  scorePlayer[player] += score;

  //send score to 7 segment
  scoreDisplay.println(scorePlayer[player]);
  scoreDisplay.writeDisplay();
  delay(10);
  return;
}
