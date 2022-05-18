#include "WiFi.h"
#include "AsyncUDP.h"
#include <Wire.h>                                 // * Подключаем библиотеку для работы с аппаратной шиной I2C.
#include <iarduino_I2C_SHT.h>                     //   Подключаем библиотеку для работы с датчиком температуры и влажности I2C-flash (Sensor Humidity and Temperature).
iarduino_I2C_SHT sht;                             //   Объявляем объект sht для работы с функциями и методами библиотеки iarduino_I2C_SHT.
                                                  //   Если при объявлении объекта указать адрес, например, sht(0xBB), то пример будет работать с тем модулем, адрес которого был указан.

const char * ssid = "MGTS_GPON_5779";
const char * password = "Y6CR7FZR";

AsyncUDP udp;

void setup()
{
    delay(500);
    Serial.begin(115200);
    while(!Serial){;}                             // * Ждём завершения инициализации шины UART.
    sht.begin();
    sht.setTemChange( 0.1 );                      //   Указываем фиксировать изменение температуры окружающей среды более чем на 0.1°С.
    sht.setHumChange( 1.0 ); 
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    // 192.168.1.68 20001
    if(udp.connect(IPAddress(192,168,1,68), 20001)) {
        Serial.println("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
        //Send unicast
        udp.print("Hello Server!");
    }
}

void loop()
{
    if( sht.getTemChanged() ){                    //   Если температура окружающей среды изменилась более чем на значение указанное в функции setTemChange(), то ...
        Serial.print("Температура = ");           //
        Serial.print( sht.getTem() );             //   Выводим текущую температуру окружающей среды, от -40 до +125°С.
        String tem = "Temp:" + String(sht.getTem());
        char charBuf[50];
        tem.toCharArray(charBuf, 50);
        udp.broadcastTo(charBuf, 20001);
        Serial.print(" °С.\r\n");                 //
    }                                             //
    if( sht.getHumChanged() ){                    //   Если влажность воздуха изменилась более чем на значение указанное в функции setHumChange(), то ...
        Serial.print("Влажность = ");             //
        Serial.print( sht.getHum() );             //   Выводим текущую температуру окружающей среды, от -40 до +125°С.
        String hum =  "Hum:" + String(sht.getHum());             //   Выводим текущую влажность воздуха, от 0 до 100%.
        char charBuf[50];
        hum.toCharArray(charBuf, 50);
        udp.broadcastTo(charBuf, 20001);
        Serial.print(" %.\r\n");                  //
    }     
    delay(10000);
}
