#include <HurosWaterMonitoring.h>

// Initial Pin
#define ph_pin 35
HurosWaterMonitoring hwm;

void setup() {
  Serial.begin(9600);
  hwm.PHInit(ph_pin, 0, 0);

}

void loop() {
  float ph = hwm.PHGetCalibration();

  Serial.print("\tPH(volt) : ");
  Serial.print(ph);

  Serial.println();
}
