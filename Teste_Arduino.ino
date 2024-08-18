#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

// Define as portas dos LEDs
const int ledVermelho = 8;
const int ledVerde = 5;

// Define a porta digital do sensor de Temperatura e Umidade 
const int sensorTu = 2; 

// Define o tipo de sensor DHT
#define DHTTYPE DHT11

// Inicializa o sensor DHT
DHT dht(sensorTu, DHTTYPE);

// Define o limiar de detecção de temperatura 
const float limiarTemperatura = 30.0;  // Valor em graus Celsius

// Network settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address for Ethernet shield
IPAddress ip(169, 254, 48, 199);                      // Static IP address

// Inicializa o servidor na porta 80
EthernetServer server(80);

void setup() {
  // Configura os LEDs como saídas
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  
  // Inicializa a comunicação serial para monitoramento
  Serial.begin(9600);
  
  // Inicializa o sensor DHT
  dht.begin();

  Ethernet.begin(mac, ip);

  // Inicia o servidor
  server.begin();

  // Give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("Ethernet initialized");

  // Exibe o IP atribuído
  Serial.print("Servidor iniciado. Acesse o IP: ");
  Serial.println(Ethernet.localIP());

}

void loop() {
  // Lê a temperatura do sensor
  float temperatura = dht.readTemperature();
  
  // Verifica se as leituras são válidas
  if (isnan(temperatura)) {
    Serial.println("Falha ao ler do sensor DHT11!");
    return;
  }
  
  // Exibe os valores lidos e manda para a porta serial
  Serial.println(temperatura);
  
  // Verifica se a temperatura ou umidade excede o limiar
  if (temperatura >= limiarTemperatura) {
    // Acende o LED vermelho e apaga o verde
    digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);
  } else {
    // Acende o LED verde e apaga o vermelho
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);
  }

  // Connect to the web server:
  // Escuta por clientes
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("Novo cliente conectado");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Se detecta um fim de linha, responde ao cliente
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<h1>CONECTADO</h1>");;
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
  }
  // Fecha a conexão
  client.stop();
  // Pequena pausa para evitar leituras excessivas
  delay(2000);
}