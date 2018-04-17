//Including library
#include <Wire.h> //I2C communication.
#include <SPI.h>
#include "SdFat.h" //New SD library.
#include <LiquidCrystal.h> //LCD library
#include "RTClib.h" //RTC library
#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11

SdFat SD;
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(9,8,5,4,3,2); //The lcd is connected to pins 9, 8, 5, 4, 3, 2. We are creating an object called lcd of the class LiquidCrystal.
RTC_DS3231 RTC; //We are creating an object named RTC of the class RTC_DS3231

byte first_run = 1;
float theta_sum[2] = {0,0};
float theta_first[2] = {0,0};
float phi_sum = 0;
unsigned int first_step; //MINUTES.
unsigned int counter = 1;
const byte chipSelect = 10;

//This is a function to print date and time.
void printdate(DateTime date)
{
  Serial.print(date.day(), DEC);
  Serial.print('/');
  Serial.print(date.month(), DEC);
  Serial.print('/');
  Serial.print(date.year(), DEC);
  Serial.print(' ');
  Serial.print(date.hour(), DEC);
  Serial.print(':');
  Serial.print(date.minute(), DEC);
  Serial.print(':');
  Serial.print(date.second(), DEC);
  Serial.println();
}


//Inside setup function we just set all module and check them out.
void setup() 
{
  bool set_time = 0;
  
  analogReference(INTERNAL);
  Serial.begin(9600); //We are opening the serial communication.
  delay(1000);
  Wire.begin();
  RTC.begin();
  File myFile; //We are creating an object called myFile of the class File.
  dht.begin();

  //RTC's section
  if (!RTC.begin())
  {
    Serial.println("Problem with RTC module.");
    while(1);
  }

  if (RTC.lostPower())
  {
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (set_time)
  {
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //LCD's section
  lcd.begin(16, 2); //row and cols number.
  lcd.clear();
  lcd.setCursor(0, 0); //We are setting cursor in the right place.
  lcd.print("T1: ");
  lcd.setCursor(10, 0);
  lcd.print("\337C"); //\337C is used to write °C. 
  lcd.setCursor(0, 1);
  lcd.print("T2: ");
  lcd.setCursor(10, 1);
  lcd.print("\337C");

  //SD's section
  if (!SD.begin(chipSelect))
  {
    Serial.println("Problem with SD card: absent or broken.");
  }
  else
  {
    //File initialization
    if (!SD.exists("log1.txt")) //if it doesn't exist create it.
    {
      myFile = SD.open("log1.txt", FILE_WRITE);
      
      if (myFile)
      {
        Serial.println("I'm writing to log1.txt.");
        myFile.close();
      }
      /*else
      {
        Serial.println("I'm sorry but I couldn't create the file.");
      }
      */
    }
  }

}

void loop() 
{
  int k, h, j;
  int time_step = 1; //MINUTES. You are going to register measurement on the SD card every time_step minutes.
  int integration_time = 10; //SECONDS. We are going to update measurement on the LCD display every integration_time seconds.
  int exit_flag = 0;
  float stop_time = 0.5; //SECONDS. We have to wait 0.5 seconds between 2 time update, in this way we can register measurements correctly.
  float integration_time2;
  float phi = 0;
  float D7;
  int A0, A1;
  float theta[2] = {0,0};//Every loop we need to empty theta.
  unsigned int sensor0 = 0; //LM35 connected pin A0
  unsigned int sensor1 = 1; //LM 35 colnnected to pin A1
  DateTime now, next;
  File myFile; //We are creating again the object myFile of the class File because we did it in the setup function, not in loop.

  /*
   * Vogliamo che il programma capisca in che momento della giornata si trova: l'obiettivo è infatti quello di registrare sulla scheda SD
   * ogni volta che si raggiunge un multiplo di time_step, non ogni time_step.
   * Pare dunque ovvio che la prima volta sia necessario capire quanto manca alla prossima ora che sia multiplo di time_step.
   * Per farlo è necessario definire i minuti che mancano alla prossima scrittura, così da poter definire il formato ora next che rappresenta
   * il momento della prossima scrittura (preciso al secondo, ovvero allo scoccare del minuto).
   */
  
  now = RTC.now();
  first_step = time_step - now.minute()%time_step;
  next = now + TimeSpan(0, 0, first_step, -now.second());
  
  integration_time2 = integration_time/stop_time; //How many time do we need to do the for cycle? If we are updating time every 0.5 seconds and we are waiting integration_time seconds between two measurements, we have to do it for integration_time/0.5 times.

  if (first_run == 1) //It works just the first time.
  {
    first_run = 0; //Obviously we don't want to enter again in this part of the code.
    printdate(now);
    printdate(next);
   
    if ((now.second()%integration_time) == 0)
    {
      delay(1000);
      now = RTC.now();
    }

    for (h = 0; h < 20; h++)
    {
      A0 = analogRead(sensor0);
      theta_first[0] += A0;
    }
    theta_first[0] = theta_first[0]/9.31/20;

    for (h = 0; h < 20; h++)
    {
      A1 = analogRead(sensor1);
      theta_first[1] += A1;
    }
    theta_first[1] = theta_first[1]/9.31/20;

    D7 = dht.readHumidity();
    Serial.println(D7);

    lcd.setCursor(4, 0);
    lcd.print(theta_first[0], 2);
    lcd.setCursor(4, 1);
    lcd.print(theta_first[1], 2);
  }

  else
  {
    printdate(now);
    printdate(next);
    
    if ((now.second()%integration_time) == 0)
    {
      //Serial.println("Problem!!!!");
      delay(1000);
      now = RTC.now(); //A second is gone, so we need to update time!
      //printdate(now);
      //printdate(next);
    }
  }

  //While section
  while (j < integration_time2)
  {
    if (now.minute() == next.minute() && now.second() == next.second())
    {
      j = integration_time + 2;
      Serial.println("Registering on the SD card.");
      myFile = SD.open("log1.txt", FILE_WRITE);
      Serial.println(myFile);

      if (myFile)
      {
        myFile.print(now.day(), DEC);
        myFile.print('/');
        myFile.print(now.month(), DEC);
        myFile.print('/');
        myFile.print(now.year(), DEC);
        myFile.print(" ; ");
        myFile.print(now.hour(), DEC);
        myFile.print(':');
        myFile.print(now.minute(), DEC);
        myFile.print(':');
        myFile.print(now.second(), DEC);
        myFile.print(" ; ");

        myFile.print(theta_sum[0]/(counter-1));
        myFile.print(" ; ");
        myFile.print(theta_sum[1]/(counter-1));
        myFile.print(" ; ");
        myFile.print(phi_sum/(counter-1));
        myFile.print(" ; ");
        myFile.println(counter);
        myFile.close();

        //printdate(now);
        //printdate(next);

        theta_sum[0] = 0;
        theta_sum[1] = 0;
        phi_sum = 0;
        counter = 1;
        exit_flag = 0;
        now = RTC.now();
        next = now + TimeSpan(0, 0, time_step, -now.second());
        delay (1000);
      }

      /*else
      {
        Serial.println("Error opening file.");
        Serial.println(myFile);
      }
      */
    }
    
    else
    {
      j += 1;
      now = RTC.now();
      //printdate(now);
      //printdate(next);
      //Serial.println(counter);
      //Serial.println(j);

      delay(stop_time*1000);

      if (j == integration_time2)
      {
        exit_flag = 1;
      }

      else
      {
        exit_flag == 1;
      }
    }
  }

  if (exit_flag == 1)
  {
    Serial.println("Reading temperature:  ");

    for(k = 0; k < 20; k++)
    {
      A0 = analogRead(sensor0);
      theta[0] += A0;
    }
    theta[0] = theta[0]/9.31/20;
    theta_sum[0] += theta[0];

    for (k = 0; k < 20; k++)
    {
      A1 = analogRead(sensor1);
      theta[1] += A1;
    }
    theta[1] = theta[1]/9.31/20;
    theta_sum[1] += theta[1];

    for (k = 0; k < 20; k++)
    {
      D7 = dht.readHumidity();
      phi += D7;
    }
    phi = phi/20;
    phi_sum += phi;

    Serial.print("   * ");
    Serial.print(theta[0]);
    Serial.print("  ;  ");
    Serial.print(theta_sum[0]/counter);
    //Serial.println(" Current/Average (deg Celsius, sensor 0)");
    Serial.print("   * ");
    Serial.print(theta[1]);
    Serial.print("  ;  ");
    Serial.print(theta_sum[1]/counter);
    //Serial.println(" Current/Average (deg Celsius, sensor 1)");
    Serial.print("   * ");
    Serial.print(phi);
    Serial.print("  ;  ");
    Serial.println(phi_sum/counter);

    lcd.setCursor(4, 0);
    lcd.print(theta_sum[0]/counter, 2);
    lcd.setCursor(4, 1);
    lcd.print(theta_sum[1]/counter, 2);

    counter += 1;
  }
}
