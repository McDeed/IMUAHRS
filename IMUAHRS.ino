#include <LSM303.h>

#include <Qduino.h>

#include "Qduino.h"
#include "Wire.h"

#include <Wire.h>

#define GYRO_ADDRESS  (107) //gyroscope ADDRESS
#define ACC_ADDRESS (0x32 >> 1) //accelerometer address
#define MAG_ADDRESS (0x3C >> 1) //Magnetometer address
#define OUTPUT__BAUD_RATE 57600 //baud rate for output

#include <Wire.h>
#include <LSM303.h>


#include <Wire.h>
#include <LSM303.h>
LSM303 compass;


const float alpha = 0.15;
float fXa = 0;
float fYa = 0;
float fZa = 0;
float fXm = 0;
float fYm = 0;
float fZm = 0;

void setup() {
Serial.begin(9600);
Wire.begin();
compass.init();
compass.enableDefault();
}

void loop()
{
compass.read();
float pitch, pitch_print, roll, roll_print, Heading, Xa_off, Ya_off, Za_off, Xa_cal, Ya_cal, Za_cal, Xm_off, Ym_off, Zm_off, Xm_cal, Ym_cal, Zm_cal, fXm_comp, fYm_comp;

// Accelerometer calibration
Xa_off = compass.a.x/16.0 + 6.008747;
Ya_off = compass.a.y/16.0 - 18.648762;
Za_off = compass.a.z/16.0 + 10.808316;
Xa_cal =  0.980977*Xa_off + 0.001993*Ya_off - 0.004377*Za_off;
Ya_cal =  0.001993*Xa_off + 0.998259*Ya_off - 0.000417*Za_off;
Za_cal = -0.004377*Xa_off - 0.000417*Ya_off + 0.942771*Za_off;

// Magnetometer calibration
Xm_off = compass.m.x*(100000.0/1100.0) - 8397.862881;
Ym_off = compass.m.y*(100000.0/1100.0) - 3307.507492;
Zm_off = compass.m.z*(100000.0/980.0 ) + 2718.831179;
Xm_cal =  0.949393*Xm_off + 0.006185*Ym_off + 0.015063*Zm_off;
Ym_cal =  0.006185*Xm_off + 0.950124*Ym_off + 0.003084*Zm_off;
Zm_cal =  0.015063*Xm_off + 0.003084*Ym_off + 0.880435*Zm_off;

// Low-Pass filter accelerometer
fXa = Xa_cal * alpha + (fXa * (1.0 - alpha));
fYa = Ya_cal * alpha + (fYa * (1.0 - alpha));
fZa = Za_cal * alpha + (fZa * (1.0 - alpha));

// Low-Pass filter magnetometer
fXm = Xm_cal * alpha + (fXm * (1.0 - alpha));
fYm = Ym_cal * alpha + (fYm * (1.0 - alpha));
fZm = Zm_cal * alpha + (fZm * (1.0 - alpha));

// Pitch and roll
roll  = atan2(fYa, sqrt(fXa*fXa + fZa*fZa));
pitch = atan2(fXa, sqrt(fYa*fYa + fZa*fZa));
roll_print = roll*180.0/M_PI;
pitch_print = pitch*180.0/M_PI;

// Tilt compensated magnetic sensor measurements
fXm_comp = fXm*cos(pitch)+fZm*sin(pitch);
fYm_comp = fXm*sin(roll)*sin(pitch)+fYm*cos(roll)-fZm*sin(roll)*cos(pitch);

// Arctangent of y/x
Heading = (atan2(fYm_comp,fXm_comp)*180.0)/M_PI;
if (Heading < 0)
Heading += 360;

Serial.print("Pitch (X): "); Serial.print(pitch_print); Serial.print("  ");
Serial.print("Roll (Y): "); Serial.print(roll_print); Serial.print("  ");
Serial.print("Heading: "); Serial.println(Heading);
delay(250);
}
////constants
//float gyrogain = 0.070; //deg/s/LSB TODO:what is LSB?
//float looptime = 0.02; //Looptime
//unsigned long startTime;
//float RAD = 57.29578;
//
//
////variables
//int rawgyro[3];
//int x_rate;
//int y_rate;
//int z_rate;
//float x_anglegyro;
//float y_anglegyro;
//float z_anglegyro;
//
//int rawacc[3];
//float x_angleacc;
//float y_angleacc;
//
//int rawmag[3];
//float x_compmag;
//float y_compmag;
//
//float x_Complementaryangle;
//float y_Complementaryangle;
//
//byte readbyte[6];
//  
//
//void setup()
//{
//  Wire.begin();        // join i2c bus (address optional for master)
//  Serial.begin(OUTPUT__BAUD_RATE);  // start serial for output
//
//  // enable gyro, TODO: consider adding filter
//  writereg(GYRO_ADDRESS,0b0100000,0b00001111); // normal power mode enable xyz axis
//  writereg(GYRO_ADDRESS,0b0100011,0b00110000);  //2000 dps
//
//  //enable accelerometer
//  writereg(ACC_ADDRESS, 0b0100000,0b01010111); //  enable xyz axis 100 Hz data rate, TODO: consider 2Xrate
//  writereg(ACC_ADDRESS, 0b0100011,0b00110000); // +/- 16g
//
//  writereg(MAG_ADDRESS,0x00, 0b0011000);   // rate 75 Hz
//  writereg(MAG_ADDRESS,0x01, 0b11100000);   // +/-8.1gauss
//  writereg(MAG_ADDRESS,0x02, 0b00000000);   // Continuous-conversion mode
//  
//}
//
//
//
//
//void loop() {
//  startTime = millis();// TODOO figure this out
//
//  readreg(GYRO_ADDRESS, 0x80 | 0b0101000, 6, readbyte);
//  rawgyro[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
//  rawgyro[1] = (int) (readbyte[2] | readbyte[3] << 8);
//  rawgyro[2] = (int) (readbyte[4] | readbyte[5] << 8);
//
//  readreg(ACC_ADDRESS, 0x80 | 0b0101000, 6, readbyte);
//  rawacc[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
//  rawacc[1] = (int) (readbyte[2] | readbyte[3] << 8);
//  rawacc[2] = (int) (readbyte[4] | readbyte[5] << 8);
//
//  readreg(MAG_ADDRESS, 0x80 | 0b0000011, 6, readbyte);
//  rawmag[0] = (int)(readbyte[0] | (readbyte[1] << 8));   
//  rawmag[2] = (int)(readbyte[2] | (readbyte[3] << 8));
//  rawmag[1] = (int)(readbyte[4] | (readbyte[5] << 8));
//  
//  x_rate = (float)rawgyro[0]* gyrogain;
//  y_rate = (float)rawgyro[1]* gyrogain;
//  z_rate = (float)rawgyro[2]* gyrogain;
//  
//  x_anglegyro += x_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
//  y_anglegyro += y_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
//  z_anglegyro += z_rate*looptime;//euler integration
//
//  x_angleacc = (float) (atan2(rawacc[1],rawacc[2])+PI)*RAD-180.0; //TODO: why 180 and 270
//  y_angleacc = (float) (atan2(rawacc[2],rawacc[0])+PI)*RAD;
//
//  if (y_angleacc > 90)
//          y_angleacc -= (float)270;
//  else
//    y_angleacc += (float)90;
//  
//  x_Complementaryangle = 0.97*(x_Complementaryangle+x_rate*looptime)+(0.03* x_angleacc);
//  y_Complementaryangle = 0.97*(y_Complementaryangle+y_rate*looptime)+(0.03* y_angleacc);
//
////
// Serial.print("\t x_Complementaryangle:");
//    Serial.print(x_Complementaryangle);
//    Serial.print("\t y_Complementaryangle:");
//    Serial.print(y_Complementaryangle);
//
//   
////  x_compmag = rawmag[0]*cos(y_Complementaryangle)+rawmag[2]*sin(y_Complementaryangle);
////  y_compmag = rawmag[0]*sin(y_Complementaryangle)*sin(x_Complementaryangle)+rawmag[1]*cos(x_Complementaryangle)-rawmag[2]*sin(x_Complementaryangle)*cos(y_Complementaryangle);
//    float attitude = 180 * atan2(rawmag[1],rawmag[0])/PI; // try other rawmagvalues or inverting y-axis
//    if  (attitude < 0)
//      attitude += 360;
//      
//   x_Complementaryangle=x_Complementaryangle*(-1);
//
//   float magxn = rawmag[0]*cos(y_Complementaryangle)+ rawmag[2]*sin(y_Complementaryangle);
//   float magyn = rawmag[0]*sin(x_Complementaryangle)*sin(y_Complementaryangle)+rawmag[1]*cos(x_Complementaryangle)-rawmag[2]*sin(x_Complementaryangle)*cos(y_Complementaryangle);
//   float magzn = -rawmag[0]*cos(x_Complementaryangle)*sin(y_Complementaryangle)+rawmag[1]*sin(x_Complementaryangle)+rawmag[2]*cos(x_Complementaryangle)*cos(y_Complementaryangle);
//    
//    float heading = 180 * atan2(magyn,magxn)/PI; // try other rawmagvalues or inverting y-axis
//    if(magxn>0 and magyn >=0){
//      }
//    else if(magxn<0){
//      heading+=180;
//    }
//    else if(magxn>0 and magyn <=0){
//      heading +=360;
//      }
//    else if(magxn=0 and magyn <=0){
//      heading +=90;
//      }
//    if  (heading < 0)
//      heading += 360;
//      
//    Serial.print("\t attitude:");
//    Serial.print(attitude);
//    Serial.print("\t heading:");
//    Serial.print(heading);
//    Serial.print("\t magx:");
//    Serial.print(rawmag[0]);
//    Serial.print("\t magy:");
//    Serial.print(rawmag[1]);
//    Serial.print("\t magz:");
//    Serial.println(rawmag[2]);
//  
//    
////    Serial.print("\t x_anglegyro :");
////    Serial.print(x_anglegyro); 
////    Serial.print("\t y_anglegyro :");
////    Serial.print(y_anglegyro);
////    Serial.print("\t z_anglegyro :"); 
////    Serial.print(z_anglegyro); 
////
////    Serial.print("\t AccXangle:");
////    Serial.print(x_angleacc);
////    Serial.print("\t AccYangle:");
////    Serial.print(y_angleacc);
//
// 
//
//
//// looptime is 20 ms
//  while(millis() - startTime < (looptime*1000))
//        {
//            delay(1);
//        }
// // Serial.println( millis()- startTime);
//  
//
//
//  
//}
//
//
//    void writereg(int deviceaddress, int registeraddress, int registerval){
//    Wire.beginTransmission(deviceaddress);
//    Wire.write(registeraddress);
//    Wire.write(registerval);
//    Wire.endTransmission();
//
//    }
//
//  void readreg(int deviceaddress, int registeraddress, int numbytes, byte readbyte[]){
//    Wire.beginTransmission(deviceaddress);
//    Wire.write(registeraddress);
//    Wire.endTransmission();
//
//    Wire.beginTransmission(deviceaddress);
//    Wire.requestFrom(deviceaddress, numbytes); //request n bytes from register
//
//    int i = 0;
//    while(Wire.available())    //device may send less than requested (abnormal)
//    { 
//      readbyte[i] = Wire.read(); // read a byte
//      i++;
//    }
//    Wire.endTransmission(); 
//  }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
























