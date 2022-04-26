#include <Wire.h>

#define d2r (3.141592 / 180.0)

float calc_dist(float lat1, float long1, float lat2, float long2);
void set1(void);
void set2(void);
void onRequest(void);

struct STREAM_DATA {
  float     climb       = 1.4;        // rise speed
  float     roll        = 2.4;        // roll degree
  float     pitch       = -11.3;      // pitch degree
  float     yaw         = 15.6;       // yaw degree
  float     altitude    = 1401;       // GPS altitude
  float     gps_lon     = 51.405274;  // GPS longitude
  float     gps_lat     = 35.701239;  // GPS latitude
  float     home_lon    = 51.447338;  // base longitude
  float     home_lat    = 35.689688;  // base latitude
  uint8_t   gps_sats    = 12;         // number of GPS satellites
  uint16_t  gps_speed   = 87;         // GPS linear speed
  uint16_t  battVoltage = 13200;      // in millivolts
};

STREAM_DATA streamData;
boolean set = false;

void setup() {
  Serial.begin(9600);

  Wire.begin(4);
  Wire.onRequest(onRequest);

  Serial.println("Started up");
}

void loop() {
 /* NOTHING */
}

float calc_dist(float lat1, float long1, float lat2, float long2) {
  double dlong = (long2 - long1) * d2r;
  double dlat = (lat2 - lat1) * d2r;
  double a = pow(sin(dlat / 2.0), 2) + cos(lat1 * d2r) * cos(lat2 * d2r) * pow(sin(dlong / 2.0), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float d = 6372.2 * c * 1000; // in meters
  return d = (d>=0) ? d : -d;
}

void set1(void) {

  int  alt    = streamData.altitude * 10;
  byte altHi  = highByte(alt);
  byte altLo  = lowByte(alt);

  int  yaw    = streamData.yaw * 100;
  byte yawHi  = highByte(yaw);
  byte yawLo  = lowByte(yaw);

  int  speed2   = streamData.gps_speed * 10;
  byte speedHi  = highByte(speed2);
  byte speedLo  = lowByte(speed2);

  int  roll   = streamData.roll * 10;
  byte rollHi = highByte(roll);
  byte rollLo = lowByte(roll);

  int  pitch    = streamData.pitch * 10;
  byte pitchHi  = highByte(pitch);
  byte pitchLo  = lowByte(pitch);

  int  distance   = calc_dist(streamData.gps_lat, streamData.gps_lon, streamData.home_lat, streamData.home_lon) * 100;
  byte distanceHi = highByte(distance);
  byte distanceLo = lowByte(distance);

  byte buffer[16] = {0x89, 0xAB, streamData.gps_sats, altHi, altLo, yawHi, yawLo, speedHi, speedLo, rollHi , rollLo, pitchHi, pitchLo, distanceHi, distanceLo, 0x00};
  Wire.write(buffer, 16);
}
void set2(void) {

  int  rise     = streamData.climb * 10;
  byte  riseHi  = highByte(rise);
  byte  riseLo  = lowByte(rise);

  byte voltesHi = highByte(streamData.battVoltage);
  byte voltesLo = lowByte(streamData.battVoltage);

  int32_t lat = 1e7*streamData.gps_lat;
  byte latHHi = byte((lat >> 24) & 0x000000FF);
  byte latHi  = byte((lat >> 16) & 0x000000FF);
  byte latLo  = byte((lat >> 8)  & 0x000000FF);
  byte latLLi = byte((lat >> 0)  & 0x000000FF);

  int32_t lon = 1e7*streamData.gps_lon;
  byte lonHHi = byte((lon >> 24) & 0x000000FF);
  byte lonHi  = byte((lon >> 16) & 0x000000FF);
  byte lonLo  = byte((lon >> 8)  & 0x000000FF);
  byte lonLLi = byte((lon >> 0)  & 0x000000FF);

  byte buffer[16] = {0x89, 0xCD, streamData.gps_sats, riseHi, riseLo, voltesHi, voltesLo, latHHi, latHi, latLo , latLLi, lonHHi, lonHi, lonLo , lonLLi, 0x00};
  Wire.write(buffer, 16);
}


void onRequest(void) {
  if (set)
    set1();
  else
    set2();
  set = !set;
}
