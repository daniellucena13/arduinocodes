//=========================HEADER=============================================================
/*
   Dual LS7366 Quadrature Counter Test Code
   AUTHOR: Jason Traud
   DATE: June 22, 2013
   
   This is a simple test program to read encoder counts
   collected by the LS7366 breakout board. The counts are
   then displayed in the Arduino's serial monitor at a 
   baud rate of 9600
   
   Hardware: Arduino Uno R3
   Powered 
   
   LS7366 Breakout    -------------   Arduino
   -----------------                    -------
            MOSI   -------------------   SDO (D11)
            MISO   -------------------   SDI (D12)
            SCK    -------------------   SCK (D13)
            SS1    -------------------   SS1 (D7)
            SS2    -------------------   SS2 (D8)
            GND    -------------------   GND
            VDD    -------------------   VCC (5.0V)
			
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 

//============================================================================================
*/

// Inclde the standard Arduino SPI Library, please ensure the SPI pins are
// connected properly for your Arduino version
#include <SPI.h>

// Slave Select pins for encoders 1, 2 and 3
// Feel free to reallocate these pins to best suit your circuit
const int slaveSelectEnc1 = 46;
const int slaveSelectEnc2 = 47;
const int slaveSelectEnc3 = 49;

// These hold the current encoder count.
double encoder1count = 0;
double encoder2count = 0;
double encoder3count = 0;

void initEncoders() {
  
  // Set slave selects as outputs
  pinMode(slaveSelectEnc1, OUTPUT);
  pinMode(slaveSelectEnc2, OUTPUT);
  pinMode(slaveSelectEnc3, OUTPUT);  
  
  // Raise select pins
  // Communication begins when you drop the individual select signsl
  digitalWrite(slaveSelectEnc1,HIGH);
  digitalWrite(slaveSelectEnc2,HIGH);
  digitalWrite(slaveSelectEnc3,HIGH);
  
  SPI.begin();
  
  // Initialize encoder 1
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    non-quatrature count mode (A=clock, B=direction)
  // NOTE: For more information on commands, see datasheet
  digitalWrite(slaveSelectEnc1,LOW);        // Begin SPI conversation
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x00);                       // Configure to non-quadrature mode
  digitalWrite(slaveSelectEnc1,HIGH);       // Terminate SPI conversation 

  digitalWrite(slaveSelectEnc1,LOW);        // Begin SPI conversation 
  SPI.transfer(0x90);                       // Write to MDR1
  SPI.transfer(0x00);                       // 4-byte counter mode
  digitalWrite(slaveSelectEnc1,HIGH);       // Begin SPI conversation

  // Initialize encoder 2
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    non-quatrature count mode (A=clock, B=direction)
  // NOTE: For more information on commands, see datasheet
  digitalWrite(slaveSelectEnc2,LOW);        // Begin SPI conversation
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x00);                       // Configure to non-quadrature mode
  digitalWrite(slaveSelectEnc2,HIGH);       // Terminate SPI conversation 

  digitalWrite(slaveSelectEnc2,LOW);        // Begin SPI conversation 
  SPI.transfer(0x90);                       // Write to MDR1
  SPI.transfer(0x00);                       // 4-byte counter mode
  digitalWrite(slaveSelectEnc2,HIGH);       // Begin SPI conversation  
  
  // Initialize encoder 3
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    non-quatrature count mode (A=clock, B=direction)
  // NOTE: For more information on commands, see datasheet
  digitalWrite(slaveSelectEnc3,LOW);        // Begin SPI conversation
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x00);                       // Configure to non-quadrature mode
  digitalWrite(slaveSelectEnc3,HIGH);       // Terminate SPI conversation 

  digitalWrite(slaveSelectEnc3,LOW);        // Begin SPI conversation 
  SPI.transfer(0x90);                       // Write to MDR1
  SPI.transfer(0x00);                       // 4-byte counter mode
  digitalWrite(slaveSelectEnc3,HIGH);       // Begin SPI conversation

}

long readEncoder(int encoder) {
  
  // Initialize temporary variables for SPI read
  /*unsigned int*/ 
  long count_1, count_2, count_3, count_4;
  long count_value;  
  
  // Read encoder 1
  if (encoder == 1) {
    digitalWrite(slaveSelectEnc1,LOW);      // Begin SPI conversation
    SPI.transfer(0x60);                     // Request count
    count_1 = SPI.transfer(0x00);           // Read highest order byte
    count_2 = SPI.transfer(0x00);           
    count_3 = SPI.transfer(0x00);           
    count_4 = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelectEnc1,HIGH);     // Terminate SPI conversation 
  }
  
  // Read encoder 2
  else if (encoder == 2) {
    digitalWrite(slaveSelectEnc2,LOW);      // Begin SPI conversation
    SPI.transfer(0x60);                      // Request count
    count_1 = SPI.transfer(0x00);           // Read highest order byte
    count_2 = SPI.transfer(0x00);           
    count_3 = SPI.transfer(0x00);           
    count_4 = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelectEnc2,HIGH);     // Terminate SPI conversation 
  }
  
  // Read encoder 3
  else if (encoder == 3) {
    digitalWrite(slaveSelectEnc3,LOW);      // Begin SPI conversation
    SPI.transfer(0x60);                      // Request count
    count_1 = SPI.transfer(0x00);           // Read highest order byte
    count_2 = SPI.transfer(0x00);           
    count_3 = SPI.transfer(0x00);           
    count_4 = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelectEnc3,HIGH);     // Terminate SPI conversation 
  }
  
  // Calculate encoder count
  count_value = (count_1 << 8) + count_2;
  count_value = (count_value << 8) + count_3;
  count_value = (count_value << 8) + count_4;
  
  return count_value;
}

void clearEncoderCount() {
    
  // Set encoder1's data register to 0
  digitalWrite(slaveSelectEnc1,LOW);      // Begin SPI conversation  
  // Write to DTR
  SPI.transfer(0x98);    
  // Load data
  SPI.transfer(0x00);  // Highest order byte
  SPI.transfer(0x00);           
  SPI.transfer(0x00);           
  SPI.transfer(0x00);  // lowest order byte
  digitalWrite(slaveSelectEnc1,HIGH);     // Terminate SPI conversation 
  
  delayMicroseconds(100);  // provides some breathing room between SPI conversations
  
  // Set encoder1's current data register to center
  digitalWrite(slaveSelectEnc1,LOW);      // Begin SPI conversation  
  SPI.transfer(0xE0);    
  digitalWrite(slaveSelectEnc1,HIGH);     // Terminate SPI conversation   
  
  // Set encoder2's data register to 0
  digitalWrite(slaveSelectEnc2,LOW);      // Begin SPI conversation  
  // Write to DTR
  SPI.transfer(0x98);    
  // Load data
  SPI.transfer(0x00);  // Highest order byte
  SPI.transfer(0x00);           
  SPI.transfer(0x00);           
  SPI.transfer(0x00);  // lowest order byte
  digitalWrite(slaveSelectEnc2,HIGH);     // Terminate SPI conversation 
  
  delayMicroseconds(100);  // provides some breathing room between SPI conversations
  
  // Set encoder2's current data register to center
  digitalWrite(slaveSelectEnc2,LOW);      // Begin SPI conversation  
  SPI.transfer(0xE0);    
  digitalWrite(slaveSelectEnc2,HIGH);     // Terminate SPI conversation 
  
   // Set encoder3's data register to 0
  digitalWrite(slaveSelectEnc3,LOW);      // Begin SPI conversation  
  // Write to DTR
  SPI.transfer(0x98);    
  // Load data
  SPI.transfer(0x00);  // Highest order byte
  SPI.transfer(0x00);           
  SPI.transfer(0x00);           
  SPI.transfer(0x00);  // lowest order byte
  digitalWrite(slaveSelectEnc3,HIGH);     // Terminate SPI conversation 
  
  delayMicroseconds(100);  // provides some breathing room between SPI conversations
  
  // Set encoder3's current data register to center
  digitalWrite(slaveSelectEnc3,LOW);      // Begin SPI conversation  
  SPI.transfer(0xE0);    
  digitalWrite(slaveSelectEnc3,HIGH);     // Terminate SPI conversation 
}


void setup() {
 Serial.begin(9600); 
 Serial2.begin(9600);      // Serial com for data output
 
 initEncoders();       Serial.println("Encoders Initialized...");  
 clearEncoderCount();  Serial.println("Encoders Cleared...");
}

void loop() {
 delay(10);
 
 // Retrieve current encoder counters
 encoder1count = readEncoder(1); 
 clearEncoderCount();
delay(10);
 encoder2count = readEncoder(2);
delay(10);
 encoder3count = readEncoder(3);
delay(10);
 clearEncoderCount();
 
 Serial.print("Enc1: "); Serial.print(encoder1count); Serial.print(" Enc2: "); Serial.print(encoder2count); Serial.print(" Enc3: "); Serial.println(encoder3count); 
}

