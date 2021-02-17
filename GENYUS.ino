/**
   Criado por Paçoca Tech

   Youtube: https://www.youtube.com/channel/UCW6OP5j28zFsnKxpQ2v9CsA
   Instagram: http://instagram.com/pacocatech
   Github: http://github.com/pacocatech
   E-mail: pacocatech@gmail.com


   Releitura do jogo Genius
*/

#include <PushButton.h>
#include "sons.h"

//Definição dos pinos que serão utilizados para o jogo
//LEDs
#define LED_R 5 //Vermelho
#define LED_Y 4 //Amarelo
#define LED_G 3 //Verde
#define LED_B 2 //Azul

//Botões
#define SW_R 12 //SW-Vermelho
#define SW_Y 11 //SW-Amarelo
#define SW_G 10 //SW-Verde
#define SW_B 9 //SW-Azul

PushButton BUTTON_RED(SW_R, 30);
PushButton BUTTON_YELLOW(SW_Y, 30);
PushButton BUTTON_GREEN(SW_G, 30);
PushButton BUTTON_BLUE(SW_B, 30);

//som
#define BUZZER 7

//variavel que controla o debug
#define DEBUG

//melodia para cara cor
#define S_R 1047
#define S_Y 587
#define S_G 659
#define S_B 698

//definições para o jogo
#define maxLevel 10
#define speedGame 700

//controla as jogadas
byte hiddenSequence[maxLevel];
byte playerSequence[maxLevel];

//vez da partida
byte turn = 0;

//o jogador tem 3 segundos para iniciar a jogada
unsigned int playerLimitTime = 3000;

//armazenamos todos os leds que temos no jogo
byte display_game[] = {LED_R, LED_Y, LED_G, LED_B};
//armazenamos todos os sons para cada led
int sound_color[] = {S_R, S_Y, S_G, S_B};

//define que o jogar perdeu a vez
bool timeOut = false;

void controlAllLeds(int value);
void createTurnBot();
void createTurnPlayer();
bool checkEndGame();
void checkResultWinGame();
void changePlayer();

void setup()
{
  Serial.begin(9600);

  //LEDs
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  //por garantia no começo do jogo desligamos todos os leds
  controlAllLeds(LOW);

  //BUZZER
  pinMode(BUZZER, OUTPUT);

  //vamos alimentar o randomSeed para que o jogo nunca seja igual
  randomSeed(analogRead(A0));

#ifdef DEBUG
  Serial.println("Finalizou a configuração do jogo");
#endif

  delay(1000);
}

void loop()
{

  //iniciamos a vez do bot
  createTurnBot();

  //troca a vez do jogador
  changePlayer();

  //iniciamos a vez do jogador
  createTurnPlayer();

  //vamos verificar se o jogador não errou a sequencia
  if (checkResultEndGame()) {
    
    for (byte nota = 0 ; nota < 156 ; nota++) {
      tone(BUZZER, mario[nota], duracaoMario[nota]);
      delay(pausaMario[nota]);
    }

    for (byte nota = 0; nota < 2 ; nota++) {
      tone(BUZZER, errado[nota], duracaoErrado[nota]);
      delay(duracaoErrado[nota]);
    }

    Serial.println("VOCÊ ERROU!, FIM DE JOGO");
    controlAllLeds(LOW);
    while (true);
  }

  //vamos verificar se o jogo foi finalizado
  checkResultWinGame();

  //troca a vez do jogador
  changePlayer();
}

void createTurnBot() {
  //aqui vamos acender um dos leds de forma aleatoria
  hiddenSequence[turn] = random(0, 4);

  for (byte i = 0 ; i <= turn; i++) {
    digitalWrite(display_game[hiddenSequence[i]], HIGH);
    tone(BUZZER, sound_color[hiddenSequence[i]], speedGame);
    delay(speedGame);
    digitalWrite(display_game[hiddenSequence[i]], LOW);
    delay(speedGame / 2);
  }

  turn++;
}

void createTurnPlayer() {
  //capturamos o tempo de quando iniciou a vez do jogador
  unsigned long timeControl = millis();

  byte readPressSwitch = 0;

  while (readPressSwitch < turn) {

    BUTTON_RED.button_loop(); BUTTON_YELLOW.button_loop(); BUTTON_GREEN.button_loop(); BUTTON_BLUE.button_loop();

    if (BUTTON_RED.pressed()) {
      
#ifdef DEBUG
      Serial.println("RED");
#endif

      playerSequence[readPressSwitch] = 0;
      timeControl = millis();
      readPressSwitch++;

      digitalWrite(display_game[1], LOW);
      digitalWrite(display_game[2], LOW);
      digitalWrite(display_game[3], LOW);
      digitalWrite(display_game[0], HIGH);
      tone(BUZZER, sound_color[0], speedGame);

    } else if (BUTTON_YELLOW.pressed()) {
      
#ifdef DEBUG
      Serial.println("YELLOW");
#endif

      playerSequence[readPressSwitch] = 1;
      timeControl = millis();
      readPressSwitch++;

      digitalWrite(display_game[0], LOW);
      digitalWrite(display_game[2], LOW);
      digitalWrite(display_game[3], LOW);
      digitalWrite(display_game[1], HIGH);
      tone(BUZZER, sound_color[1], speedGame);

    } else if (BUTTON_GREEN.pressed()) {
#ifdef DEBUG
      Serial.println("GREEN");
#endif

      playerSequence[readPressSwitch] = 2;
      timeControl = millis();
      readPressSwitch++;

      digitalWrite(display_game[0], LOW);
      digitalWrite(display_game[1], LOW);
      digitalWrite(display_game[3], LOW);
      digitalWrite(display_game[2], HIGH);
      tone(BUZZER, sound_color[2], speedGame);

    } else if (BUTTON_BLUE.pressed()) {
#ifdef DEBUG
      Serial.println("BLUE");
#endif

      playerSequence[readPressSwitch] = 3;
      timeControl = millis();
      readPressSwitch++;

      digitalWrite(display_game[0], LOW);
      digitalWrite(display_game[1], LOW);
      digitalWrite(display_game[2], LOW);
      digitalWrite(display_game[3], HIGH);
      tone(BUZZER, sound_color[3], speedGame);
    }

    if (millis() > timeControl + playerLimitTime) {
      timeOut = true;
      break;
    }
  }

  delay(speedGame / 2);
}

bool checkResultEndGame() {
  //iremos conferir todas as rodadas que já aconteceram
  for (byte i = 0; i < turn; i++) {
    //se houver falha ou timeout é fim de jogo
    if (hiddenSequence[i] != playerSequence[i] || timeOut == true) {
      return true;
    }
  }
  return false;
}

void checkResultWinGame() {

  if (turn == maxLevel) {
    for (byte nota = 0; nota < 12 ; nota++) {
      tone(BUZZER, vitoria[nota], duracaoVitoria);
      delay(duracaoVitoria);
    }
#ifdef DEBUG
    Serial.println("VOCÊ VENCEU!!!! PARABENS");
#endif

    while (true) {
      for (byte i = 0 ; i < 4; i++) {
        digitalWrite(display_game[i], HIGH);
        delay(150);
      }

      for (byte i = 4 ; i > 0; i--) {
        digitalWrite(display_game[i - 1], LOW);
        delay(150);
      }
    }
  }
}

void controlAllLeds(int value) {
  digitalWrite(LED_R, value);
  digitalWrite(LED_Y, value);
  digitalWrite(LED_G, value);
  digitalWrite(LED_B, value);
}

void changePlayer() {
  controlAllLeds(HIGH);
  delay(speedGame);
  controlAllLeds(LOW);
  delay(speedGame);
}
