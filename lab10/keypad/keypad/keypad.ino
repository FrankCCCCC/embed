#include <Keypad.h>
#define KEY_ROWS 4
#define KEY_COLS 4

char keymap[KEY_ROWS][KEY_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Column pin 1~4
byte colPins[KEY_COLS]={9, 8, 7, 6};
// Column pin 1~4
byte rowPins[KEY_ROWS]={13, 12, 11, 10}; 

// Init KetPad
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);

void setup(){
  Serial.begin(9600);
}

void loop(){
  char key = myKeypad.getKey();
  
  if(key){Serial.println(key);}
}
