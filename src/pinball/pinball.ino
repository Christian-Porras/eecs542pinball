#include <SPI.h>
#include <MuxShield.h>
#include "Adafruit_LED_Backpack-master/Adafruit_LEDBackpack.h"
#include "Adafruit-GFX-Library-master/Adafruit_GFX.h"

#define SOLENOIDS_ON true
#define ENABLE_DEBUG true

#define PDB_COMMAND_WRITE 1

#define num_coils 7
#define board 0

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
};

struct s_coils coils [num_coils] = {

//  Coil Enum         Switch Num    Coil Num     Coil String
  { left_flipper,     0,            2,           "Left Flipper" },
  { right_flipper,    0,            1,           "Right Flipper" },
  { left_bumper,      1,            6,           "Left Bumper" },
  { right_bumper,     2,            5,           "Right Bumper" },
  { top_bumper,       3,            3,           "Top Bumper" },
  { left_slingshot,   4,            7,           "Left Slingshot" },
  { right_slingshot,  7,            7,           "Right Slingshot" }
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
}

void testDisplay(){
  // try to print a number thats too long
  matrix.print(10000, DEC);
  matrix.writeDisplay();
  delay(500);

  // print a hex number
  matrix.print(0xBEEF, HEX);
  matrix.writeDisplay();
  delay(500);

  // print a floating point
  matrix.print(12.34);
  matrix.writeDisplay();
  delay(500);

  // print with print/println
  for (uint16_t counter = 0; counter < 9999; counter++) {
    matrix.println(counter);
    matrix.writeDisplay();
    delay(10);
  }

  // method #2 - draw each digit
  uint16_t blinkcounter = 0;
  boolean drawDots = false;
  for (uint16_t counter = 0; counter < 9999; counter ++) {
    matrix.writeDigitNum(0, (counter / 1000), drawDots);
    matrix.writeDigitNum(1, (counter / 100) % 10, drawDots);
    matrix.drawColon(drawDots);
    matrix.writeDigitNum(3, (counter / 10) % 10, drawDots);
    matrix.writeDigitNum(4, counter % 10, drawDots);

    blinkcounter+=50;
    if (blinkcounter < 500) {
      drawDots = false;
    } else if (blinkcounter < 1000) {
      drawDots = true;
    } else {
      blinkcounter = 0;
    }
    matrix.writeDisplay();
    delay(10);
  }
}

void loop() {
  testDisplay();
  // Used to store which coils to be activated
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
}
