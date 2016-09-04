int encoder_A = 10;
int encoder_B = 9;

//int count_A;
//int count_B;

int A = 0;
int B = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(encoder_A, INPUT);
  pinMode(encoder_B, INPUT);
}

void loop()
{
  A = digitalRead(10);
  B = digitalRead(9);
  
//  A = A + count_A;
//  B = B + count_B;
  
  Serial.print("A= "); Serial.print(A); Serial.print(" B= "); Serial.println(B);
}

  
  
  
