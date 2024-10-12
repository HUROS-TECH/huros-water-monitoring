/* 
 * Copyright HUROS TECH
 * Maintenance Danmirror
 * Year 2024
 * 
*/


#ifndef HUROS_WATER_MONITORING_H
#define HUROS_WATER_MONITORING_H
#include "Arduino.h"

#include <DallasTemperature.h>
#include <OneWire.h>
#define SCOUNT 30   

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

    // china
    float m_tdsCoef;
    int m_analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
    int m_analogBufferTemp[SCOUNT];

    // DF Robot
    float m_tdsFactor;
    float m_kVal;

    // KF
    float m_q;
    float m_r;

private:
    int GetMedianNum(int bArray[], int iFilterLen);

public:
    HurosWaterMonitoring();
    HurosWaterMonitoring(DallasTemperature &dallas);
    ~HurosWaterMonitoring();

    void KFInit();
    void SetKF_Q(float q);
    void SetKF_R(float r);

    float KFRun(float SensorData);

    void DallasInit();
    void PHInit(int pin, float ph4, float ph7);
    void TDSInit(int pin, float coef);
    void TDSDFRobotInit(int pin, float tdsFactor, float kVal);

    float PHGetData();
    float PHGetCalibration();
    float TDSGetData(float temp);
    float TDSDFRobotGetData(float temp);
    float DallasGetTemperatureC();


};

#endif 