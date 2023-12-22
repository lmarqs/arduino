#include <Arduino.h>
#include <LiquidCrystal.h>

const int backLight = 10;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

struct {
  int limite;
  char *nome;
} teclas[] = {{50, "Direita "},  {150, "Cima    "}, {300, "Baixo   "},
              {500, "Esquerda"}, {750, "Select  "}, {1024, "        "}};

void setup() {
  Serial.begin(9600);
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH);
  lcd.begin(16, 2);
  lcd.print("SuperMaker");
}

void loop() {
  const unsigned int leitura = analogRead(A0);

  Serial.print(leitura, DEC);
  Serial.print("\n");

  unsigned int teclaNova = 0;

  while (leitura >= teclas[++teclaNova].limite)
    ;

  lcd.setCursor(0, 1);
  lcd.print(teclas[teclaNova].nome);

  delay(100);
}
