/*

 Basic program to transmitt frames  
*/

#include <due_can.h>  //https://github.com/collin80/due_can
#include <due_wire.h> //https://github.com/collin80/due_wire
#include <DueTimer.h>  //https://github.com/collin80/DueTimer
#include <Wire_EEPROM.h> //https://github.com/collin80/Wire_EEPROM


//These two lines redefine any serial calls to the native Serial port on the CANdue or EVTVDue and enables syntactic sugar to allow streaming to serial port Serial<<"Text";
#define Serial SerialUSB
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } //Sets up serial streaming Serial<<someshit;

 // Useful macros for setting and resetting bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
unsigned int RPM;
byte  Gcount; //gear display counter byte
unsigned int GLeaver;  //unsigned int to contain result of message 0x192. Gear selector lever position
int shiftPos; //contains byte to display gear position on dash



CAN_FRAME outframe;  //A structured variable according to due_can library for transmitting CAN data.




void setup() 
  {
  Serial.begin(115200);
  
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can0.begin(CAN_BPS_500K);
  Can1.begin(CAN_BPS_500K);
   DashOn(); //turn on tacho backlight and set rpm limit bar

    Timer4.attachInterrupt(Frames10MS).start(10000); // Send frames every 10ms
    Timer3.attachInterrupt(Frames200MS).start(200000); // Send frames every 200ms
   


Can1.watchFor(0x192); //watch out for messages from the gear lever and use mailbox 1.      
Can1.attachCANInterrupt(Incoming);   
     
RPM=1000;
Gcount=0x0d;
 shiftPos=0xb4; //select neutral


  
}
   



void loop()
{ 

 
      
}















void Frames10MS()
{
        word RPM_A;// rpm value for E90
        RPM_A=RPM*4;
        outframe.id = 0x0AA;            // Set our transmission address ID
        outframe.length = 8;            // Data payload 8 bytes
        outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
        outframe.rtr=1;                 //No request
        outframe.data.bytes[0]=0x5f;
        outframe.data.bytes[1]=0x59;  
        outframe.data.bytes[2]=0xff;
        outframe.data.bytes[3]=0x00;
        outframe.data.bytes[4]=lowByte(RPM_A);
        outframe.data.bytes[5]=highByte(RPM_A);
        outframe.data.bytes[6]=0x80;
        outframe.data.bytes[7]=0x99;
       


        Can1.sendFrame(outframe); 


       
}

void Frames200MS()
{


///////////////////////////////////////////////////////////////////////////////////////////////////
        outframe.id = 0x1D2;            // current selected gear message
        outframe.length = 5;            // Data payload 5 bytes
        outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
        outframe.rtr=1;                 //No request
        outframe.data.bytes[0]=shiftPos;  //e1=P  78=D  d2=R  b4=N
        outframe.data.bytes[1]=0x0c;  
        outframe.data.bytes[2]=0x8f;
        outframe.data.bytes[3]=Gcount;
        outframe.data.bytes[4]=0xf0;
        Can1.sendFrame(outframe);
        ///////////////////////////
        //Byte 3 is a counter running from 0D through to ED and then back to 0D///
        //////////////////////////////////////////////

         Gcount=Gcount+0x10;
         if (Gcount==0xED)
         {
          Gcount=0x0D;
         }
         
///////////////////////////////////////////////////////////////////////////////////////////////////////////

       
}


void DashOn(){

        outframe.id = 0x332;            // dash on message
        outframe.length = 2;            // Data payload 5 bytes
        outframe.extended = 0;          // Extended addresses - 0=11-bit 1=29bit
        outframe.rtr=1;                 //No request
        outframe.data.bytes[0]=0x61;  //
        outframe.data.bytes[1]=0x82;  
        Can1.sendFrame(outframe);
  
    
}


void Incoming (CAN_FRAME *frame){

      GLeaver=frame->data.low;
      GLeaver=GLeaver&0x00ffffff; //mask off byte 3
     // Serial.println(GLeaver,HEX);
    switch (GLeaver) {
      case 0x80006a:  //not pressed
        
        break;
      case 0x80506a:  //park button pressed
        shiftPos=0xe1;
        break;
      case 0x800147:  //R position
        shiftPos=0xd2;
        break;
      case 0x80042d: //R+ position
             shiftPos=0xb4; //select neutral
        break;
      case 0x800259:  //D pressed
            shiftPos=0x78;
        break;
      case 0x800374:  //D+ pressed
            shiftPos=0xb4; //select neutral
        break;
      case 0x81006a:  //Left Back button pressed
 
        break;
      case 0x82006a:  //Left Front button pressed
 
        break;
      case 0x84006a:  //right Back button pressed
 
        break;

      case 0x88006a:  //right Front button pressed
 
        break;

      case 0xa0006a:  //  S-M-D button pressed
 
        break;        
      default:
      {
        
      }
      }
    }

      













