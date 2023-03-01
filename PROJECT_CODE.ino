#include <ECE3.h>

    uint16_t sensorValues[8];
    uint16_t offset[8] = {505, 412, 551, 573, 573, 505, 575, 458};
    float scale[8] = {1.995, 2.088, 1.949, 1.927, 1.927, 1.995, 1.925, 2.042};
    int16_t sensorValues_2[8];
    int16_t weights[8] = {-8,-4,-2,-1,1,2,4,8};
    float k_p = 0.014; //0.01 for base_speed 20
    float k_d = 0.30; //
   
    int16_t curr_err = 0;
    int16_t prev_err = 0;
    int left_base = 110; 
    int right_base = 110;

    const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
    const int left_dir_pin=29;
    const int left_pwm_pin=40;
    const int right_dir_pin=30;
    const int right_pwm_pin=39;
    const int right_nslp_pin=11;

    int16_t blacks = 0;

    int16_t time_count = 0;

    bool donut = false;

void setup() {
   
    ECE3_Init();
    //Serial.begin(9600); //if we need serial monitor delete // in the beginning of this line

    pinMode(left_nslp_pin,OUTPUT);
    pinMode(left_dir_pin,OUTPUT);
    pinMode(left_pwm_pin,OUTPUT);
    pinMode(right_nslp_pin,OUTPUT);
    pinMode(right_dir_pin,OUTPUT);
    pinMode(right_pwm_pin,OUTPUT);
 
    digitalWrite(left_nslp_pin,HIGH);
    digitalWrite(left_dir_pin,LOW);
    digitalWrite(right_nslp_pin,HIGH);
    digitalWrite(right_dir_pin,LOW);



     
    delay(200);

}

void loop() {
  ECE3_read_IR(sensorValues);
 

    //offset and scale
    for (uint16_t i = 0; i < 8; i++)
    {
        sensorValues_2[i] = (sensorValues[i]-offset[i])/scale[i];
       
    }

    //weighs sensors and adds them
    for (unsigned char i = 0; i < 8; i++)
    {
      curr_err += (sensorValues_2[i]*weights[i]);
    }
   

  
    uint16_t leftSpd = left_base - (k_p*curr_err) + (k_d*(prev_err-curr_err)); 
    uint16_t rightSpd = right_base + (k_p*curr_err) - (k_d*(prev_err-curr_err));
   
    prev_err = curr_err;
    curr_err = 0;

    //Serial.println(curr_err); //if we need serial monitor delete
   
    //light = low, dark = high 
    //LEFT WHEEL S8 S7 S6 S5 S4 S3 S2 S1 RIGHT WHEEL


    uint16_t sensorSum = 0;
    
    for (uint16_t i = 0; i < 8; i++)
    {
        sensorSum += sensorValues[i];
    }
   
    if (sensorSum > 19000)
      {
          blacks++;
      }

    if(blacks > 2)
      {

        digitalWrite(left_dir_pin,HIGH);
        leftSpd = 180;
        rightSpd = 180;
        time_count++;
            if(time_count > 41 )
            {
              digitalWrite(left_dir_pin,LOW);
              blacks = 0;
              time_count = 0;
              donut = true;
            }
            else if(donut && time_count > 3)
            {
                digitalWrite(left_nslp_pin,LOW);
                digitalWrite(right_nslp_pin,LOW);
            }
        }



    analogWrite(left_pwm_pin,leftSpd);
    analogWrite(right_pwm_pin, rightSpd);
      //delay(50);

}
