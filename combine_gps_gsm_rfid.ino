#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN         8
#define SS_PIN          9



unsigned long previousMillis = 0;
const long interval = 12000;
int RfidNo = 0;
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
String nmea[15];
const int rs = 14, en = 15, d4 = 4, d5 = 3, d6 = 2, d7 = 1;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// The serial connection to the GPS device
SoftwareSerial gps_ser(10, 11);//Rx,Tx
// The serial connection to the GSM device
SoftwareSerial gsm_ser(6, 7); //Rx,Tx
MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {

  //lcd.print("hello world");
  //  Serial.begin(9600);
  gps_ser.begin(9600);
  gsm_ser.begin(9600);
  SPI.begin();

  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  // delay(30000);


}


void loop() {




  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    nativeCodeGps();
  }
  lcd.setCursor(0, 0);
  lcd.print("scan your id");
  RfidScan();
}

void sendLatLngAndDateTimeToServer(String latitude, String longitude, String timeFromGps, String dateFromGps) {

  //  Serial.println("writing message");
  gsm_ser.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gsm_ser.println("AT+CMGS=\"+923362602053\"\r"); // Replace x with mobile number
  delay(1000);
  gsm_ser.println(String(latitude) + " " + String(longitude) + " " + timeFromGps + " " + dateFromGps); // The SMS text you want to send
  delay(100);
  gsm_ser.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void sendRFIDToServer(String id) {
  //  Serial.println("writing message");
  gsm_ser.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gsm_ser.println("AT+CMGS=\"+923362602053\"\r"); // Replace x with mobile number
  delay(1000);
  gsm_ser.println("RFID: " + id);
  delay(100);
  gsm_ser.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
//void workOnGps() {
//  Serial.println("entered in work on gps method");
//  while (gps_ser.available() > 0)
//    if (gps.encode(gps_ser.read())) {
//      Serial.println("sending Info");
//      sendLatLngAndDateTimeToServer(getLat(),getLng(),getTime(),getDate());
//      Serial.println("Info sent");
//
//    }
//
//
//  if (millis() > 5000 && gps.charsProcessed() < 10)
//  {
//    Serial.println(F("No GPS detected: check wiring."));
//    while (true);
//  }
//  Serial.println("exit from work on gps method");
//
//}
void RfidScan()
{

  if ( ! mfrc522.PICC_IsNewCardPresent()) {

    return;
  }


  if ( ! mfrc522.PICC_ReadCardSerial()) {

    return;
  }

  dumpByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);
  delay(1000);
}
void dumpByteArray(byte *buffer, byte bufferSize) {
  //  Serial.print("~");
  if (buffer[0] == 194) {
    RfidNo = 1;
    //    Serial.print("valid user");
    lcd.clear();
    lcd.print("Authorised");
    sendRFIDToServer(String(*buffer));
  }
  else if (buffer[0] == 61) {
    RfidNo = 2;
    //    Serial.print("valid user");
    lcd.clear();
    lcd.print("Authorised");
    sendRFIDToServer(String(*buffer));
  }
  else {
    lcd.clear();
    lcd.print("invalid user");
    //    Serial.print("not valid user");
  }

  //  Serial.print("!");
}
void nativeCodeGps() {
  //  Serial.flush();
  gps_ser.flush();
  while (gps_ser.available() > 0)
  {
    gps_ser.read();

  }
  if (gps_ser.find("$GPRMC,")) {
    String tempMsg = gps_ser.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    //    for (int i = 0; i < 9; i++) {
    //      Serial.print(labels[i]);
    //      Serial.print(nmea[i]);
    //      Serial.println("");
    //    }
    sendLatLngAndDateTimeToServer(nmea[2], nmea[4], nmea[0], nmea[8]);
  }
  else {

    failedUpdates++;

  }
  stringplace = 0;
  pos = 0;
  //  delay(12000);
}



String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);

    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}



