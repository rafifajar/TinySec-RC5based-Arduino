#define CE_PIN 9
#define CSN_PIN 10
#include <SPI.h>
#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);

byte address[11] = "SimpleNode";
//byte addresss[11] = "Node";
unsigned long payload = 0;
unsigned long waktu;

String s[14];
String l[2];
String plaintext;
String masukan = "12";
String chipertext;
String paystring;
String IV1 = "12", IV2 = "12";
String IV3, IV4;

int countkey = 1;
int keyrand;
int countIV = 1;

void setup() {
  Serial.begin(9600);
  radio.begin(); // Start up the radio
  radio.openReadingPipe(1, address); // Write to device address 'SimpleNode'
  //radio.openReadingPipe(2, addresss);
  radio.startListening();
}

void loop() {
  radio.read( &payload, sizeof(unsigned long) );
  if (payload != 0) {
    
    if (countIV == 1) {
      IV1 = bin(dec(fullfill0(IV1)));
      IV2 = bin(dec(fullfill0(IV2)));
    }
    countIV ++;
    
    Serial.print("key = ");
    Serial.println(masukan);
    Serial.print("IV = ");
    Serial.println(IV1 + IV2);

    //Serial.println(payload);


    paystring = bin32(payload);

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
    dekripsi(); 
    
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
    
    
  } else {
    //Serial.println("NRF24L01 belum Terdeteksi");
  }
  delay(1000);
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

String bin32 (unsigned long x) {
  int i = 31;
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

String dekripsi () {
  int tmp;
  String ct1, ct2, finalct, ctp1, ctp2;
  int panjang = paystring.length();
  int x = (panjang + 1) / 2;
  int y = panjang + 1;
  ct1 = fullfill0(paystring.substring(0, x));
  ct2 = fullfill0(paystring.substring(x, y));
  ctp1 = (hex(ct1));
  ctp2 = (hex(ct2));
  Serial.print("Ciphertext received : ");
  Serial.println(ctp1 + ctp2);
  ct1 = fullfill0(hex(ct1));
  ct2 = fullfill0(hex(ct2));
  ct1 = fullfill0(bin(dec(ct1)));
  IV3 = ct1;
  ct2 = fullfill0(bin(dec(ct2)));
  IV4 = ct2;
  for (int i = 6; i >= 1; i--) {
    int pengurangan = (dec(hex(ct2))) - (dec(hex(s[(2 * i) + 1])));
    ct2 = fullfill0(bin(pengurangan));
    tmp = dec(hex(ct1)) % 16;
    ct2 = (ct2.substring(ct2.length() - tmp)) + (ct2.substring(0, ct2.length() - tmp));
    ct2 = eksor(ct2, ct1);
    int pengurangan1 = (dec(hex(ct1))) - (dec(hex(s[2 * i])));
    ct1 = fullfill0(bin(pengurangan1));
    tmp = dec(hex(ct2)) % 16;
    ct1 = (ct1.substring(ct1.length() - tmp)) + (ct1.substring(0, ct1.length() - tmp));
    ct1 = eksor(ct1, ct2);
  }
  int last1 = (dec(hex(ct1))) - (dec(hex(s[0])));
  ct1 = fullfill0(bin(last1));
  ct1 = eksor(ct1, IV1);
  int last2 = (dec(hex(ct2))) - (dec(hex(s[1])));
  ct2 = fullfill0(bin(last2));
  ct2 = eksor(ct2, IV2);
  IV1 = IV3;
  IV2 = IV4;
  String finalpt = ct1.substring(8, 16) + ct2.substring(8, 16);
  String hasilpt = hex(finalpt);
  Serial.print("got plain text : ");
  Serial.println(hasilpt);
  Serial.print("waktu :");
  waktu = millis();
  Serial.println(waktu);
  return hasilpt;
}

