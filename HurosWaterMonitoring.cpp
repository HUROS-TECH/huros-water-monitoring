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

void HurosWaterMonitoring::KFInit()
{
    m_r = 10;
    m_q = 0.1;
}
void HurosWaterMonitoring::SetKF_Q(float q)
{
    m_q = q;
}
void HurosWaterMonitoring::SetKF_R(float r)
{
    m_r = r;
}

float HurosWaterMonitoring::KFRun(float SensorData)
{
  static float Xt = 0;
  static float Xt_update = 0;
  static float Xt_prev = 0;
  static float Pt = 0;
  static float Pt_update = 0;
  static float Pt_prev = 1;
  static float Kt = 0;

  Xt_update = Xt_prev;
  Pt_update = Pt_prev + m_q;
  Kt = Pt_update/(Pt_update + m_r);
  Xt = Xt_update + (Kt * (SensorData - Xt_update));
  Pt = (1-Kt)*Pt_update;
 
  Xt_prev = Xt;
  Pt_prev = Pt;
 
  return Xt;
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

void HurosWaterMonitoring::TDSInit(int pin, float coef)
{
    m_tdsPin = pin;
    m_tdsCoef = coef;
    this->KFInit();
    pinMode(m_tdsPin, INPUT);
}


float HurosWaterMonitoring::TDSGetData(float temp) {

  static float tdsValue = 0.0f;
  static int analogBufferIndex = 0;
  static float averageVoltage = 0.0f;

  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    m_analogBuffer[analogBufferIndex] = analogRead(m_tdsPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (int copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      m_analogBuffer[copyIndex] = m_analogBuffer[copyIndex];
    averageVoltage = this->GetMedianNum(m_analogBuffer, SCOUNT) * (float)m_espVoltage / m_espADC;                                                                                                                // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);                                                                                                                                   //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                                                //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5 * m_tdsCoef;  //convert voltage value to tds value
  }
  return KFRun(tdsValue);
}

int HurosWaterMonitoring::GetMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
void HurosWaterMonitoring::TDSDFRobotInit(int pin, float tdsFactor, float kVal) 
{
    m_tdsPin = pin;
    m_kVal = kVal;
    m_tdsFactor = tdsFactor;

    pinMode(m_tdsPin,INPUT);
}

float HurosWaterMonitoring::TDSDFRobotGetData(float temp) 
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
