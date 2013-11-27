#include <EEPROM.h>
#include <SPI.h>
#include <I2C.h>
#include "openIMUL.h"
#include "UBLOXL.h"
#include "PIDL.h"//the L is for local in case there is already a library by that name
#include <Streaming.h>

//LED defines
#define RED 38
#define YELLOW 40
#define GREEN 42
//ROM defines
#define ALL_DEF 7
#define GAIN_DEF 6
#define COMP_DEF 5
#define ACC_DEF 4 
#define ALL_CAL 3
#define GAIN_CAL 2
#define COMP_CAL 1
#define ACC_CAL 0


//general SPI defines
#define READ 0x80
#define WRITE 0x00
#define MULTI 0x40
#define SINGLE 0x00

//gyro defines - ST L3G2
#define L3G_CTRL_REG1 0x20
#define L3G_CTRL_REG2 0x21
#define L3G_CTRL_REG3 0x22
#define L3G_CTRL_REG4 0x23
#define L3G_CTRL_REG5 0x24
#define L3G_OUT_X_L 0x28

//acc defines - Analog Devices ADXL345
#define BW_RATE 0x2C
#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define DATAX0 0x32

/*
//accelerometer calibration values
 //this is where the values from the accelerometer calibration sketch belong
 #define ACC_OFFSET_X 8.0300521
 #define ACC_OFFSET_Y -1.0967937
 #define ACC_OFFSET_Z 6.9418921
 #define ACC_SCALE_X 0.0388304
 #define ACC_SCALE_Y 0.0381390
 #define ACC_SCALE_Z 0.0389158
 
 //magnetometer calibration values
 #define MAG_OFFSET_X 10.370532f
 #define MAG_OFFSET_Y 90.317025f
 #define MAG_OFFSET_Z -16.633857f
 #define MAG_W_INV_00 0.956960f
 #define MAG_W_INV_01 -0.004750f
 #define MAG_W_INV_02 -0.011861f
 #define MAG_W_INV_10 -0.004750f
 #define MAG_W_INV_11 0.950338f
 #define MAG_W_INV_12 -0.005574f
 #define MAG_W_INV_20 -0.011861f
 #define MAG_W_INV_21 -0.005574f
 #define MAG_W_INV_22 1.170783f
 */

//mag defines ST LSM303DLHC - will work with the HMC5883L
#define MAG_ADDRESS 0x1E
#define LSM303_CRA_REG (uint8_t)0x00 
#define LSM303_CRB_REG 0x01
#define LSM303_MR_REG 0x02
#define LSM303_OUT_X_H 0x03

//barometer defines
#define BMP085_ADDRESS 0x77
#define POLL_RATE 0
/*#define OSS 0x00
 #define CONV_TIME 5*/
/*#define OSS 0x01
 #define CONV_TIME 8*/
/*#define OSS 0x02
 #define CONV_TIME 14*/
#define OSS 0x03
#define CONV_TIME 26

//however digitalWrite will work when using SPI 
#define GyroSSOutput() DDRL |= 1<<0 //this is the same as pinMode(49,OUTPUT)
#define GyroSSHigh() PORTL |= 1<<0 //this is like digitalWrite(49,HIGH) but faster
#define GyroSSLow() PORTL &= ~(1<<0)

#define D22Output() DDRA |= 1<<0 //this is the same as pinMode(49,OUTPUT)
#define D22High() PORTA |= 1<<0 //this is like digitalWrite(49,HIGH) but faster
#define D22Low() PORTA &= ~(1<<0)
#define D22Toggle() PORTA ^= (1<<0);

#define D23Output() DDRA |= 1<<1 //this is the same as pinMode(49,OUTPUT)
#define D23High() PORTA |= 1<<1 //this is like digitalWrite(49,HIGH) but faster
#define D23Low() PORTA &= ~(1<<1)
#define D23Toggle() PORTA ^= (1<<1);

#define D24Output() DDRA |= 1<<2 //this is the same as pinMode(49,OUTPUT)
#define D24High() PORTA |= 1<<2 //this is like digitalWrite(49,HIGH) but faster
#define D24Low() PORTA &= ~(1<<2)
#define D24Toggle() PORTA ^= (1<<2);

#define D25Output() DDRA |= 1<<3 //this is the same as pinMode(49,OUTPUT)
#define D25High() PORTA |= 1<<3 //this is like digitalWrite(49,HIGH) but faster
#define D25Low() PORTA &= ~(1<<3)
#define D25Toggle() PORTA ^= (1<<3);

#define D26Output() DDRA |= 1<<4 //this is the same as pinMode(49,OUTPUT)
#define D26High() PORTA |= 1<<4 //this is like digitalWrite(49,HIGH) but faster
#define D26Low() PORTA &= ~(1<<4)
#define D26Toggle() PORTA ^= (1<<4);

#define D27Output() DDRA |= 1<<5 //this is the same as pinMode(49,OUTPUT)
#define D27High() PORTA |= 1<<5 //this is like digitalWrite(49,HIGH) but faster
#define D27Low() PORTA &= ~(1<<5)
#define D27Toggle() PORTA ^= (1<<5);

#define D28Output() DDRA |= 1<<6 //this is the same as pinMode(49,OUTPUT)
#define D28High() PORTA |= 1<<6 //this is like digitalWrite(49,HIGH) but faster
#define D28Low() PORTA &= ~(1<<6)
#define D28Toggle() PORTA ^= (1<<6);

#define D29Output() DDRA |= 1<<7 //this is the same as pinMode(49,OUTPUT)
#define D29High() PORTA |= 1<<7 //this is like digitalWrite(49,HIGH) but faster
#define D29Low() PORTA &= ~(1<<7)
#define D29Toggle() PORTA ^= (1<<7);

#define AccSSOutput() DDRL |= 1<<1 //this is the same as pinMode(48,OUTPUT)
#define AccSSHigh() PORTL |= 1<<1 //this is like digitalWrite(48,HIGH) but faster
#define AccSSLow() PORTL &= ~(1<<1)

//control defines 
#define TAKEOFF 0 
#define HH_ON 1
#define HH_OFF 2
#define LAND 3
#define LIFTOFF 1175 //3s

#define ACTIVE 0
#define STABLE 1
#define HEAD_FREE 2
#define CARE_FREE 3
#define WAYPOINT 4
#define RTB 5

#define WP_HOLD 0
#define WP_TRAVEL 1
#define WP_END 2
#define WP_RTB 3
#define WP_FAIL_RTB 4

//motor defines
#define FREQ 400
#define PRESCALE 8
#define PERIOD ((F_CPU/PRESCALE/FREQ) - 1)

#define Motor1WriteMicros(x) OCR3B = x * 2//motor 1 is attached to pin2
#define Motor2WriteMicros(x) OCR3C = x * 2//motor 2 is attached to pin3
#define Motor3WriteMicros(x) OCR3A = x * 2//motor 3 is attached to pin5
#define Motor4WriteMicros(x) OCR4A = x * 2//motor 4 is attached to pin6
#define Motor5WriteMicros(x) OCR4B = x * 2//motor 1 is attached to pin7
#define Motor6WriteMicros(x) OCR4C = x * 2//motor 2 is attached to pin8
#define Motor7WriteMicros(x) OCR1A = x * 2//motor 3 is attached to pin11
#define Motor8WriteMicros(x) OCR1B = x * 2//motor 4 is attached to pin12

//radio control defines
//RC defines
#define DSM2 0//to do remove - DSM2 will not be supported
#define DSMX 1
#define SBUS 2
#define RC 3

//telemetery defines
#define RADIO_BUF_SIZE 256
#define radio Serial2
#define NUM_WAY_POINTS 0x14

//to do - move these vars to appropriate headers 

//sensor related vars
typedef union{
  struct{
    int16_t x;
    int16_t y;
    int16_t z;
  }
  v;
  uint8_t buffer[6];
}
Sensor_t;

Sensor_t gyro;
Sensor_t acc;
Sensor_t mag;

//barometer variables
long pressure;
short temperature;
uint32_t baroTimer;
int pressureState;
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;
unsigned char msb;
unsigned char lsb;
unsigned char xlsb;
long x1;
long x2;
long x3;
long b3;
long b5;
long b6;
long p;
unsigned long b4;
unsigned long b7;
unsigned int ut;
unsigned long up;
uint32_t baroPollTimer;
boolean newBaro = false;
float pressureRatio;
int baroCount;
float baroSum;
long pressureInitial; 

//IMU related vars
int32_t gyroSumX,gyroSumY,gyroSumZ;
int16_t offsetX,offsetY,offsetZ;
float radianGyroX,radianGyroY,radianGyroZ;
float degreeGyroX,degreeGyroY,degreeGyroZ;
float smoothAccX,smoothAccY,smoothAccZ;
float floatMagX,floatMagY,floatMagZ;//needs to be a float so the vector can be normalized
float shiftedMagX,shiftedMagY,shiftedMagZ;
float scaledAccX,scaledAccY,scaledAccZ;
float accToFilterX,accToFilterY,accToFilterZ;

//GPS related vars
typedef union{
  struct{
    int32_t lat;
    int32_t lon;
    int32_t alt;
  }
  coord;
  uint8_t buffer[12];
}
WayPoint_t;

WayPoint_t homeBase;
WayPoint_t wayPoints[20];
WayPoint_t loiterWP;//rename

float rawX,rawY,rawZ;

UBLOX gps;
volatile boolean GPSDetected;
float beeLineDist;
float beeLineHeading;

//RC related vars
uint8_t readState,inByte,byteCount,channelNumber;
volatile uint8_t rcType;
uint32_t frameTime;
boolean detected = false;
volatile boolean newRC = false;
boolean frameStart = true;
boolean frameValid = false;

uint8_t spekBuffer[14];

int bufferIndex=0;

typedef union{
  struct{
    uint16_t aileron;//A8
    uint16_t aux1;//A9
    uint16_t elevator;//A10
    uint16_t gear;//A11
    uint16_t rudder;//A12
    uint16_t aux2;//A13
    uint16_t throttle;//A14
    uint16_t aux3;//A15 only for futaba or standard RC
  }
  values;
  byte buffer[16];
  uint16_t standardRCBuffer[8];
}
RadioControl_t;

volatile RadioControl_t rcCommands;
volatile uint16_t throttleCommand;

uint8_t currentPinState = 0;
uint8_t previousPinState = 0;
uint8_t changeMask = 0;
uint8_t lastPinState = 0;
uint16_t currentTime = 0;
uint16_t timeDifference = 0;
uint16_t changeTime[8];
int16_t offsetAileron,offsetElevator,offsetRudder = 0;
uint8_t sBusData[25];

//timers and DTs
uint32_t imuTimer,GPSTimer;
uint32_t generalPurposeTimer;
float imuDT,GPSDT;

//protocol related vars 
typedef union{
  struct{
    float pitch;//0
    float roll;//4
    float yaw;//8
    int32_t lattitude;//12
    int32_t longitude;//16
    float baroAltitude;//20
    int32_t gpsAltitude;//24
    int32_t velN;//28
    int32_t velE;//32
    int32_t velD;//36
    int32_t gpsHeading;//40
    uint32_t _3DSpeed;//44
    uint32_t groundSpeed;//48

    float kp_pitch_rate;//52
    float ki_pitch_rate;//56
    float kd_pitch_rate;//60
    float fc_pitch_rate;//64

    float kp_roll_rate;//68
    float ki_roll_rate;//72
    float kd_roll_rate;//76
    float fc_roll_rate;//80

    float kp_yaw_rate;//84
    float ki_yaw_rate;//88
    float kd_yaw_rate;//92
    float fc_yaw_rate;//96

    float kp_pitch_attitude;//100
    float ki_pitch_attitude;//104
    float kd_pitch_attitude;//108
    float fc_pitch_attitude;//112

    float kp_roll_attitude;//116
    float ki_roll_attitude;//120
    float kd_roll_attitude;//124
    float fc_roll_attitude;//128

    float kp_yaw_attitude;//132
    float ki_yaw_attitude;//136
    float kd_yaw_attitude;//140
    float fc_yaw_attitude;//144

    float kp_altitude_position;//148
    float ki_altitude_position;//152
    float kd_altitude_position;//156
    float fc_altitude_position;//160

    float kp_altitude_rate;//164
    float ki_altitude_rate;//168
    float kd_altitude_rate;//172
    float fc_altitude_rate;///176

    float kp_loiter_pos_x;//180
    float ki_loiter_pos_x;//184
    float kd_loiter_pos_x;//188
    float fc_loiter_pos_x;//192

    float kp_loiter_rate_x;//196
    float ki_loiter_rate_x;//200
    float kd_loiter_rate_x;//204
    float fc_loiter_rate_x;//208

    float kp_loiter_pos_y;//212
    float ki_loiter_pos_y;//216
    float kd_loiter_pos_y;//220
    float fc_loiter_pos_y;//224

    float kp_loiter_rate_y;//228
    float ki_loiter_rate_y;//232
    float kd_loiter_rate_y;//236
    float fc_loiter_rate_y;//240

    float kp_waypoint_position;//244
    float ki_waypoint_position;//248
    float kd_waypoint_position;//252
    float fc_waypoint_position;//256

    float kp_waypoint_velocity;//260
    float ki_waypoint_velocity;//264
    float kd_waypoint_velocity;//268
    float fc_waypoint_velocity;//272

    float kp_cross_track;//276
    float ki_cross_track;//280
    float kd_cross_track;//284
    float fc_cross_track;//288

    float declination;//292

    uint8_t gpsFix;//296
    uint8_t flightMode;//297


  }
  v;
  uint8_t buffer[300];
}
FC_Data_type;//think of better name

FC_Data_type d;

uint8_t numBytesIn,numBytesOut,inByteCount,inByteRadio,parseIndex=0,cmdIndex=0,cmdState=0,cmdBuffer[64];
volatile uint8_t inBuffer[RADIO_BUF_SIZE];
volatile uint16_t bufferIndexRadio=0;
uint16_t varIndex=0;
volatile boolean newRadio = false;
uint32_t radioTimer;
uint8_t inputSum=0,inputDoubleSum=0,outputSum=0,outputDoubleSum=0,packetTemp[2];
uint16_t packetNumberLocalOrdered,packetNumberRemoteOrdered,packetNumberLocalUn,packetNumberRemoteUn;
boolean ordered = false;
volatile boolean handShake = false;
uint8_t temp;
boolean tuningTransmit = false;
uint8_t numOfItems;
uint8_t itemBuffer[30];
uint32_t refreshMillis;
uint32_t tuningTimer;
boolean tuningTrasnmitOK = false;

typedef union{
  float num;
  uint8_t buffer[4];
}
Float_Union_t;

Float_Union_t outFloat;

uint8_t lenOfTuningDataPacket;
uint8_t refreshRate;


//control related vars
float pitchSetPoint;
float rollSetPoint;
float yawSetPoint;
float rateSetPointX;    
float rateSetPointY;
float rateSetPointZ;
float adjustmentX;
float adjustmentY;
float adjustmentZ; 
float altitudeSetPoint;
float throttleAdjustment;    
float velSetPointX,velSetPointY;
float targetAltitude,targetVelAlt,actualAltitude,distToAlt;
float yawInput;
float zero = 0.0;
float pitchSetPointTX,rollSetPointTX;
float distToWayPoint,targetVelWayPoint;
float speed2D_MPS;
float setPointX,setPointY;
uint8_t HHState = 3;
float motorCommand1,motorCommand2,motorCommand3,motorCommand4;

boolean integrate = false;
boolean throttleHold = true;
boolean enterState = true;

uint8_t currentWayPointNumber = 0, inputWayPointNumber = 0,wayPointState = 0;
boolean startFlight = false;
int32_t latTarget,lonTarget;//check to make sure this is correct for loiter
boolean endOfWPCheck = false; 
boolean RTBFailSafe = false;
boolean RTBFlag = false;

float altitudeDifference; 
boolean GPSPID = false;//needed?
uint32_t endOfWPTimer;
boolean calcYaw;

boolean headFreeOK = false,activeOK = false,loiterOK = false;


//failsafe related vars
boolean failSafe = false;
boolean toggle;
boolean watchDogStartCount;
uint32_t watchDogFailSafeCounter,RCFailSafeCounter,_200HzISRCounter;

//figure out where to put later--------------------
float xTarget,yTarget;
uint8_t calibrationFlags;

uint8_t outFloatIndex;

boolean allCalibrated = false;
boolean calibrationMode = false;


float ACC_OFFSET_X;
float ACC_OFFSET_Y;
float ACC_OFFSET_Z;
float ACC_W_INV_00;
float ACC_W_INV_01;
float ACC_W_INV_02;
float ACC_W_INV_10;
float ACC_W_INV_11;
float ACC_W_INV_12;
float ACC_W_INV_20;
float ACC_W_INV_21;
float ACC_W_INV_22;


//magnetometer calibration values
float MAG_OFFSET_X;
float MAG_OFFSET_Y;
float MAG_OFFSET_Z;
float MAG_W_INV_00;
float MAG_W_INV_01;
float MAG_W_INV_02;
float MAG_W_INV_10;
float MAG_W_INV_11;
float MAG_W_INV_12;
float MAG_W_INV_20;
float MAG_W_INV_21;
float MAG_W_INV_22;

uint32_t _200HzTimer;
float rateDT;

float gravSum;
float gravAvg;
float shiftedAccX,shiftedAccY,shiftedAccZ;

float headingFreeInitial;
uint16_t startingThrottle;
float velXBody,velYBody;
float wpXDist,wpYDist;



//constructors //fix the dts
openIMU imu(&radianGyroX,&radianGyroY,&radianGyroZ,&accToFilterX,&accToFilterY,&accToFilterZ,&scaledAccX,&scaledAccY,&scaledAccZ,&floatMagX,&floatMagY,&floatMagZ,&rawX,&rawY,&rawZ,&imuDT,&d.v.declination);

/*PID PitchAngle(&pitchSetPoint,&imu.pitch,&rateSetPointY,&integrate,&d.v.kp_pitch_attitude,&d.v.ki_pitch_attitude,&d.v.kd_pitch_attitude,&d.v.fc_pitch_attitude,&imuDT,800,800);
PID RollAngle(&rollSetPoint,&imu.roll,&rateSetPointX,&integrate,&d.v.kp_roll_attitude,&d.v.ki_roll_attitude,&d.v.kd_roll_attitude,&d.v.fc_roll_attitude,&imuDT,800,800);
YAW YawAngle(&yawSetPoint,&imu.yaw,&rateSetPointZ,&integrate,&d.v.kp_yaw_attitude,&d.v.ki_yaw_attitude,&d.v.kd_yaw_attitude,&d.v.fc_yaw_attitude,&imuDT,800,800);


PID PitchRate(&rateSetPointY,&degreeGyroY,&adjustmentY,&integrate,&d.v.kp_pitch_rate,&d.v.ki_pitch_rate,&d.v.kd_pitch_rate,&d.v.fc_pitch_rate,&rateDT,400,400);
PID RollRate(&rateSetPointX,&degreeGyroX,&adjustmentX,&integrate,&d.v.kp_roll_rate,&d.v.ki_roll_rate,&d.v.kd_roll_rate,&d.v.fc_roll_rate,&rateDT,400,400);
PID YawRate(&rateSetPointZ,&degreeGyroZ,&adjustmentZ,&integrate,&d.v.kp_yaw_rate,&d.v.ki_yaw_rate,&d.v.kd_yaw_rate,&d.v.fc_yaw_rate,&rateDT,400,400);


PID AltHoldPosition(&targetAltitude,&actualAltitude,&targetVelAlt,&integrate,&d.v.kp_altitude_position,&d.v.ki_altitude_position,&d.v.kd_altitude_position,&d.v.fc_altitude_position,&imuDT,1,1);
PID AltHoldRate(&targetVelAlt,&imu.velZ,&throttleAdjustment,&integrate,&d.v.kp_altitude_rate,&d.v.ki_altitude_rate,&d.v.kd_altitude_rate,&d.v.fc_altitude_rate,&imuDT,800,800);

PID WayPointPosition(&zero,&distToWayPoint,&targetVelWayPoint,&integrate,&d.v.kp_waypoint_position,&d.v.ki_waypoint_position,&d.v.kd_waypoint_position,&d.v.fc_waypoint_position,&GPSDT,10,10);
PID WayPointRate(&targetVelWayPoint,&velXBody,&pitchSetPoint,&integrate,&d.v.kp_waypoint_velocity,&d.v.ki_waypoint_velocity,&d.v.kd_waypoint_velocity,&d.v.fc_waypoint_velocity,&imuDT,45,45);
PID CrossTrack(&zero,&velYBody,&rollSetPoint,&integrate,&d.v.kp_cross_track,&d.v.ki_cross_track,&d.v.kd_cross_track,&d.v.fc_cross_track,&imuDT,30,30);

PID LoiterXPosition(&xTarget,&imu.XEst,&velSetPointX,&integrate,&d.v.kp_loiter_pos_x,&d.v.ki_loiter_pos_x,&d.v.kd_loiter_pos_x,&d.v.fc_loiter_pos_x,&imuDT,2,2);
PID LoiterXRate(&velSetPointX,&imu.velX,&setPointX,&integrate,&d.v.kp_loiter_rate_x,&d.v.ki_loiter_rate_x,&d.v.kd_loiter_rate_x,&d.v.fc_loiter_rate_x,&imuDT,25,25);
PID LoiterYPosition(&yTarget,&imu.YEst,&velSetPointY,&integrate,&d.v.kp_loiter_pos_y,&d.v.ki_loiter_pos_y,&d.v.kd_loiter_pos_y,&d.v.fc_loiter_pos_y,&imuDT,2,2);
PID LoiterYRate(&velSetPointY,&imu.velY,&setPointY,&integrate,&d.v.kp_loiter_rate_y,&d.v.ki_loiter_rate_y,&d.v.kd_loiter_rate_y,&d.v.fc_loiter_rate_y,&imuDT,25,25);*/


void setup(){
  delay(500);
  pinMode(RED,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(GREEN,OUTPUT);
  //digitalWrite(RED,HIGH);
  pinMode(13,OUTPUT);
  AccSSOutput();//this was moved from the init
  AccSSHigh();//if high isn't written to both devices befor config 
  GyroSSOutput();//the SPI bus will be addressing both devices 
  GyroSSHigh();
  D22Output();
  //pinMode(22,INPUT);
  D23Output();
  D24Output();
  D25Output();
  D26Output();
  D27Output();
  D28Output();
  D29Output();

  Serial.begin(115200);
  //DetectRC();
  _200HzISRConfig();
  MotorInit();
  //CalibrateESC();//throttle high will trigger this reset after calibration
  HandShake();
  I2c.begin();
  I2c.setSpeed(1);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);   



  if (calibrationMode == true){
    AccInit();
    MagInit();
    CalibrateSensors();  
  }
  ROMFlagsCheck();
  //DEBUG_DUMP();
  digitalWrite(RED,HIGH);
  //Arm();//move the rudder to the right to begin calibration
  digitalWrite(RED,LOW);
  digitalWrite(YELLOW,HIGH);
  AccInit();
  MagInit();
  GyroInit();
  imu.InitialQuat();
  CalcGravityOffSet();
  BaroInit();
  GPSStart();

  SafetyCheck();
  digitalWrite(GREEN,HIGH);
  delay(500);
  digitalWrite(GREEN,LOW);
  imuTimer = micros();
  _200HzTimer = micros();
  generalPurposeTimer = millis();
  watchDogStartCount = true;
}

void loop(){
  /*if ( micros() - _200HzTimer >= 2500){
    rateDT = (micros() - _200HzTimer) * 0.000001;
    _200HzTimer = micros();
    GetGyro();
    PitchRate.calculate();
    RollRate.calculate();
    YawRate.calculate();
    MotorHandler();
  }  */
  if ( micros() - imuTimer >= 10000){
    //estimators
    imuDT = (micros() - imuTimer) * 0.000001;
    imuTimer = micros();


    GetGyro();
    GetAcc();
    GetMag();

    imu.AHRSupdate();
    /*rateDT = (micros() - _200HzTimer) * 0.000001;
    _200HzTimer = micros();
    PitchRate.calculate();
    RollRate.calculate();
    YawRate.calculate();
    MotorHandler();*/
    imu.AccKalUpdate();
    imu.GetEuler();
    //end estimators
    //flight SM
    /*if (throttleHold == false){
      FlightSM();
    }
    //end flight SM
    if (d.v.flightMode > 0){
      PitchAngle.calculate();
      RollAngle.calculate();
      if (calcYaw == true){//allows for input of yaw with HH processor
        YawAngle.calculate();
      }
    }
    d.v.pitch = imu.pitch;
    d.v.yaw = imu.yaw;
    d.v.roll = imu.roll;

    tuningTrasnmitOK = true;*/
  }
  if (handShake == true){
    Radio();
    if (tuningTrasnmitOK == true){
      TuningTransmitter();
      tuningTrasnmitOK = false;
    }
  }



  if (gps.newData == true){
    gps.newData = false;
    GPSPID = true;
    gps.DistBearing(&homeBase.coord.lat,&homeBase.coord.lon,&gps.data.vars.lat,&gps.data.vars.lon,&rawX,&rawY,&beeLineDist,&beeLineHeading);
    imu.GPSKalUpdate();
    d.v.lattitude = gps.data.vars.lat;
    d.v.longitude = gps.data.vars.lon;
    d.v.gpsAltitude = gps.data.vars.height;
    d.v.velN = gps.data.vars.velN;
    d.v.velE = gps.data.vars.velE;
    d.v.velD = gps.data.vars.velD;
    d.v.gpsHeading = gps.data.vars.heading;
    d.v._3DSpeed = gps.data.vars.speed3D;
    d.v.groundSpeed = gps.data.vars.speed2D;
  }
  PollPressure();
  if (newBaro == true){
    newBaro = false;
    GetAltitude(&pressure,&pressureInitial,&rawZ);
    imu.BaroKalUpdate();
    d.v.baroAltitude = imu.ZEst;
  }


  if (millis() - generalPurposeTimer > 50){
   generalPurposeTimer = millis();
   //Serial<<rcCommands.values.aileron<<","<<rcCommands.values.elevator<<","<<rcCommands.values.throttle<<","<<rcCommands.values.rudder<<"\r\n";
   //Serial<<rollSetPoint<<","<<pitchSetPoint<<"\r\n";
   //debug 1 verify that the vel and pos est are correct
   //Serial<<generalPurposeTimer<<","<<rawX<<","<<rawY<<","<<imu.velX<<","<<imu.XEst<<","<<imu.velY<<","<<imu.YEst<<","<<imu.velZ<<","<<imu.ZEst<<"\r\n";
   //degub 2 verify that the PID loops are behaving correctly
   //Serial<<generalPurposeTimer<<","<<xTarget<<","<<imu.XEst<<","<<velSetPointX<<","<<imu.velX<<","<<setPointX<<","<<pitchSetPoint<<"\r\n";
   //Serial<<generalPurposeTimer<<","<<YTarget<<","<<imu.YEst<<","<<velSetPointY<<","<<imu.velY<<","<<setPointY<<<","<<rollSetPoint<<"\r\n";
   //Serial<<generalPurposeTimer<<",",<<targetAltitude<<","<<actualAltitude<<","<<targetVelAlt<<","<<imu.velZ<<","<<throttleAdjustment<<"\r\n";
   //Serial<<d.v.kp_waypoint_velocity<<","<<d.v.fc_waypoint_velocity<<"\r\n";
   Serial<<imu.pitch<<","<<imu.roll<<","<<imu.yaw<<"\r\n";
   //Serial<<imu.pitch<<","<<imu.roll<<","<<imu.yaw<<","<<imu.velX<<","<<imu.XEst<<","<<imu.velY<<","<<imu.YEst<<","<<imu.velZ<<","<<imu.ZEst<<"\r\n";
   //Serial<<imu.inertialX<<","<<imu.inertialY<<","<<imu.inertialZ<<"\r\n";
   //Serial<<imu.inertialZ<<","<<imu.velZ<<","<<imu.ZEst<<"\r\n";
   //Serial<<imu.inertialX<<","<<imu.velX<<","<<imu.XEst<<","<<imu.inertialY<<","<<imu.velY<<","<<imu.YEst<<","<<imu.inertialZ<<","<<imu.velZ<<","<<imu.ZEst<<"\r\n";
   //Serial<<acc.v.x<<","<<acc.v.y<<","<<acc.v.z<<","<<scaledAccX<<","<<scaledAccY<<","<<scaledAccZ<<"\r\n";
   }

  if (newRC == true){
    newRC = false;
    ProcessChannels();
    RCFailSafeCounter = 0;
  }  
  /*if (RCFailSafeCounter >= 200 || failSafe == true){
    TIMSK5 = (0<<OCIE5A);
    digitalWrite(13,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(YELLOW,LOW);
    digitalWrite(GREEN,LOW);
    Motor1WriteMicros(1000);//set the output compare value
    Motor2WriteMicros(1000);
    Motor3WriteMicros(1000);
    Motor4WriteMicros(1000);
    Motor5WriteMicros(1000);
    Motor6WriteMicros(1000);
    Motor7WriteMicros(1000);
    Motor8WriteMicros(1000);
    if (failSafe == true){
      digitalWrite(RED,HIGH);
    }
    while(1){

      digitalWrite(YELLOW,HIGH);
      delay(500);
      digitalWrite(YELLOW,LOW);
      delay(500);
    }
  }*/
  watchDogFailSafeCounter = 0;
  //Serial.println(freeMemory());
}


/*void FlightSM(){
  switch (d.v.flightMode){
  case ACTIVE://active
    if (enterState == true){    
      throttleAdjustment = 0;
      enterState = false;
      digitalWrite(13,HIGH);
      digitalWrite(RED,HIGH);
      digitalWrite(YELLOW,HIGH);
      digitalWrite(GREEN,HIGH);


    }
    if (newRC == true){
      if (rcCommands.values.gear > 1500){
        d.v.flightMode = STABLE;
        enterState = true;
        headFreeOK = false;
        PitchAngle.reset();
        RollAngle.reset();
        YawAngle.reset();
        HHState = HH_OFF;
      }
    }
    break;
  case STABLE://stable
    if (enterState == true){
      throttleAdjustment = 0;
      enterState = false;
      digitalWrite(13,HIGH);
      digitalWrite(RED,LOW);
      digitalWrite(YELLOW,LOW);
      digitalWrite(GREEN,LOW);
    }
    if (newRC == true){
      if (rcCommands.values.gear > 1500){
        activeOK = true;
      }

      if (rcCommands.values.gear < 1500 && activeOK == true){
        activeOK = false;
        d.v.flightMode = ACTIVE; 
        enterState = true;
      }
      if (rcCommands.values.aux1 < 1500 && rcCommands.values.aux2 < 1500 && rcCommands.values.aux3 < 1500){
        headFreeOK = true;
      }
      if (rcCommands.values.aux1 > 1500 && headFreeOK == true){
        activeOK = false;
        headFreeOK = false;
        d.v.flightMode = HEAD_FREE; 
        //d.v.flightMode = LOITER;
        enterState = true;
        //headingFreeInitial = imu.yaw; //add later
      }
    }
    HeadingHold();
    break;
  case HEAD_FREE:
    if (enterState == true){
      throttleAdjustment = 0;
      enterState = false;
      //GPSState = MANUAL; // add once the decision is final regarding modes
      digitalWrite(13,LOW);
      digitalWrite(RED,HIGH);
      digitalWrite(YELLOW,LOW);
      digitalWrite(GREEN,LOW);
      headingFreeInitial = imu.yaw;
    }
    HeadingHold();
    Rotate2D(&imu.yaw,&headingFreeInitial,&pitchSetPointTX,&rollSetPointTX,&pitchSetPoint,&rollSetPoint);
    if (newRC == true){
      if (rcCommands.values.aux1 < 1500){
        d.v.flightMode = STABLE;
        enterState = true;
        //stableCause = 2;
      }
      if (rcCommands.values.aux2 < 1500){
        loiterOK = true;
      }
      if (rcCommands.values.aux2 > 1500 && loiterOK == true && GPSDetected == true && rcCommands.values.aux3 < 1500){
        d.v.flightMode = CARE_FREE;
        enterState = true;
      }
    }
    break;
  case CARE_FREE:
    if (enterState == true){
      SetAltHold();
      SetXYLoiterPosition();
      enterState = false;
      startingThrottle = rcCommands.values.throttle; //will this bue used or will we do simple controls like up down?
      digitalWrite(13,LOW);
      digitalWrite(RED,LOW);
      digitalWrite(YELLOW,HIGH);
      digitalWrite(GREEN,LOW);
    }
    HeadingHold();
    GPSStable();


    if (newRC == true){
      if (rcCommands.values.aux1 < 1500){
        d.v.flightMode = STABLE;
        enterState = true;
        loiterOK = false;
      }

      if (rcCommands.values.aux2 < 1500){
        d.v.flightMode = HEAD_FREE; 
        enterState = true;
      }
      if (rcCommands.values.aux3 > 1500 && handShake == true && RTBFailSafe == false){
        d.v.flightMode = WAYPOINT; 
        WayPointPosition.reset();
        WayPointRate.reset();
        enterState = true;
        calcYaw = true;
        endOfWPCheck = false; 

      }
    }
    break;
  case WAYPOINT:
    if (enterState == true){
      //GPSTimer = millis();
      //altHoldTimer = GPSTimer;
      altitudeDifference = d.v.gpsAltitude - d.v.baroAltitude;
      enterState = false;
      yawSetPoint = imu.yaw;
      wayPointState = WP_HOLD;
      digitalWrite(13,LOW);
      digitalWrite(RED,LOW);
      digitalWrite(YELLOW,LOW);
      digitalWrite(GREEN,HIGH);
    }
    WayPointControl();
    if (newRC == true){
      if (rcCommands.values.aux1 < 1500){
        d.v.flightMode = STABLE;
        enterState = true;
        loiterOK = false;
      }
      if (rcCommands.values.aux2 < 1500){
        d.v.flightMode = HEAD_FREE; 
        enterState = true;
      }
      if (rcCommands.values.aux3 < 1500){
        d.v.flightMode = CARE_FREE;
        enterState = true;
      }   
    }
    break;
  default:
    failSafe = true;
    break;
  }
}*/





















