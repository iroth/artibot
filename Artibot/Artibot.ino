#include <Servo.h> 
#include <Math.h>
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x90, 0xA2, 0xBA, 0x0D, 0x2F, 0x19 };
IPAddress server(192,168,2,102); // Insert server address here

EthernetClient client;

Servo shoulder;   // create servo object to control a servo 
Servo elbow;      // create servo object to control a servo 
Servo pen;        // create servo object to control a servo 

float origX = 0.0;
float origY = 0.0;

int penIsDown = 0;

int lastB = 0;
int lastA = 0;

void movePenUp() {
      pen.write(0);
      penIsDown = 0;
      delay(20);}

void movePenDown() {
      pen.write(10);
      penIsDown = 1;
      delay(20);
}

// this function moves the shoulder smoothly to avoid abrupt movements

void changeSmoothlyA (int fromVal, int toVal) {
  if (fromVal == toVal) {
    return;
  }
  if (fromVal < toVal) {
    for (int v = fromVal ; v <= toVal ; v++) {
      shoulder.write(v);
      delay(20);
    }
  }
  else {
    for (int v = fromVal ; v >= toVal ; v--) {
      shoulder.write(v);
      delay(20);
    }
  }
}

// this function moves the elbow smoothly to avoid abrupt movements

void changeSmoothlyB (int fromVal, int toVal) {
  if (fromVal == toVal) {
    return;
  }
  if (fromVal < toVal) {
    for (int v = fromVal ; v <= toVal ; v++) {
      elbow.write(v);
      delay(20);
    }
  }
  else {
    for (int v = fromVal ; v >= toVal ; v--) {
      elbow.write(v);
      delay(20);
    }
  }
}

void setArmPosition (int a, int b) {
      changeSmoothlyA(lastA, a);
      lastA = a;
      delay(20);
      changeSmoothlyB(lastB, b);
      lastB = b;
      delay(20);
}

void setup() 
{ 
  shoulder.attach(9);   // attaches the servo on pin 9 to the servo object 
  elbow.attach(8);      // attaches the servo on pin 8 to the servo object 
  pen.attach(7);        // attaches the servo on pin 7 to the servo object 

   Serial.begin(9600);  // use serial for debugging - very usefull...
   while (! Serial) {
     ;
   }

   Serial.println("Starting...");
  
  lastA = 10;
  shoulder.write(lastA);
  delay(40);
  lastB = 10 ;
  elbow.write(lastB);
  delay(40);
  movePenUp();
  
  setupConnection();
}

void setupConnection(){
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for(;;)   // no point in carrying on, so do nothing forevermore:
      ;
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}
 
// the following function sends a request for the current alpha (shoulder position)

void getAlphaValues(int alpha) {
  Serial.println("GetValue. connecting...");

    if(client.connect(server, 8081)){
      Serial.println("Connection established.");
      // Make a HTTP request:
      String request = String("GET /arduino?alpha=");
      request += alpha;
      request += " HTTP/1.0";
          
      Serial.println(request);
      client.println(request);
      client.println();
    } else{
      Serial.print("Connection failed.");
      for(;;)  // no point in going on...
        ;
    }

}

int resetMovingSpeed = 400;

void reset(){
  Serial.println("Elbow Left to origin point");
  for (int b = lastB ; b >= 10 ; b = b - 1) {
    lastB = b;
    elbow.write (b);
    delay (resetMovingSpeed);
  }
  for (int a = lastA ; a >= 10 ; a = a - 1) {
    lastA = a;
    shoulder.write (a);
    delay (resetMovingSpeed);
  }
  movePenUp();
}

char getChar() {
    while (! client.available()) {
      delay(5);
    }
    return client.read();
}

void drawDot(int intensity) {
  for (int i = 0 ; i < intensity ; i++){
    movePenDown();
    delay(200);
    movePenUp();
    delay (20);
  }
}

void loop() 
{
  
  movePenUp();
  int a;
  int b;
  byte c;

  for (a = 40 ; a < 120 ; a = a + 1) {
    setArmPosition(a,lastB);
    delay(100);
    Serial.print("Now shoulder at: ");
    Serial.println(a);
   
    getAlphaValues(a);
    delay(20);
 
    if (client.connected()) {
        Serial.println("client Connected. Process input.");
        byte firstByte;
        int i = 0;
        do{
          firstByte = (byte)getChar();
          if (firstByte == 10){
            i++;
          }
        }while (i <= 6);
        firstByte = (byte)getChar();
        Serial.println(firstByte);
        if( firstByte == (byte)'0') {
           Serial.println("Finish printing! next request in 6 sec...");
           reset();
           delay(6000);
           break;
         }
         else if(firstByte == (byte)'1') {
           Serial.println("First byte is 1");
           for(b = 10 ; b <= 120 ; b++) {
              c = (byte) getChar();
              delay(20);
              // here we added support for 4 levels of intensity (0 to 3 dots drawn)
              if(c < 64) {
                 setArmPosition (a,b);
                 drawDot(3);
               }
              else if(c < 128) {
                  setArmPosition (a,b); 
                 drawDot(2);
               }
              else if(c < 192) {
                 setArmPosition (a,b); 
                 drawDot(1);
               }
            }
         }
         else {
           Serial.println(String("Wrong reponse!!" + firstByte));
           for(;;)  // no point in doing anything
             ;
         }
    } // end if client connected

      client.stop();
      while (client.status() != 0) {
           delay(5);
      }
      Serial.println("client stopped. will issue another request in a sec...");
      delay (100);

    } // end for loop on alpha

      changeSmoothlyA(lastA, 10);
      lastA = 0;
      delay(20);
      changeSmoothlyB(lastB, 0);
      lastB = 0;
   
    Serial.println("done drawing...");
    for (;;)
      ;

} // end loop function
