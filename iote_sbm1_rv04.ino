/*
 * 
 * RAINCUBE V0.4
 * WIN 16 - SPR 17 
 * RAINCUBE LLC
 * BY CARLOS V.
 * 
 * DESCRIPTION:
 * The system provides remote access to 4 built-in SSRs and
 * an external HC-SR04 ultrasonic ranging sensor. SSRs 1-3
 * manage 24VAC solenoids. SSR 4 controls a 120VAC water
 * pump. Remote access via TCP connection over GPRS using
 * SIM900 module. 
 * 
 * CIRCUIT:
 * Main board based on Microchip ATmega328p microcontroller
 * with 4 built-in AC solid state relays, 5 pins for SIM900
 * GSM module, 4 pins for a HC-SR04 ultrasonic distance sensor,
 * and 6 pins for an external FTDI cable/board. All different
 * power requirements (120VAC, 24VAC, 5VDC) are met by
 * external components. 
 * 
 * LIBRARIES:
 * GSM GPRS Shield for Arduino Uno - Mod SIM900
 * 
 * CONSTANTS:
 * CUBE_PIN     -> input for distance sensor
 * MAX_DISTANCE -> max ping distance (cm) for rangefinder
 * HOST_NAME    -> server ip
 * HOST_PORT    -> server com port
 * 
 */

#include "SIM900.h"
#include "inetGSM.h"
#include <NewPing.h>
#include <SoftwareSerial.h>
InetGSM inet;

#define CUBE_PIN 13
#define MAX_DISTANCE 200
#define HOST_NAME "carlos.raincube.us"
#define HOST_PORT (3170)

boolean started = false;
char msg[50];
char inSerial[50];
char buffer[128];
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 60000;
const long interval2 = 3600000;

NewPing cubeSonar(CUBE_PIN, CUBE_PIN, MAX_DISTANCE);

void setup() {
  pinMode(4, OUTPUT);     //SSR #1 solenoid/zone 1
  pinMode(5, OUTPUT);     //SSR #2 solenoid/zone 2
  pinMode(6, OUTPUT);     //SSR #3 solenoid/zone 3
  pinMode(7, OUTPUT);     //SSR #4 water pump
  
  pinMode(8, OUTPUT);     //SSR #1 led board indicator
  pinMode(9, OUTPUT);     //SSR #2 led board indicator
  pinMode(10, OUTPUT);    //SSR #3 led board indicator
  pinMode(11, OUTPUT);    //SSR #4 led board indicator
  
  Serial.begin(9600);
  Serial.println("----------------------------\n\n");
  Serial.println("GSM Module Test\n");
  Serial.println("----------------------------\n\n");

  if(gsm.begin(9600)){
    Serial.println("----------------------------\n\n");
    Serial.println("Module Status: READY\n");
    Serial.println("----------------------------\n\n");
    started = true;
  }else{
    Serial.println("----------------------------\n\n");
    Serial.println("Module Status: IDLE\n");
    Serial.println("----------------------------\n\n");
  }

  if(started){

    /*
    if(inet.attachGPRS("internet.wind", "", "")){
      Serial.println("STATUS: ATTACHED\n");
    }else{
      Serial.println("STATUS: ERROR\n");
    }
    delay(1000);
    */

    gsm.SimpleWriteln("AT+CIFSR");
    delay(5000);

    gsm.WhileSimpleRead();
    
    if(inet.connectTCP(HOST_NAME, HOST_PORT)){
      Serial.println("----------------------------\n\n");
      Serial.println("NETWORK STATUS: READY\n");
      Serial.println("----------------------------\n\n");
    }else{
      Serial.println("----------------------------\n\n");
      Serial.println("NETWORK STATUS: ERROR\n");
      Serial.println("----------------------------\n\n");
    }
    delay(3000);

    sendData();
    delay(1000);
    sendData();

    Serial.println("----------------------------\n\n");
    Serial.println("INITIAL SETUP END\n");
    Serial.println("----------------------------\n\n");
  }
}

/*
 * Void Loop
 * Data uploads (buffer) with box id and water level every 60 seconds
 * System TCP connection status check every 60 minutes
 * Data downloads (buffer) constant read for status update
 * SSR mangement based on buffer content
 */
void loop(){
  
  unsigned long currentMillis = millis();

  //Data upload every 60 seconds
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    Serial.println("----------------------------\n\n");
    Serial.println("60s Interval - Data Upload");
    Serial.println("----------------------------\n\n");
    sendData();
  }

  //Check TCP connection every 60 minutes
  if(currentMillis - previousMillis >= interval2){
    previousMillis2 = currentMillis;
    if(inet.connectedClient()){
      Serial.println("----------------------------\n\n");
      Serial.println("Connection ok ...");
      Serial.println("----------------------------\n\n");
    }else{
      Serial.println("----------------------------\n\n");
      Serial.println("Reconnecting ...");
      Serial.println("----------------------------\n\n");
      inet.disconnectTCP();
      inet.connectTCP(HOST_NAME, HOST_PORT);
    }
  }

  //pre-designed buffer read function calls
  //serialhwread();
  //serialswread();

  /*
   * Operate peripherals based on incoming commands
   * Data string size: 6 characters (0 thru 5)
   * Char 0 & 1: zone number (1-3 based on board specs)
   * Char 2 & 3: open (OP) or close (CL) 
   * Char 4 & 5: irrigation time (non determined format)
   */

  //Read buffer download
  gsm.WhileSimpleReadReturn(buffer);
  Serial.println(buffer);

  //Perform operations based on buffer content
  if(buffer[1] == '1'){
    if(buffer[2] == 'O'){
      //zone 2 & zone 3 off
      digitalWrite(5, LOW);
      digitalWrite(10, LOW);
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);

      //pump on
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      
      //zone 1 on
      digitalWrite(4, HIGH);
      digitalWrite(9, HIGH);
    }else if(buffer[2] == 'C'){
      //pump & zone 1 off
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
    }
  }else if(buffer[1] == '2'){
    if(buffer[2] == 'O'){
      //zone 1 & zone 3 off
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);

      //pump on
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);

      //zone 2 on
      digitalWrite(5, HIGH);
      digitalWrite(10, HIGH);
    }else if(buffer[2] == 'C'){
      //pump & zone 2 off
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(5, LOW);
      digitalWrite(10, LOW);
    }
  }else if(buffer[1] == '3'){
    if(buffer[2] == 'O'){
      //zone 1 & zone 2 off
      digitalWrite(4, LOW);
      digitalWrite(9, LOW);
      digitalWrite(5, LOW);
      digitalWrite(10, LOW);

      //pump on
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);

      //zone 3 on
      digitalWrite(6, HIGH);
      digitalWrite(11, HIGH);
    }else if(buffer[2] == 'C'){
      //pump & zone 3 off
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);
    }
  }
}

/*
 * Harware read pre-designed function
 */
void serialhwread(){
  int i=0;
  
  if (Serial.available() > 0){
                
    while (Serial.available() > 0){
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }
    
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }
    
    inSerial[0]='\0';
    
  }
}

/*
 * Software read pre-designed function
 */
void serialswread(){
  gsm.SimpleRead();
}

/*
 * Function sendData
 * Data upload (buffer): box id and water level
 */
void sendData(){
  char charBuf[50];
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';
  
  //Data String: id=001&r=0& 
  String waterLevel = String(getRaincubeLevel());
  String boxID = "id=001&r=" + waterLevel + "&";
  int str_len = boxID.length();
  boxID.toCharArray(charBuf, str_len);
  char *dataStream = charBuf;

  gsm.SimpleWriteln("AT+CIPSEND");
  delay(3000);
  gsm.SimpleWrite(dataStream);
  gsm.SimpleWrite(end_c);
  delay(3000);
  
  Serial.println("---------------------");
  Serial.println("DATA UPLOAD COMPLETED");
  Serial.println("---------------------");  
}

/*
 * Function getRaincubeLevel
 * Collects data from hc-sr04 sensor
 */
unsigned int getRaincubeLevel(){
  unsigned int uS = cubeSonar.ping();
  return (uS/US_ROUNDTRIP_CM);  
}
