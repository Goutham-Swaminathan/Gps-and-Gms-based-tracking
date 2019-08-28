#include <SoftwareSerial.h>
SoftwareSerial gsm(7,8); // RX, TX

//String str="";
char str[70];
String gpsString="";

char *test="$GPGGA";

String latitude="No Range      ";
String longitude="No Range     ";

int temp=0,i;
boolean gps_status=0;

void setup() 
{
  Serial.begin(9600);
  gsm.begin(9600);
  delay(200);
  gsm_init();
  gsm.println("AT+CNMI=2,2,0,0,0");
  get_gps();
  delay(200);
  temp=0;
  Serial.println("setup done");
}

void loop()
{
  serialEvent();
  if(temp)
  {
    Serial.println("in loop");
    get_gps();
    tracking();
  }
}

void serialEvent()
{
  while(gsm.available())
  {
    if(gsm.find("Send"))
    {
      Serial.println("Send recieved");
      temp=1;
      break;
    }
    else
    temp=0;
  }
}

void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (Serial.available()>0)            //checking serial data from GPS
   {
    char inChar = (char)Serial.read();
     gpsString+= inChar;                    //store data from GPS into gpsString
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //checking for $GPGGA sentence
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void gsm_init()
{
  boolean at_flag=1;
  while(at_flag)
  {
    gsm.println("AT");
    while(gsm.available()>0)
    {
      if(gsm.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
}

void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    longitude="";
    int comma=0;
    while(x<str_lenth)
    {
      if(gpsString[x]==',')
      comma++;
      if(comma==2)        //extract latitude from string
      latitude+=gpsString[x+1];     
      else if(comma==4)        //extract longitude from string
      longitude+=gpsString[x+1];
      x++;
    }
    int l1=latitude.length();
    latitude[l1-1]=' ';
    l1=longitude.length();
    longitude[l1-1]=' ';
    i=0;x=0;
    str_lenth=0;
    delay(2000);
   }
}

void init_sms()
{
  gsm.println("AT+CMGF=1");
  delay(400);
  gsm.println("AT+CMGS=\"+918951372303\"");   // use your 10 digit cell no. here
  delay(400);
}

void send_data(String message)
{
  gsm.println(message);
  delay(200);
}

void send_sms()
{
  gsm.write((char)26);
}

void tracking()
{
    init_sms();
    send_data("Vehicle Tracking Alert:");
    send_data("Your Vehicle Current Location is:");
    gsm.print("Latitude:");
    send_data(latitude);
    gsm.print("Longitude:");
    send_data(longitude);
    send_sms();
    delay(2000);
}
