#include "jsonGenerator.h"


JsonGenerator::JsonGenerator(uint8_t parameters_, uint8_t samples)
{
  c = 1;          //data per sensor
  array_size=samples;
  
  parameters = parameters_;

//  setBuf();
  
}

JsonGenerator::~JsonGenerator()
{

}

String JsonGenerator::makeJsonData()
{
  content = "";
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& shieldParams = jsonBuffer.createObject();    // Create the root of the object tree.
  int i;


  // Add nested arrays.
  if (((parameters & BATTERY_STATUS) == BATTERY_STATUS) && !(BATTERY_STATUS & errors))
  {
    JsonArray& batteryStatusJson = shieldParams.createNestedArray("Battery Status");
    for (i = 0; i < c; ++i)
      {
        batteryStatusJson.add(batteryStatus[i]);
      }
  }
  if (((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE) && !(BATTERY_STATE_OF_CHARGE & errors))
  {
    JsonArray& batteryStateOfChargeJson = shieldParams.createNestedArray("Battery State of Charge");
    for (int i = 0; i < c; ++i)
    {
      batteryStateOfChargeJson.add(batteryStateOfCharge[i]);
    }
  }

  if (((parameters & TEMPARATURE) == TEMPARATURE) && !(TEMPARATURE & errors))
  {
    JsonArray& temparatureJson = shieldParams.createNestedArray("Temparature");
    for (i = 0; i < c; ++i)
    {
      temparatureJson.add(temparature[i]);
    }
  }

  if (((parameters & LIGHT_LEVEL) == LIGHT_LEVEL) && !(LIGHT_LEVEL & errors))
  {
    JsonArray& lightLevelJson = shieldParams.createNestedArray("Light Level");
    for (i = 0; i < c; ++i)
    {
      lightLevelJson.add(lightLevel[i]);
    }
  }

  
  
  shieldParams.printTo(content);    //create JSON

return content;
}


String JsonGenerator::makeJsonDataWithAccel(String message_id,String device_id,float* x, float* y, float* z) {

  jsonBuffer.clear();
  Serial.println("jsonbuffer has been removed");
  DynamicJsonBuffer  jsonBuffer;
  Serial.println("jsonBuffer has been made");
  JsonObject& shieldParams = jsonBuffer.createObject();    // Create the root of the object tree.
  
  
  content = "";
  
  int i;
  shieldParams["messageid"]=message_id;
//  shieldParams["deviceid"]=device_id;
  JsonArray& accelXJson = shieldParams.createNestedArray("accel_x");
  JsonArray& accelYJson = shieldParams.createNestedArray("accel_y");
  JsonArray& accelZJson = shieldParams.createNestedArray("accel_z");
//  JsonArray& accelvecJson = shieldParams.createNestedArray("vecmg");
  
  Serial.println("OK reset accel finish1");
  //int maxNumSensorReadings = sizeofArray;///
  
  for (i = 0; i < array_size; ++i) { 
    accelXJson.add((x[i]));
    accelYJson.add((y[i]));
    accelZJson.add((z[i]));
//	accelvecJson.add((vecmg[i]));
  }
  Serial.println("OK reset accel finish2");

  shieldParams.printTo(content); //store the JSON created in shieldParams to the String content
  Serial.println("OK reset accel finish3");
  Serial.println(String(content));
   
  return content; //pass this string into TelstraWeb.post(content) function
}

uint8_t JsonGenerator::collectSensorData(TelstraM1Device* IoTDevice)   // samplesPerSecond timer
{
  if (recollect) 
  {
    setBuf();
  }

  if ((parameters & BATTERY_STATUS) == BATTERY_STATUS)
  {
    if (IoTDevice->getBatteryStateOfCharge() != 0)
    {
    batteryStatus [count] = IoTDevice->getBatteryStatus();
    }
    else
    {
      errors = errors | BATTERY_STATUS;
    }
  }

    if ((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE)
  {
    if (IoTDevice->getBatteryStateOfCharge() != 0)
    { 
      batteryStateOfCharge [count] = IoTDevice->getBatteryStateOfCharge();
    }
    else
    {
      errors = errors | BATTERY_STATE_OF_CHARGE;
    }
  }

  if ((parameters & TEMPARATURE) == TEMPARATURE)
  {
    char charTemp[15];
    IoTDevice->getTemperature(charTemp);
    if (charTemp != NULL)
    {
      temparature [count] = atof(charTemp);
    }
    else
    {
     errors = errors | TEMPARATURE;
    }
  }

  if ((parameters & LIGHT_LEVEL) == LIGHT_LEVEL)
  {
    char charLightLevel[15];
    IoTDevice->getLightLevel(charLightLevel);
    if (charLightLevel != NULL)
    {
      lightLevel [count] = atoi(charLightLevel);
    }
    else
    {
      errors = errors | LIGHT_LEVEL;
    }
  }
  // tell user you are full
  if ((count + 1) == c)
  {
    content = makeJsonData();
    resetBuf();
    count = 0;
    recollect = true;
    return errors;
  }
  else
  {
    count ++;    //seperate counts!
    return errors;
  }

}

void JsonGenerator::setBuf()
{
	
   if ((parameters & BATTERY_STATUS) == BATTERY_STATUS)
  {
    batteryStatus = new uint8_t[c];
  }

    if ((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE)
  {
    batteryStateOfCharge = new uint8_t[c];
  }

  if ((parameters & TEMPARATURE) == TEMPARATURE)
  {
    temparature = new float[c];
  }

  if ((parameters & LIGHT_LEVEL) == LIGHT_LEVEL)
  {
    lightLevel = new unsigned int[c];
  }
  

  recollect = false;
}

void JsonGenerator::resetBuf()
{
  delete[] batteryStatus, batteryStatus = nullptr;
  delete[] batteryStateOfCharge, batteryStateOfCharge = nullptr;
  delete[] temparature, temparature = nullptr;
  delete[] lightLevel, lightLevel = nullptr;

  
}

String JsonGenerator::printJson()
{
  //makeJsonData();
  return content;
}



//
//
//
//#include "jsonGenerator.h"
//
//
//JsonGenerator::JsonGenerator(uint8_t parameters_, uint8_t samples)
//{
//  c = 1;          //data per sensor
//  array_size=samples;
//  
//  parameters = parameters_;
//  setBuf();
//  
//}
//
//JsonGenerator::~JsonGenerator()
//{
//
//}
//
//String JsonGenerator::makeJsonData()
//{
//  content = "";
//  DynamicJsonBuffer  jsonBuffer;
//  JsonObject& shieldParams = jsonBuffer.createObject();    // Create the root of the object tree.
//  int i;
//
//
//  // Add nested arrays.
//  if (((parameters & BATTERY_STATUS) == BATTERY_STATUS) && !(BATTERY_STATUS & errors))
//  {
//    JsonArray& batteryStatusJson = shieldParams.createNestedArray("Battery Status");
//    for (i = 0; i < c; ++i)
//      {
//        batteryStatusJson.add(batteryStatus[i]);
//      }
//  }
//  if (((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE) && !(BATTERY_STATE_OF_CHARGE & errors))
//  {
//    JsonArray& batteryStateOfChargeJson = shieldParams.createNestedArray("Battery State of Charge");
//    for (int i = 0; i < c; ++i)
//    {
//      batteryStateOfChargeJson.add(batteryStateOfCharge[i]);
//    }
//  }
//
//  if (((parameters & TEMPARATURE) == TEMPARATURE) && !(TEMPARATURE & errors))
//  {
//    JsonArray& temparatureJson = shieldParams.createNestedArray("Temparature");
//    for (i = 0; i < c; ++i)
//    {
//      temparatureJson.add(temparature[i]);
//    }
//  }
//
//  if (((parameters & LIGHT_LEVEL) == LIGHT_LEVEL) && !(LIGHT_LEVEL & errors))
//  {
//    JsonArray& lightLevelJson = shieldParams.createNestedArray("Light Level");
//    for (i = 0; i < c; ++i)
//    {
//      lightLevelJson.add(lightLevel[i]);
//    }
//  }
//
//  
//  
//  shieldParams.printTo(content);    //create JSON
//
//return content;
//}
//
//
//String JsonGenerator::makeJsonDataWithAccel(String message_id,String device_id,float* x, float* y, float* z) {
//
//
//  
//  content = "";
//  DynamicJsonBuffer  jsonBuffer;
//  Serial.println("oh hohoh");
//  JsonObject& shieldParams = jsonBuffer.createObject();    // Create the root of the object tree.
//  
//  int i;
//  shieldParams["messageid"]=message_id;
//  shieldParams["deviceid"]=device_id;
//  JsonArray& accelXJson = shieldParams.createNestedArray("accel_x");
//  JsonArray& accelYJson = shieldParams.createNestedArray("accel_y");
//  JsonArray& accelZJson = shieldParams.createNestedArray("accel_z");
//  Serial.println("OK reset accel finish1");
//  //int maxNumSensorReadings = sizeofArray;///
//  
//  for (i = 0; i < array_size; ++i) { 
//    accelXJson.add((x[i]));
//    accelYJson.add((y[i]));
//    accelZJson.add((z[i]));
//  }
//  Serial.println("OK reset accel finish2");
//
//  shieldParams.printTo(content); //store the JSON created in shieldParams to the String content
//  Serial.println("OK reset accel finish3");
// 
//  
//  Serial.println("OK reset accel finis4");
//  
//  return content; //pass this string into TelstraWeb.post(content) function
//}
//
//uint8_t JsonGenerator::collectSensorData(TelstraM1Device* IoTDevice)   // samplesPerSecond timer
//{
//  if (recollect) 
//  {
//    setBuf();
//  }
//
//  if ((parameters & BATTERY_STATUS) == BATTERY_STATUS)
//  {
//    if (IoTDevice->getBatteryStateOfCharge() != 0)
//    {
//    batteryStatus [count] = IoTDevice->getBatteryStatus();
//    }
//    else
//    {
//      errors = errors | BATTERY_STATUS;
//    }
//  }
//
//    if ((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE)
//  {
//    if (IoTDevice->getBatteryStateOfCharge() != 0)
//    { 
//      batteryStateOfCharge [count] = IoTDevice->getBatteryStateOfCharge();
//    }
//    else
//    {
//      errors = errors | BATTERY_STATE_OF_CHARGE;
//    }
//  }
//
//  if ((parameters & TEMPARATURE) == TEMPARATURE)
//  {
//    char charTemp[15];
//    IoTDevice->getTemperature(charTemp);
//    if (charTemp != NULL)
//    {
//      temparature [count] = atof(charTemp);
//    }
//    else
//    {
//     errors = errors | TEMPARATURE;
//    }
//  }
//
//  if ((parameters & LIGHT_LEVEL) == LIGHT_LEVEL)
//  {
//    char charLightLevel[15];
//    IoTDevice->getLightLevel(charLightLevel);
//    if (charLightLevel != NULL)
//    {
//      lightLevel [count] = atoi(charLightLevel);
//    }
//    else
//    {
//      errors = errors | LIGHT_LEVEL;
//    }
//  }
//  // tell user you are full
//  if ((count + 1) == c)
//  {
//    content = makeJsonData();
//    resetBuf();
//    count = 0;
//    recollect = true;
//    return errors;
//  }
//  else
//  {
//    count ++;    //seperate counts!
//    return errors;
//  }
//
//}
//
//void JsonGenerator::setBuf()
//{
//  
//   if ((parameters & BATTERY_STATUS) == BATTERY_STATUS)
//  {
//    batteryStatus = new uint8_t[c];
//  }
//
//    if ((parameters & BATTERY_STATE_OF_CHARGE) == BATTERY_STATE_OF_CHARGE)
//  {
//    batteryStateOfCharge = new uint8_t[c];
//  }
//
//  if ((parameters & TEMPARATURE) == TEMPARATURE)
//  {
//    temparature = new float[c];
//  }
//
//  if ((parameters & LIGHT_LEVEL) == LIGHT_LEVEL)
//  {
//    lightLevel = new unsigned int[c];
//  }
//  
//
//  recollect = false;
//}
//
//void JsonGenerator::resetBuf()
//{
//  delete[] batteryStatus, batteryStatus = nullptr;
//  delete[] batteryStateOfCharge, batteryStateOfCharge = nullptr;
//  delete[] temparature, temparature = nullptr;
//  delete[] lightLevel, lightLevel = nullptr;
//
//  
//}
//
//String JsonGenerator::printJson()
//{
//  //makeJsonData();
//  return content;
//}
