
#include <Wire.h>
//#include <SoftwareSerial.h>

//SoftwareSerial LEDSerial(6, 7); // RX, TX

/*

PinScore adaptor

 */

enum
{
  LED_ZERO,
  LED_ONE,
  LED_TWO,
  LED_THREE,
  PLAYER_ONE,
  PLAYER_TWO,
  PLAYER_THREE, // added for Retro Spa
  PLAYER_FOUR,  // added for Retro Spa
  CREDIT_BALLS
};

#define TESTMODE 0
//#define LED_CHAN PLAYER_ONE
//#define LED_CHAN PLAYER_TWO
//#define LED_CHAN PLAYER_THREE
//#define LED_CHAN PLAYER_FOUR
#define LED_CHAN CREDIT_BALLS

#define INLINE_RX 0

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

// -------------------------------------------------------
//	VARIABLES FOR CAN STRING PARSING
char sPartial[41], sComplete[41]; // string
unsigned char lenPartial,lenComplete;
const char msgHdrTwo[] = "$CAN";
const char msgCR[] = "\r\n";
const char msgNull[] = "\n";
const char sComma[] = ",";
char sResult;

unsigned char upDown=1;	// start off going UP
unsigned char cylon=0;  // determines which LED is on 0 to 4

unsigned char selectDigit = 0;	// start no digit selected
unsigned char digitBCD = 0;	// start no digit selected
const unsigned char selectLatch = 12;	// start no digit selected
const unsigned char selectBlank = 13;	// start no digit selected
const unsigned char selectOffset = 0;	// start no digit selected

//byte x = 1;
unsigned char x = 1;
unsigned char j = 0;
unsigned char msgIndex = 0;
unsigned char idCanOut = 4;
unsigned char cDataCanOut[8];
  unsigned char k = 0;
int led = 13;


void setup() {

  // todo change this to unit specific address, ie player + 4 or credit_balls == 8
  Wire.begin(LED_CHAN);                // join i2c bus with address

  Wire.onReceive(receiveEvent); // register event
//pinMode(led, OUTPUT);


#if (TESTMODE == 1)
  //LEDSerial.begin(9600);
  Serial.begin(9600);           // start serial for output
#endif

  // initialize the digital pins as outputs.
  DDRB = B00111111;  // sets Arduino port B pins 0 to 4 as outputs
  DDRD = B11111111;  // sets Arduino port B pins 0 to 4 as outputs
}

void loop() {

  #if (TESTMODE == 1)

  for (int j = 0; j < 10; j++)

  {

    digitBCD = j; // code for digit 0-9

    for (int k = 0; k < 60; k++) {

      //for (int i = 0; i < 6; i++) { // WAS RETRO SPA
      for (int i = 2; i < 8; i++) { // NEW FOR MISS ADVENTURE
        selectDigit = i;

        PORTD = (0); // clear all
        digitalWrite(selectDigit,HIGH);
        PORTB = (digitBCD);
        digitalWrite(selectLatch,HIGH);

        delay(3) ;             // wait for a second
      }

    }

    //x *= 2;
    //if(x > 128){
    //  x = 1;
    //}

    Serial.write("$CAN");        // sends five bytes
    Serial.write(x);              // sends one byte
    //Serial.write(",");        // sends five bytes
    Serial.write(x);              // sends one byte
    Serial.print(x);              // sends one byte
    Serial.print(x);              // sends one byte
    Serial.write("end\r\n");        // sends five bytes

      if(x > 127){
      x = 1;
      }
      else{
        //x *= 2;
        x += 1;
      }

  } // end for

  #endif // TESTMODE



      // start led scan loop

    //noInterrupts();

      for (int i = 0; i < 6; i++)
        {

        //selectDigit = i;
        selectDigit = i+2; // new for Miss Adventure
        PORTD = (0); // clear all
        digitalWrite(selectDigit,HIGH);
        //PORTB = (digitBCD);
        PORTB = cDataCanOut[i];
        digitalWrite(selectLatch,HIGH);

        delay(4) ;             // wait for a second
        }

      PORTD = (0); // clear all

    //interrupts();


    delay(3) ;             // wait for a second


      // end led scan loop

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{

  delay(5);

 //digitalWrite(led, !(digitalRead(led)));

 unsigned char k = 0;

  while(Wire.available() > 0) // loop through all but the last
    {

    sResult = Wire.read();

    if (sResult == ASCII_DOLLAR ) // LOOK FOR HEADER, IF FOUND SET INDEX TO ZERO
    	{
    	msgIndex = 0;
    	}
    else
	{
	msgIndex += 1;
	if (msgIndex > 20)
	    {
	    msgIndex = 0;
	    }
	}

    sPartial[msgIndex] = sResult; // PUT NEW INPUT INTO STRING

    if ( ((sPartial[msgIndex] == ASCII_CR)) && (sPartial[0] == ASCII_DOLLAR)) // LOOK FOR CARRAGE RETURN
	{
	idCanOut = sPartial[4]; // can id

	if( idCanOut == LED_CHAN ) //4 )
	    {
	    for (k=0; k<8; k++)
		{
		cDataCanOut[k] = sPartial[k + 5];
		}
	     }

        // reset
        sPartial[msgIndex] = 0;
        sPartial[0] = 0;
        msgIndex = 0;


          if (cDataCanOut[0] == 0)
          {
          cDataCanOut[0] = 10;
          }

        if ((cDataCanOut[0] == 10) && (cDataCanOut[1] == 0))
          {
          cDataCanOut[1] = 10;
          }

        if ((cDataCanOut[1] == 10) && (cDataCanOut[2] == 0))
          {
          cDataCanOut[2] = 10;
          }

         if ((cDataCanOut[2] == 10) && (cDataCanOut[3] == 0))
          {
          cDataCanOut[3] = 10;
          }

         if ((cDataCanOut[3] == 10) && (cDataCanOut[4] == 0))
          {
          cDataCanOut[4] = 10;
          }


        }

    }


}
