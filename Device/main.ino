#include <TelstraWeb.h>
#include <connection4g.h>
#include <TelstraM1Device.h>
#include <TelstraM1Interface.h>
#include "azureIoT.h"
#include "jsonGenerator.h"
#include "OLED.h"
#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library

#define SECURE_CONNECTION true
#define INSECURE_CONNECTION false

////////////
MMA8452Q accel;
OLED OLED;
#define BUTTON 7
#define LED 13
///////////
//======== Variables required for FSM ============
int FREQ_RATE = 25; //25Hz
float PERIOD = 1/FREQ_RATE; //Seconds
const int size_mem = 75; //FREQ_RATE @ 25Hz = 75 samples for 3 seconds (pre,impact,post)
const int scale = 2;
float x[scale*size_mem], y[scale*size_mem], z[scale*size_mem], acc_vm[scale*size_mem]; // can be changed during testing
int glob_count = 0;
//============================================================
int state, multi_peak_timer, sg_timer, peak_time;
float peak = 0, temp_peak = 0;
float accel_vm;
unsigned long time1, time2, res_time;
float threshold = 1.8;
unsigned long wait = 0;
//================================================
struct SensorMeasurements {
  float xAccelReadings[size_mem];
  float yAccelReadings[size_mem];
  float zAccelReadings[size_mem];
  float accelVM[size_mem];
};
float calculatedAccelX; 
float calculatedAccelY;
float calculatedAccelZ;
int decimal_precision=2;
float decimal_cal=pow(10,decimal_precision)*1.0;
////////=============================================

//need to turn on, to display on serial monitor
boolean debugger = true;

char host[] = "Host Name";
char path[] = "/devices/telstrakit/messages/events?api-version=2016-02-03"; // 
int port = 443;
//--Unsecure---

TelstraM1Interface commsif;
TelstraM1Device IoTDevice(&commsif);
Connection4G conn(SECURE_CONNECTION,&commsif);
TelstraWeb WebIoT(&conn,&IoTDevice);
AzureIoT myAzure("Host Name", "telstrakit","KEY",10080);
JsonGenerator myJson((BATTERY_STATUS | BATTERY_STATE_OF_CHARGE | TEMPARATURE | LIGHT_LEVEL), size_mem);

boolean port_open = false;
int message_count =0;
int message_count_limit=300;
int message_trig=0;
void setup() {
   
  commsif.begin(); // Must include for proper SPI communication
  myAzure.init(&IoTDevice);
//  Serial.begin(115200);
//  if(debugger){
//  while(Serial.available()==0);
//  serialFlush();
//  }
  
  Serial.println("Waiting until cellular system has finished booting...");
 // Printlog("Waiting until cellular system has finished booting...");
  IoTDevice.waitUntilCellularSystemIsReady();
  Serial.println(" Opening TCP connection!");
  WebIoT.setHost(host,path,port);
  
  if(conn.openTCP(host,port)==CONNECTION4G_STATUS_OK){
    port_open=true;
    //Serial.println("(initial port is  " +String(port_open));
    Serial.println("open TCP Success!");
    delay(500);
//    myJson.collectSensorData(&IoTDevice);
    myAzure.setPostContent(myJson.makeJsonDataWithAccel(String(message_count),"Telstrakit",x,y,z));
    // Build HTTPS request.
    myAzure.setPostContent(myJson.printJson());
    Serial.println(myAzure.getPostPacket());
    WebIoT.post(myAzure.getPostPacket());
    Serial.println("Packet arrived");
    
  } else {
     Serial.println(" OpenTCP() failed.");
  }
//==========================================================================================
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH); 
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  OLED.begin();
  OLED.fill_OLED(0x00,0x00,0x00); // Clear screen
  OLED.drawString("Press the buttonn ", 15, 15, 255, 255, 255, 1);
  while(digitalRead(BUTTON)); 
  //initialize sensors and connection to the network
  accel.init(SCALE_8G); //the original code used 8G accel scale
//==========================================================================================

 
        
        
  //==========================================================================================
}

void loop() {
Serial.println("state is" + String(state));
Serial.println("global count = "+ String(glob_count));
 //==========================================================================================//
//  Getting Accelerometer data                                                                                 //
//==========================================================================================//
    
    accel.read();
    calculatedAccelX = accel.cx;
    calculatedAccelY = accel.cy;
    calculatedAccelZ = accel.cz;

    accel_vm = sqrt(calculatedAccelX*calculatedAccelX + calculatedAccelY*calculatedAccelY + calculatedAccelZ*calculatedAccelZ);
     
    x[glob_count] = (int(calculatedAccelX*decimal_cal))/decimal_cal;
    y[glob_count] = (int(calculatedAccelY*decimal_cal))/decimal_cal;
    z[glob_count] = (int(calculatedAccelZ*decimal_cal))/decimal_cal;
    acc_vm[glob_count] = accel_vm;
    
    glob_count++;
    kav_mia();
  
//==========================================================================================//
//  Dynamic Timing control using Delay function of Arduino                                                                                  //
//==========================================================================================//
 time2=millis();
    unsigned long elapsed = time2 - time1;
    if (elapsed < PERIOD*1000) { //convert to milliseconds
      wait = PERIOD*1000 - elapsed;
      OLED.drawString(String(wait), 90, 1, 0, 255, 0,1);
      Serial.println(String(wait));
      delay(wait);
    }  else {
//    Serial.println("The process takes more time");
    }
    time1=millis();
 //==========================================================================================//
//  Sending Accel data to HUB             //
//==========================================================================================//
//  if(port_open){
//    if(message_trig >1){
//      Serial.println("port is  opens");
//      if(message_count <message_count_limit){
//            Serial.println("generating Json");
//            JsonGenerator myJson((BATTERY_STATUS | BATTERY_STATE_OF_CHARGE | TEMPARATURE | LIGHT_LEVEL), size_mem);
//
//           // myJson.collectSensorData(&IoTDevice);
//            delay(500);
//            Serial.println("SetupPostPacket");
////            Serial.println(sizeof(x)/sizeof(char *));
//            myAzure.setPostContent(myJson.makeJsonDataWithAccel(String(message_count),"Telstrakit",x,y,z));
//            // Build HTTPS request.
////            Serial.println(myAzure.getPostPacket());
//            WebIoT.post(myAzure.getPostPacket());
//            Serial.println("Packet arrived");
//            delay(500);
//            message_count++;
//         }else{
//              Serial.println("Session end, No Packet Post");
//              conn.closeTCP();
//            }
//         message_trig=0;
//       }
//  }else{
//  Serial.println("Waiting until cellular system has finished booting...");
//  IoTDevice.waitUntilCellularSystemIsReady();
//  Serial.println(" Opening TCP connection!");
//  WebIoT.setHost(host,path,port);
//  if(conn.openTCP(host,port)==CONNECTION4G_STATUS_OK){
//    port_open=true;
//    //Serial.println("(initial port is  " +String(port_open));
//    Serial.println("open TCP Success!");
//      }else {    Serial.println(" OpenTCP() failed.");
//        }  
//   }
//==========================================================================================//
//==========================================================================================//  
 
}

void send_message(){
    if(port_open){
    
      Serial.println("port is  opens");
      if(message_count <message_count_limit){
            Serial.println("generating Json");
            JsonGenerator myJson((BATTERY_STATUS | BATTERY_STATE_OF_CHARGE | TEMPARATURE | LIGHT_LEVEL), size_mem);

           // myJson.collectSensorData(&IoTDevice);
            delay(500);
            Serial.println("SetupPostPacket");
//            Serial.println(sizeof(x)/sizeof(char *));
            myAzure.setPostContent(myJson.makeJsonDataWithAccel(String(message_count),"Telstrakit",x,y,z));
            // Build HTTPS request.
//            Serial.println(myAzure.getPostPacket());
            WebIoT.post(myAzure.getPostPacket());
            Serial.println("Packet arrived");
            delay(500);
            message_count++;
         }else{
              Serial.println("Session end, No Packet Post");
              conn.closeTCP();
            }
       
  }else{
  Serial.println("Waiting until cellular system has finished booting...");
  IoTDevice.waitUntilCellularSystemIsReady();
  Serial.println(" Opening TCP connection!");
  WebIoT.setHost(host,path,port);
  if(conn.openTCP(host,port)==CONNECTION4G_STATUS_OK){
    port_open=true;
    //Serial.println("(initial port is  " +String(port_open));
    Serial.println("open TCP Success!");
      }else {    Serial.println(" OpenTCP() failed.");
        }  
   }
}
void Printlog(String object){
  if(debugger){
  Serial.println(object);
  }
}

void serialFlush(){
  while(Serial.available() > 0) {
    Serial.read();
  }
}

void kav_mia(){
  OLED.fillRect(1,1,28,10, 0xFF, 0x00, 0x00);   // x y w h 
  OLED.fill_OLED(0x00,0x00,0x00);
  switch(state){
    
    case 0:
    //collect data
      OLED.drawString("State 0", 1, 1, 255, 255, 255,1);
      if (glob_count == (1*FREQ_RATE)){
        state = 1;
      }
    break;

    case 1:
      OLED.drawString("State 1", 1, 1, 255, 255, 255,1);
      if (accel_vm > threshold){
        //change state to State 2
        peak = accel_vm;
        state = 2;
        multi_peak_timer = (1*FREQ_RATE) - 1; // edited by Edy; this should be 1*FREQ_RATE not 2*FREQ_RATE
      }else{
        for(int i=1 ; i< glob_count; i++){
          //shift 1 sample left
          acc_vm[i-1] = acc_vm[i];
          x[i-1] = x[i];
          y[i-1] = y[i];
          z[i-1] = z[i];
          //light[i-1] = light[i];
        }
        glob_count = glob_count -1 ;
      }
    break;

    case 2:
      //OLED.fill_OLED(0x00,0x00,0x00);
      OLED.drawString("State 2", 1, 1, 255, 255, 255,1);
      temp_peak = 0;
      if (multi_peak_timer > 1){
        
        //search for the highest peak 
        if (accel_vm > peak){
          
          //find a higher peak
          
          peak = accel_vm;
          multi_peak_timer = (1*FREQ_RATE)-1;
          int tem_index = glob_count - ((1*FREQ_RATE)+1);
          //shift 
         
          for (int j = tem_index; j< glob_count; j++){
            acc_vm[j-tem_index] = acc_vm[j];
            x[j-tem_index] = x[j];
            y[j-tem_index] = y[j];
            z[j-tem_index] = z[j];
            //light[j-tem_index] = light[j];         
          }          
          glob_count = glob_count - tem_index;
        }else{
          multi_peak_timer--;
        }
      }else{
        // change to state 3
        state = 3;
        sg_timer = (1*FREQ_RATE); //edited by eddy , it was (2*FREQ_RATE)
      }
     
    break;
    
    case 3:
      
      //OLED.fill_OLED(0x00,0x00,0x00);
      OLED.drawString("State 3", 1, 1, 255, 255, 255,1);
      //OLED.drawString(String(sg_timer), 1, 31, 255, 255, 255,1);   
      if (sg_timer>1){
        //check whether there is peak or not
     
        if (accel_vm > threshold && accel_vm > temp_peak){
          Serial.println("found peak in state 3");
          OLED.drawString("found peak in state 3", 1, 11, 0, 255, 0,1);
          peak_time = glob_count;
          temp_peak = accel_vm;
        }else{
          OLED.drawString("No Peak", 1, 11, 255, 255, 255,1);
        }
        sg_timer --;
          OLED.drawString(String(sg_timer), 1, 20, 255, 255, 255,1);   
      }else{
        //do feature extraction/ send the data
        OLED.drawString("Possible Fall Detected!", 1, 21, 255, 255, 255,1);   
        
        Serial.println("sending data to HUB");
//        message_trig=10;
        send_message();
        
        if (temp_peak == 0){
          // go back to state 1 and shift to left only the buffer
          state = 1;
          int temp_ind_1 = 2*FREQ_RATE;// Edited by Edy, this should be (N-1)*FREQ_RATE, used to be 4*FREQ_RATE
          Serial.println("glob_count 1 is ");
          Serial.println(glob_count);
          for (int k = temp_ind_1; k< glob_count ; k++){
            acc_vm [k-temp_ind_1] = acc_vm[k]; 
             x[k-temp_ind_1] = x[k];
            y[k-temp_ind_1] = y[k];
            z[k-temp_ind_1] = z[k];
            //light[k-temp_ind_1] = light[k];
          }
         Serial.println("x is");
         for(int i = 0; i < 150 ; i ++){
           Serial.print(String(x[i])+" ");
            }
           Serial.println("y is");
           for(int i = 0; i < 150 ; i ++){
           Serial.print(String(y[i])+" ");
            }
            Serial.println("z is");
            for(int i = 0; i < 150 ; i ++){
           Serial.print(String(z[i])+" ");
            }
          glob_count = glob_count - temp_ind_1; //reset the global counter

        }else{
          // go to state 2
          state = 2;
          multi_peak_timer = (1*FREQ_RATE)-1;  // used to be multi_peak_timer = (2*FREQ_RATE)-1;
          int temp_ind_2 = peak_time - (FREQ_RATE+1);
          Serial.println("glob_count 2 is ");
          Serial.println(glob_count);
          for(int l = temp_ind_2 ; l < glob_count ; l++ ){
            acc_vm[l-temp_ind_2] = acc_vm[l];
             x[l-temp_ind_2] = x[l];
            y[l-temp_ind_2] = y[l];
            z[l-temp_ind_2] = z[l];
            //light[l-temp_ind_2] = light[l];
          }
          Serial.println("x is");
         for(int i = 0; i < 150 ; i ++){
           Serial.print(String(x[i])+" ");
            }
           Serial.println("y is");
           for(int i = 0; i < 150 ; i ++){
           Serial.print(String(y[i])+" ");
            }
            Serial.println("z is");
            for(int i = 0; i < 150 ; i ++){
           Serial.print(String(z[i])+" ");
            }
          glob_count = glob_count - temp_ind_2;
        }
      }
            
    break;

    default:
        OLED.fill_OLED(0x00,0x00,0x00);
        OLED.drawString("State Not Found", 1, 1, 255, 255, 255,1);
    break;
    
   }

   
}
