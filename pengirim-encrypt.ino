#include "RF24.h"

RF24 radio(9, 10);

byte address[11] = "SimpleNode";
byte addresss[11] = "SimpleNode1";
const char payload_length = 32;
byte data[payload_length];
unsigned long payload;
unsigned long waktu;

String s[14];
String l[2];
String plaintext;
String masukan = "12";
String IV1 = "12", IV2 = "12";
String chipertext;

const int sensorpin = A1;
int countkey = 1;
int keyrand;
int countIV = 1;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensorpin, INPUT);
  radio.begin(); // Start up the radio
  radio.openWritingPipe(address);
  radio.openWritingPipe(addresss);// Write to device address 'SimpleNode'
}

void loop() {
  if (Serial.available() != 0){
    String xxxx = Serial.readString();
    if (countIV == 1) {
      IV1 = bin(dec(fullfill0(IV1)));
      IV2 = bin(dec(fullfill0(IV2)));
    }
    countIV++;
    int data = analogRead(sensorpin);
    plaintext = fulldata0(String(data));

    Serial.print("key = ");
    Serial.println(masukan);
    Serial.print("IV = ");
    Serial.println(IV1 + IV2);

    String key = fullfill0(masukan);

    s[0] = bin(dec("B7E1"));
    String maks = bin(dec("9E37"));

    for (int i = 1; i < 14; i++) {
      String datas = Add(s[i - 1], maks);
      s[i] = datas;
    }

    for (int a = 0; a < 2; a++) {
      int start = (1 - a) * 8;
      int batas = ((1 - a) + 1) * 8;
      String datal = bin(dec(key.substring(start, batas)));
      l[a] = datal;
    }

    int i = 0, j = 0;
    String c = "", d = "", tempr, tempr1, tempr2, tempr3;
    for (int count = 0; count < 42; count++) {
      tempr1 = Add(fullfill0(c), fullfill0(d));
      tempr = Add(s[i], tempr1);
      c = tempr.substring(3) + tempr.substring(0, 3);
      s[i] = c;
      tempr2 = Add(fullfill0(c), fullfill0(d));
      tempr3 = Add(l[j], tempr2);
      String hexa = hex(tempr2);
      long le = dec(hexa) % 16;
      int len = le;
      d = tempr3.substring(len) + tempr3.substring(0, len);
      l[j] = d;
      i = (i + 1) % 14;
      j = (j + 1) % 2;
    }
    Serial.println("expand key process ");
    for (int counts = 0; counts < 14; counts++) {
      int keycount = 1;
      Serial.print("key ");
      Serial.print(keycount);
      Serial.print(" = ");
      Serial.println(s[counts]);
      keycount++;
    }
    chipertext = enkripsi();

    Serial.print("Ciphertext send : ");
    Serial.println(chipertext);
    payload = dec(chipertext);
    radio.write( &payload, sizeof(unsigned long) );

    if (countkey == 10) {
      keyrand = random(10, 99);
      masukan = String(keyrand);
      IV1 = String(keyrand);
      IV2 = String(keyrand);
      countkey = 1;
      countIV = 1;
    }
    Serial.print("counter update key and IV : ");
    Serial.println(countkey);
    countkey++;
    delay(1500);
  }
}

String fulldata0 (String x) {
  String nol = get0(4 - x.length());
  String full = nol + x;
  return full;
}

String fullfill0 (String x) {
  String nol = get0(16 - x.length());
  String full = nol + x;
  return full;
}

String get0(int len) {
  String result = "";
  for (int i = 0; i < len; i++) {
    result += "0";
  }
  return result;
}

String Add (String x, String y) {
  String result = "";
  boolean carry = false;
  for (int i = 16 - 1; i >= 0; i--) {
    char a = x.charAt(i);
    char b = y.charAt(i);
    if ((a == b && carry == false) || (a != b && carry == true)) {
      result = "0" + result;
    } else {
      result = "1" + result;
    }
    if ((a == '1' && b == '1') || (a == '1' && b == '1' && carry == true) || (x.charAt(i) != y.charAt(i) && carry == true)) {
      carry = true;
    } else {
      carry = false;
    }
  }
  //Serial.println(result);
  return result;
}

String bin (unsigned long x) {
  int i = 15;
  String result = "";
  while (i >= 0) {
    if ((x >> i) & 1) {
      result += "1";
    } else {
      result += "0";
    }
    --i;
  }
  return result;
}


String hex (String x) {
  String binToHex = "", tmp = "0000";
  for (int j = 0; j < x.length(); j += 4) {
    tmp = x.substring(j, j + 4);
    if      (tmp == "0000") {
      binToHex += "0";
    }
    else if (tmp == "0001") {
      binToHex += "1";
    }
    else if (tmp == "0010") {
      binToHex += "2";
    }
    else if (tmp == "0011") {
      binToHex += "3";
    }
    else if (tmp == "0100") {
      binToHex += "4";
    }
    else if (tmp == "0101") {
      binToHex += "5";
    }
    else if (tmp == "0110") {
      binToHex += "6";
    }
    else if (tmp == "0111") {
      binToHex += "7";
    }
    else if (tmp == "1000") {
      binToHex += "8";
    }
    else if (tmp == "1001") {
      binToHex += "9";
    }
    else if (tmp == "1010") {
      binToHex += "A";
    }
    else if (tmp == "1011") {
      binToHex += "B";
    }
    else if (tmp == "1100") {
      binToHex += "C";
    }
    else if (tmp == "1101") {
      binToHex += "D";
    }
    else if (tmp == "1110") {
      binToHex += "E";
    }
    else if (tmp == "1111") {
      binToHex += "F";
    }
  }
  return binToHex;
}

unsigned long ipow(unsigned long base, unsigned long ex)
{
  unsigned long result = 1;
  while (ex)
  {
    if (ex & 1)
      result *= base;
    ex >>= 1;
    base *= base;
  }

  return result;
}

unsigned long dec (String x) {
  unsigned long proses = 0, pangkat = 0, pengkali = 0, kuadrat = 0;
  String tmp = "";
  int ujung = x.length();
  int potongan = x.length() - 1;
  for (int j = ujung; j >= 0; j -= 1) {
    tmp = x.substring(potongan, ujung);
    potongan -= 1;
    ujung -= 1;
    if      (tmp == "0") {
      pengkali = 0;
    }
    else if (tmp == "1") {
      pengkali = 1;
    }
    else if (tmp == "2") {
      pengkali = 2;
    }
    else if (tmp == "3") {
      pengkali = 3;
    }
    else if (tmp == "4") {
      pengkali = 4;
    }
    else if (tmp == "5") {
      pengkali = 5;
    }
    else if (tmp == "6") {
      pengkali = 6;
    }
    else if (tmp == "7") {
      pengkali = 7;
    }
    else if (tmp == "8") {
      pengkali = 8;
    }
    else if (tmp == "9") {
      pengkali = 9;
    }
    else if (tmp == "A") {
      pengkali = 10;
    }
    else if (tmp == "B") {
      pengkali = 11;
    }
    else if (tmp == "C") {
      pengkali = 12;
    }
    else if (tmp == "D") {
      pengkali = 13;
    }
    else if (tmp == "E") {
      pengkali = 14;
    }
    else if (tmp == "F") {
      pengkali = 15;
    } else {
      pengkali = 0;
    }
    kuadrat = ipow(16, pangkat);
    proses = proses + (pengkali * kuadrat);
    pangkat += 1;
  }
  return proses;
}

String eksor (String x, String y) {
  String result = "";
  for (int i = 0; i < x.length(); i++) {
    if (x.charAt(i) == y.charAt(i)) {
      result += "0";
    } else {
      result += "1";
    }
  }
  return result;
}

String enkripsi () {
  int panjang, x, y, tmp;
  String pt1, pt2, finalpt;
  Serial.print("sensor data as a plaintext : ");
  Serial.println(plaintext);
  panjang = plaintext.length();
  x = (panjang + 1) / 2;
  y = panjang + 1;
  pt1 = fullfill0(plaintext.substring(0, x));
  pt2 = fullfill0(plaintext.substring(x, y));
  pt1 = fullfill0(bin(dec(pt1)));
  pt1 = eksor(pt1, IV1);
  pt2 = fullfill0(bin(dec(pt2)));
  pt2 = eksor(pt2, IV2);
  pt1 = Add(pt1, s[0]);
  pt2 = Add(pt2, s[1]);
  for (int i = 1; i <= 6; i++) {
    tmp = dec(hex(pt2)) % 16;
    pt1 = eksor(pt1, pt2);
    pt1 = pt1.substring(tmp) + pt1.substring(0, tmp);
    pt1 = Add(pt1, s[2 * i]);
    tmp = dec(hex(pt1)) % 16;
    pt2 = eksor(pt2, pt1);
    pt2 = pt2.substring(tmp) + pt2.substring(0, tmp);
    pt2 = Add(pt2, s[(2 * i) + 1]);
  }
  IV1 = pt1;
  IV2 = pt2;
  finalpt = pt1 + pt2;
  String hasilct = hex(finalpt);
  Serial.print("waktu :");
  waktu = millis();
  Serial.println(waktu);
  return hasilct;
}
