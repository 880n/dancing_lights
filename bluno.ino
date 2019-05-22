int m = 0;
int b = 0;

int connected = 0;
String input;
void setup()
{
  Serial.begin(115200); //initial the Serial
  //flag mode
  pinMode(8, OUTPUT);
  //flag brightness
  pinMode(9, OUTPUT);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

  //DEFAULT BLE MODE
  Serial.write("+++");
  Serial.write("AT+SETTING=DEFAULT\r\n");
  Serial.write("AT+EXIT\r\n");
}

void loop()
{
  input=Serial.readString();
  Serial.println(input);
  Serial.println("char:");
  Serial.println(input.charAt(0));
    if (input.charAt(0) == 'm')
    {
//      Serial.println("m recieved");
      if (m == 0)
      {
        digitalWrite(8, HIGH);
        m = 1;
      }
      if (m == 1)
      {
        digitalWrite(8, LOW);
        m = 0;
      }
    }
    if (input.charAt(0) == 'b')
    {
//      Serial.println("b recieved");
      if (m == 0)
      {
        digitalWrite(9, HIGH);
        m = 1;
      }
      if (m == 1)
      {
        digitalWrite(9, LOW);
        m = 0;
      }
    }
}
