#include <Qduino.h>

#include "Qduino.h"
#include "Wire.h"

#include <Wire.h>

#define GYRO_ADDRESS  (107) //gyroscope ADDRESS
#define ACC_ADDRESS (0x32 >> 1) //accelerometer address
#define MAG_ADDRESS (0x3C >> 1) //Magnetometer address
#define OUTPUT__BAUD_RATE 57600 //baud rate for output

//constants
float gyrogain = 0.070; //deg/s/LSB TODO:what is LSB?
float looptime = 0.02; //Looptime
unsigned long startTime;
float RAD = 57.29578;


//variables
int rawgyro[3];
int x_rate;
int y_rate;
int z_rate;
float x_anglegyro;
float y_anglegyro;
float z_anglegyro;

int rawacc[3];
float x_angleacc;
float y_angleacc;

int rawmag[3];
float x_compmag;
float y_compmag;

float x_Complementaryangle;
float y_Complementaryangle;

byte readbyte[6];
  

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(OUTPUT__BAUD_RATE);  // start serial for output

  // enable gyro, TODO: consider adding filter
  writereg(GYRO_ADDRESS,0b0100000,0b00001111); // normal power mode enable xyz axis
  writereg(GYRO_ADDRESS,0b0100011,0b00110000);  //2000 dps

  //enable accelerometer
  writereg(ACC_ADDRESS, 0b0100000,0b01010111); //  enable xyz axis 100 Hz data rate, TODO: consider 2Xrate
  writereg(ACC_ADDRESS, 0b0100011,0b00110000); // +/- 16g

  writereg(MAG_ADDRESS,0x00, 0b0001100);   // rate 75 Hz
  writereg(MAG_ADDRESS,0x01, 0b11100000);   // +/-8.1gauss
  writereg(MAG_ADDRESS,0x02, 0b00000000);   // Continuous-conversion mode
  
}




void loop() {
  startTime = millis();// TODOO figure this out

  readreg(GYRO_ADDRESS, 0x80 | 0b0101000, 6, readbyte);
  rawgyro[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
  rawgyro[1] = (int) (readbyte[2] | readbyte[3] << 8);
  rawgyro[2] = (int) (readbyte[4] | readbyte[5] << 8);

  readreg(ACC_ADDRESS, 0x80 | 0b0101000, 6, readbyte);//prøv med _H register
  rawacc[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
  rawacc[1] = (int) (readbyte[2] | readbyte[3] << 8);
  rawacc[2] = (int) (readbyte[4] | readbyte[5] << 8);

  readreg(MAG_ADDRESS, 0x80 | 0b0000011, 6, readbyte);
  rawmag[0] = (int)(readbyte[0] | (readbyte[1] << 8));   
  rawmag[1] = (int)(readbyte[2] | (readbyte[3] << 8));
  rawmag[2] = (int)(readbyte[4] | (readbyte[5] << 8));
  
  x_rate = (float)rawgyro[0]* gyrogain;//euler integration
  y_rate = (float)rawgyro[1]* gyrogain;
  z_rate = (float)rawgyro[2]* gyrogain;
  
  x_anglegyro += x_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
  y_anglegyro += y_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
  z_anglegyro += z_rate*looptime;

  x_angleacc = (float) (atan2(rawacc[1],rawacc[2])+PI)*RAD-180.0; //TODO: why 180 and 270
  y_angleacc = (float) (atan2(rawacc[2],rawacc[0])+PI)*RAD;

  if (y_angleacc > 90)
          y_angleacc -= (float)270;
  else
    y_angleacc += (float)90;
  
  x_Complementaryangle = 0.97*(x_Complementaryangle+x_rate*looptime)+(0.03* x_angleacc);
  y_Complementaryangle = 0.97*(y_Complementaryangle+y_rate*looptime)+(0.03* y_angleacc);


//  x_compmag = rawmag[0]*cos(y_Complementaryangle)+rawmag[2]*sin(x_Complementaryangle);
//  y_compmag = rawmag[0]*sin(x_Complementaryangle)*sin(y_Complementaryangle)+rawmag[1]*cos(x_Complementaryangle)-rawmag[2]*sin(x_Complementaryangle)*cos(y_Complementaryangle);
  float attitude = 180 * atan2(rawmag[1],rawmag[0])/PI;
  if  (attitude < 0)
      attitude += 360;



    Serial.print("\t x_Complementaryangle:");
    Serial.print(x_Complementaryangle);
    Serial.print("\t y_Complementaryangle:");
    Serial.print(y_Complementaryangle);

    Serial.print("\t attitude:");
    Serial.println(attitude);
  
    
//    Serial.print("\t x_anglegyro :");
//    Serial.print(x_anglegyro); 
//    Serial.print("\t y_anglegyro :");
//    Serial.print(y_anglegyro);
//    Serial.print("\t z_anglegyro :"); 
//    Serial.print(z_anglegyro); 
//
//    Serial.print("\t AccXangle:");
//    Serial.print(x_angleacc);
//    Serial.print("\t AccYangle:");
//    Serial.print(y_angleacc);

 


// looptime is 20 ms
  while(millis() - startTime < (looptime*1000))
        {
            delay(1);
        }
 // Serial.println( millis()- startTime);
  


  
}


    void writereg(int deviceaddress, int registeraddress, int registerval){
    Wire.beginTransmission(deviceaddress);
    Wire.write(registeraddress);
    Wire.write(registerval);
    Wire.endTransmission();

    }

  void readreg(int deviceaddress, int registeraddress, int numbytes, byte readbyte[]){
    Wire.beginTransmission(deviceaddress);
    Wire.write(registeraddress);
    Wire.endTransmission();

    Wire.beginTransmission(deviceaddress);
    Wire.requestFrom(deviceaddress, numbytes); //request n bytes from register

    int i = 0;
    while(Wire.available())    //device may send less than requested (abnormal)
    { 
      readbyte[i] = Wire.read(); // read a byte
      i++;
    }
    Wire.endTransmission(); 
  }
















































