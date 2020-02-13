// Purdue University, CGT 572, Project 1

// Credit to Michael Schoeffler 2017, http://www.mschoeffler.de for the GY-521 specific code
// Credit to Ashish Choudhary 2019 https://circuitdigest.com/users/ashish-choudhary for the step calculation code

#include "Wire.h" // This library allows you to communicate with I2C devices.
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t xaccl, yaccl, zaccl = 0; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data - read but unused
int16_t temperature; // variables for temperature data - read but unused
int steps, flag = 0;
int threshold = 100; // determined by trial and error
int16_t xval, yval, zval; // variables accelerometer raw data in calibration
int16_t xavg, yavg, zavg = 0; // variables for average accelerometer values calculated in calibration
int totvect; // magnitude of acceleration vector
int prev_totvect; // previous magnitude of acceleration vector
int totave; // average of previous and current acceleration vectors - opted not to use
int first_time; // flags first time loop() runs
int seconds = 0; // counts approx number of seconds elapsed
int seconds_10 = 0; // counts approx number of 0.1 seconds elapsed

char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

char* convert_long_to_str(long i) { // converts long to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6ld", i);
  return tmp_str;
}

void setup() {
  Serial.begin(9600); // establish serial connection
  // The proceeding code establishes communication to the GY-521 accelerometer over I2C
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  first_time = 1;

  calibrate();
}
void loop() {  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  xaccl = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  yaccl = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  zaccl = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  totvect = sqrt(((xaccl - xavg) * (xaccl - xavg)) + ((yaccl - yavg) * (yaccl - yavg)) + ((zaccl - zavg) * (zaccl - zavg)));
// The reference code includes an averaging of vectors, such as:
//  if (first_time) {
//    totave = totvect;
//  } else {
//    totave = (totvect + prev_totvect) / 2 ;
//    first_time = 0;
//  }
// However, the justification for this averaging was unclear. Instead, we simply did the following:
  totave = totvect;
  
  delay(100);

  // Count elapsed seconds
  seconds_10++;
  if(seconds_10 = 10) {
    seconds_10 = 0;
    seconds++;
  }
  
  if (totave > threshold && flag == 0) // Step has occurred!
  {
    steps = steps + 1;
    flag = 1;
    Serial.println(seconds);
  }
  else if (totave > threshold && flag == 1) // Step has occurred but has already been flagged!
  {
    // Don't Count
  }
  if (totave < threshold   && flag == 1) // Step is done occurring and flag should be removed!
  {
    flag = 0;
  }
  if (steps < 0) {
    steps = 0;
  }

  prev_totvect = totvect;
    
  delay(10);
}

void calibrate() {
//  Serial.println("Calibrating...");
  
  long xsum = 0L; // longs were used because the sum of 100 int_16t may overflow for an int_16t
  long ysum = 0L;
  long zsum = 0L;
  
  for(int a = 0; a < 100; a++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
    
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    xval = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    yval = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    zval = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

    // keep a running sum of all x, y, and z calues
    xsum = xval + xsum;
    ysum = yval + ysum;
    zsum = zval + zsum;
  }
  delay(100); // this delay may not be essential, but better safe than sorry :)
  // calculate the average acceleration for each axis
  xavg = xsum / 100.0;
  yavg = ysum / 100.0;
  zavg = zsum / 100.0;
}
