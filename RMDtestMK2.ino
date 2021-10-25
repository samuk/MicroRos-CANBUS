/*
 * ESP32 CAN-Bus demo
 * 
 * www.skpang.co.uk
 * 
 * v1.0 March 2020
 * 
 * For use with ESP-32 CAN-Bus board
 * http://skpang.co.uk/catalog/esp32-canbus-board-p-1586.html
 * 
 */
#include <ESP32CAN.h>
#include <CAN_config.h>
#define ON  LOW
#define OFF HIGH

int LED_R = 2;
int LED_B = 4;
int LED_G = 15;


CAN_device_t CAN_cfg;
int i = 0;
long d = 0;
CAN_frame_t rx_frame;
unsigned long startMillis;  
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  
  digitalWrite(LED_B, ON);
  rx_frame.FIR.B.FF = CAN_frame_std;
  rx_frame.MsgID = 0x101;
  rx_frame.FIR.B.DLC = 8;
  rx_frame.data.u8[0] = 'h';
  rx_frame.data.u8[1] = 'e';
  rx_frame.data.u8[2] = 'l';
  rx_frame.data.u8[3] = 'l';
  rx_frame.data.u8[4] = 'o';
  rx_frame.data.u8[5] = 'c';
  rx_frame.data.u8[6] = 'a';
  rx_frame.data.u8[7] = i++;
    
  ESP32Can.CANWriteFrame(&rx_frame);
  startMillis = millis();
 
  
}
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
