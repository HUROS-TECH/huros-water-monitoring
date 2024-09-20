/* 
 * Copyright HUROS ROBOTICS
 * Maintenance Danu andrean
 * Year 2024
 * 
*/


#ifndef HUROS_WATER_MONITORING_H
#define HUROS_WATER_MONITORING_H
#include "Arduino.h"

#include <DallasTemperature.h>
#include <OneWire.h>


class HurosWaterMonitoring
{

private:
    DallasTemperature *m_dallas;
    int m_phPin;
    int m_tdsPin;

    float m_espADC;
    float m_espVoltage;
    float m_ph4;
    float m_ph7;
    float m_tdsFactor;
    float m_kVal;

public:
    HurosWaterMonitoring();
    HurosWaterMonitoring(DallasTemperature &dallas);
    ~HurosWaterMonitoring();

    void DallasInit();
    void PHInit(int pin, float ph4, float ph7);
    void TDSInit(int pin, float tdsFactor, float kVal);

    float PHGetData();
    float PHGetCalibration();
    float TDSGetData(float temp);
    float DallasGetTemperatureC();


};

#endif 