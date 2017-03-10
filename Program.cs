using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace IMUsimulering
{
    class Program

    {
        

        static void Main(string[] args)
        {
            Random rand = new Random(DateTime.Now.Millisecond);
            double magval = (double)(rand.Next(360));
            double roll = (double)(rand.Next(180))-90;
            double pitch = (double)(rand.Next(180))-90;
            double tiltheading = 0;
            #region //1.kvadrant
            if (roll > 0 & pitch > 0)
            {
                tiltheading = 0;

                if((Math.Max(roll, pitch)/Math.Min(roll, pitch)) < 2)
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
            #endregion

            #region //4.kvadrant
            else if (roll < 0 & pitch > 0)
            {
                tiltheading = 270;
            
                if ((Math.Max(Math.Abs(roll), Math.Abs(pitch)/Math.Min(Math.Abs(roll), Math.Abs(pitch))) <2))
                {
                    tiltheading += 45;
                }
                else if (Math.Abs(pitch) > Math.Abs(roll))
                {
                    tiltheading += 75;
                }
                else
                {
                    tiltheading += 15;
                }
            }
            #endregion
            #region //3. kvadrant

            else if (roll < 0 & pitch < 0)
            {
                tiltheading = 180;

                if ((Math.Max(Math.Abs(roll), Math.Abs(pitch)/Math.Min(Math.Abs(roll), Math.Abs(pitch))))<2)
                {
                    tiltheading += 45;
                }
                else if (Math.Abs(roll) > Math.Abs(pitch))
                {
                    tiltheading += 75;
                }
                else
                {
                    tiltheading += 15;
                }
            }
            #endregion
            #region //2.kvadrant
            else
            {
                tiltheading = 90;

                if ((Math.Max(Math.Abs(roll), Math.Abs(pitch))/(Math.Min(Math.Abs(roll), Math.Abs(pitch)))) < 2)
                {
                    tiltheading += 45;
                }
                else if (Math.Abs(roll) > Math.Abs(pitch))
                {
                    tiltheading += 15;
                }
                else
                {
                    tiltheading += 75;
                }
            }
            #endregion
            Console.WriteLine("tiltval:{0}",tiltheading);
            tiltheading = tiltheading - magval;
            if(tiltheading < 0)
            {
                tiltheading += 360;
            }
            Console.WriteLine("magval:{0}",magval);
            Console.WriteLine("pitch:{0}",pitch);
            Console.WriteLine("roll:{0}",roll);
            Console.WriteLine("tiltheading:{0}", tiltheading);
            Console.ReadKey();
        }
    }
}
