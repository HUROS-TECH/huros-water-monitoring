#include "HurosWaterMonitoring.h"

HurosWaterMonitoring::HurosWaterMonitoring()
: m_espADC(4096.0)
, m_espVoltage(3.3)
{
}

HurosWaterMonitoring::HurosWaterMonitoring(DallasTemperature &dallas)
: m_espADC(4096.0)
, m_espVoltage(3.3)
{
    m_dallas = &dallas;
}
HurosWaterMonitoring::~HurosWaterMonitoring()
{
}

void HurosWaterMonitoring::PHInit(int pin, float ph4, float ph7)
{
    m_phPin = pin;
    m_ph4 = ph4;
    m_ph7 = ph7;

    pinMode(m_phPin, INPUT);
}

float HurosWaterMonitoring::PHGetData() 
{
    float volt = analogRead(m_phPin) * (m_espVoltage / m_espADC);

    float ph_step = ( m_ph4 - m_ph7) / 3;
    return 7.00 + ((m_ph7 - volt) / ph_step);
}

float HurosWaterMonitoring::PHGetCalibration() 
{
    float volt = analogRead(m_phPin) * (m_espVoltage / m_espADC);

    return volt;
}

void HurosWaterMonitoring::TDSInit(int pin, float tdsFactor, float kVal) 
{
    m_tdsPin = pin;
    m_kVal = kVal;
    m_tdsFactor = tdsFactor;

    pinMode(m_tdsPin,INPUT);
}

float HurosWaterMonitoring::TDSGetData(float temp) 
{
    float analogValue = analogRead(m_tdsPin);
	float voltage = analogValue / m_espADC * m_espVoltage;
	float ecValue = (133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage) * m_kVal;
	float ecValue25  =  ecValue / (1.0+0.02*(temp-25.0));  //temperature compensation
	return ecValue25 * m_tdsFactor;
}

void HurosWaterMonitoring::DallasInit()
{
    m_dallas->begin();
}

float HurosWaterMonitoring::DallasGetTemperatureC()
{
    static double value = 0;
    m_dallas->requestTemperatures(); 
    if(m_dallas->getTempCByIndex(0) > 0)
    {
        double staging = m_dallas->getTempCByIndex(0);
        value = staging > 0 ? staging : value; 
    }
    return value;

}