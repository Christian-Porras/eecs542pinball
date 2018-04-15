#include <SPI.h>

#define DEBUG_MODE false

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
  { left_flipper,     3,            2,           "Left Flipper" },
  { right_flipper,    3,            1,           "Right Flipper" },
  { left_bumper,      4,            6,           "Left Bumper" },
  { right_bumper,     5,            5,           "Right Bumper" },
  { top_bumper,       6,            3,           "Top Bumper" },
  { left_slingshot,   8,            7,           "Left Slingshot" },
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

void setup() {
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // Need 8 MHz.  So use DIV2 on a 16 MHz board.

  Serial.begin(9600);

  // Initialize coil pins as outputs
  for( int i = 0; i < num_coils; i++ )
  {
    pinMode( coils[i].switch_num, OUTPUT );
  }
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0b00000000);
}

void loop() {

  byte solenoids = 0;

  for( int i = 0; i < num_coils; i++ )
  {
    if( digitalRead( coils[i].switch_num ) )
    {
      solenoids += ( 1 << ( coils[i].coil_num ) ); 
      Serial.println( strcat( coils[i].coil_name, " Activated" ) );
    }
  }

  #if !(DEBUG_MODE)
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, solenoids);  
  #endif
  
  delay(20);
  sendPDBCommand(board, PDB_COMMAND_WRITE, 1, 0);
  delay(10);
}
