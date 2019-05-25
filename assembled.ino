  #include <Adafruit_NeoPixel.h>
//FFT library
#include <arduinoFFT.h>
//LED output pin
#define PIN 6

//LED object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(256, PIN, NEO_GRB + NEO_KHZ800);

//Sample rate
#define SAMPLES 256
#define SAMPLING_FREQUENCY 2000

//position of strips for dancing lights
#define xres 64
#define yres 8

//y values and peaks at different x
int yvalue;
int peaks[xres];

//FFT variables
double vReal[SAMPLES];
double vImag[SAMPLES];
char data_avgs[xres];

//sampling variables
unsigned int sampling_period_us;
unsigned long microseconds;

//FFT object
arduinoFFT FFT = arduinoFFT();

//snake initial speed
int speed=500;

//all time button pin
int button1 = 24;
int button2 = 25;
int button3 = 26;
int button4 = 27;
int button5 = 28;
int button6 = 29;
int button7 = 30;
int button8 = 31;

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

//Direction for snake
#define up 24
#define down 25
#define left 26
#define right 27

// bitmask for register manipulation
int mask_1 = digitalPinToBitMask(button1);
int mask_2 = digitalPinToBitMask(button2);
int mask_3 = digitalPinToBitMask(button3);
int mask_4 = digitalPinToBitMask(button4);

//Bluetooth flag to toggle mode and brightness;
int flag_brightness=46;
int flag_mode=45;
int toggle_b=0;
int toggle_m=0;
int mode=0;
int brightness=1;

int count_score=0;

//position vectors in LED map
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

//inversed x y position vectors
int ipos(int x, int y)
{
  x=31-x;
  if (x % 2 == 1)
  {
    int s = (8 - y) + x * 8 - 1;
    return s;
  }
  int sum = y + x * 8;
  return sum;
}

//dancing lights modes
//rainbow color
uint32_t Mode_0[] = {strip.Color(255, 0, 0), strip.Color(255, 255, 0), strip.Color(255, 255, 0), strip.Color(0, 255, 0), strip.Color(0, 255, 255), strip.Color(0, 0, 255), strip.Color(139, 0, 255), strip.Color(255, 255, 255)};
//red blue strips
uint32_t Mode_1[] = {strip.Color(255, 0, 0), strip.Color(0, 0, 255), strip.Color(255, 0, 0), strip.Color(0, 0, 255), strip.Color(255, 0, 0), strip.Color(0, 0, 255), strip.Color(255, 0, 0), strip.Color(0, 0, 255)};
//full green
uint32_t Mode_2[] = {strip.Color(0,255, 0), strip.Color(0,255, 0), strip.Color(0,255, 0), strip.Color(0,255, 0),strip.Color(0,255, 0), strip.Color(0,255, 0), strip.Color(0,255, 0), strip.Color(0,255, 0)};
uint32_t Mode_3[] = {strip.Color(255, 0, 0), strip.Color(255, 127, 0), strip.Color(255, 255, 0), strip.Color(0, 255, 0), strip.Color(0, 255, 255), strip.Color(0, 0, 255), strip.Color(139, 0, 255), strip.Color(255, 255, 255)};

//display the game over screen
void display_game_over(){
  //pixel positions
  int pixel[]={ipos(6,2),ipos(6,3),ipos(6,4),ipos(6,5),ipos(6,6),ipos(7,2),ipos(8,2),ipos(7,6),ipos(8,6),ipos(9,5),ipos(9,4),ipos(9,3),
  ipos(11,2),ipos(12,2),ipos(13,2),ipos(12,3),ipos(12,4),ipos(12,5),ipos(12,6),ipos(13,6),ipos(11,6),
  ipos(15,2),ipos(16,2),ipos(17,2),ipos(18,2),ipos(15,3),ipos(15,4),ipos(16,4),ipos(17,4),ipos(15,5),ipos(15,6),ipos(16,6),ipos(17,6),ipos(18,6),
  ipos(20,2),ipos(21,2),ipos(22,2),ipos(20,3),ipos(20,4),ipos(20,5),ipos(20,6),ipos(21,6),ipos(22,6),ipos(23,5),ipos(23,4),ipos(23,3),
  ipos(25,2),ipos(25,4),ipos(25,5),ipos(25,6)};
  //show the screen
  for(int n=0; n<50;n++){
    strip.setPixelColor(pixel[n], strip.Color(255, 0, 0));
  }
  strip.show();
  delay(500);
  lights_off();
  delay(500);
}

void game_over()
{
  //turn the lights off
  lights_off();
  //show the game over screen
  while(1){
  display_game_over();
  if(digitalRead(button3)==LOW){
    delay(500);
    menu();
  }
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

  //flag pin
  pinMode(flag_brightness, INPUT);
  pinMode(flag_mode,INPUT);

//read noise for random seed
  randomSeed(analogRead(1));

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
  //Assign interrupts
  attachInterrupt(digitalPinToInterrupt(24), dir_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(25), dir_down, FALLING);
  attachInterrupt(digitalPinToInterrupt(26), dir_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(27), dir_right, FALLING);

  //initialize the LED matrix
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);
  delay(50);
}

//Display main menu
void display_menu(){
  //pixels for bluetooth
  int pixel_b[]={ipos(2,1),ipos(3,2),ipos(4,3),ipos(4,5),ipos(3,6),ipos(2,7),ipos(5,0),ipos(5,1),ipos(5,2),ipos(5,3),ipos(5,4),ipos(5,5),ipos(5,6),ipos(5,7),ipos(6,7),ipos(7,7),
               ipos(6,0),ipos(6,4),ipos(7,1),ipos(7,3),ipos(7,5),ipos(8,2),ipos(8,6)};
  for(int n=0; n<23;n++){
    strip.setPixelColor(pixel_b[n], strip.Color(0,0,255));
  }

  //pixels for dancing lights
  int pixel_d[]={ipos(10,0),ipos(10,1),ipos(11,0),ipos(11,1),ipos(11,2),ipos(12,0),ipos(12,1),ipos(12,2),ipos(13,1),ipos(13,2),ipos(13,3),ipos(13,4),ipos(13,5),ipos(13,6),ipos(13,7),
                 ipos(14,7),ipos(15,7),ipos(16,7),ipos(16,0),ipos(16,1),ipos(17,7),ipos(17,0),ipos(17,1),ipos(17,2),ipos(18,7),ipos(18,0),ipos(18,1),ipos(18,2),ipos(19,1),
                 ipos(19,2),ipos(19,3),ipos(19,4),ipos(19,5),ipos(19,6),ipos(19,7)};
  for(int n=0; n<35;n++){
    strip.setPixelColor(pixel_d[n], strip.Color(255,97,0));
  }

  //pixels for snake game
  int pixel_s[]={ipos(23,2),ipos(24,2),ipos(25,2),ipos(26,2),ipos(28,2),ipos(23,3),ipos(23,4),ipos(24,4),ipos(25,4),ipos(26,4),ipos(27,4),ipos(28,4),ipos(28,5),ipos(28,6),ipos(27,6),ipos(26,6),ipos(25,6),ipos(24,6),ipos(23,6)};
  for(int n=0; n<19;n++){
    strip.setPixelColor(pixel_s[n], strip.Color(0,255,0));
  }
  strip.setPixelColor(pixel_s[4], strip.Color(255,255,0));
  strip.show();
}

//set all LED off
void lights_off(){
  for(int n=0;n<256;n++){
    strip.setPixelColor(n, strip.Color(0, 0, 0));
  }
  strip.show();
}

//main process of dancing lights
void main_dancing_lights()
{
  //sample rate
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));      
  Serial.print("running\n");
  while (1)
  {
    /*SAMPLING*/
    for (int i = 0; i < SAMPLES; i++)
    {
      //start time
      microseconds = micros(); 

      //record data
      vReal[i] = analogRead(0);
      vImag[i] = 0;

      //run for sampling period
      while (micros() < (microseconds + sampling_period_us))
      {
      }
    }

    //  FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    //  re-arrange FFT result to match with no. of columns on display ( xres )
    int step = (SAMPLES / 2) / xres;
    int c = 0;
    for (int i = 0; i < (SAMPLES / 2); i += step)
    {
      data_avgs[c] = 0;
      for (int k = 0; k < step; k++)
      {
        data_avgs[c] = data_avgs[c] + vReal[i + k];
      }
      data_avgs[c] = data_avgs[c] / step;
      c++;
    }

    // send to display according measured value
    //mode 0,1,2
    if(mode==0||mode==1||mode==2){
    char data_avgsA[xres];
    for (int i = 0; i < xres; i++)
    {
      // set max & min values for buckets
      data_avgsA[i] = constrain(data_avgs[i], 0, 80);  
      // remap averaged values to yres
      data_avgsA[i] = map(data_avgsA[i], 0, 80, 0, yres); 
      yvalue = data_avgsA[i];

      // decay by one light
      peaks[i] = peaks[i] - 1; 
      if (yvalue > peaks[i])
      {
        peaks[i] = yvalue;
      }
      yvalue = peaks[i];

      //set colour according to pattern
      for (int a = 0; a < yres; a++)
      {
        if (a < yvalue)
        {
          if(mode==0){
          strip.setPixelColor(pos(a, i), Mode_0[a]);
          }
          if(mode==1){
          strip.setPixelColor(pos(a, i), Mode_1[a]);
          }
          if(mode==2){
          strip.setPixelColor(pos(a, i), Mode_2[a]);
          if(a==yvalue-1){
            strip.setPixelColor(pos(a, i), strip.Color(255,255, 255));
          }
          
          }
        }
        else
        {
          strip.setPixelColor(pos(a, i), strip.Color(0, 0, 0));
        }
      }
    }
    }

    //mode 3
    if(mode==3){
      char data_avgsB[xres];
      for (int x = 0; x < xres; x++){
      // set max & min values for buckets
      data_avgsB[x] = constrain(data_avgs[x], 0, 80);  
      // remap averaged values to yres
      data_avgsB[x] = map(data_avgsB[x], 0, 80, 0, yres);
      }
    for(int i=0;i<8;i++){
      //four  lines together
      int peak=0;
      for(int n=0;n<4;n++){
        if(data_avgsB[4*i+n]>peak){
          peak= data_avgsB[4*i+n];
        }
      }
      for(int m=0;m<yres;m++){
        for(int n=0;n<4;n++){
          if(m<peak){
            strip.setPixelColor(pos(m,4*i+n), Mode_3[i]);
          }
          else
        {
          strip.setPixelColor(pos(m,4*i+n), strip.Color(0, 0, 0));
        }
        }
      }
    }
    }








    strip.show();
    //if the button is pressed agian return to main menu;
   if(digitalRead(button2)==LOW){
    Serial.println(digitalRead(button2));
     lights_off();
     delay(1000);
      menu();
   }
   //button 8 pressed change mode
   if(digitalRead(button8)==LOW){
    mode++;
    if(mode>=4){
      mode=0;
    }
   }
   //button 7 pressed change brightness
   if(digitalRead(button7)==LOW){
    brightness=brightness+10;
    if(brightness>=255){
        brightness=1;
    }
    strip.setBrightness(brightness);
   }
   //button 6 pressed change brightness
   if(digitalRead(button6)==LOW){
    brightness=brightness-10;
    if(brightness<1){
        brightness=255;
    }
    strip.setBrightness(brightness);
   }
  }
}

void snake_initial()
{
  //initial direction
  dir = 3;
  //set the border
  for (int n = 0; n < 8; n++)
  {
    strip.setPixelColor(pos(n, 7), strip.Color(255, 255, 255));
    strip.setPixelColor(pos(n, 25), strip.Color(255, 255, 255));
  }
  //initial snake length=3
  length = 3;
  //set the snake pixel positions
  snake[0][0] = 3;
  snake[0][1] = 7;
  snake[1][0] = 3;
  snake[1][1] = 6;
  snake[2][0] = 3;
  snake[2][1] = 5;
  //generate score point
  generate_score_point();
  //generate bonus point
  generate_bonus();
  //show the snake
  show_snake();
}

//generate bonus randomly
void generate_bonus()
{
  //30% possibility to generate a bonus point
  int possibility=30;
  if(random(0,100)>possibility){
    bonus[0]=0;
    bonus[1]=0;
    return;
  }

  //generate random positions
  randomSeed(analogRead(0));
  int column = random(0, 8);
  randomSeed(analogRead(0));
  int row = random(0, 16) + 8;

  //check availability
  for (int n = 0; n < length; n++)
  {
    //the point is on snake, regenerate
    if (pos(snake[n][0], snake[n][1]) == pos(column, row))
    {
      return generate_bonus();
    }
  }
  //the bonus is at the same position as score point, regenerate
  if (pos(score_point[0], score_point[1]) == pos(column, row))
  {
    return generate_bonus();
  }

  //set the position of bonus point
  bonus[0] = column;
  bonus[1] = row;
  //show the point
  strip.setPixelColor(pos(column, row), strip.Color(255, 255, 0));
  strip.show();
  return;
}

//generate score randomly
void generate_score_point()
{
  //generate random positions
  randomSeed(analogRead(0));
  int column = random(0, 8);
  randomSeed(analogRead(0));
  int row = random(0, 16) + 8;

  //check availability
  for (int n = 0; n < length; n++)
  {
    //the point is on snake, regenerate
    if (pos(snake[n][0], snake[n][1]) == pos(column, row))
    {
      return generate_score_point();
    }
  }
  //the score point is at the same position as bonus point, regenerate
  if (pos(bonus[0], bonus[1]) == pos(column, row))
  {
    return generate_score_point();
  }

  //set the position of bonus point
  score_point[0] = column;
  score_point[1] = row;
  //show the point
  strip.setPixelColor(pos(column, row), strip.Color(255, 0, 0));
  strip.show();
  return;
}

//Help method for the snake movement
void right_shift()
{
  //right shift the vectors of the snake
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
  //right shift the snake vectors
  right_shift();

  //check the direction and determin the head position
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

  //the snake eat itself
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
    //increase the length to add the deleted tail
    length++;
    //generate new score point
    generate_bonus();
    generate_score_point();
    //increase the speed every time
    speed=speed-5;
  }

  //get bonus -1
  if (snake[0][0] == bonus[0] && snake[0][1] == (bonus[1] - 8))
  {
    //if the length is over 3, decrease the length by 1
    if (length > 3)
    {
      //set the LED for tail off
      strip.setPixelColor(pos(snake[length][0], snake[length][1] + 8), strip.Color(0, 0, 0));
      length--;
    }
    //generate new bonus point
    generate_bonus();
  }

  //deleted tail LED off
  strip.setPixelColor(pos(snake[length][0], snake[length][1] + 8), strip.Color(0, 0, 0));
  //show the snake
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

//interrupt funtions to change the direction of the snake
//unable to turn to the inverse direction of the current
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

//main process of the snake game
void snake_main_process(){
  //initialize variables
  snake_initial();
  //initial speed
  while (1)
  {
    //move the snake
    move_snake();
    delay(speed);
  }
}

//bluetooth control, controlling the brightness and mode of the dancing lights
void bluetooth(){
  //signal for the brightness 
  //increase by 10 and will not excess 255
  while(1){
  if(toggle_b!=digitalRead(flag_brightness)){
    Serial.println("b recieved");
    brightness+=10;
    if(brightness>=255){
      brightness=1;
    }
    strip.setBrightness(brightness);
    toggle_b=digitalRead(flag_brightness);
    delay(1000);
    menu();
  }

  //signal for the mode
  //5 modes in total
  if(toggle_m!=digitalRead(flag_mode)){
    Serial.println("m recieved");
    mode++;
    if(mode>=4){
      mode=0;
    }
    toggle_m=digitalRead(flag_mode);
    delay(1000);
    menu();
  }

  //button pressed again, return to main menu
  if(digitalRead(button1)==LOW){
    delay(1000);
    menu();
  }

  delay(500);
  }
}

///main menu
void menu()
{
  //display the main menu
  display_menu();
  while(1){
    //button 1 for bluetooth commands
    if(digitalRead(button1)==LOW){
      lights_off();
      delay(500);
      bluetooth();
    }

    //button 2 for dancing lights
    if(digitalRead(button2)==LOW){
      lights_off();
      delay(500);
      main_dancing_lights();
    }

    //button 3 for snake game
    if(digitalRead(button3)==LOW){
      lights_off();
      delay(500);
      snake_main_process();
    }
  }
}

//loop
void loop()
{
  //run the main menu
  menu();
}
