#include <Arduino.h>
#include <Servo.h> // Подключаем библиотеку для сервоприводов

const int Trig = 8;
const int Echo = 9;
const int ledPin = A0; // 13 – если будете использовать встроенный в Arduino светодиод
//##### SERVO START #####################
// Создаем объект для сервопривода
Servo vservo;
// Массив для хранения углов поворота сервопривода (шаг 15 градусов)
const int vservo_array[13] = {
    0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180};
// Массив для хранения данных о расстоянии под различными углами поворота сервопривода
int vHC_SR04_array[13];
// Пины, используемые ультразвуковым дальномером
const int vTrig = 8;
const int vEcho = 9;
// Переменные, для хранения данных с дальномера
unsigned int vtime_us = 0;
unsigned int vdistance_sm = 0;
// Минимальное расстояние в сантиметрах, при котором нужно искать новый маршрут движения
const int vmindistance = 30;
// Переменная для циклов перебора значения массивов vservo_array и vHC_SR04_array
int vservo_int;
// Переменные для цикла поиска максимального значения в массивах
int vmaxarrayindex_int;
int vmaxarrayvalue_int;
/* Функция определения расстояния с дальномера */
/*
int getDistance()
{
  unsigned int time_us = 0;
  unsigned int distance_sm = 0;
  digitalWrite(Trig, HIGH);      // Подаем сигнал на выход микроконтроллера
  delayMicroseconds(10);         // Удерживаем 10 микросекунд
  digitalWrite(Trig, LOW);       // Затем убираем
  time_us = pulseIn(Echo, HIGH); // Замеряем длину импульса
  distance_sm = time_us / 58;    // Пересчитываем в сантиметры
  Serial.println(distance_sm);   // Выводим на порт

  if (distance_sm < 50) // Если расстояние менее 50 сантиметром
  {
    digitalWrite(ledPin, 1); // Зажигаем светодиод
  }
  else
  {
    digitalWrite(ledPin, 0); // иначе тушим
  }
  return distance_sm;
}
*/
int vHC_SR04()
{
  //return getDistance();
  digitalWrite(vTrig, HIGH);       // Подаем сигнал на выход микроконтроллера
  delayMicroseconds(10);           // Удерживаем 10 микросекунд
  digitalWrite(vTrig, LOW);        // Затем убираем
  vtime_us = pulseIn(vEcho, HIGH); // Замеряем длину импульса
  vdistance_sm = vtime_us / 58;    // Пересчитываем в сантиметры
 // Serial.println("Distance : " + String(vdistance_sm));
  return vdistance_sm;             // Возвращаем значение
}
#define SERVO_PIN 10
void initServo()
{
  // Выбираем пин к которому подключен сервопривод
  vservo.attach(SERVO_PIN); // или 10, если воткнули в крайний разъём
  // Поворачиваем сервопривод в положение 90 градусов при каждом включении
  vservo.write(90);
}
/* Режим работы с использованием ультразвукового дальномера */
void vultrasoundmode()
{
  vservo.write(90);
  delay(200);
  Serial.print("Now ");
  Serial.println(vHC_SR04());
  // Если расстояние меньше наименьшего, то
  if (vHC_SR04() < vmindistance)
  {
    digitalWrite(ledPin, HIGH);
    // Останавливаем двигатели
    Serial.println("vrelease");
    // Крутим серву измеряя расстояния и занося данные в массив
    for (vservo_int = 0; vservo_int < 13; vservo_int = vservo_int + 1)
    {
      vservo.write(vservo_array[vservo_int]);
      delay(200);
      vHC_SR04_array[vservo_int] = vHC_SR04();
      // Выводим данные для отладки
      Serial.print(vservo_int);
      Serial.print(" ");
      Serial.println(vHC_SR04_array[vservo_int]);
    }
    vservo.write(90);
    delay(500);
    // Поиск в массиве позиции с максимальным значением
    vmaxarrayindex_int = 0;
    vmaxarrayvalue_int = 0;
    for (vservo_int = 0; vservo_int < 13; vservo_int = vservo_int + 1)
    {
      if (vHC_SR04_array[vservo_int] > vmaxarrayvalue_int)
      {
        vmaxarrayindex_int = vservo_int;
        vmaxarrayvalue_int = vHC_SR04_array[vservo_int];
      }
    }
    Serial.print("Max index ");
    Serial.println(vmaxarrayindex_int);
    // Проверка - если максимальное значение массива меньше минимального расстояния, то едем назад
    if (vHC_SR04_array[vmaxarrayindex_int] < vmindistance)
    {
      Serial.println("vbackward");
      delay(500);
    }
    /* Проверка - если индекс максимального значения массива меньше 6 то поворачиваем вправо,
иначе влево */
    if (vmaxarrayindex_int < 6)
    {
      Serial.println("vright");
      delay(500);
    }
    else
    {
      Serial.println("vleft");
      delay(500);
    }
  }
  else
  {
  digitalWrite(ledPin, LOW);

    // Едем прямо
    Serial.println("vforward");
  }
}

//##### SERVO END #####################

void setup()
{
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  initServo();
}

void loop()
{
  // Режим работы с использованием ультразвукового дальномера
  vultrasoundmode();
 // getDistance();

  delay(100);
}