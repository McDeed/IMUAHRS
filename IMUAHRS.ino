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

//constants
float gyrogain = 0.070; //deg/s/LSB TODO:what is LSB?
float looptime = 0.02; //Looptime
unsigned long startTime;
float RAD = 57.29578;
int count = 0;

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
  


void setup() {
Serial.begin(9600);
Wire.begin(); // join i2c bus (address optional for master)
compass.init();
compass.enableDefault();
compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
       
  Serial.begin(OUTPUT__BAUD_RATE);  // start serial for output

  // enable gyro, TODO: consider adding filter
  writereg(GYRO_ADDRESS,0b0100000,0b00001111); // normal power mode enable xyz axis
  writereg(GYRO_ADDRESS,0b0100011,0b00110000);  //2000 dps

  //enable accelerometer
  writereg(ACC_ADDRESS, 0b0100000,0b01010111); //  enable xyz axis 100 Hz data rate, TODO: consider 2Xrate
  writereg(ACC_ADDRESS, 0b0100011,0b00110000); // +/- 16g

  writereg(MAG_ADDRESS,0x00, 0b0011000);   // rate 75 Hz
  writereg(MAG_ADDRESS,0x01, 0b11100000);   // +/-8.1gauss
  writereg(MAG_ADDRESS,0x02, 0b00000000);   // Continuous-conversion mode
}

void loop()
{

startTime = millis();// take starttime of the loop to stabilize looptime

compass.read(); //read magnetometer values



  //read the GYRO values
  readreg(GYRO_ADDRESS, 0x80 | 0b0101000, 6, readbyte);
  rawgyro[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
  rawgyro[1] = (int) (readbyte[2] | readbyte[3] << 8);
  rawgyro[2] = (int) (readbyte[4] | readbyte[5] << 8);
  //read the accelerometer values
  readreg(ACC_ADDRESS, 0x80 | 0b0101000, 6, readbyte);
  rawacc[0] = (int) (readbyte[0] | readbyte[1] << 8);//combines 2 bytes in to a 16 bit int
  rawacc[1] = (int) (readbyte[2] | readbyte[3] << 8);
  rawacc[2] = (int) (readbyte[4] | readbyte[5] << 8);
  //read magnetometer values
  readreg(MAG_ADDRESS, 0x80 | 0b0000011, 6, readbyte);
  rawmag[0] = (int)(readbyte[0] | (readbyte[1] << 8));   
  rawmag[2] = (int)(readbyte[2] | (readbyte[3] << 8));
  rawmag[1] = (int)(readbyte[4] | (readbyte[5] << 8));

  //calculate angular rate
  x_rate = (float)rawgyro[0]* gyrogain;
  y_rate = (float)rawgyro[1]* gyrogain;
  z_rate = (float)rawgyro[2]* gyrogain;

  //calculate the angle measured by accelerometer
  x_angleacc = (float) (atan2(rawacc[1],rawacc[2])+PI)*RAD-180.0; //TODO: add pre RAD variable to use in magcalc
  y_angleacc = (float) (atan2(rawacc[2],rawacc[0])+PI)*RAD;

  if (y_angleacc > 90)
          y_angleacc -= (float)270;
  else
    y_angleacc += (float)90;

  //calculate complementary angle
  x_Complementaryangle = 0.97*(x_Complementaryangle+x_rate*looptime)+(0.03* x_angleacc);
  y_Complementaryangle = 0.97*(y_Complementaryangle+y_rate*looptime)+(0.03* y_angleacc);

//
  if (count > 10)
      {
        Serial.print("\t x_Complementaryangle:");
        Serial.print(x_Complementaryangle);
        Serial.print("\t y_Complementaryangle:");
        Serial.print(y_Complementaryangle);
        
       }
  y_Complementaryangle=y_Complementaryangle*PI/180;//convert to rad for further calculations
  x_Complementaryangle=x_Complementaryangle*PI/180;
  float tilt = acos(cos(x_Complementaryangle)*cos(y_Complementaryangle)); //pitch and roll are swapped
  tilt = tilt*180/PI;


  float Heading2 = compass.heading((LSM303::vector<int>){1, 0, 0});
  if (Heading2 < 0)
  Heading2 += 360;

  
  
  
  float attitude = 180 * atan2(rawmag[1],rawmag[0])/PI; // try other rawmagvalues or inverting y-axis
  if  (attitude < 0)
    attitude += 360;
      x_Complementaryangle = 180*x_Complementaryangle/PI;//change values to deg for later calculations
  y_Complementaryangle = 180*y_Complementaryangle/PI;
  if (count > 10)
     {
        Serial.print("\t tiltangle:");Serial.print(tilt);
        Serial.print("\t Heading2: "); Serial.print(Heading2);  
        Serial.print("\t attitude:");Serial.print(attitude);
        tiltheading(x_Complementaryangle, y_Complementaryangle, Heading2);
        
        count = 0;
      }
  
  
 


// looptime is 20 ms
  while(millis() - startTime < (looptime*1000))
        {
            delay(1);
        }
 // Serial.println( millis()- startTime); //add this to make sure the loop time is consistent
  


  

//delay(250);
  count=count+1;
}

void tiltheading(float x_Complementaryangle, float y_Complementaryangle, float Heading1  ){
    float roll = x_Complementaryangle;
    float pitch = y_Complementaryangle;
    float tiltheading;
    // 1.kvadrant
    if (roll > 0 & pitch > 0)
    {
      tiltheading = 0;

      if((max(roll, pitch)/min(roll, pitch)) < 2)
      {
        tiltheading += 45; 
      }

      else if (pitch > roll)
      {
        tiltheading += 15;
      }

      else
      {
        tiltheading += 75;
      }
     }
  
    // 4.kvadrant
    else if (roll < 0 & pitch > 0)
    {
      tiltheading = 270;

      if ((max(abs(roll), abs(pitch)) / min(abs(roll), abs(pitch)) < 2))
      {
        tiltheading += 45;
      }

      else if (abs(pitch) > abs(roll))
      {
        tiltheading += 75;
      }

      else
      {
        tiltheading += 15; 
      }
    }

     // 3.kvadrant
     else if (roll < 0 & pitch < 0)
     {
        tiltheading = 180;

        if ((max(abs(roll), abs(pitch))) / (min(abs(roll), abs(pitch))) < 2)
        {
          tiltheading += 45;
        }

        else if (abs(roll) > abs(pitch))
        {
          tiltheading += 75; 
        }

        else
        {
          tiltheading += 15;
        }
        
      }

      // 2.kvadrant
      else
      {
          tiltheading = 90;

        if ((max(abs(roll), abs(pitch)) / (min(abs(roll), abs(pitch)))) < 2)
        {
          tiltheading += 45;
        }

        else if (abs(roll) > abs(pitch))
        {
          tiltheading += 15;
        }

        else
        {
          tiltheading += 75;
        }

        
      }
      tiltheading = tiltheading - Heading1;
      if( tiltheading <0){tiltheading += 360;}
    Serial.print("\t tiltheading:"); Serial.println(tiltheading);



    
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



//const float alpha = 0.15;
//float fXa = 0;
//float fYa = 0;
//float fZa = 0;
//float fXm = 0;
//float fYm = 0;
//float fZm = 0;
//float pitch, pitch_print, roll, roll_print, Heading, Xa_off, Ya_off, Za_off, Xa_cal, Ya_cal, Za_cal, Xm_off, Ym_off, Zm_off, Xm_cal, Ym_cal, Zm_cal, fXm_comp, fYm_comp;

/// Accelerometer calibration
//Xa_off = compass.a.x/16.0 + 6.008747;
//Ya_off = compass.a.y/16.0 - 18.648762;
//Za_off = compass.a.z/16.0 + 10.808316;
//Xa_cal =  0.980977*Xa_off + 0.001993*Ya_off - 0.004377*Za_off;
//Ya_cal =  0.001993*Xa_off + 0.998259*Ya_off - 0.000417*Za_off;
//Za_cal = -0.004377*Xa_off - 0.000417*Ya_off + 0.942771*Za_off;
//
//// Magnetometer calibration
//Xm_off = compass.m.x*(100000.0/1100.0) + 8109.248723; //X-axis combined bias (Non calibrated data - bias)
//Ym_off = compass.m.y*(100000.0/1100.0) + 10318.856388; //Y-axis combined bias (Default: substracting bias)
//Zm_off = compass.m.z*(100000.0/980.0 ) - 8306.688377; //Z-axis combined bias
//
//Xm_cal =  0.992015*Xm_off + 0.015030*Ym_off + 0.044168*Zm_off; //X-axis correction for combined scale factors (Default: positive factors)
//Ym_cal =  0.015030*Xm_off + 1.030652*Ym_off + 0.011862*Zm_off; //Y-axis correction for combined scale factors
//Zm_cal =  0.044168*Xm_off + 0.011862*Ym_off + 1.041935*Zm_off; //Z-axis correction for combined scale factors
//
//
//// Low-Pass filter accelerometer
//fXa = Xa_cal * alpha + (fXa * (1.0 - alpha));
//fYa = Ya_cal * alpha + (fYa * (1.0 - alpha));
//fZa = Za_cal * alpha + (fZa * (1.0 - alpha));
//
//// Low-Pass filter magnetometer
//fXm = Xm_cal * alpha + (fXm * (1.0 - alpha));
//fYm = Ym_cal * alpha + (fYm * (1.0 - alpha));
//fZm = Zm_cal * alpha + (fZm * (1.0 - alpha));
//
//// Pitch and roll consider removing this
//roll  = atan2(fYa, sqrt(fXa*fXa + fZa*fZa)); //TODO: use fusion pitch and roll
//pitch = atan2(fXa, sqrt(fYa*fYa + fZa*fZa));
//roll_print = roll*180.0/M_PI;
//pitch_print = pitch*180.0/M_PI;



  
  // not needed
//  x_anglegyro += x_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
//  y_anglegyro += y_rate*looptime;//this value drifts over time, consider resetting it after hitting 360 or just remove from file
//  z_anglegyro += z_rate*looptime;//euler integration




  // Tilt compensated magnetic sensor measurements
  // consider using fusion tiltvalues
//  fXm_comp = fXm*cos(x_Complementaryangle)+fZm*sin(x_Complementaryangle);
//  fYm_comp = fXm*sin(y_Complementaryangle)*sin(x_Complementaryangle)+fYm*cos(y_Complementaryangle)-fZm*sin(y_Complementaryangle)*cos(x_Complementaryangle);
  
  // Arctangent of y/x
//  Heading = (atan2(fYm_comp,fXm_comp)*180.0)/M_PI;












