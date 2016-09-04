/**********************************************************/
//               Robot Racer Controller                   //
//               Written By: Sirui Song                   //
//               Last Updated: Jun 30, 2014               //
/**********************************************************/
//#define TEST_OUTPUT

//Import Libraries
#include <Servo.h>       
#include <ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/Point32.h>
#include <SPI.h>

//servo
Servo throttleServo, steerServo, brakeServo;
#define SERVO_THROTTLE_PIN    10
#define SERVO_STEER_PIN       11
#define SERVO_BRAKE_PIN       12
#define THROTTLE_MAX          1650
#define THROTTLE_REV_MAX      1300

unsigned int throttle_auto  = 1500;
unsigned int steering_auto  = 1500;
unsigned int throttle_rc    = 1500;
unsigned int steering_rc    = 1500;
unsigned int brake_rc       = 1500;
unsigned int throttle       = 1500;
unsigned int steering       = 1500;
unsigned int brake          = 2000;


//Define Analog Input Pin
#define BATT_VIN_1_PIN        A0
#define BATT_VIN_2_PIN        A1  
#define STR_CURRENT_PIN       A3
#define STR_ENCODER_PIN       A2

//Define State Pins
#define RELAY_PIN             4

/*Velocity encoder*/
#define Hz                    40
#define PPR                   512          //512 for full Quad Decoding, 
#define RadtoM                0.11/1.6  //For front two wheels

//Encoder pin assignment
int ss1 = 46; //
int ss2 = 47; //
int ss3 = 49; // Rear 

/**********************************************************/
//                   Helper Functions                     //
/**********************************************************/
//double servo_scale(int value, int _min, int _max) {
//  int delta = _max - _min;
//  return (double) 180 *  (value-_min - delta/2)/delta;
//};

//Calculate velocity from encoder pulses

/**********************************************************/
//                Longitudinal PWM Code                   //
/**********************************************************/
//May not be needed. 

/**********************************************************/
//                     RC Read In                         //
/**********************************************************/
//int ppm[8];
//void RC_read ()
//{
//  if (Serial2.available() > 0){
//    int count00       = 0;
//    int buffer_size   = 0;
//    int incomingByte  = 0;
//    while (count00 < 50){
//      // throw away extra bytes
//      if (Serial2.available() > 16) {
//        incomingByte = Serial2.read();
//        count00 = 0;
//      } else {
//        count00++;
//      }
//
//      // wait for a full transmition
//      if (Serial2.available() != buffer_size)
//        count00 = 0;
//
//      // make sure we are at the end of a transmition
//      buffer_size = Serial2.available();
//    }
//    
//    if (Serial2.available() == 16) {
//      int ppm_data  = 0;
//      int ppm_ch    = 0;
//      for (unsigned int i=0; i<16; i++){
//        incomingByte = Serial2.read();
//        if (i >= 2) {
//          if (i%2 == 0) {
//            ppm_ch        = (incomingByte >> 3);
//            incomingByte -= (ppm_ch << 3);
//            ppm_data      = (incomingByte << 8);
//
//            if (ppm_ch > 7) break;
//          } else {
//            ppm_data     += incomingByte;
//            ppm[ppm_ch]   = ppm_data;
//          }
//        }
//      }
//    }
//  }
//        #ifdef TEST_OUTPUT
//        Serial.print("RX Data: ");
//        Serial.print(ppm[0], DEC); // throttle
//        Serial.print(",");
//        Serial.print(ppm[1], DEC); // aliron
//        Serial.print(",");
//        Serial.print(ppm[2], DEC); // elevator
//        Serial.print(",");
//        Serial.print(ppm[3], DEC); // rudder
//        Serial.print(",");
//        Serial.print(ppm[4], DEC); // gear
//        Serial.print(",");
//        Serial.print(ppm[5], DEC); // RC/Auto
//        Serial.print(",");
//        Serial.print(ppm[6], DEC); // ch 7
//        Serial.print("\n");
//      #endif
//}
///**********************************************************/
////               ROS communication setup                  //
///**********************************************************/
//
ros::NodeHandle nh;

std_msgs::Float32 cmdVelocity;
std_msgs::Float32 cmdSteering;
std_msgs::Int8 state_msg;
std_msgs::Int16 str_msg;
std_msgs::Int16 str_cur_msg;
std_msgs::Int16 enc_cnt_msg;

geometry_msgs::Point32 encoder_msg;
#include <MegaEncoderCounter.h>

ros::Publisher state("vehState", &state_msg);
ros::Publisher encoder("encoderVelocity", &encoder_msg);
ros::Publisher str("strAngle", &str_msg);
ros::Publisher str_cur("strCurrent",&str_cur_msg);
ros::Publisher enc_cnt("encoderCount",&enc_cnt_msg);

void cmd_vel_cb( const std_msgs::Float32& cmd_vel_msg);
void cmd_str_cb( const std_msgs::Float32& cmd_str_msg);

ros::Subscriber <std_msgs::Float32> velocity_msg ("/RR1/velocity_cmd", cmd_vel_cb); 
ros::Subscriber <std_msgs::Float32> steering_msg ("/RR1/steering_cmd", cmd_str_cb); 

int ROS_watchdog = 0;
//
void cmd_vel_cb(const std_msgs::Float32& cmd_vel_msg)
{
    ROS_watchdog = 0;
    throttle_auto = round(cmd_vel_msg.data*10.7+1557);
    return;
}

void cmd_str_cb(const std_msgs::Float32& cmd_str_msg)
{
    ROS_watchdog = 0;
    steering_auto = round(cmd_str_msg.data);
    return;
}
////ESC Setup
//void setup_ESC()
//{
//  long tmp_time = millis();
//  
//  while ( (millis() - tmp_time) <5000)
//  {
//  
//    throttleServo.writeMicroseconds(1500);
//  }
//}
//Encoder Counter Functions
void setup_counter(int ss)
{
  digitalWrite(ss,HIGH);
  digitalWrite(ss,LOW);
  SPI.transfer(0b00100000); //Write to counter
  digitalWrite (ss,HIGH);
  
  digitalWrite(ss,HIGH);
  digitalWrite(ss,LOW);
  SPI.transfer(0b10001000); //10001xxx Write to MDR0
  SPI.transfer(0b00000011); //00000011
  digitalWrite (ss,HIGH);
  
  delay (10);
  
  digitalWrite(ss,HIGH);
  digitalWrite (ss,LOW); 
  SPI.transfer(0b10010000); //10010xxx Write to MDR1
  SPI.transfer(0b00000010); //00000010
  digitalWrite(ss,HIGH); 
  
}

int enc_pos_read(int ss)
{
  digitalWrite(ss,HIGH);
  digitalWrite(ss,LOW);
  SPI.transfer(0b01100000);
  byte response1 = SPI.transfer (0);
  byte response2 = SPI.transfer (0);
  digitalWrite(ss,HIGH);
  
  int temp = 0;
  temp = ((response1 << 8)|response2) ;
  //temp <<= 8;
  //temp = (temp | response2);
  //Serial.print(temp,DEC);
  
  /*
  Serial.print("\t\tByte1: ");
  Serial.print(response1,BIN);
  Serial.print("\tByte2: ");
  Serial.println(response2,BIN);
  */
      
  return temp;
  
}
//
////reset counter only
void enc_pos_reset(int ss)
{
  digitalWrite(ss,HIGH);
  digitalWrite(ss,LOW);
  SPI.transfer(0b00100000);
  digitalWrite(ss,HIGH);
  
}
void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  nh.getHardware()->setBaud(115200);
  nh.initNode();  
  nh.advertise(state);
  nh.advertise(encoder);
  nh.advertise(str);
  nh.advertise(str_cur);
  //nh.advertise(enc_cnt);
  nh.subscribe(velocity_msg);
  nh.subscribe(steering_msg);
  
  pinMode(RELAY_PIN, INPUT);
  
  throttleServo.attach(SERVO_THROTTLE_PIN);
  steerServo.attach(SERVO_STEER_PIN);
  brakeServo.attach(SERVO_BRAKE_PIN);
//  
  //Velocity Encoder Setup
  pinMode(ss1,OUTPUT);
  pinMode(ss2,OUTPUT);
  pinMode(ss3,OUTPUT);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  
  delay(1000);
  
  setup_counter(ss1);
  setup_counter(ss2);
  setup_counter(ss3);
  
//  setup_ESC();
//  
}
long CurrentTime = micros();
static long PrevTime = 0;
long timeDiff = CurrentTime - PrevTime;
//  
void loop()
{  
//  nh.spinOnce(); 
//  RC_read ();
//   
/**********************************************************/
//                      Encoder Read                      //
/**********************************************************/
  CurrentTime = micros();
  timeDiff = CurrentTime - PrevTime;  
  if (timeDiff >= (1.0/Hz * 1E6)) 
  {    
      int ENC_L_CNT = enc_pos_read(ss1);
      int ENC_R_CNT = enc_pos_read(ss2); 
      int ENC_M_CNT = enc_pos_read(ss3);
      
      encoder_msg.x = (-1E6*ENC_L_CNT/PPR)/1.6*(PI*0.11)/timeDiff;
      encoder_msg.y = 1E6/PPR*ENC_R_CNT*PI*RadtoM/timeDiff;    
      encoder_msg.z = 1E6/(PPR*4)*ENC_M_CNT*PI*RadtoM/timeDiff;
     
      encoder.publish (&encoder_msg);
      
      //enc_cnt_msg.data = enc_cnt_msg.data + ENC_R_CNT;

      //enc_cnt.publish (&enc_cnt_msg);
      if (encoder_msg.x > 0 || encoder_msg.y > 0 || encoder_msg.z > 0){
        //Serial.println();
        //Serial.println(ENC_L_CNT);
        //Serial.println(ENC_R_CNT);
        //Serial.println(ENC_M_CNT);
        /*Serial.println();
        Serial.println(encoder_msg.x);
        Serial.println(encoder_msg.y);
        Serial.println(encoder_msg.z);*/
      }
            
      //Serial.println(analogRead(STR_ENCODER_PIN));
//      str_msg.data = analogRead(STR_ENCODER_PIN)-657;
//      str.publish(&str_msg);
      
      //Serial.println(analogRead(STR_CURRENT_PIN));
//      str_cur_msg.data = analogRead(STR_CURRENT_PIN);
//      str_cur.publish(&str_cur_msg);
      
      enc_pos_reset(ss1);
      enc_pos_reset(ss2);
      enc_pos_reset(ss3);
      
      PrevTime = CurrentTime;
  } 
  
///**********************************************************/
////                      State Machine                     //
///**********************************************************/
//  boolean RC_on         = true;              //True for RC, false for Auto 
//  boolean Estop_on      = true;              //True for RC, false for Auto
//  
////  if (ppm[4] < 700) 
//    Estop_on = true;
//  else
//    Estop_on = false;
//  
//  if (Estop_on) {
//    
//    #ifdef TEST_OUTPUT
//    Serial.println("ESTOP MODE");
//    #endif 
//    //Estop state:     
//  
//    while(throttle >= 1500)
//    {
//      throttle -= 5;
//      throttleServo.writeMicroseconds(throttle);
//      delay(100);
//      
//    }//end while 
//    steering = 1500;
//    brake = 2000;
//    
//    //Reset Integrator 
//  } 
//  else if (ppm[5] < 700) {
//
//    //RC state
//    //Transfer PWM, with a max output limited
//
//    #ifdef TEST_OUTPUT
//    Serial.print("RC MODE \t"); 
//    Serial.print("throttle PWM: ");  
//    Serial.println(throttle_rc);
//    #endif  
//   
//    //Read in PWM values:     
//    throttle_rc = round((ppm[2] - 1022) / 1.416) + 1500;
//    steering_rc = -round((ppm[3] - 1022) / 1.432) + 1500;
//    brake_rc = round((ppm[0] - 1022) / 1.432) + 1500;   
//    
//    throttle = throttle_rc;
//    steering = steering_rc;
//    brake = brake_rc;
//  }
//  else {
//    #ifdef TEST_OUTPUT
//    Serial.println("AUTO MODE"); 
//    #endif 
//    
//    //Auto state
//    //Output to motor: (May implement a min(input, from computer) for throttle
//   
//    throttle = throttle_auto;
//    steering = steering_auto;
//    }
//  
//  
//  
//  if (throttle > THROTTLE_MAX )
//    throttle = THROTTLE_MAX ;
//  if (throttle < THROTTLE_REV_MAX)
//    throttle = THROTTLE_REV_MAX;   
//
Serial.println(encoder_msg.x);
Serial.println(encoder_msg.y);
Serial.println(encoder_msg.z);
Serial.println("___________");

//  steering = 1500;
//  brake = 2000;
//  throttle = 1500;

  steerServo.writeMicroseconds (steering); 
  throttleServo.writeMicroseconds (throttle);
  brakeServo.writeMicroseconds (brake);

  
}





