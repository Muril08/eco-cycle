// Definicao dos Pinos
const int sensorUmidade = A0;
const int sensorGas = A1;
const int sensorTemperatura = A2;  // Pino do sensor de temperatura
const int pinoRele = 8;

// --- CONFIGURACAO DO RELE ---
const int LIGAR_RELE = HIGH;    // Estado para LIGAR a ventoinha
const int DESLIGAR_RELE = LOW;  // Estado para DESLIGAR a ventoinha

// --- CALIBRACAO DOS SENSORES ---
const int VALOR_SENSOR_SECO = 595;   // Valor lido com o sensor totalmente no ar
const int VALOR_SENSOR_MOLHADO = 260; // Valor lido com o sensor mergulhado na água

// --- CONFIGURACAO DOS LIMITES ---
const int UMIDADE_LIMITE_ALTA = 80;  // Liga a ventoinha se umidade > 80%
const int UMIDADE_LIMITE_BAIXA = 70; // Faixa ideal: 70% a 80%
const int GAS_LIMITE_ALERTA = 200;   // Liga a ventoinha se "gás" > 200
const int TEMP_LIMITE_ALTA = 30;     // Liga a ventoinha se temperatura > 30°C
const int TEMP_LIMITE_BAIXA = 18;    // Alerta se temperatura < 18°C

// Variaveis de controle de tempo
unsigned long ultimaAtualizacao = 0;
const unsigned int intervaloAtualizacao = 60000;  // 5 segundos

void setup() {
    Serial.begin(9600);
    pinMode(pinoRele, OUTPUT);
    
    // Garante que a ventoinha comece DESLIGADA
    digitalWrite(pinoRele, DESLIGAR_RELE); 
    
    Serial.println("ECOCYCLE - Monitoramento Iniciado");
    
    // Mostra os dados pela primeira vez assim que inicia
    mostrarDadosSensores();
}

void loop() {
    // Atualiza os dados no intervalo definido
    if (millis() - ultimaAtualizacao >= intervaloAtualizacao) {
        ultimaAtualizacao = millis();
        mostrarDadosSensores();
    }
    delay(100); 
}

// Função para ler e mostrar todos os dados dos sensores
void mostrarDadosSensores() {
    // 1. Leitura dos sensores
    
    int leituraUmidade = analogRead(sensorUmidade);
    int umidade = map(leituraUmidade, VALOR_SENSOR_SECO, VALOR_SENSOR_MOLHADO, 0, 100);
    umidade = constrain(umidade, 0, 100); 

    int leituraGas = analogRead(sensorGas);
    int ppm = map(leituraGas, 200, 800, 0, 400);
    ppm = constrain(ppm, 0, 400);

    // LEITURA DA TEMPERATURA (LM35 - 10mV por grau Celsius)
    int leituraTemp = analogRead(sensorTemperatura);
    float temperatura = (leituraTemp * 5.0 / 1024.0) * 100;

    // 2. Controle da Ventoinha (Rele)
    // Adiciona temperatura às condições para ligar a ventoinha
    if (umidade > UMIDADE_LIMITE_ALTA || ppm > GAS_LIMITE_ALERTA || temperatura > TEMP_LIMITE_ALTA) {
        digitalWrite(pinoRele, LIGAR_RELE);
        Serial.println("\nStatus: Ventoinha LIGADA (Condições fora do ideal)");
    } else {
        digitalWrite(pinoRele, DESLIGAR_RELE);
        Serial.println("\nStatus: Ventoinha DESLIGADA (Condições ideais)");
    }

    // 3. Exibe os dados no Serial Monitor
    Serial.println("---------------------");
    Serial.print("UMIDADE: ");
    Serial.print(umidade);
    Serial.print("% - ");
    if (umidade > UMIDADE_LIMITE_ALTA) {
        Serial.println("ALTA (Risco de anaerobiose)");
    } else if (umidade < UMIDADE_LIMITE_BAIXA) {
        Serial.println("BAIXA (Verificar/Irrigar)");
    } else {
        Serial.println("IDEAL");
    }
    
    Serial.print("GAS (Aprox): ");
    Serial.print(ppm);
    Serial.print("ppm - ");
    if (ppm > GAS_LIMITE_ALERTA) {
        Serial.println("ALERTA! (Alta concentração detectada)");
    } else {
        Serial.println("NORMAL");
    }

    // EXIBIÇÃO DA TEMPERATURA
    Serial.print("TEMPERATURA: ");
    Serial.print(temperatura);
    Serial.print("°C - ");
    if (temperatura > TEMP_LIMITE_ALTA) {
        Serial.println("ALTA (Ventoinha ligada)");
    } else if (temperatura < TEMP_LIMITE_BAIXA) {
        Serial.println("BAIXA (Aquecer composteira)");
    } else {
        Serial.println("IDEAL");
    }
}