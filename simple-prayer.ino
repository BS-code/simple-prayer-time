#include <RTClib.h>
#include <Keypad.h>
#include <U8glib.h>
#include <PrayerTimes.h>

RTC_DS1307 rtc;
const byte ROWS = 4; //jumlah baris keypad
const byte COLS = 4; //jumlah kolom keypad
char keys[ROWS][COLS] = { //matriks keypad
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = { 9, 8, 7, 6 }; //pin baris keypad
byte colPins[COLS] = { 5, 4, 3, 2 }; //pin kolom keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE); // SCK, MOSI, CS, A0

PrayerTimes prayers;

void setup () {
  Serial.begin(9600);
  while (!Serial); //tunggu koneksi serial
  if (!rtc.begin()) {
    Serial.println("Tidak ada modul RTC yang terdeteksi");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, harap atur waktu!");
  }
  Serial.println("Tekan tombol A untuk mengatur waktu dan tanggal");
  u8g.setFont(u8g_font_unifont);
}

void draw() {
  u8g.drawStr(0, 12, "Jadwal Sholat:");
  DateTime now = rtc.now();
  char buffer[10];
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  u8g.drawStr(0, 36, buffer);

  // Hitung dan tampilkan jadwal sholat
  DateTime currentDate = rtc.now();
  prayers.setDate(currentDate.year(), currentDate.month(), currentDate.day());

  u8g.drawStr(0, 60, "Subuh:");
  u8g.drawStr(60, 60, prayers.formatTime(prayers.getFajr()));

  u8g.drawStr(0, 48, "Dzuhur:");
  u8g.drawStr(60, 48, prayers.formatTime(prayers.getDhuhr()));

  u8g.drawStr(0, 36, "Ashar:");
  u8g.drawStr(60, 36, prayers.formatTime(prayers.getAsr()));

  u8g.drawStr(0, 24, "Maghrib:");
  u8g.drawStr(60, 24, prayers.formatTime(prayers.getMaghrib()));

  u8g.drawStr(0, 12, "Isya:");
  u8g.drawStr(60, 12, prayers.formatTime(prayers.getIsha()));
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'A': // tombol A untuk mengatur waktu dan tanggal
        Serial.println("Mengatur waktu dan tanggal...");
        setRTCDateTime();
        Serial.println("Waktu dan tanggal telah diatur!");
        delay(1000);
        break;
      default:
        break;
    }

  }

  // Menggambar tampilan LCD
  u8g.firstPage();
  do {
    draw();
  } while (u8g.nextPage());

  delay(1000);
}

void setRTCDateTime() {
  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  Serial.print("Sekarang: ");
  Serial.print(year);
  Serial.print("-");
  Serial.print(month);
  Serial.print("-");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  Serial.println("Masukkan tahun (YYYY): ");
  year = readNumber(9999, 0);
  Serial.println("Masukkan bulan (MM): ");
  month = readNumber(12, 1);
  Serial.println("Masukkan hari (DD): ");
  day = readNumber(31, 1);
  Serial.println("Masukkan jam (HH): ");
  hour = readNumber(23, 0);
  Serial.println("Masukkan menit (MM): ");
  minute = readNumber(59, 0);
  Serial.println("Masukkan detik (SS): ");
  second = readNumber(59, 0);

  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

int readNumber(int maxValue, int minValue) {
  int value = 0;
  char buffer[5];
  int index = 0;

  while (1) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        value = atoi(buffer);
        if (value >= minValue && value <= maxValue) {
          Serial.println(value);
          break;
        } else {
          Serial.println("Input tidak valid!");
          index = 0;
        }
      } else if (key == '*') {
        Serial.println("Batal");
        break;
      } else {
        buffer[index++] = key;
        buffer[index] = '\0';
        Serial.print(key);
      }
    }
  }

  return value;
}
