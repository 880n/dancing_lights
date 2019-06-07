# dancing_lights
This is a project made for ELEC3607 in 2019.

The size of the LED matrix is 8x32.

Actual connection of circuit:

<img src="https://github.com/880n/dancing_lights/blob/master/image/connection.jpg" width="320">


There are two parts contained in this part: Arduino and Android 
The code for Arduino can only runs on Arduino due as there are some registers for debouncing used in the code.

There are two ways to control the system: Bluetooth remote and button set.

Two main functions are designed for the system: Dancing lights and Snake Game.

Main menu:

<img src="https://github.com/880n/dancing_lights/blob/master/image/main.png" width="320">
Dancing lights:

The dancing lights uses arduinoFFT library to transform the audio signal recieved at analog pin A0 to frequency domain and map the value onto the x domain. Then the peak value of each column will be used to show the pattern according to the mode and brightness.
four light patterns are designed for this part.

Pattern 1:

<img src="https://github.com/880n/dancing_lights/blob/master/image/1.png" width="320">

Pattern 2:

<img src="https://github.com/880n/dancing_lights/blob/master/image/2.png" width="320">

Pattern 3:

<img src="https://github.com/880n/dancing_lights/blob/master/image/3.png" width="320">

Pattern 4:

<img src="https://github.com/880n/dancing_lights/blob/master/image/4.png" width="320">


Snake Game:

<img src="https://github.com/880n/dancing_lights/blob/master/image/snake.png" width="320">


The snake game is limited in a 8x16 space. There are two types of points in the game: score point and bonus point. Eating the score point will increase the length of the snake by 1 and increase the game speed. Eating the bonus point reduce the length by 1 and decrease the game speech.

The control of the snake game is designed using debouncing interrupts, so the game can be played in a fluent way.

Android app:

<img src="https://github.com/880n/dancing_lights/blob/master/image/app.jpg" width="320">


In the Android app, we made several buttons sending bytes of commands to the arduino board. The app can do the all the controls as the button set. Additionally, we add the voice recognition to the app. Speak while pressing the voice button will activate it. After releasing it, the words recognized will be shown on the screen. If the specific commands are detected in the words, the commands will be excuted.
