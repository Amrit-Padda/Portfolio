#include "mcp_can.h"
#include <SPI.h>
#include <stdio.h>


#define INT8U unsigned char
const int SPI_CS_PIN = 10;

INT8U len = 0;
INT8U buf[8];
unsigned char canId;
char str[20];


MCP_CAN CAN(SPI_CS_PIN);

int rpm1 ;
int rpm2 ;
int temp ;

void setup() {
 
  Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))   // canbus baudrate 500kbps
    {
        Serial.println("CAN BUS Shield init fail!!!");
        Serial.println("Init CAN BUS Shield again...");
        delay(100);
    }

    Serial.println("CAN BUS Initialisation Succesful");
}

void loop() {
  // put your main code here, to run repeatedly:
    while (CAN_MSGAVAIL == CAN.checkReceive())
        {
      CAN.readMsgBuf(&len, buf);
      canId = CAN.getCanId();
      rpm1 = buf[0];
      rpm2 = buf[1];
      temp = buf[2] ;
      }
      
       Serial.print(rpm1);
       Serial.print(rpm2);
       Serial.println(temp);
    }
}
