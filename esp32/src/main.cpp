// Bibliotecas
#include <Arduino.h>
#include "ESP32MotorControl.h"

// Inicialização dos motores
ESP32MotorControl MotorControl = ESP32MotorControl();

int speed = 45;            // Velocidade do motores
int steering_speed = 25;   // Velocidade dos motores enquanto a mudança de direção

#define sen_1 34  // Sensor de Chama da Esquerda
#define sen_2 35  // Sensor de Chama do Centro
#define sen_3 32  // Sensor de Chama da Direita

// Pinos Sensor de cor
#define SENSOR_COR_S2 4
#define SENSOR_COR_S3 16
#define SENSOR_COR_OUT 5

// Função para descobrir a cor vermelha a partir de sua frequência
unsigned long red = 0;

int cor_vermelho()
{
  digitalWrite(SENSOR_COR_S2, 0);
  digitalWrite(SENSOR_COR_S3, 0);
  unsigned long freq = pulseIn(SENSOR_COR_OUT, 0);
  return freq;
}

// Declaração de funções
void print_sen_chama(int,int,int);  // Apresentação de valores dos sensores de chama
void motorForward();                // Função 'Andar em frente'
void motorReverse();                // Função 'Andar para trás'
void motorRight();                  // Funçao 'Andar à direita'
void motorLeft();                   // Função 'Andar à esqueda'
void motorSTOP();                   // Função 'Parar'

void setup() {
  // Na função serial o baud rate funciona melhor em 115200
  // Para isso necessitamos alterar o ficheiro platformio.ini
  // Acrescentando a seguinte linha:
  // monitor_speed = 115200 
  Serial.begin(115200);

  // 17,18,26,27 pinos dos motores no ESP32 
  // correspondentes aos motores esquerdos e direitos
  MotorControl.attachMotors(17, 18, 27, 26);

  pinMode(sen_1,INPUT);   // Declara o Sensor de Chama da Esquerda com entrada
  pinMode(sen_2,INPUT);   // Declara o Sensor de Chama do Centro com entrada
  pinMode(sen_3,INPUT);   // Declara o Sensor de Chama da Direita com entrada

  pinMode(SENSOR_COR_OUT, INPUT);   // Declara o Sensor de Cor com entrada
}

void loop() {

  // O inicia a 'Andar em frente' por 4 segundos
  motorForward();
  delay(4000);
  // Após os 4 segundos, 'Para' por 1 segundo
  motorSTOP();
  delay(1000);

  // Enquanto o Sensor de Cor não detetar uma cor aproximada de vermelho
  // Continua o programa em ciclo
  while (true) {

    int val_sen_1 = !digitalRead(sen_1);   // Valor do Sensor de Chama (Esquerda)
    int val_sen_2 = !digitalRead(sen_2);   // Valor do Sensor de Chama (Centro)
    int val_sen_3 = !digitalRead(sen_3);   // Valor do Sensor de Chama (Direita)

    // Chama a função para apresentar a informação dos Sensores de Chama
    print_sen_chama(val_sen_1,val_sen_2,val_sen_3);

    // Guarda a frequência detetada pelo Sensor de Cor em uma variável
    red = cor_vermelho();  
    
    // Apresenta essa frenquência
    Serial.print("Vermelho: ");
    Serial.println(red);

    // Se ...
    if (red < 100) {   // não for vermelho
      // Se ...
      if (val_sen_1 == true && val_sen_2 == true && val_sen_3 == true) {    // os três Sensores de Chama detetarem calor
        // 'Anda em frente'
        motorForward();
      }
      // Se ...
      else if (val_sen_1 == false && val_sen_2 == true && val_sen_3 == true) {    // o Sensor de Chama da Esqueda não detetar, enquantos os outros sim
        // 'Anda à direita'
        motorRight();
      }
      // Se ...
      else if (val_sen_1 == true && val_sen_2 == true && val_sen_3 == false) {    // o Sensor de Chama da Direita não detetar, enquantos os outros sim
        // 'Anda à equerda'
        motorLeft();
      }
      // Senão ...
      else {    // caso nehum dos Sensores de Chama detetar calor
        // 'Para'
        motorSTOP();
      }
    }
    // Senão ...
    else {    // Caso o Sensor de Cor detetar uma cor aproximada de vermelho
      // 'Andar para trás' por 10 segundos
      motorReverse();
      delay(10000);
      // Termina o ciclo
      break;
    }
    // Aguarda 200 milisegudos
    delay(200);
  }
}

void print_sen_chama(int val_sen_1,int val_sen_2,int val_sen_3) {
  Serial.printf("Sensor de Chama (Esquerda): ");
  Serial.println(val_sen_1);
  Serial.printf("Sensor de Chama (Centro): ");
  Serial.println(val_sen_2);
  Serial.printf("Sensor de Chama (Direito): ");
  Serial.println(val_sen_3);
}

void motorForward() {
  MotorControl.motorForwardLeft(speed);
  MotorControl.motorForwardRight(speed);
}

void motorReverse() {
  MotorControl.motorReverseLeft(speed);
  MotorControl.motorReverseRight(speed);
}

void motorRight() {
  MotorControl.motorForwardLeft(speed);
  MotorControl.motorForwardRight(steering_speed);
}

void motorLeft() {
  MotorControl.motorForwardLeft(steering_speed);
  MotorControl.motorForwardRight(speed);
}

void motorSTOP() {
  MotorControl.motorSTOP(0);
  MotorControl.motorSTOP(1);
}