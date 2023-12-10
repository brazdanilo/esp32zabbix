//Trabalho do Curso de Pós-Graduação Lato Sensu em Telecomunicações: Prédios Inteligentes IFG - Instituto Federal de Goiás
//Aluno: Danilo Braz da Silva - danilobrazsilva@gmail.com
//Professor Me. Marcelo Antônio Adad de Araújo
//MONITORAMENTO DE AMBIENTE COM UTILIZAÇÃO DE SENSORES DE TENSÃO E TEMPERATURA E ENVIO DE DADOS PARA SISTEMA ZABBIX REMOTO OU LOCAL 

#include <WiFiManager.h> // Biblioteca para conecão do Wifi - https://github.com/tzapu/WiFiManager
#include <ESP32ZabbixSender.h> //Inclusão da biblioteca de envio dos dados para o Zabbix
#include "EmonLib.h" //Inclusão da biblioteca Sensores de Tensão
#include "DHT.h" //Inclusão da Biblioteca de Sensor de Temperatura

#define SERVERADDR 192, 168, 43, 73 // Endereço Ip do Servidor Zabbix
#define ZABBIXPORT 10051            //Porta Zabbix
#define ZABBIXAGHOST "ESP32"        //Zabbix item's host name

#define CALIB_SENSOR 234.3 //Valor de calibração
#define PINTENSAO1 34 //Pino do sensor de tensão
#define DHTPIN 32     // Pino do sensor de temperatura

#define DHTTYPE DHT11   // Defino o modelo do sensor de temperatura, DHT11, DHT22
DHT dht(DHTPIN, DHTTYPE);

ESP32ZabbixSender zSender; //Cria instância da biblioteca ZabbixSender.
EnergyMonitor emon1; //Cria uma instância na biblioteca EmonLib



WiFiManager wm; //cria uma instância para a biblioteca WiFiManager
     bool res; //cria uma váriavel para utilizar com WifiManager
void setup() {
  Serial.begin(115200);
    WiFiManager wm; //cria uma instância para a biblioteca WiFiManager
    res = wm.autoConnect("ESP32","monitoramento"); //Caso não conecte em alguma rede é criado uma Ponto de acesso com Nome e senha para realizar a primeira conexão
    if(!res) {
        Serial.println("Failed to connect");
         ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("CONECTADO)");
    }
  zSender.Init(IPAddress(SERVERADDR), ZABBIXPORT, ZABBIXAGHOST); // Informaçãos de inicialização do Servidor Zabbix
  emon1.voltage(PINTENSAO1, CALIB_SENSOR, 1.7); //Parâmetros (Pino analógico, Valor da Calibração, Mudança de Fase) Sensor de tensão
  dht.begin(); //inicialização do sensor de temperatura
}

void loop() {
  
  emon1.calcVI(20,2000);// Calculo RMS, quantidade de ciclo
  tensao1  = emon1.Vrms; //Variável que recebe o valor de tensão
  umidade = dht.readHumidity(); //Leitura do umidade
  temperatura = dht.readTemperature(); //Leitura da temperatura em Celsius
       
  zSender.ClearItem(); //Limpa o valor enviado anterior    
    zSender.ClearItem();    
  
  //if utilizado para não enviar as primeiras 10 leituras do sensor de tensão, devido a um instabilidade na inicilização do ESP32
  if (i<10)
  {
    i++;
   }
  else 
  {
  zSender.AddItem("tensao1", tensao1); //envia o valor da tensão para o zabbix
  }
  
  zSender.AddItem("temperatura", temperatura); //envia o valor da temperatura para o zabbix
  zSender.AddItem("umidade", umidade); //envia o valor da umidade para o zabbix

  //Verifica se foi enviado com sucesso
  if (zSender.Send() == EXIT_SUCCESS) {
    Serial.println("ZABBIX SEND: OK");
  } else {
    Serial.println("ZABBIX SEND: NG");
  }

  //Imprimir valores
  Serial.println(tensao1);
  Serial.println(temperatura);
  Serial.println(umidade);
  delay(500);
}
