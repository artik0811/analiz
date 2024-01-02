#include <CppLinuxSerial/SerialPort.hpp>
#include <string>
#include <fstream>
#include "math.h"

using namespace mn::CppLinuxSerial;
bool flag = false;
const std::string REQUEST = "scn20 2400000000 2480000000 1000000 200 20 10700000 8500 8\r\n";
float avg_s;//среднекважратичное отклонение
float avg;//среднее значение амплитуды
std::vector <uint8_t> readData;
SerialPort serialPort("/dev/ttyACM0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
std::ofstream out;


float amplitudas(char first_byte, char second_byte)
{
  uint16_t val = first_byte << 8 | second_byte;
  uint16_t data = val & 0b0000011111111111;
  uint16_t index = (val & 0b1111100000000000) >> 11;
  float amplitude = (10.0 * 80.0 - data) / 10.0;
  out << "Index: " << index << "    |    Ampltude: " << amplitude  << std::endl;
  return amplitude;
}

void readFromAnalizator()
{
  while (readData.size()<200)
    {
	    serialPort.ReadBinary(readData);
    }
	serialPort.Close();
}

void expRunningAverageAdaptive(float *amplitude,int size) 
{
  float k;
  for (int i = 0; i != size - 1; ++i)
  {
    if (abs(amplitude[i + 1] - amplitude[i]) > 1.5) 
      k = 0.9;// резкость фильтра зависит от модуля разности значений
    else k = 0.4;
    amplitude[i] += (amplitude[i + 1] - amplitude[i]) * k;
  }
}

void groza()
{
  
}

void avg_sqr(float* amplitude, int size)
{
  avg = 0;
  for (int i = 0; i != size; ++i)
    avg += amplitude[i];
  avg = avg / size; //среднее значение амплитуды
  avg_s = 0;
  for (int i = 0; i != size; ++i)
  {
    avg_s += amplitude[i]*amplitude[i] - 2*amplitude[i]*avg + avg*avg;
  }
  avg_s = sqrt(avg_s/size); // среднеквадратичное отклонение амплитуды
}

void analiz(float *amplitude, int size)
{
  avg_sqr(amplitude,size);
  int n = 0;
  for (int i=0;i!=size;++i)
  {
    if(abs(amplitude[i])-abs(avg)>avg_s)
      n++;
    else if(n>=4)
      groza();
    else
      n = 0;
  }
}

void loop() 
{
  int size;
  int iterator = 0;
  serialPort.Open();
  serialPort.Write(REQUEST);
  readFromAnalizator();
  size = readData.size()/2; //размер массива
  float amplitudes[size];
  for (int i = 24; i < size*2-17; i++)
  {
      amplitudes[iterator] = amplitudas(readData[i], readData[i+1]);
      iterator++;
      i++;
  }
    expRunningAverageAdaptive(amplitudes,size);
    analiz(amplitudes, size);
}
int main() {
  out.open("test.txt");
    loop();
    out.close();
}