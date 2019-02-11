#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN         8
#define SS_PIN          9



unsigned long previousMillis = 0;
const long interval = 5000;
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
  //    Serial.begin(9600);
  gps_ser.begin(9600);
  gsm_ser.begin(9600);
  SPI.begin();

  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  delay(5000);
  connectGPRS();

}


void loop() {




  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    //    connectGPRS();
    nativeCodeGps();
  }
  lcd.setCursor(0, 0);
  lcd.print("scan your id");
  //  RfidScan();
  rfidCode();
}

void sendLatLngAndDateTimeToServer(String latitude, String longitude, String timeFromGps, String dateFromGps) {

  

  gsm_ser.println("AT+HTTPINIT");
  //  myGsm.println("AT+CIFSR"); //init the HTTP request
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPSSL=1");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPPARA=\"CID\",1");
  // myGsm.println("AT+CIPSTART=\"TCP\",\"122.178.80.228\",\"350\"");
  delay(1000);
  //  printSerialData();
  //  delay(5000);
  gsm_ser.println("AT+HTTPPARA=\"URL\",\"warm-thicket-69046.herokuapp.com/live\"");
  // myGsm.println("AT+CIPSEND");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  //sendtemp();
  delay(1000);
  //myGsm.println("AT+CIPCLOSE");
  //  printSerialData();
  String reading = "";
  int indexLat = String(latitude).indexOf(".");
  int indexLong = String(longitude).indexOf(".");
  if (indexLat == 0 || indexLong == 0) {
    reading = "{\"bus_name\":\"HU-02\",  \"lat\" :" + String(0.0000) + ",  \"lng\" :" + String(0.000) + " }";
  } else {
    reading = "{\"bus_name\":\"HU-02\",  \"lat\" :" + String(latitude) + ",  \"lng\" :" + String(longitude) + " }";
  }

  gsm_ser.println("AT+HTTPDATA=" + String(reading.length()) + ",100000");
  //    Serial.println(reading);
  //myGsm.println("AT+CIPSHUT");
  delay(1000);
  //  printSerialData();


  gsm_ser.println(reading);
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPACTION=1");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPREAD");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPTERM");
  delay(1000);
  //  printSerialData();

}

void sendRFIDToServer(String id) {
  gsm_ser.println("AT+HTTPINIT");
  //  myGsm.println("AT+CIFSR"); //init the HTTP request
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPSSL=1");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPPARA=\"CID\",1");
  // myGsm.println("AT+CIPSTART=\"TCP\",\"122.178.80.228\",\"350\"");
  delay(1000);
  //  printSerialData();
  //  delay(5000);
  gsm_ser.println("AT+HTTPPARA=\"URL\",\"warm-thicket-69046.herokuapp.com/attendance\"");
  // myGsm.println("AT+CIPSEND");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  //sendtemp();
  delay(1000);
  //myGsm.println("AT+CIPCLOSE");
  //  printSerialData();
  String reading = "";

  reading = "{\"bus_name\":\"HU-02\",  \"rfid\" :\"" + id + "\" }";



  gsm_ser.println("AT+HTTPDATA=" + String(reading.length()) + ",100000");
  //    Serial.println(reading);
  //myGsm.println("AT+CIPSHUT");
  delay(1000);
  //  printSerialData();


  gsm_ser.println(reading);
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPACTION=1");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPREAD");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+HTTPTERM");
  delay(1000);
  //  printSerialData();


}

void rfidCode() { // RFID Module code
  // Look for new cards
  lcd.clear();
  lcd.print("Scan Your ID");
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    //    Serial.print("new card present");
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    //    Serial.print("purana card present");
    return;
  }
  //Show UID on serial monitor
  //  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //  Serial.println();
  //  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content.substring(1));
  if (content.substring(1) == "60 25 1B 14" || content.substring(1) == "3D 27 08 C2" || content.substring(1) == "3D 8A 43 B2") //change here the UID of the card/cards that you want to give access
  {
    lcd.clear();
    lcd.print("Authorized User");
    delay(500);
    lcd.clear();
    lcd.print("Welcome!");
    delay(500);
    sendRFIDToServer(content.substring(1));
    lcd.clear();
    //    Serial.println("Authorized access");
    //    Serial.println();

  }

  else   {
    //    Serial.println(" Access denied");
    lcd.clear();
    lcd.print("UnAuthorizedUser");
    delay(500);
    lcd.clear();
    //      lcd.print("Welcome!");
    //      delay(1000);
    lcd.clear();
  }
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
      lngfirst = nmea[4].substring(1, i - 2);
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

void connectGPRS() {

  gsm_ser.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+SAPBR=3,1,\"APN\",\"ufone.pinternet \"");//APN
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+SAPBR=1,1");
  delay(1000);
  //  printSerialData();

  gsm_ser.println("AT+SAPBR=2,1");
  delay(1000);
  //  printSerialData();
}
void printSerialData()
{
  while (gsm_ser.available() != 0) {
    Serial.println(gsm_ser.read());
    //    Serial.println("in loop");
  }

}
