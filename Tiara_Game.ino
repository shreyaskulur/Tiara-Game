/*
  | Arduino   | MAX7219       |
  | --------- | ------------- |
  | MOSI (11) | DIN (1)       |
  | SCK (13)  | CLK (13)      |
  | I/O (7)   | LOAD/CS (12)  |

 */
#include <SPI.h>   //to communicate with MAX7219


#define LOAD_PIN 7  //load pin of MAX7219
#define RING A0
#define WIRE_LOOP A1
#define END_POINT A2
#define END_INDICATOR 3
#define TRIAL_TIME 1000 //time until which game doesnt end

long int startTime = 0;
long int endTime = 0;
int gameStatus = 0; //0 = not started, 1= Started ongoing, 2=ended, 3=won

/*
 * Transfers data to a MAX7219/MAX7221 register.
 * 
 * address The register to load data into
 * value   Value to store in the register
 */
void maxTransfer(int address, int value) {

  // Ensure LOAD/CS is LOW
  digitalWrite(LOAD_PIN, LOW);

  // Send the register address
  SPI.transfer(address);

  // Send the value
  SPI.transfer(value);

  // Tell chip to load in data
  digitalWrite(LOAD_PIN, HIGH);
}

void init_display()
{
  // Set load pin to output
  pinMode(LOAD_PIN, OUTPUT);

  // Reverse the SPI transfer to send the MSB first  
  SPI.setBitOrder(MSBFIRST);
  
  // Start SPI
  SPI.begin();

  // Enable mode B
  maxTransfer(0x09, 0xFF);
  
  // Use lowest intensity
  maxTransfer(0x0A, 0x05);
  
  // Activate display of all 8 digits
  maxTransfer(0x0B, 0x07);
  
  // Turn on chip
  maxTransfer(0x0C, 0x01);
  
  //blank all the digits
  for( int i=1; i<=8; i++)
    maxTransfer(i, 0x0F);
  
}

void print2digits(unsigned int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void display_time()
{
  //blank all the digits
  //for( int i=1; i<=8; i++)
   // maxTransfer(i, 0x0F);
  long int temp = 0;
  if(gameStatus == 1)
    temp = (millis() - startTime) / 10;
  else if(gameStatus == 2 || gameStatus == 3)
    temp = (endTime - startTime)/10;
  //if(temp<100);
  //maxTransfer(6, 0x80);
  //if(temp<1000)
  //maxTransfer(5, 0);
  for(int i=0; temp>0; i++)
  {
    if(8-i == 6)
      maxTransfer(8-i, 0x80|temp%10);
    else
      maxTransfer(8-i, temp%10);
    temp = temp/10;
  }
}

void setup() {
  
  Serial.begin(9600);
  Serial.println("Game");
  Serial.println("---------------------------");  
  init_display(); //initialise display

  pinMode(RING , OUTPUT);
  pinMode(WIRE_LOOP , INPUT_PULLUP);
  pinMode(END_POINT , INPUT_PULLUP);
  pinMode(END_INDICATOR, OUTPUT);

  digitalWrite(RING, LOW);
  digitalWrite(END_INDICATOR, LOW);
}


void loop() {
  if(gameStatus == 0)
    if(digitalRead(WIRE_LOOP) == HIGH)
    {
      startTime = millis();
      gameStatus = 1; 
    }
  if(gameStatus == 1)
    if((millis() - startTime) > TRIAL_TIME)
      if(digitalRead(WIRE_LOOP) == LOW)
      {
        endTime = millis();
        gameStatus = 2;
        digitalWrite(END_INDICATOR, HIGH);
      }
  if(gameStatus == 1)
    if(digitalRead(END_POINT)== LOW)
    {
      gameStatus = 3;
    }
  display_time();
  delay(1);
}  

