#include <HurosWaterMonitoring.h>

HurosWaterMonitoring hwm;
// Initial Pin
#define tds_pin 34

// Calibration TDS
#define CALIBRATION_COEF  1.0
#define VALUE_DESIRED 342


void setup() {
  Serial.begin(9600);
  hwm.TDSInit(tds_pin, CALIBRATION_COEF);

}

void loop() {
  float tds = hwm.TDSGetData(30);
  float coef = VALUE_DESIRED/tds;
  Serial.print("TDS(ppm) : ");
  Serial.print(tds);
  Serial.print("\tCOEF : ");
  Serial.print(coef);

  Serial.println();
}
