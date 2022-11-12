
#include "WiFiEsp.h"
char ssid[] = "Cobadulu";
char pass[] = "rizqi1234";
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  Serial1.begin(9600);
  WiFi.init(&Serial1);
//  if (WiFi.status() == WL_NO_SHIELD) {
//    Serial.println("WiFi Module Tidak ditemukan");
//    while (true);
//  }
  while ( status != WL_CONNECTED) {
    Serial.print("Mencoba Terhubung ke WiFi dengan SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  Serial.print("Kamu Telah Terhubung Ke WiFi: ");
  Serial.println(ssid);
  printWifiStatus();
}

void loop() {

}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("Alamat IP: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Kekuatan Sinyal (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
