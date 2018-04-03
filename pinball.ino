#include <SPI.h>
#include <MuxShield.h>

#define pinMOSI 11
#define PDB_COMMAND_WRITE 1

#define leftFlipper 15
#define rightFlipper 14
#define bumper 13

#define INPUT_BANK 2

//Initialize the Mux Shield
MuxShield muxShield;

byte board = 0;

byte bumper_on = 0;
 
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

void setup() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Need 8 MHz.  So use DIV2 on a 16 MHz board.

  Serial.begin(9600);

  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, 0b00000000);

  muxShield.setMode(2,DIGITAL_IN);
  muxShield.setMode(3,DIGITAL_OUT);

  muxShield.digitalWriteMS( 3, 15, HIGH );
  
  delay(1000);

}

byte lastData = 0;
int Vals[16];

void loop() {

  for (int i=0; i<16; i++)
  {
    //Digital read on all 16 inputs on IO1, IO2, and IO3
    Vals[i] = muxShield.digitalReadMS(INPUT_BANK,i);
  }
  
  byte solenoids = 0;  
  if( Vals[leftFlipper] ){
    solenoids += 2;
    Serial.println( "Left Button Pressed" );
  }
  if( Vals[rightFlipper] ){
    solenoids += 1;
    Serial.println( "Right Button Pressed" );
  }
  if( Vals[bumper] ){
    solenoids += 4;
    Serial.println( "Bumper Switch Activated" );
    bumper_on = 1;
  }
  
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, solenoids);  
  delay(10);
  sendPDBCommand(board, PDB_COMMAND_WRITE, 0, 0);
  delay(10);

}
