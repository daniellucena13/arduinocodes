//Inclui a biblioteca do LCD
#include <LiquidCrystal.h>

//Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Define digital pins
#define BLUE_LED    10
#define GREEN_LED   9  
#define RED_LED     6
#define BUZZER      7
#define LA0         440

//Define analog input pin
#define  TEMPERATURE_PIN    A0

/**********************************************************/
//                 Temperature Read In                    //
/**********************************************************/

float tempRead() {
  float temp;
  temp = analogRead(TEMPERATURE_PIN);
  temp = temp * 0.48828125;
  return temp;
}

void setup() {
  Serial.begin(9600);
  
  //Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  //Print the first line only once
  lcd.setCursor(0, 1);
  lcd.print("T= ");
  
}

void loop() {
  delay(500);
  //Set the cursor to column 0, line 1
  //(note: line 1 is the second row, since counting begins with 0):  
  lcd.setCursor(3, 1);
  lcd.print(tempRead(),1);
  lcd.print(" Celsius");
  
  if (tempRead()<31.0)
  {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(5,0);
    lcd.print("T IDEAL");
  }
  else if (tempRead()<33.0)
  {
    digitalWrite(BLUE_LED, LOW);
    analogWrite(RED_LED, 200);                    
    analogWrite(GREEN_LED, 90);
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(5,0);
    lcd.print("T ALTA");
  }
  else 
  {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(3,0);
    lcd.print("T CRITICA"); 
    tone(BUZZER,LA0,200); 
  }
    
}
