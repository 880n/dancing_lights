#include <string.h>
//LED library
#include <Adafruit_NeoPixel.h>
//FFT library
#include <arduinoFFT.h>
//SPI library
#include <SPI.h>

//LED output pin
#define PIN 6

//SD write pin
#define CS 10

//SD library
#include <SD.h>

PROGMEM const byte header [44] =
 // This contains the header of a WAV file. Without this, it is not recognized as such.
 // 44 bytes long.
{
 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56,
 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00,
 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x01,
 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00, 0x00, 0x00
};

const byte firstPos = 4;
// Position in the file to write down the overall file size minus 8.
const byte secondPos = 40;
// Position in the file to write down the overall file size minus 44.

byte playState = LOW;
File recording;

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

//Sample rate
#define SAMPLES 128
#define SAMPLING_FREQUENCY 1000

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

//dancing lights modes
uint32_t MY_MODE[] = {strip.Color(255, 0, 0), strip.Color(255, 165, 0), strip.Color(255, 255, 0), strip.Color(0, 255, 0), strip.Color(0, 127, 255), strip.Color(0, 0, 255), strip.Color(139, 0, 255), strip.Color(255, 255, 255)};

//recording module
int rec=51;
int play=52;

//common variables
int brightness = 1;
int light_mode=0;
int stop_dancing_lights = 0;
int voice_mode=0;

void lights_off(){
  for(int n=0;n<256;n++){
    strip.setPixelColor(n, strip.Color(0, 0, 0));
  }
  strip.show();
}

void home_button(){
  menu();
}


void writeHeader() {
  for (byte pos = 0; pos < sizeof(header); pos++) {
   recording.write(pgm_read_byte(&header[pos]));
 }
}

void record() {
 byte sample = 0;
 while (playState == HIGH) {
   sample = analogRead(3) >> 2;
   recording.write(sample);
   playState = digitalRead(play);
   delayMicroseconds(0);
   // Make a trial an error test to know the amount of delay
 }
}

void finalize() {
 // Fills up two necessary variables in the file's header,
 // then ensures a successfully saved recording.
 byte finalValue[4];
 unsigned long fileSize = recording.size();
 unsigned long riffSize = fileSize - 8;
 unsigned long dataSize = fileSize - 44;

 finalValue[0] = riffSize & 0xff;
 finalValue[1] = (riffSize >> 8) & 0xff;
 finalValue[2] = (riffSize >> 16) & 0xff;
 finalValue[3] = (riffSize >> 24) & 0xff;
 // Is possible to make a fuction that returns an array of bytes?

 recording.seek(firstPos);
 recording.write(finalValue, 4);
 // Check if already in little-endian order

 finalValue[0] = dataSize & 0xff;
 finalValue[1] = (dataSize >> 8) & 0xff;
 finalValue[2] = (dataSize >> 16) & 0xff;
 finalValue[3] = (dataSize >> 24) & 0xff;
 // Is possible to make a fuction that returns an array of bytes?

 recording.seek(secondPos);
 recording.write(finalValue, 4);
 // Check if already in little-endian order
 recording.close();
}


void standby() {
 while (true) {
   if (digitalRead(play) == HIGH) {
     break;
   }
   delay(500);
 }
}


void save_initial(){
 REG_ADC_MR = (REG_ADC_MR & 0xf8) | 0x04; // ADC fast mode
 SD.begin(10);
 if (SD.exists("REC.wav")) {
   SD.remove("REC.wav");
 }
}

void save_wav(){
   recording = SD.open("REC.wav", FILE_WRITE);
   if (recording) {
     writeHeader();
     standby();
     playState = HIGH;
     record();
     finalize();
   }
}

void hold(){
  digitalWrite(play,LOW);
  playState=LOW;
}

void voice_button(){
  lights_off();
  int mask_v = digitalPinToBitMask(play);
  REG_PIOB_IFER = mask_v;
  REG_PIOB_DIFSR = mask_v;
  REG_PIOB_SCDR = 0x04;
  attachInterrupt(digitalPinToInterrupt(play), hold, RISING);

  //recording
  digitalWrite(rec,HIGH);
  while(1){
    if(digitalRead(button7)==HIGH){
      break;
    }
  }
  digitalWrite(rec,LOW);
  //save to wav
  //send signal to raspi
  save_initial();
  digitalWrite(play,HIGH);
  playState=HIGH;
  save_wav();
  delay(50);

  Serial.write("FINISHED RECORDING\n");
  //wait for some time
  delay(1000);

  String text=Serial.readString();
  //1 dancing lights
  //0 menu
  if(voice_mode==0){
    if(text.equals("bluetooth")){
      bluetooth_status();
    }
    if(text.equals("dancing lights")){
      main_dancing_lights();
    }
    if(text.equals("snake")){
      snake_main_process();
    }
  }
  if(voice_mode==1){
    if(text.equals("mode")){
      change_mode();
    }
  }
}












//button snake mode
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

void change_mode(){
  light_mode++;
  if(light_mode>5){
    light_mode=0;
  }
}

void button_dancing()
{
  
  Serial.flush();
  REG_PIOC_IFER = mask_7;
  Serial.flush();
  REG_PIOC_DIFSR = mask_7;
  Serial.flush();
  REG_PIOC_SCDR = 0x04;
  Serial.flush();
  REG_PIOD_IFER = mask_8;
  Serial.flush();
  REG_PIOD_DIFSR = mask_8;
  Serial.flush();
  REG_PIOD_SCDR = 0x04;
  Serial.flush();
  attachInterrupt(digitalPinToInterrupt(button7), voice_button, FALLING);
  Serial.flush();
  attachInterrupt(digitalPinToInterrupt(button8), home_button, FALLING);
  Serial.flush();
  Serial.print("hello");
  Serial.flush();

  int mask_cm = digitalPinToBitMask(flag_mode);
  REG_PIOA_IFER = mask_cm;
  REG_PIOA_DIFSR = mask_cm;
  REG_PIOA_SCDR = 0x04;
  attachInterrupt(digitalPinToInterrupt(flag_mode), change_mode, CHANGE);
  REG_PIOA_IFER = mask_1;
  REG_PIOA_DIFSR = mask_1;
  REG_PIOA_SCDR = 0x04;
  attachInterrupt(digitalPinToInterrupt(button1), change_mode, FALLING);
}

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
    strip.setPixelColor(n, strip.Color(124, 252, 0));
  }
  strip.show();
}

//snake game over
void game_over()
{
  lights_off();
  display_game_over();
  while(1){
    if(digitalRead(button8)==LOW){
      lights_off();
      menu();
    }
    if(digitalRead(button7)==LOW||digitalRead(button6)==LOW||digitalRead(button5)==LOW||digitalRead(button4)==LOW||digitalRead(button3)==LOW||digitalRead(button2)==LOW||digitalRead(button1)==LOW){
      lights_off();
      snake_main_process();
    }
  }
}

//Game start count down
void count_down()
{
  int three[]={ipos(15,2),ipos(16,2),ipos(14,3),ipos(17,3),ipos(16,4),ipos(17,5),ipos(16,6),ipos(15,6),ipos(14,5)};
  int two[]={ipos(14,2),ipos(15,2),ipos(16,2),ipos(17,2),ipos(15,3),ipos(16,4),ipos(17,5),ipos(16,6),ipos(15,6),ipos(14,5)};
  int one[]={ipos(14,2),ipos(15,2),ipos(16,2),ipos(15,3),ipos(15,4),ipos(15,6),ipos(14,5)};
  lights_off();
  for(int n=0;n<9;n++){
    strip.setPixelColor(three[n], strip.Color(0, 0, 0));
  }
  delay(1000);
  lights_off();
  for(int n=0;n<10;n++){
    strip.setPixelColor(two[n], strip.Color(0, 0, 0));
  }
  delay(1000);
  lights_off();
  for(int n=0;n<7;n++){
    strip.setPixelColor(one[n], strip.Color(0, 0, 0));
  }
  delay(1000);
  lights_off();
}

//Initialize snake game;
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

//Initial interrupt for snake
void interrupt_snake()
{
  button_snake();
  attachInterrupt(digitalPinToInterrupt(24), dir_up, FALLING);
  attachInterrupt(digitalPinToInterrupt(25), dir_down, FALLING);
  attachInterrupt(digitalPinToInterrupt(26), dir_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(27), dir_right, FALLING);
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

//main process of snake;
void snake_main_process()
{
  snake_initial();
  interrupt_snake();
  count_down();
  while (1)
  {
    move_snake();
    delay(500);
  }
}

//Initialize sample period for dancing lights
void dancing_lights_initial()
{
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

//main process for dancing lights
void main_dancing_lights()
{
  voice_mode=1;
  Serial.flush();
  dancing_lights_initial();
  stop_dancing_lights=0;
  while (stop_dancing_lights != 1)
  {
  Serial.flush();
    /*SAMPLING*/
    for (int i = 0; i < SAMPLES; i++)
    {
      Serial.print("running\n");
      microseconds = micros(); //Overflows after around 70 minutes!

      vReal[i] = analogRead(0);
      vImag[i] = 0;

      while (micros() < (microseconds + sampling_period_us))
      {
      }
    }

    // ++ FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    // -- FFT

    // ++ re-arrange FFT result to match with no. of columns on display ( xres )
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
    // -- re-arrange FFT result to match with no. of columns on display ( xres )

    // ++ send to display according measured value
    for (int i = 0; i < xres; i++)
    {
      data_avgs[i] = constrain(data_avgs[i], 0, 80);    // set max & min values for buckets
      data_avgs[i] = map(data_avgs[i], 0, 80, 0, yres); // remap averaged values to yres
      yvalue = data_avgs[i];

      peaks[i] = peaks[i] - 1; // decay by one light
      if (yvalue > peaks[i])
      {
        peaks[i] = yvalue;
      }
      yvalue = peaks[i];

      for (int a = 0; a < yres; a++)
      {
        if (a < yvalue)
        {
          strip.setPixelColor(pos(a, i), MY_MODE[a]);
        }
        else
        {
          strip.setPixelColor(pos(a, i), strip.Color(0, 0, 0));
        }
      }
    }
    strip.show();
  }
  menu();
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

  pinMode(rec,OUTPUT);
  pinMode(play,OUTPUT);

  pinMode(flag_mode,INPUT);
  pinMode(brightness_flag,INPUT);
  pinMode(connected,INPUT);

  pinMode(CS, OUTPUT);

  randomSeed(analogRead(1));

  strip.begin();
  strip.show();
  strip.setBrightness(brightness);
  delay(50);
}

void display_unconnected(){
  int pixel_s[]={ipos(9,3),ipos(9,4),ipos(9,5),ipos(10,6),ipos(10,2),ipos(11,6),ipos(11,2),ipos(9,3),ipos(12,3),ipos(12,4),
                 ipos(12,5),ipos(14,2),ipos(14,2),ipos(14,3),ipos(14,4),ipos(14,5),ipos(14,6),ipos(15,6),ipos(16,6),ipos(17,6),
                 ipos(15,4),ipos(16,4),ipos(19,2),ipos(19,3),ipos(19,4),ipos(19,5),ipos(19,6),ipos(20,6),ipos(21,6),ipos(22,6),
                 ipos(20,4),ipos(21,4)};
    for(int n=0;n<32;n++){
    strip.setPixelColor(n, strip.Color(255,97,0));
  }
  strip.show();
}

void display_connected(){
  int pixel_s[]={ipos(11,2),ipos(11,3),ipos(11,4),ipos(12,5),ipos(12,1),ipos(13,1),ipos(13,5),ipos(14,4),ipos(14,3),ipos(14,2),
                 ipos(16,1),ipos(16,2),ipos(16,3),ipos(16,4),ipos(16,5),ipos(17,4),ipos(18,3),ipos(19,1),ipos(19,2),ipos(19,3),
                 ipos(19,4),ipos(19,5)};
    for(int n=0; n<22;n++){
    strip.setPixelColor(n, strip.Color(255,97,0));
  }
  strip.show();
}

void bluetooth_status(){
  if(connected==0){
    display_unconnected();
  }
  if(connected==1){
    display_connected();
  }
  delay(3000);
  lights_off();
  menu();
}

void display_menu(){
  int pixel_b[]={ipos(2,1),ipos(3,2),ipos(4,3),ipos(4,5),ipos(3,6),ipos(2,7),ipos(5,0),ipos(5,1),ipos(5,2),ipos(5,3),ipos(5,4),ipos(5,5),ipos(5,6),ipos(5,7),ipos(6,7),ipos(7,7),
               ipos(6,0),ipos(6,4),ipos(7,1),ipos(7,3),ipos(7,5),ipos(8,2),ipos(8,6)};
  for(int n=0; n<23;n++){
    strip.setPixelColor(pixel_b[n], strip.Color(0,0,255));
  }

  int pixel_d[]={ipos(10,0),ipos(10,1),ipos(11,0),ipos(11,1),ipos(11,2),ipos(12,0),ipos(12,1),ipos(12,2),ipos(13,0),ipos(13,1),ipos(13,2),ipos(13,3),ipos(13,4),ipos(13,5),ipos(13,6),ipos(13,7),
                 ipos(14,7),ipos(15,7),ipos(16,7),ipos(16,0),ipos(16,1),ipos(17,7),ipos(17,0),ipos(17,1),ipos(17,2),ipos(18,7),ipos(18,0),ipos(18,1),ipos(18,2),ipos(19,1),
                 ipos(19,2),ipos(19,3),ipos(19,4),ipos(19,5),ipos(19,6),ipos(19,7)};
  for(int n=0; n<36;n++){
    strip.setPixelColor(pixel_d[n], strip.Color(255,97,0));
  }

  int pixel_s[]={ipos(23,2),ipos(24,2),ipos(25,2),ipos(26,2),ipos(28,2),ipos(23,3),ipos(23,4),ipos(24,4),ipos(25,4),ipos(26,4),ipos(27,4),ipos(28,4),ipos(28,5),ipos(28,6),ipos(27,6),ipos(26,6),ipos(25,6),ipos(24,6),ipos(23,6)};
  for(int n=0; n<19;n++){
    strip.setPixelColor(pixel_s[n], strip.Color(0,255,0));
  }
  strip.show();
}

void change_brightness(){
  Serial.print("changing\n");
  brightness=brightness+10;
  if(brightness>255){
    brightness=1;
  }
}

///main menu
void menu()
{
  voice_mode=0;
  display_menu();
  int mask_cm = digitalPinToBitMask(brightness_flag);
  REG_PIOA_IFER = mask_cm;
  REG_PIOA_DIFSR = mask_cm;
  REG_PIOA_SCDR = 0x04;
  attachInterrupt(digitalPinToInterrupt(brightness_flag), change_brightness, CHANGE);
  while(1){
    if(digitalRead(button1)==LOW){
      lights_off();
      bluetooth_status();
    }
    if(digitalRead(button2)==LOW){
      Serial.flush();
      lights_off();
      main_dancing_lights();
    }
    if(digitalRead(button3)==LOW){
      lights_off();
      snake_main_process();
    }
    if(digitalRead(button7)==LOW){
      voice_button();
    }
  }
}

void loop()
{
  menu();
}
