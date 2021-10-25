// demo: CAN-BUS Shield, send data
// loovee@seeed.cc

#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

// Define Joystick connection pins 
#define UP     A1
#define DOWN   A3
#define LEFT   A2
#define RIGHT  A5
#define CLICK  A4
    
//Define LED pins
#define LED2 8
#define LED3 7

#define StepValue 140

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

long GenPos = 0;
long GenVel = 200;

// variable to receive the message
unsigned char len = 0;
// variable to send the message
unsigned char buf[8];

void setup()
{
    SERIAL.begin(115200);
    delay(1000);
    while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");

    //Initialize pins as necessary
    pinMode(UP,INPUT);
    pinMode(DOWN,INPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
      
    //Pull analog pins high to enable reading of joystick movements
    digitalWrite(UP, HIGH);
    digitalWrite(DOWN, HIGH);
      
    //Write LED pins low to turn them off by default
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

    //read initial position
    //motor 1
    buf[0] = 0x94;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0x00;
    CAN.sendMsgBuf(0x141, 0, 8, buf);
    //receive message
    delay(3);
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        unsigned long canId = CAN.getCanId();
        SERIAL.print((buf[6] << 8) | buf[7]);
        SERIAL.print("\t");
        unsigned int EstPos = (buf[7] << 8) | buf[6];
        if (EstPos>18000) {
          GenPos = -36000+EstPos;
        } else {
          GenPos = EstPos;
        }
        SERIAL.print(buf[6], HEX);
        SERIAL.print("\t");
        SERIAL.print(buf[7], HEX);
        SERIAL.print("\t");
        SERIAL.print(EstPos);
        SERIAL.print("\t");
        SERIAL.println(GenPos);
    }
    delay(10000);
}


void loop()
{
    
    if (digitalRead(UP)==LOW)
    {
      GenPos = GenPos + StepValue;
    }

    if (digitalRead(DOWN)==LOW)
    {
      GenPos = GenPos - StepValue;
    }
    buf[0] = 0xA4;
    buf[1] = 0x00;
    buf[2] = GenVel;
    buf[3] = GenVel >> 8;
    buf[4] = GenPos;
    buf[5] = GenPos >> 8;
    buf[6] = GenPos >> 16;
    buf[7] = GenPos >> 24;
    CAN.sendMsgBuf(0x141, 0, 8, buf);

    delay(1);
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();
        
        //SERIAL.println("-----------------------------");
        //SERIAL.print("Get data from ID: 0x");
        //SERIAL.println(canId, HEX);

        for(int i = 0; i<len; i++)    // print the data
        {
            SERIAL.print(buf[i], HEX);
            SERIAL.print("\t");
        }
        unsigned int p_in = (buf[7] << 8) | buf[6];
        SERIAL.print(GenPos);
        SERIAL.println(" GenPos");
    }
    delay(1);
    
    buf[0] = 0x94;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0x00;
    CAN.sendMsgBuf(0x141, 0, 8, buf);

    delay(1);
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        unsigned long canId = CAN.getCanId();
        for(int i = 0; i<len; i++)    // print the data
        {
            SERIAL.print(buf[i], HEX);
            SERIAL.print("\t");
        }
        unsigned int p_in = (buf[7] << 8) | buf[6];
        SERIAL.println(p_in);
    }
   
    delay(10);                       // send data per 100ms
}

// END FILE
