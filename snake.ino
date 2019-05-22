#include <Adafruit_NeoPixel.h>

//LED output pin
#define PIN 6

//LED object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(256, PIN, NEO_GRB + NEO_KHZ800);

//LED map
int space[8][32];

//snake length
int length;

//snake position vectors
int snake[8 * 16][2];

//Direction of the snake
int dir;

//position of the score
int score_point[2];

//position of the bonus
int bonus[2];

//flag to show bluetooth Command
int flag_mode=45;
int brightness_flag=46;
int connected=47;

//Direction for snake
#define up 24
#define down 25
#define left 26
#define right 27
//all time button pin
int button1 = 24;
int button2 = 25;
int button3 = 26;
int button4 = 27;
int button5 = 28;
int button6 = 29;
int button7 = 30;
int button8 = 31;

// bitmask for register manipulation
int mask_1 = digitalPinToBitMask(button1);
int mask_2 = digitalPinToBitMask(button2);
int mask_3 = digitalPinToBitMask(button3);
int mask_4 = digitalPinToBitMask(button4);
int mask_5 = digitalPinToBitMask(button5);
int mask_6 = digitalPinToBitMask(button6);
int mask_7 = digitalPinToBitMask(button7);
int mask_8 = digitalPinToBitMask(button8);
int ipos(int x, int y)
{
  if (x % 2 == 1)
  {
    int s = (8 - y) + x * 8 - 1;
    return s;
  }
  int sum = y + x * 8;
  return sum;
}
void display_game_over(){
  int pixel[]={ipos(6,2),ipos(6,3),ipos(6,4),ipos(6,5),ipos(6,6),ipos(7,2),ipos(8,2),ipos(7,6),ipos(8,6),ipos(9,5),ipos(9,4),ipos(9,3),
  ipos(11,2),ipos(12,2),ipos(13,2),ipos(12,3),ipos(12,4),ipos(12,5),ipos(12,6),ipos(13,6),ipos(11,6),
  ipos(15,2),ipos(16,2),ipos(17,2),ipos(18,2),ipos(15,3),ipos(15,4),ipos(16,4),ipos(17,4),ipos(15,5),ipos(16,5),ipos(15,6),ipos(16,6),ipos(17,6),ipos(18,6),
  ipos(20,2),ipos(21,2),ipos(22,2),ipos(20,3),ipos(20,4),ipos(20,5),ipos(20,6),ipos(21,6),ipos(22,6),ipos(23,5),ipos(23,4),ipos(23,3),
  ipos(25,2),ipos(25,4),ipos(25,5),ipos(26,5)};
  for(int n=0; n<51;n++){
    strip.setPixelColor(pixel[n], strip.Color(124, 252, 0));
  }
  strip.show();
}

void game_over()
{
    display_game_over();
  while(1){
  }
}
int pos(int y, int x)
{
  if (x % 2 == 1)
  {
    int s = (8 - y) + x * 8 - 1;
    return s;
  }
  int sum = y + x * 8;
  return sum;
}
void button_snake()
{
  // activate input filters
  REG_PIOA_IFER = mask_1;
  // choose debounce filter as input filter
  REG_PIOA_DIFSR = mask_1;
  // set clock divider for slow clock
  REG_PIOA_SCDR = 0x04;
  REG_PIOB_IFER = mask_2;
  REG_PIOB_DIFSR = mask_2;
  REG_PIOB_SCDR = 0x04;
  REG_PIOC_IFER = mask_3;
  REG_PIOC_DIFSR = mask_3;
  REG_PIOC_SCDR = 0x04;
  REG_PIOD_IFER = mask_4;
  REG_PIOD_DIFSR = mask_4;
  REG_PIOD_SCDR = 0x04;
}

void interrupt_snake()
{
  button_snake();
  attachInterrupt(digitalPinToInterrupt(24), dir_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(25), dir_down, FALLING);
  attachInterrupt(digitalPinToInterrupt(26), dir_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(27), dir_right, FALLING);
}

void snake_initial()
{
  dir = 3;
  for (int n = 0; n < 8; n++)
  {
    strip.setPixelColor(pos(n, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(pos(n, 25), strip.Color(255, 255, 255));
  }
  length = 3;
  snake[0][0] = 3;
  snake[0][1] = 7;
  snake[1][0] = 3;
  snake[1][1] = 6;
  snake[2][0] = 3;
  snake[2][1] = 5;
  generate_score_point();
  generate_bonus();
  show_snake();
}

//generate bonus randomly
void generate_bonus()
{
  randomSeed(analogRead(0));
  int column = random(0, 8);
  randomSeed(analogRead(0));
  int row = random(0, 16) + 8;
  for (int n = 0; n < length; n++)
  {
    if (pos(snake[n][0], snake[n][1]) == pos(column, row))
    {
      return generate_bonus();
    }
  }
  if (pos(score_point[0], score_point[1]) == pos(column, row))
  {
    return generate_bonus();
  }
  bonus[0] = column;
  bonus[1] = row;
  strip.setPixelColor(pos(column, row), strip.Color(255, 255, 0));
  strip.show();
  return;
}

//generate score randomly
void generate_score_point()
{
  randomSeed(analogRead(0));
  int column = random(0, 8);
  randomSeed(analogRead(0));
  int row = random(0, 16) + 8;
  for (int n = 0; n < length; n++)
  {
    if (pos(snake[n][0], snake[n][1]) == pos(column, row))
    {
      return generate_score_point();
    }
  }
  if (pos(bonus[0], bonus[1]) == pos(column, row))
  {
    return generate_score_point();
  }
  score_point[0] = column;
  score_point[1] = row;
  strip.setPixelColor(pos(column, row), strip.Color(255, 0, 0));
  strip.show();
  return;
}

//Help method for the snake movement
void right_shift()
{
  for (int n = length; n > 0; n--)
  {
    snake[n][0] = snake[n - 1][0];
    snake[n][1] = snake[n - 1][1];
  }
  return;
}

//Move the snake
void move_snake()
{
  right_shift();
  switch (dir)
  {
  case 0:
    snake[0][0]++;
    break;
  case 1:
    snake[0][0]--;
    break;
  case 2:
    snake[0][1]--;
    break;
  case 3:
    snake[0][1]++;
    break;
  }

  //out of border
  if (snake[0][0] > 7)
  {
    game_over();
  }
  if (snake[0][0] < 0)
  {
    game_over();
  }
  if (snake[0][1] < 0)
  {
    game_over();
  }
  if (snake[0][1] > 16)
  {
    game_over();
  }

  for (int n = 1; n < length; n++)
  {
    if (pos(snake[n][0], snake[n][1]) == pos(snake[0][0], snake[0][1]))
    {
      game_over();
    }
  }

  //get point
  if (snake[0][0] == score_point[0] && snake[0][1] == (score_point[1] - 8))
  {
    length++;
    generate_score_point();
  }

  //get bonus -1
  if (snake[0][0] == bonus[0] && snake[0][1] == (bonus[1] - 8))
  {
    if (length > 3)
    {
      strip.setPixelColor(pos(snake[length][0], snake[length][1] + 8), strip.Color(0, 0, 0));
      length--;
    }
    generate_bonus();
  }

  strip.setPixelColor(pos(snake[length][0], snake[length][1] + 8), strip.Color(0, 0, 0));
  show_snake();
}

//show snake on the LED matrix
void show_snake()
{
  //head
  strip.setPixelColor(pos(snake[0][0], snake[0][1] + 8), strip.Color(255, 0, 0));
  //body
  for (int n = 1; n < length - 1; n++)
  {
    strip.setPixelColor(pos(snake[n][0], snake[n][1] + 8), strip.Color(0, 255, 0));
  }
  //tail
  strip.setPixelColor(pos(snake[length - 1][0], snake[length - 1][1] + 8), strip.Color(0, 0, 255));
  strip.show();
}
void dir_up()
{
  if (dir != 1)
  {
    dir = 0;
  }
}
void dir_down()
{
  if (dir != 0)
  {
    dir = 1;
  }
}
void dir_left()
{
  if (dir != 3)
  {
    dir = 2;
  }
}
void dir_right()
{
  if (dir != 2)
  {
    dir = 3;
  }
}


void setup()
{
  Serial.begin(9600);
  //button pin
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(button6, INPUT);
  pinMode(button7, INPUT);
  pinMode(button8, INPUT);

  randomSeed(analogRead(1));

  strip.begin();
  strip.show();
  delay(50);
}

//main process of snake;
void loop()
{
  snake_initial();
  interrupt_snake();
  while (1)
  {
    move_snake();
    delay(500);
  }
}
