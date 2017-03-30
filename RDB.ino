//void setup() {
//  // put your setup code here, to run once:
//
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
//
//}


// KODE
void setup() {
  // put your setup code here, to run once:
  //I2C opsett
  //RS485 opsett
  //Qduino opsett
  //

}

void loop() {
  // put your main code here, to run repeatedly:
  //behandle rådata til roll pitch & yaw
  //bruke trigonometri for å kalkulere helningsvinkel
  // muligens switchcase for å finne retning på helningsvinkelen
  //sende retning og helningsvinkel ut fra top-end Qduino

  void tiltheading(float x_Complementaryangle, float y_Complementaryangle, float heading  ){
    float roll = x_Complementaryangle;
    float pitch = y_Complementaryangle;

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

      if ((max(abs(roll)), abs(pitch)) / min(abs(roll), abs(pitch)) < 2)
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




    
    }

}
