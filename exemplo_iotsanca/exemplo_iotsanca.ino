#include <ESP8266WiFi.h>                                                  // Biblioteca utilizada para funções de WiFi

char ssid[] = "Asilo 2.4 GHz";                                            // Insira o SSID do seu WiFi
char pass[] = "reppolter99";                                              // Insira a pass do seu WiFi
String Device_Token = "db05824d-0dfc-47d6-baeb-8a499b36c3d1";             // Insira o token de seu device na plataforma Tago
char server[] = "api.tago.io";                                            // Endereço do servidor da Tago
int serverPort = 80;                                                      // Porta do servidor da tago
int status = WL_IDLE_STATUS;                                              // Variável pra guardar o status do WiFi
WiFiClient client;                                                        // Cliente WiFi utilizado para fazer comunicação com o servidor da tago via http request
int sensorPin = A0;                                                       // Pino analógico para leitura do sensor LM35 ou potênciometro
String value_string = "";                                                 // String de armazenamento do dado que vai ser enviado pra plataforma Tago
unsigned long lastConnectionTime = 0;                                     // Última vez que conectou com o servidor em milisegundos
const unsigned long postingInterval = 2L * 1000L;                         // Delay entre updates, em milisegundos

void setup() {
  Serial.begin(115200);                                                   // Inicia a comunicação serial para acompanhar status da placa
  SetupWifi();                                                            // Chama a rotina de setup do Wifi
}

void loop() {
  if (millis() - lastConnectionTime > postingInterval) {                  // Função para realizar update a cada 2 segundos
    int analogValue = analogRead(sensorPin);                              // Realiza a leitura analógica do pino A0
    float millivolts = (analogValue / 1024.0) * 3300;                     // Transforma a leitura analógica em milivolts
    float celsius = millivolts / 10;                                      // Transforma a tensão encontrada em graus celsius seguindo a lógica do LM35
    value_string = String(celsius);                                       // Armazena o valor de temperatura na variável global
    httpRequest();                                                        // Chama a rotina httpRequest
  }
}

void httpRequest() {
  client.stop();                                                          // Fecha qualquer conexão antes de fazer um novo request
  Serial.println("\Iniciando conexão com o servidor da tago...");
  String PostData = String("{\"variable\":\"temperature\", \"value\":") +
                    String(value_string) + String(",\"unit\":\"C\"}");    // String no formato JSON com o valor de temperatura que vai ser postado no servidor da tago
  String Dev_token = String("Device-Token: ") + String(Device_Token);
  if (client.connect(server, serverPort)) {                               // Inicia conexão com servidor da tago
    Serial.println("Conectado!");
    client.println("POST /data? HTTP/1.1");                               // Inicio da montagem do pacote HTTP
    client.println("Host: api.tago.io");
    client.println("_ssl: false");
    client.println(Dev_token);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);                                             // Fim da montagem do pacote HTTP
    Serial.println("Pacote de dados enviado com sucesso com temperatura: ");
    Serial.println(value_string);
    lastConnectionTime = millis();
  }
  else {
    Serial.println("Conexão falhou");
  }
}

void SetupWifi() {
  Serial.println();
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);                                                    // Seta o modo de funcionamento do WiFi
  WiFi.begin(ssid, pass);                                                 // Conecta no WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
}
