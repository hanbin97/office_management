#include "SoftwareSerial.h"
#include "WiFiEsp.h"

const byte RELAY = 8;
const byte PIR = 6;
const byte BUTTON = 2;  //BUTTON Interrupt pin

byte Flag = 0;

volatile byte ramp = LOW;  //Variables for interrupt functions, ON/OFF VALUE OF LAMP


unsigned long Time_count = 0;   //Variable for using motion sensor millis()
unsigned long Time_count2 = 0;   //Variables for using wifi communication millis()

SoftwareSerial Serial1(4, 5); // RX, TX

char ssid[] = "wifi ID"; // your network SSID (name)

char pass[] = "wifi PASSWORD"; // your network password

int status = WL_IDLE_STATUS; // the Wifi radio's status

IPAddress serverIP(server ip); //Categorized by , not .  / ex) 192,0,0,1

WiFiEspClient client;

char strBuf[255];

void setup() {
  
  pinMode(RELAY, OUTPUT); //Set digital port to output
  pinMode(PIR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);  //INPUT_PULLUP FOR USING INTERRUP

  attachInterrupt(digitalPinToInterrupt(BUTTON), BT_ISR, CHANGE); //Interrupt Settings
  
  Serial.begin(9600);

  // initialize serial for ESP module

  Serial1.begin(9600);

  // initialize ESP module

  WiFi.init(&Serial1);
  // attempt to connect to WiFi network

  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to WPA SSID: ");

    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

  }

  Serial.println("You're connected to the network");

  client.connect(serverIP,9090);

}

void loop() {
  int BUTTON_Value = digitalRead(BUTTON);  //Read the value of the input pin and save it to a variable
  int PIR_Value = digitalRead(PIR);

  digitalWrite(RELAY, ramp);
  
  //Motion Sensing Part
  if(ramp){
    if(PIR_Value == LOW){  //If there's no movement,
      
      if(Time_count == 0){    //First time to discover no movement.
        Time_count = millis();
      }
      else{       
        
       // Serial.print("No movement(ms) : ");
        //Serial.println(millis() - Time_count);

        if(millis() - Time_count > 10000){    //It's been more than 10 seconds since we first found it.
          Serial.println("No movement, turn off the LED.");
          Serial.println("");
          digitalWrite(RELAY, LOW);
          ramp = LOW;
        
        Time_count = 0;
        }
        
      }
      
    }
    else{
      Time_count = 0;
    }

  }
  else{
    Time_count = 0;
  }

  //Socket communication part
  if(client.available()){
    memset(strBuf,0x00,sizeof(strBuf));
    int recv=client.read(strBuf,255);
    Serial.write(strBuf);
    Serial.println("");
  }

  if(Time_count2 == 0){    // Pass values every n seconds
        Time_count2 = millis();
        if(ramp){
          client.write("201YES");
        }
        else{
          client.write("201NO");
        }
  }
  else{
    if(millis() - Time_count2 > 10000){    // 10 second
      Time_count2 = 0;
    }
  }


  
//  if(Serial.available()){
//    String recvStr=Serial.readString();
//    client.write((char*)recvStr.c_str(),recvStr.length());
////    client.write(Serial.read());


//  }

}


void BT_ISR(){    //Function executed when button interrupt occurs
  if(digitalRead(BUTTON)==LOW){
    if(Flag==0){  //Run if Flag is 0
      ramp = !ramp;

      Flag = 1;
  
      if(ramp){
        Serial.println("LED ON");
        Serial.println("");
      }
      else{
        Serial.println("LED OFF");
        Serial.println("");
      }
      delay_(200);
    }  
    else{
      //No response.
    }
  }
  else{ //If the switch is not pressed, turn Flag back to zero.
    Flag=0;
    //Serial.println("떼졌다");
  }

}

unsigned long count = 0;
void delay_(int ms){  //Function added to use delay within interrupt

  while(count!=ms){
    delayMicroseconds(1000);
    count++;
  }
  count=0; 
}
