#include <WiFi.h>
#include <PubSubClient.h>

// ----------------------------------------------------------------------
// 1. CONFIGURAÇÕES DE WI-FI E MQTT (MUDAR ESTAS CREDENCIAIS!)
// ----------------------------------------------------------------------
const char* ssid = "iPhone";        // <<<< TROQUE AQUI
const char* password = "kkkkkkkk";    // <<<< TROQUE AQUI

// Broker MQTT Público (HiveMQ)
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_client_id = "ESP32_Monitor_Cardiaco_UNICO_20251129_FINAL";// Tópicos de Publicação
const char* topico_bpm = "monitor/cardiaco/bpm";
const char* topico_ppg_sinal = "monitor/cardiaco/ppg_sinal";

// ----------------------------------------------------------------------
// 2. VARIÁVEIS DO SENSOR (SUA LÓGICA DE PULSO ESTÁVEL)
// ----------------------------------------------------------------------
const int pulsePin = 34;    // Pino de Sinal do Pulse Sensor -> GPIO 34
const int PinoLED = 2;     // LED nativo do ESP32 para indicar o pulso

int THRESHOLD = 2000;       // Ajuste este valor após o teste com o dedo
unsigned long lastBeatTime = 0;
int bpm = 0;
bool wasAbove = false;

// Variáveis para Limitar a Frequência de Publicação MQTT (1 vez por segundo)
unsigned long lastPublishTime = 0; 
const long publishInterval = 1000; // Publicar a cada 1000ms (1 segundo)

// Objetos de Rede
WiFiClient espClient;
PubSubClient client(espClient);

// ----------------------------------------------------------------------
// 3. FUNÇÕES DE SUPORTE
// ----------------------------------------------------------------------

// Função de Filtro (Sua função)
int readFiltered() {
  long soma = 0;
  const int N = 4;
  for (int i = 0; i < N; i++) {
    soma += analogRead(pulsePin);
    delay(5);
  }
  return soma / N;
}

void setup_wifi() {
  // ... (Função para conectar ao WiFi) ...
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // ... (Função para reconectar ao broker MQTT) ...
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tenta novamente em 5 segundos.");
      delay(5000);
    }
  }
}

// ----------------------------------------------------------------------
// 4. SETUP PRINCIPAL
// ----------------------------------------------------------------------
void setup() {
  Serial.begin(115200); 
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);

  // Setup do Sensor e LED
  pinMode(PinoLED, OUTPUT);
  pinMode(pulsePin, INPUT); 
  
  Serial.println("--- SISTEMA MQTT/PULSO PRONTO ---");
  Serial.print("THRESHOLD ATUAL: ");
  Serial.println(THRESHOLD);
}

// ----------------------------------------------------------------------
// 5. LOOP PRINCIPAL
// ----------------------------------------------------------------------
void loop() {
  // 1. Manter conexão MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  // 2. Lógica do Sensor (Sua lógica anterior, roda rápido)
  int sensorValue = readFiltered();
  unsigned long now = millis();

  // Detecção de batimento
  if (sensorValue > THRESHOLD && !wasAbove && (now - lastBeatTime) > 300) {
    unsigned long intervalo = now - lastBeatTime;
    lastBeatTime = now;

    if (intervalo > 300 && intervalo < 2000) { 
      bpm = 60000 / intervalo;
      digitalWrite(PinoLED, HIGH); // Pisca o LED no pico
    }

    wasAbove = true;
  }

  if (sensorValue < THRESHOLD) {
    digitalWrite(PinoLED, LOW); // Apaga o LED
    wasAbove = false;
  }
  
  // 3. PORTÃO DE TEMPO MQTT (Publicar apenas a cada 1 segundo)
  if (now - lastPublishTime >= publishInterval) { 
      
      // A. Publicar Sinal PPG (Sempre útil para o gráfico)
      String ppg_payload = String(sensorValue);
      client.publish(topico_ppg_sinal, ppg_payload.c_str());

      // B. ZERAR BPM (Sua lógica de tolerância)
      if (now - lastBeatTime > 5000) { 
        bpm = 0;
      }

      // C. Publicar BPM (Publica apenas se houver um valor calculado > 30)
      if (bpm > 30) {
        String bpm_payload = String(bpm);
        client.publish(topico_bpm, bpm_payload.c_str());
        Serial.print("BPM publicado: ");
        Serial.println(bpm);
      } else {
         // Publica 0 se o BPM for zero (para dashboard não travar)
        client.publish(topico_bpm, "0");
      }
      
      lastPublishTime = now; // Reinicia o cronômetro de publicação
  }
 
  delay(2); 
}
