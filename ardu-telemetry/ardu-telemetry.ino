#include <Wire.h>

struct STREAM_DATA {
  uint16_t battVoltage = 13200; //millivolt
  float altitude = 3145; //ertefa
  float climb = 23.4; // rise speed
  float roll = -1.4;
  float pitch = -11.3;
  float yaw = 305.6;
  uint8_t gps_sats = 12;
  float gps_lon = 52.619566;
  float gps_lat = 34.379513;
  float home_lon = 52.619766;
  float home_lat = 34.379713;
  uint16_t gps_speed = 87;
  boolean gps_fix = true;
  uint32_t home_distance = 0;
};

STREAM_DATA streamData;

boolean set = false;


void setup() {
  Serial.begin(9600);

  Wire.begin(4);
  Wire.onRequest(onRequest);

  Serial.println("Starting up...");

}

void loop() {

}

#define d2r (M_PI / 180.0)
float calc_dist(float lat1, float long1, float lat2, float long2) {
  double dlong = (long2 - long1) * d2r;
  double dlat = (lat2 - lat1) * d2r;
  double a = pow(sin(dlat / 2.0), 2) + cos(lat1 * d2r) * cos(lat2 * d2r) * pow(sin(dlong / 2.0), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float d = 6367 * c * 1000; // in meters
  if (d >= 0) {
    return d;
  } else {
    return -d;
  }
}

void set1() {

  int  alt = streamData.altitude * 10;
  byte  altHi = highByte(alt )  ;
  byte  altLo =  lowByte(alt ) ;

  int  yaw = streamData.yaw * 100;
  byte yawHi = highByte(yaw );
  byte yawLo = lowByte(yaw );


  int  speed2 = streamData.gps_speed * 10;
  byte speedHi = highByte(speed2);
  byte speedLo = lowByte(speed2);


  int  roll = streamData.roll * 10;
  byte rollHi = highByte(roll);
  byte rollLo = lowByte(roll);


  int  pitch = streamData.pitch * 10;
  byte pitchHi = highByte(pitch);
  byte pitchLo = lowByte(pitch);

  int distance = calc_dist(streamData.gps_lat, streamData.gps_lon, streamData.home_lat, streamData.home_lon) * 100;
  byte distanceHi = highByte(distance);
  byte distanceLo = lowByte(distance);

  byte buffer[16] = {0x89, 0xAB, streamData.gps_sats, altHi, altLo, yawHi, yawLo, speedHi, speedLo, rollHi , rollLo, pitchHi, pitchLo, distanceHi, distanceLo, 0x00};
  Wire.write(buffer, 16);
}
void set2() {

  int  rise = streamData.climb * 10;
  byte  riseHi = highByte(rise);
  byte  riseLo =  lowByte(rise);

  byte voltesHi = highByte(streamData.battVoltage);
  byte voltesLo = lowByte(streamData.battVoltage);

  int32_t latt = 1e7*streamData.gps_lat;
  byte latHHi = byte((latt >> 24) & 0x000000FF);
  byte latHi  = byte((latt >> 16) & 0x000000FF);
  byte latLo  = byte((latt >> 8)  & 0x000000FF);
  byte latLLi = byte((latt >> 0)  & 0x000000FF);

  int32_t longg = 1e7*streamData.gps_lon;
  byte logHHi = byte((longg >> 24) & 0x000000FF);
  byte logHi  = byte((longg >> 16) & 0x000000FF);
  byte logLo  = byte((longg >> 8)  & 0x000000FF);
  byte logLLi = byte((longg >> 0)  & 0x000000FF);

  byte buffer[16] = {0x89, 0xCD, streamData.gps_sats, riseHi, riseLo, voltesHi, voltesLo, latHHi, latHi, latLo , latLLi, logHHi, logHi, logLo , logLLi, 0x00};
  Wire.write(buffer, 16);
}


void onRequest() {
  if (set) {
    set = false;
    set1();
  } else {
    set = true;
    set2();
  }
}
