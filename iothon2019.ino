/*

*/

#include <MKRNB.h>
#include <coap-simple.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;

unsigned int localPort = 2390;      // local port to listen for UDP packets

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);

// CoAP server endpoint url callback
void callback_light(CoapPacket &packet, IPAddress ip, int port);

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

NBClient ipclient;
// URL, path and port (for example: example.org)
char server[] = "ifconfig.co";
char path[] = "/ip";
int port = 80; // port 80 is the default for HTTP

char dserver[] = "hedebele.pythonanywhere.com";
char dpath[] = "/device1";

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

//device parameters
bool devOn;
String powerConsumption;

// A UDP instance to let us send and receive packets over UDP
NBUDP Udp;

// UDP and CoAP class
Coap coap(Udp);

// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  String message(p);

  if (message.equals("0"))
    LEDSTATE = false;
  else if(message.equals("1"))
    LEDSTATE = true;
      
  if (LEDSTATE) {
    digitalWrite(LED_BUILTIN, HIGH) ; 
    coap.sendResponse(ip, port, packet.messageid, "1");
  } else { 
    digitalWrite(LED_BUILTIN, LOW) ; 
    coap.sendResponse(ip, port, packet.messageid, "0");
  }
}

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Coap Response got]");
  
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  Serial.println(p);
}

void getIp()
{
  if (ipclient.connect(server, port)) {
    Serial.println("connected");
    // Make a HTTP request:
    ipclient.print("GET ");
    ipclient.print(path);
    ipclient.println(" HTTP/1.1");
    ipclient.print("Host: ");
    ipclient.println(server);
    ipclient.println("Connection: close");
    ipclient.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void getData()
{
  String content;
  char lastchar;
  NBClient httpclient;
  Serial.println("Connecting to get data...");
  if (httpclient.connect(dserver, port)) {
    Serial.println("connected");
    // Make a HTTP request:
    httpclient.print("GET ");
    httpclient.print(dpath);
    httpclient.println(" HTTP/1.1");
    httpclient.print("Host: ");
    httpclient.println(dserver);
    httpclient.println("Connection: close");
    httpclient.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  while(true)
  {
    if (httpclient.available()) {
      lastchar = (char)httpclient.read(); 
      if (lastchar == '_')
      {
        Serial.println();
        Serial.println("stopping...");

        if (content.indexOf("ON") > 0)
        {
          devOn = true;
          powerConsumption = content.substring(content.indexOf("ON")+3);
        }
        if (content.indexOf("OFF") > 0)
        {
          devOn = false;
          powerConsumption = content.substring(content.indexOf("OFF")+4);
        }

        Serial.println(devOn);
        Serial.println(powerConsumption);
        
        delay(100);
        //httpclient.stop();
        delay(100);
        return;
      }
      content+= lastchar;
      Serial.print(lastchar);
    }

    // if the server's disconnected, stop the client:
    if (!httpclient.available() && !httpclient.connected()) {
      delay(100);
      Serial.println();
      Serial.println("disconnecting.");
      httpclient.stop();
  
      break;
    }
  }
}

#define HYPERDISPLAY_USE_PRINT 1
#include "HyperDisplay_UG2856KLBAG01.h"   // Your library can be installed here: http://librarymanager/All#SparkFun_Transparent_Graphical_OLED
                                          // The rest of the Layer Cake:         http://librarymanager/All#SparkFun_HyperDisplay_SSD1309
                                          //                                     http://librarymanager/All#SparkFun_HyperDisplay                                          

#define HYPERDISPLAY_USE_PRINT 1
//////////////////////////
//      User Setup      //
//////////////////////////
#define SERIAL_PORT Serial  
#define WIRE_PORT Wire      // Used if USE_SPI == 0
#define SPI_PORT SPI        // Used if USE_SPI == 1

#define RES_PIN 2           // Optional
#define CS_PIN 4            // Used only if USE_SPI == 1
#define DC_PIN 5            // Used only if USE_SPI == 1

#define USE_SPI 0           // Choose your interface. 0 = I2C, 1 = SPI

// Object Declaration. A class exists for each interface option
#if USE_SPI
  UG2856KLBAG01_SPI myTOLED;  // Declare a SPI-based Transparent OLED object called myTOLED
#else
  UG2856KLBAG01_I2C myTOLED;  // Declare a I2C-based Transparent OLED object called myTOLED
#endif /* USE_SPI */

void setupDisplay()
{  
  #if USE_SPI 
  SPI_PORT.begin();
  myTOLED.begin(CS_PIN, DC_PIN, SPI_PORT);                  // Begin for SPI requires that you provide the CS and DC pin numbers
#else
  WIRE_PORT.begin();
  myTOLED.begin(WIRE_PORT, false, SSD1309_ARD_UNUSED_PIN);  // Begin for I2C has default values for every argument
  Wire.setClock(400000);
#endif /* USSE_SPI */

  bool kedi = true;
  
  myTOLED.setCurrentWindowColorSequence((color_t) &kedi);
  
  myTOLED.setTextCursor(20,20);

  myTOLED.println("Starting...");

  //myTOLED.rectangleSet(0, 0, 65, 35,true);

  //myTOLED.setCurrentWindowColorSequence((color_t) &kedi);

  //myTOLED.setTextCursor(0,0);

  /*myTOLED.write('k');
  myTOLED.write('e');
  myTOLED.write('d');
  myTOLED.write('i');*/
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  setupDisplay();

  Serial.println("Starting Arduino GPRS NTP client.");
  // connection state
  boolean connected = false;

  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);  
    }
  }

  //Get IP.
  IPAddress LocalIP = gprs.getIPAddress();
  Serial.println("Server IP address=");
  Serial.println(LocalIP);

  //getIp();

  // LED State
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  LEDSTATE = true;

  // add server url endpoints.
  // can add multiple endpoint urls.
  // exp) coap.server(callback_switch, "switch");
  //      coap.server(callback_env, "env/temp");
  //      coap.server(callback_env, "env/humidity");
  Serial.println("Setup Callback Light");
  coap.server(callback_light, "light");

  // client response callback.
  // this endpoint is single callback.
  Serial.println("Setup Response Callback");
  coap.response(callback_response);

  /*Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);*/
  
  // start coap server/client
  coap.start();
}

void updateDisplay()
{
  bool kedi = true;
  
  myTOLED.setCurrentWindowColorSequence((color_t) &kedi);

  myTOLED.rectangleClear(20, 20, 128, 28, true); //clear display
  myTOLED.rectangleClear(50, 40, 80, 48, true); //clear display
  
  myTOLED.setTextCursor(20,20);
  
  if (devOn)
  {
    myTOLED.println("Device running...");
  }
  else
  {
    myTOLED.println("Device is off");
  }

  myTOLED.setTextCursor(50,40);
  myTOLED.println(powerConsumption);
}

void loop()
{  
  getData();
  updateDisplay();
  delay(1000);
  /*if (ipclient.available()) {
    Serial.print((char)ipclient.read());
  }

  // if the server's disconnected, stop the client:
  if (!ipclient.available() && !ipclient.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    ipclient.stop();

    delay(2000);

    getData();
  }*/
  
  // send GET or PUT coap request to CoAP server.
  // To test, use libcoap, microcoap server...etc
  // int msgid = coap.put(IPAddress(10, 0, 0, 1), 5683, "light", "1");
  //coap.me
  //Serial.println("Send Request");
  //134.102.218.18
  //int msgid = coap.get(IPAddress(134, 102, 218, 18), 5683, "3");
  
  coap.loop();
}
