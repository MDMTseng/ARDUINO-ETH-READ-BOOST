#include <Ethernet.h>
#include "Eth_Boost.h"
#include "RingBuff.h"
#include <SPI.h>         // needed for Arduino versions later than 0018

#define UBYTE char
#define UINT unsigned int
//#define DEBUG_
#ifdef DEBUG_
#define DEBUG_print(A, ...) Serial.print(A,##__VA_ARGS__)
#define DEBUG_println(A, ...) Serial.println(A,##__VA_ARGS__)
#else
#define DEBUG_print(A, ...)
#define DEBUG_println(A, ...)
#endif

byte outputPin = 2;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 52);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
unsigned int localPort = 5213;      // local port to listen on
const unsigned int BufferL = 400;
char packetBuffer[BufferL]; //buffer to hold incoming packet,


EthernetUDP Udp;
RingBuff RB;
EthernetServer server(localPort);


byte LampArr[5] = {0, 0, 0, 0, 0};
char LampArrL[41] = {'0'};

char PkgTmple[]  =
  "HTTP/1.1 200 OK\r\n\
Content-Type: text/javascript\r\n\r\n\
<]P                                                                                 ";

char* p_PkgTmplepassage;


void setup() {
  cb_init(&RB, packetBuffer, sizeof(packetBuffer));
  //UDP_INIT();
  TCP_INIT();
  Serial.begin(57600);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(outputPin, OUTPUT);
  p_PkgTmplepassage = strstr(PkgTmple, "<]P"); //find anchor
  LampArrL[40] = '\0';
}
void UDP_INIT()
{
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
}
void TCP_INIT()
{

  Ethernet.begin(mac, ip);
  server.begin();
}

unsigned int TT = 0;
unsigned int packetSize = 0;
unsigned int packetSize2 = 0;
char S = 0;
EthernetClient client;
boolean ifReturn=false;
byte rip[4];
void loop() {

  //EthernetClient client = server.available();
  //if(client)
  client = server.available();  // try to get client
  if (client) {  // got client?
    //printclientIP();
    while (client.connected()) {
      if (client.available()) {   // client data available to read

        printclientIP();
        DEBUG_print(client._sock);
        DEBUG_print(":::");
        DEBUG_println(TT++);

        T0() ;
        client.stop();
      }
    }
  }
}


/*

char responsePkg2[]  =
"HTTP/1.1 200 OK\r\n\
Content-Type: text/javascript\r\n\
Access-Control-Allow-Origin: *\r\n\
\r\njQuery111107806073897518218_1402274103076({\"message\":\"10.0.0.6\"})\r\n" ;


char responsePkgOK[]  =
"HTTP/1.1 200 OK\r\n\
Date: Mon, 09 Jun 2014 01:17:25 GMT\r\n\
Server: Apache/2.2.22 (Debian)\r\n\
X-Powered-By: PHP/5.4.4-14+deb7u9\r\n\
Content-Length: 49\r\n\
Keep-Alive: timeout=5, max=99\r\n\
Connection: Keep-Alive\r\n\
Content-Type: text/javascript\r\n\
\r\n\
jQuery({\"name\":\"10.0.0.6\",\"message\":\"10.0.0.52\"})";
*/
int kkdkd = 0;

char* lampConCmd = "lcc";
char* readLampCmd = "rlc";
char NameBuff[10];
char ValueBuff[50];



void T0()
{
  unsigned int L = 0;
  L = RECVData(&RB, client._sock);
  //extern unsigned int CC_ADDR, CC_L;

  if (RB.head != RB.tail)
  {

    char r = 99;
    unsigned int Count = 0;
    unsigned int ArgHead = 0;
    char* argStart = 0;



    for (char * rV; rV = cb_pop_front(&RB);) {
      Count++;
      DEBUG_print(*rV);
      if (Count < 100 || argStart)
      {
        if (*rV == '?')argStart = rV;
        else if (argStart && *rV == ' ')
        {
          FindArg(argStart, rV, &RB);
          break;
        }

      }
      else
        break;
    }
    RB.head = RB.tail = packetBuffer;
    char buf[100];

    DEBUG_println();
    DEBUG_print("LL:"); DEBUG_println(L );
    //DEBUG_println(LampArr[0]);
    digitalWrite(outputPin, (LampArr[0] & 1) != 0);
    if(ifReturn)
    {
      sprintf(p_PkgTmplepassage, "jsonCB_ard({\"message\":\"%s\"});", LampArrL);
      //strcpy(p_PkgTmplepassage,buf);
      client.print(PkgTmple);
      //client.print(buf);
      DEBUG_println(buf);
      ifReturn=false;
    }
  }
}

inline char* CMDArrangement(char* Name, char* Var)
{
  DEBUG_print("Name:");
  DEBUG_print(Name);
  DEBUG_print("   Value:");
  DEBUG_println(Var);

  if (strcmp(Name, lampConCmd) == 0) //success
  {
    byte idx = 0;
    strcpy(LampArrL, Var);
    for (byte tmpv = 0; *Var; Var++, idx++)
    {
      tmpv = (tmpv << 1) | ((*Var == '1') ? 1 : 0);

      if (idx & 0x7 == 0x7)LampArr[(idx) >> 3] = tmpv;
    }
    return lampConCmd;
  }
  if (strcmp(Name, readLampCmd) == 0)
  {
    //DEBUG_println(LampArrL);
    ifReturn=true;
    char* LampArrLptr = LampArrL;
    /* for(byte i=0;i<sizeof(LampArr);i++)
     {
        byte tmpV=LampArr[i];
        for(byte j=0;j<8;j++,tmpV<<=1)
          *LampArrLptr++=(tmpV&0x80)?'1':'0';
     }*/
    return readLampCmd;
  }


}

inline void FindArg(char* argS, char* argE, RingBuff* rB)
{
  //"?Arg1=value1&&Arg2=value2&Arg3=value3"
  if (argS == rB->buffer_end)argS = rB->buffer;
  else argS++;//for skip "?"

  //rule: start with Arg name, Arg value is between "=" and (argE or "&")
  byte benchSW = 0; //0 is name, 1 is value
  byte wIdx = 0;
  for (; argS != argE; argS++)
  { if (argS > rB->buffer_end)argS = rB->buffer;
    if (*argS == '=') {
      benchSW = 1;
      NameBuff[wIdx++] = '\0';
      wIdx = 0;
      continue;
    }
    if (*argS == '&') {
      benchSW = 0;
      ValueBuff[wIdx++] = '\0';
      CMDArrangement(NameBuff, ValueBuff);
      wIdx = 0;
      continue;
    }
    if (benchSW == 0)NameBuff[wIdx++] = *argS;
    else if (benchSW == 1)ValueBuff[wIdx++] = *argS;
  }
  ValueBuff[wIdx++] = '\0';
  CMDArrangement(NameBuff, ValueBuff);
}
void printclientIP()
{
  client.getRemoteIP(rip);
  for (int bcount = 0; bcount < 4; bcount++)
  {
    DEBUG_print(rip[bcount], DEC);
    if (bcount < 3) DEBUG_print(".");
  }
  DEBUG_println();
}

