#include <max6675.h>
#include <mcp_can.h>
#include <SPI.h>

// pins 
  int hall-sens = 2 ;
  int inductive-sens = 3 ;

  int ktcSO = 8 ;
  int ktcCS = 9 ;
  int ktcCLK = 10 ;

  int spi-Can = 11 ;
//----------------

  int half_revolutions1 = 0;
  int half_revolutions2 = 0;
  int rpm1 = 0;
  int rpm2 = 0;
  unsigned long lastmillis = 0;
 
  MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
  MCP_CAN CAN(spi-Can) ;
 
 void setup()
 {
   Serial.begin(9600);
   pinMode(hall-sens,INPUT_PULLUP) ;
   pinMode(inductive-sens,INPUT_PULLUP) ;
   attachInterrupt(digitalPinToInterrupt(hall-sens), magnet_detect1, FALLING);
   attachInterrupt(digitalPinToInterrupt(inductive-sens), magnet_detect2, FALLING);

   while (CAN_OK != CAN.begin(CAN_500KBPS))  // baudrate 500kbps
  {
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
      delay(100);
  }

   
 }
 void loop()//Measure RPM
 {
  if (millis() - lastmillis == 1000){ //Uptade every one second, this will be equal to reading frecuency (Hz).
       detachInterrupt(digitalPinToInterrupt(2));//Disable interrupt when calculating
       detachInterrupt(digitalPinToInterrupt(3));//Disable interrupt when calculating
       rpm1 = half_revolutions1 * 60; // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.
       rpm2 = half_revolutions2 * 60; // Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use half_revolutions * 30.

       unsigned char canMsg[8] = {rpm1, rpm2,ktc.readCelsius(),0x00, 0x00, 0x00, 0x00, 0x00} ;
       CAN.sendMsgBuf(0x07B,0,8,canMsg) ;

       half_revolutions1 = 0; // Restart the RPM counter
       half_revolutions2 = 0; // Restart the RPM counter
       lastmillis = millis(); // Uptade lasmillis
       
       attachInterrupt(digitalPinToInterrupt(2), magnet_detect, FALLING); //enable interrupt
       attachInterrupt(digitalPinToInterrupt(3), magnet_detect, FALLING); //enable interrupt
   }
 }

void magnet_detect1()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   half_revolutions1++ ;
 }
 
void magnet_detect2()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   half_revolutions2++ ;
 }
