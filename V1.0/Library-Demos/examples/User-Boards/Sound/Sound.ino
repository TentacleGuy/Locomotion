/******************************************************************************
Created with PROGRAMINO IDE for Arduino
Project     : Sound.ino
Libraries   :
Author      : UlliS
Description : Program to play a simple melody
******************************************************************************

[ INFO: This example is for ARDUINO UNO or compatible boards and NodeMCU ]

Tones are created by quickly pulsing a speaker on and off 
using PWM, to create signature frequencies.

Each note has a frequency, created by varying the period of 
vibration, measured in microseconds. We'll use pulse-width
modulation (PWM) to create that vibration.

We calculate the pulse-width to be half the period; we pulse 
the speaker HIGH for 'pulse-width' microseconds, then LOW 
for 'pulse-width' microseconds.
    This pulsing creates a vibration of the desired frequency.

(cleft) 2005 D. Cuartielles for K3
Refactoring and comments 2006 clay.shirky@nyu.edu
See NOTES in comments at end for possible improvements

Modified 06 Nov 2017
by Ullis
******************************************************************************/

// Arduino or NodeMCU (select one)
#define ARDUINO
//#define NODEMCU

/******************************************************************************
TONES
Start by defining the relationship between
note, period, &  frequency.
******************************************************************************/
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
// define a special note, 'R', to represent a rest
#define  R     0

/******************************************************************************
SETUP
Set up speaker on a PWM pin
******************************************************************************/

// USER-BOARD PINS (ARDUINO UNO or compatible)
#ifdef ARDUINO
    #define PA_PIN      3
#endif

// USER-BOARD PINS (NodeMCU)
#ifdef NODEMCU
    #define PA_PIN      14
#endif

// do we want debugging on serial out? 1 for yes, 0 for no
int DEBUG = 1;

void setup() 
{ 
    pinMode(PA_PIN, OUTPUT);

    // open serial communications and wait for port to open:
    if (DEBUG) 
    { 
        Serial.begin(19200);
        while (!Serial) 
        {
            ; // wait for serial port to connect. Needed for Leonardo only
        }
    }
}

/******************************************************************************
MELODY and TIMING
melody[] is an array of notes, accompanied by beats[],
which sets each note's relative length (higher #, longer note)
******************************************************************************/
int melody[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int MAX_COUNT = sizeof(melody) / 2; // melody length, for looping.

// set overall tempo
long tempo = 10000;

// set length of pause between notes
int pause = 1000;

// loop variable to increase Rest length
int rest_count = 100; // <-BLETCHEROUS HACK; see NOTES

// initialize core variables
int tone_     = 0;
int beat      = 0;
long duration = 0;

/******************************************************************************
PLAY TONE
Pulse the speaker to play a tone for a particular duration
******************************************************************************/
void playTone() 
{
    long elapsed_time = 0;
    
    // if this isn't a Rest beat, while the tone has 
    if (tone_ > 0) 
    {
        // played less long than 'duration', pulse speaker HIGH and LOW
        while (elapsed_time < duration) 
        {
            digitalWrite(PA_PIN,HIGH);
            delayMicroseconds(tone_ / 2);
            
            // down
            digitalWrite(PA_PIN, LOW);
            delayMicroseconds(tone_ / 2);
            
            // keep track of how long we pulsed
            elapsed_time += (tone_);
        } 
    } 
    else 
    { 
        // rest beat; loop times delay
        // see NOTE on rest_count
        for (int j = 0; j < rest_count; j++) 
        {
            delayMicroseconds(duration);  
        }                                
    }                                 
}

/******************************************************************************
LET THE WILD RUMPUS BEGIN
******************************************************************************/
void loop() 
{
    
    // set up a counter to pull from melody[] and beats[]
    for (int i=0; i<MAX_COUNT; i++) 
    {
        tone_ = melody[i];
        beat  = beats[i];
        
        // set up timing
        duration = beat * tempo;
        
        playTone(); 
        
        // a pause between notes...
        delayMicroseconds(pause);
        
        // if debugging, report loop, tone, beat, and duration
        if (DEBUG) 
        {
            Serial.print(i);
            Serial.print(":");
            Serial.print(beat);
            Serial.print(" ");    
            Serial.print(tone_);
            Serial.print(" ");
            Serial.println(duration);
        }
    }
}

/*
* NOTES
* The program purports to hold a tone for 'duration' microseconds.
* Lies lies lies! It holds for at least 'duration' microseconds, _plus_
* any overhead created by incremeting elapsed_time (could be in excess of 
* 3K microseconds) _plus_ overhead of looping and two digitalWrites()
*  
* As a result, a tone of 'duration' plays much more slowly than a rest
* of 'duration.' rest_count creates a loop variable to bring 'rest' beats 
* in line with 'tone' beats of the same length. 
* 
* rest_count will be affected by chip architecture and speed, as well as 
* overhead from any program mods. Past behavior is no guarantee of future 
* performance. Your mileage may vary. Light fuse and get away.
*  
* This could use a number of enhancements:
* ADD code to let the programmer specify how many times the melody should
* loop before stopping
* ADD another octave
* MOVE tempo, pause, and rest_count to #define statements
* RE-WRITE to include volume, using analogWrite, as with the second program at
* http://www.arduino.cc/en/Tutorial/PlayMelody
* ADD code to make the tempo settable by pot or other input device
* ADD code to take tempo or volume settable by serial communication 
* (Requires 0005 or higher.)
* ADD code to create a tone offset (higer or lower) through pot etc
* REPLACE random melody with opening bars to 'Smoke on the Water'
*/
