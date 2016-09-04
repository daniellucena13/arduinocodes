int encoder_A = 0;
int encoder_B = 1;
int A=0;
int B=0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  A = analogRead(encoder_A);
  B = analogRead(encoder_B);
  delay(100);
  Serial.print("A= "); Serial.print(A); Serial.print(" B= "); Serial.println(B);
}

