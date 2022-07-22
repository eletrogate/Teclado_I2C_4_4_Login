/***************************************************
  Teclado Matricial I2C 4×4 e Sistema de Login com Arduino Nano Exemplo
  Criado em 30 de Julho de 2021 por Michel Galvão
 ****************************************************/

// Inclusão das bibliotecas
#include <Wire.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Definição de endereço I2C do teclado matricial
#define enderecoTeclado 0x26

// Definição de pino do Buzzer para Feedback de Aperto de Tecla
#define pinoBuzzer 2

// número de linhas no teclado
const byte linhas = 4;
// número de colunas no teclado
const byte colunas = 4;

String senha = "159630"; // armazena a senha para efetuar Login

// array que armazena as teclas (Mapeação de teclas)
char keys[linhas][colunas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Definição dos pinos do PCF8574
byte rowPins[linhas] = {0, 1, 2, 3}; // conecta-se aos pinos de linha do teclado
byte colPins[colunas] = {4, 5, 6, 7}; // conecta-se aos pinos de coluna do teclado

// Cria um objeto para o teclado, com os parâmetros mapa de teclas, nº de linhas no teclado, nº de colunas no teclado,
//endereço I2C do teclado e o tipo de Expansor de Porta utilizado (Esta biblioteca, em específico, suporta os chips PCF8574, PCF8574A e PCF8575).
Keypad_I2C teclado(makeKeymap(keys), rowPins, colPins, linhas, colunas, enderecoTeclado, PCF8574 );

// Cria um objeto para o display lcd, com os parâmetros endereço I2C do display,
// nº de colunas no display, e nº de linhas no display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init(); // Inicializa o Display LCD
  lcd.clear(); // Limpa a tela LCD
  lcd.backlight(); // Deixa  a luz de fundo do siplay LCD ligada

  lcd.setCursor(0, 0);
  lcd.print("Exemplo de Login");
  lcd.setCursor(0, 1);
  lcd.print("com Teclado I2C ");

  pinMode(pinoBuzzer, OUTPUT);
  digitalWrite(pinoBuzzer, LOW);
  Wire.begin(); // Inicializa a Comunicação I2C
  teclado.begin(makeKeymap(keys)); // Inicializa o Teclado Matricial I2C
  delay(1500);
  lcd.clear(); // Limpa a tela LCD
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(millis() / 1000); // Exibe, ilustrativamente, o tempo em segundos desde que o programa se iniciou
  lcd.print(" segundos    ");
  lcd.setCursor(0, 1);
  lcd.print("Tecla A p/ logar");
  char tecla = teclado.getKey(); // armazena em uma variável o retorno da tecla obtida

  if (tecla) { // se há alguma tecla,...
    feedbackSomClique(); // emite um som no buzzer para dar feedback ao usuário de que alguma tecla foi pressionada

    if (tecla == 'A') { // se a tecla pressionada foi a tecla A,...
      if (efetuaLogin()) // inicia-se o processo para se logar
      {
        lcd.setCursor(0, 0);
        lcd.print("Voce esta logado"); // Caso o usuário efetuar o login com a senha correta, é exibida a mensagem de que está logado
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(1500);
        lcd.clear();
      }
    }
  }
}

bool efetuaLogin() {
  /*
    Instruções para o usuário de como proceder com as teclas.
  */
  lcd.setCursor(0, 0);
  lcd.print("Insira senha:   ");
  lcd.setCursor(0, 1);
  lcd.print("Teclas de 0 a 9 ");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print("Tecla # - ENTER ");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print("Tecla * - APAGAR");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print("Tecla B - SAIR  ");
  delay(1500);
  lcd.clear();

  String senhaInserida; // Váriavel para armazenar a senha inserida pelo usuário
  String senhaEscondidaExibicao; // Váriavel para armazenar a exibição de caracteres '*' para cada numero inserido pelo usuário
  int numeroTentativas = 0; // Váriavel para armazenar o número de tentativas que o usuário executou para se logar
  int numeroMaximoTentativas = 3; // Váriavel para armazenar o número máximo de tentativas para que o usuário possa se logar

  while (true) { // loop para inserção de senha
    lcd.blink(); // faz o cursor do display ficar piscando
    char tecla = teclado.getKey(); // armazena em uma variável o retorno da tecla obtida
    if (tecla) { // se alguma tecla foi obtida, ...
      feedbackSomClique(); // efetua o som de feedback de clique para o usuário
      if (tecla == '1' || tecla == '2' || tecla == '3' || tecla == 'A') {
        // se alguma tecla da linha 1 do teclado for apertada, temos que fazer debounce, pois se não o microcontrolador irá contabilizar vários cliques. Isso só ocorre na linha 1.
        delay(250);
      }
      if (tecla == '#') { // se a tecla pressionada foi a # (ENTER), ...
        if (senhaInserida == senha) { // compara se a senha inserida pelo usuário é igual a senha para fazer login
          lcd.noBlink(); // faz o cursor do dislpay LCD parar de piscar
          feedbackSomLoginBemSucedido(); // efetua o som de feedback de login bem sucedido
          return true; // retorna true para a chamada da função
        } else { // se a senhas não forem iguais, ...
          lcd.noBlink(); // faz o cursor do dislpay LCD parar de piscar
          numeroTentativas++; // incrementa a váriavel de número de tentativas do usuário
          senhaInserida = ""; // "zera" a váriavel senhaInserida
          senhaEscondidaExibicao = ""; // "zera" a váriavel senhaEscondidaExibicao
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Senha Incorreta ");
          lcd.setCursor(0, 1);
          lcd.print("Tentativa ");
          lcd.print(numeroTentativas);
          lcd.print(" de ");
          lcd.print(numeroMaximoTentativas);
          feedbackSomFalhaLogin(); // efetua o som de feedback de login falhado
          lcd.clear();
          if (numeroTentativas >= numeroMaximoTentativas) { // testa se o número de tentativas for igual ou superior ao número máximo de tentativas
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Tentat. Zeradas ");
            lcd.setCursor(0, 1);
            lcd.print("Entre novamente ");
            feedbackSomTentativasZeradas();
            lcd.clear();
            return false; // caso o usuário chegue ou ultrapasse o número máximo de tentativas, retorna false para a chamada da função
          }
        }
      } else if (tecla == '*') { // se não, se a tecla pressionada foi a * (APAGAR), ...
        // remove o último caractere da váriavel senhaInserida. O caractere a ser removido é o de número que corresponda ao número total de caracteres da váriavel subtraido 1.
        senhaInserida.remove(senhaInserida.length() - 1);
        senhaEscondidaExibicao.remove(senhaEscondidaExibicao.length() - 1); // também remove o último caractere da váriavel senhaEscondidaExibicao
        lcd.clear();
        lcd.print(senhaEscondidaExibicao);
      } else if (tecla == 'B') { // se a tecla pressionada foi a B (SAIR), ...
        return false; // retorna false para a chamada da função
      } else if (!isAlpha(tecla)) { // se a tecla pressionada não for nenhuma letra, ...
        senhaInserida += tecla; // adiciona a tecla pressionada para a váriavel senhaInserida
        senhaEscondidaExibicao += '*'; // adiciona o caractere '*' para a váriavel senhaEscondidaExibicao
        lcd.clear(); // limpa o display
        lcd.print(senhaEscondidaExibicao); // mostra a váriavel senhaEscondidaExibicao
      }
    }
  }
}

// função de toque do buzzer para feedback do aperto da tecla
void feedbackSomClique() {
  digitalWrite(pinoBuzzer, HIGH);
  delay(25);
  digitalWrite(pinoBuzzer, LOW);
}

// função de toque do buzzer para feedback de Falha de Login
void feedbackSomFalhaLogin() {
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(50);
  digitalWrite(pinoBuzzer, LOW);
}

// função de toque do buzzer para feedback de Tentativas Zeradas
void feedbackSomTentativasZeradas() {
  digitalWrite(pinoBuzzer, HIGH);
  delay(500);
  digitalWrite(pinoBuzzer, LOW);
  delay(500);
  digitalWrite(pinoBuzzer, HIGH);
  delay(500);
  digitalWrite(pinoBuzzer, LOW);
  delay(500);
  digitalWrite(pinoBuzzer, HIGH);
  delay(500);
  digitalWrite(pinoBuzzer, LOW);
}

// função de toque do buzzer para feedback de Login bem Sucedido
void feedbackSomLoginBemSucedido() {
  digitalWrite(pinoBuzzer, HIGH);
  delay(100);
  digitalWrite(pinoBuzzer, LOW);
  delay(50);
  digitalWrite(pinoBuzzer, HIGH);
  delay(100);
  digitalWrite(pinoBuzzer, LOW);
}