#include <SPI.h>
#include <MuxShield.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define SOLENOIDS_ON true
#define ENABLE_DEBUG true

#define PDB_COMMAND_WRITE 1

#define num_coils 7
#define board 0

#define num_sensors 8

#define drawDots false

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
  int light_num;
  int bank;
};

struct s_coils coils [num_coils] = {

//  Coil Enum         Switch Num     Coil Num     Coil String         Score    Light Num
  { left_flipper,     0,             2,           "Left Flipper",     0,       NULL,        1 },
  { right_flipper,    1,             1,           "Right Flipper",    0,       NULL,        1 },
  { left_bumper,      2,             6,           "Left Bumper",      50,      15,          1 },
  { right_bumper,     3,             5,           "Right Bumper",     50,      14,          1 },
  { top_bumper,       4,             3,           "Top Bumper",       100,     13,          1 },
  { left_slingshot,   5,             1,           "Left Slingshot",   25,      NULL,        0 },
  { right_slingshot,  6,             7,           "Right Slingshot",  25,      NULL,        1 }
};

//-------------------------
//      SWITCHES
//-------------------------
typedef enum e_sensors
{
  left_sensor1,
  left_sensor2,
  left_sensor3,
  left_sensor4,
  right_sensor1,
  right_sensor2,
  right_sensor3,
  right_sensor4
} sensor_t;

struct s_sensors
{
  sensor_t sensor_enum;
  int switch_num;
  uint16_t score;
};

struct s_sensors sensors [num_sensors] = {

//  Coil Enum          Switch Num      Score
  { left_sensor1,      7,              10 },
  { left_sensor2,      8,              10 },
  { left_sensor3,      9,              10 },
  { left_sensor4,      10,             10 },
  { right_sensor1,     11,             10 },
  { right_sensor2,     12,             10 },
  { right_sensor3,     13,             10 },
  { right_sensor4,     14,             10 }
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
#define input_port 1
#define output_port 3

unsigned char mode = IDLE;

void setup() {

  // Initialize SPI for Solenoid Driver Board
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Need 8 MHz.  So use DIV2 on a 16 MHz board.

  // Start Serial for communication with Serial Monitor
  Serial.begin(9600);

  scoreDisplay.begin(0x70);

  // Initialize the input port on the mux shield as digital input
  muxShield.setMode( input_port, DIGITAL_IN );
  muxShield.setMode( output_port, DIGITAL_OUT );

  // Write all the solenoids low intially
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0b00000000);
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, 0b00000000);

  Serial.println( "Setup Complete" );
  mode = INIT;
}

void updateScore(uint16_t score, int player)
{  
  scorePlayer[player] = score;

  scoreDisplay.writeDigitNum(0, (scorePlayer[player] / 1000), drawDots);
  scoreDisplay.writeDigitNum(1, (scorePlayer[player] / 100) % 10, drawDots);
  scoreDisplay.writeDigitNum(3, (scorePlayer[player] / 10) % 10, drawDots);
  scoreDisplay.writeDigitNum(4, scorePlayer[player] % 10, drawDots);
  scoreDisplay.writeDisplay();
  delay(10);
  
  return;
}

#define num_flashing_lights 3

int flashing_lights [num_flashing_lights] = {
  coils[top_bumper].light_num,
  coils[right_bumper].light_num,
  coils[left_bumper].light_num
};

int current_light;
int flashing_score = 1;
int delayTime = 175;
int idleRunCount = 0;

void loop() {
  
  switch (mode){

    default:

    case INIT: //--------------------------------------------------------------
      
      delay(1000); // Wait For Power Up
  
      player = 0;
      scorePlayer[player] = 0;
      idleRunCount = 1;
  
      mode = IDLE;
            
      break;

    case IDLE: //--------------------------------------------------------------

      Serial.println( "Idle Mode" );
      // Light Show
      muxShield.digitalWriteMS( output_port, flashing_lights[1], HIGH );
      muxShield.digitalWriteMS( output_port, flashing_lights[0], LOW );
          
      // Rotate Array of Lights
      current_light = flashing_lights[0];
      for( int i = 0; i < num_flashing_lights - 1; i++ )
      {
        flashing_lights[i] = flashing_lights[i+1];
      }
      flashing_lights[num_flashing_lights - 1] = current_light;
      
      
      //once start sensor detected, switch to start state
      if( muxShield.digitalReadMS( input_port, coils[right_flipper].switch_num ) )
      {
        mode = START;
        for( int i = 0; i < num_flashing_lights; i++ )
        {
          muxShield.digitalWriteMS( output_port, flashing_lights[i], HIGH );
        }
      }

      updateScore( flashing_score, 0 );
      if( flashing_score == 1000 )
      {
        flashing_score = 1;
      }
      else
      {
        flashing_score *= 10;
      }

      Serial.println( delayTime );
      delay(delayTime);
      if(delayTime == 25)
      {
        delayTime = 175;
      }
      else if(idleRunCount % 3 == 0)
      {
        delayTime -= 25;
      }
      idleRunCount ++;
      break;

    case START: //-------------------------------------------------------------

      Serial.println( "Entered Start Mode" );
      gameBalls = 1;
  
      //set score display to 0
      scorePlayer[player] = 0;
      updateScore(0, player);
  
      mode = PLAY;
      break;

    case PLAY: //--------------------------------------------------------------
    
      byte solenoids_1 = 0;
      byte solenoids_0 = 0;
  
      for( int i = 0; i < num_coils; i++ )
      {
        if( muxShield.digitalReadMS( input_port, coils[i].switch_num ) )
        {
          if( coils[i].bank == 1 )
          {
            solenoids_1 += ( 1 << ( coils[i].coil_num - 1 ) );
          }
          else if( coils[i].bank == 0 )
          {
            solenoids_0 += ( 1 << ( coils[i].coil_num - 1 ) );
          }
          
          
          updateScore( scorePlayer[player] + coils[i].score, player );

          muxShield.digitalWriteMS( output_port, coils[i].light_num, LOW );
  
          #if ENABLE_DEBUG
          Serial.print( coils[i].coil_name );
          Serial.print( " Activated." );
          Serial.println();
          #endif
        }
      }
      
      for( int i = 0; i < num_sensors; i++ )
      {
        if( muxShield.digitalReadMS( input_port, sensors[i].switch_num ) )
        {
          updateScore( scorePlayer[player] + sensors[i].score, player );
  
          #if ENABLE_DEBUG
          Serial.print( sensors[i].switch_num );
          Serial.print( " Activated." );
          Serial.println();
          #endif
        }
      }
  
      #if SOLENOIDS_ON
      sendPDBCommand(board, PDB_COMMAND_WRITE, 1, solenoids_1);
      sendPDBCommand(board, PDB_COMMAND_WRITE, 0, solenoids_0);
      #endif
  
      delay(20); // Amount of time the solenoids are powered
      sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0);
      sendPDBCommand(board, PDB_COMMAND_WRITE, 0, 0);
      delay(10); // Amount of time the solenoid is off

      for( int i = 0; i < num_coils; i++ )
      {
        muxShield.digitalWriteMS( output_port, coils[i].light_num, HIGH );
      }


//-----------------------------------------------------------------------------

    //check switches and update scores


    //if ball reaches outhole
    //if(outHole == /* high */){
      // 1) turn off coils
      // 2) increase gameBalls
    /*  gameBalls ++;
      // 3) check that gameBalls != maxBalls
      if(gameBalls > NUM_BALLS){
        // 4) if gameBalls == maxBalls -> GAMEOVER
        mode = GAMEOVER;
      }*/
    //}

    /*case GAMEOVER:
    //turn off solenoids

    //send score to display
    updateScore(0.0, player);
    //reset all values to defaults
    player = 0;
    for(int i = 0; i < numPlayers; i++){
      scorePlayer[i] = 0.0;
    }

    mode = INIT;
    break;*/
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


