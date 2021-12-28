#include <EncButton.h>
#include <LiquidCrystal_I2C.h>
#include <iarduino_RTC.h>
#include <GyverPower.h>
#include <powerConstants.h>
#include <Servo.h>

int sunset = 6;// gradusi
int sunrise = 7;// можно оставить только один

int relay_pin = 2;// digital 2
int servo_x_pin = 5;// digital 5
int servo_y_pin = 6;
const int buttonSettingPin = 8;
const int buttonPlusPin = 7;
const int buttonInerruptPin = 3;
const double LON = 27.5;// !!!!!!!!!!!!!! Minsk +27,5
const double lat = 53.91; // Minsk lat = 53,91
const int GTM = 3; // Minsk +3 то же самое, что и время по Гринвичу&&&&&

iarduino_RTC watch(RTC_DS1302, 11, 13, 12); //rst clk dat
LiquidCrystal_I2C lcd(0x27, 16, 2);
EncButton<EB_TICK, buttonSettingPin> buttonSetting;
EncButton<EB_TICK, buttonPlusPin> buttonPlus;
EncButton<EB_TICK, buttonInerruptPin> buttonInerrupt;
Servo servo_x;
Servo servo_y;

int yr = 2021, mo = 6, da = 7, h = 9, m = 58, s = 0;
int azimuth = 0;
int a_ltitude = 0;
double jd = 0;
double garbage = 0;

boolean relay_flag;

int buttonEnterCount = 0;
boolean flagSettings = false;
volatile boolean flagInterrupt = false;
int garbageCount = 0;

int servo_x_val = 90;
int servo_y_val = 90;

void setup() {
  Serial.begin(9600);
  servo_x.attach(servo_x_pin);
  servo_y.attach(servo_y_pin);
  pinMode(relay_pin, OUTPUT);
  //watch.begin();
  //watch.settime(0, 58, 9, 8, 6, 2021, -1);
  buttonSetting.setHoldTimeout (2000);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(1, myInterrupt, FALLING);
  // put your setup code here, to run once:
  
  lcd.init();
    lcd.backlight();
    lcd.home();
}

void loop() {
  if (flagInterrupt) {
    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.clear();
    lcd.print("interapt");
    lcd.setCursor(0, 1);
    lcd.print("Enter setting");
    settingsTime();
    flagInterrupt = false;
  }
  // put your main code here, to run repeatedly:
  tern_servo();
}

void MyClock() {
  m += 20;
  if(m >= 60) {
    h++;
    m = 0;
  } 
  if(h >= 24) {
    da++;
    h = 0;
  }
  if(da >= 31) {
    mo++;
    da = 0;
  }
  if(mo >= 13) yr++;
}

void tern_servo() {
  MyClock();
  printInfoOnLSD();
  sun_azimuth_altitude(yr, mo, da, h, m, s);

  
  if (a_ltitude < sunset && a_ltitude < sunrise) { // night

    power.sleepDelay(1000);// 3600000
  } else {
    relay_flag = true;
    digitalWrite(relay_pin, relay_flag);
    servo_x.write(azimuth);
    servo_y.write(a_ltitude);
    Serial.println("im here");
    relay_flag = !relay_flag;
    delay(500);
    digitalWrite(relay_pin, relay_flag);
    
    power.sleepDelay(1000);//300000
  }
}

void myInterrupt() {
  flagInterrupt = true;
  power.wakeUp();
}

void settingsTime() {
  unsigned long timing = millis();
  while (true) {
    if (millis() - timing > 100000) { // Вместо 10000 подставьте нужное вам значение паузы
      timing = millis();
      lcd.clear();
      lcd.print("Return to work");
      delay(1000);
      break;
    }
    buttonSetting.tick();
    buttonPlus.tick();
    if (buttonSetting.held()) {
      lcd.init();
      lcd.backlight();
      lcd.home();
      flagSettings = !flagSettings;
      lcd.print("Setting ");
      Serial.print("held");
      if (!flagSettings) {
        lcd.print("end");
        Serial.print(yr);
        watch.settime(s, m, h, da, mo, yr - 2000, -1);
        break;
      } else {
        lcd.print("strart");
        delay(500);// millis
        printTimeOnLSD();
      }
    }
    if (buttonSetting.click()) {
      buttonEnterCount++;

      if (buttonEnterCount == 6) {
        buttonEnterCount = 0;
      }
    }
    if (buttonPlus.click()) {
      switch (buttonEnterCount) {
        case 0:
          da++;
          if (da == 32) da = 1;
          break;
        case 1:
          mo++;
          if (mo == 13) mo = 1;
          break;
        case 2:
          yr++;
          if (yr == 2040 || yr < 2021) yr = 2021;
          break;
        case 3:
          h++;
          if (h == 24) h = 0;
          break;
        case 4:
          m++;
          if (m == 60) m = 0;
          break;
        case 5:
          s++;
          if (s == 60) s = 0;
          break;
      }
      printTimeOnLSD();
    }
  }
}

void printTimeOnLSD() {
  lcd.clear();
  lcd.print(da); lcd.print("."); lcd.print(mo); lcd.print("."); lcd.print(yr);
  lcd.setCursor(0, 1);
  lcd.print(h); lcd.print(":"); lcd.print(m); lcd.print(":"); lcd.print(s);
}
void printInfoOnLSD() {
  lcd.clear();
  lcd.print(da); lcd.print(mo); lcd.print(yr -2000); lcd.print(" ");
  lcd.print(h); lcd.print(":"); lcd.print(m); lcd.print(":"); lcd.print(s);
  lcd.setCursor(0, 1);
  
  lcd.print("al: "); lcd.print(a_ltitude); lcd.print(" az: "); lcd.print(azimuth);
}

void get_time() {
  watch.gettime();
  yr = 2000 + watch.year;
  mo = watch.month;
  da = watch.day;
  h = watch.Hours;
  m = watch.minutes;
  s = watch.seconds;
}

double de(int yr, int mo, int da, int h, int m, int sec) {
  Serial.print("sum = ");
  int sum = 367 * (-1990 + yr) - 200;//11177
  Serial.println(sum);
  sum += -(int)(7 * (yr + (int)((mo + 9) / 12.0)) / 4) + (int)(275 * mo / 9.0) + da;
  return (double)sum + ((sec / 60.0 + m) / 60.0 + h) / 24.0;
}

double rev(double x) {
  return x  - floor(x / 360.0) * 360.0;
}
void sun_azimuth_altitude(int yr, int mo, int da, int h, int m, int s) {
  double d = de(yr, mo, da,  h, m, s); //2459368.29167;//JD(2021, 6,2, 19, 00, 00);//

  Serial.print(" d: ");
  Serial.println(d);

  double w = 282.9404 + 0.004709 * d * 0.01;   // (longitude of perihelion)
  double a = 1.0;                    //           (mean distance, a.u.)
  double e = 0.016709 - (0.000115 * d) * 1E-5;
  double M = rev(356.0470 + 0.985600 * d);  //  (mean anomaly)
  double oblecl = 23.4393 - (3.563E-4 * d) * 1E-3; //obliquity of the ecliptic
  double L = rev(w + M); // he Sun's mean longitude
  double E = M + (180 / PI) * e * sin (M * PI / 180) * (1 + e * cos (M * PI / 180));
  double x = cos (E * PI / 180) - e;
  double y = sin (E * PI / 180) * sqrt (1 - e * e);
  double r = sqrt (x * x + y * y);
  double v = atan2 (y, x) * 180 / PI;
  double lon = rev(v + w);
  double X = r * cos (lon * PI / 180);
  double Y = r * sin (lon * PI / 180);
  double Z = 0.0;
  double xequat = X;
  double yequat = Y * cos (oblecl * PI / 180) - Z * sin (oblecl * PI / 180);
  double zequat = Y * sin (oblecl * PI / 180) + Z * cos (oblecl * PI / 180);
  r = sqrt (xequat * xequat + yequat * yequat + zequat * zequat);
  double RA = atan2 (yequat, xequat) * 180 / PI;
  double Decl = atan2 (zequat, sqrt (xequat * xequat + yequat * yequat)) * 180 / PI;
  double GMST0 = rev(L + 180) / 15;

  double UT = h + (double)m / 60 + (double)s / 3600 - GTM;

  double SIDTIME = GMST0 + UT + LON / 15;
  double HA = SIDTIME * 15 - RA;
  x = cos (HA * PI / 180) * cos (Decl * PI / 180);
  y = sin (HA * PI / 180) * cos (Decl * PI / 180);
  double z = sin (Decl * PI / 180);

  double xhor = x * sin(lat * PI / 180) - z * cos(lat * PI / 180);
  double yhor = y;
  double zhor = x * cos(lat * PI / 180) + z * sin(lat * PI / 180);

  azimuth  = round(atan2( yhor, xhor ) * 180 / PI + 180);
  a_ltitude  = round(atan2( zhor, sqrt(xhor * xhor + yhor * yhor) ) * 180 / PI);

}
