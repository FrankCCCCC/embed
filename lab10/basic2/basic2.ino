#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Keypad
#define KEY_ROWS 4
#define KEY_COLS 4

#define LEFT 0
#define STOP 1
#define RIGHT 2
#define MAXCOL 15
#define MINCOL 0
#define MAXSPEED 15
#define MINSPEED 0
#define PHOTONSTOPTHRES 550

// Keypad
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

//LCD 
LiquidCrystal_I2C lcd(0x27,16,2);
byte dinosaur[8]  = {0x07, 0x05, 0x06, 0x07, 0x14, 0x17, 0x0E, 0x0A};
byte cactus[8] = {0x04, 0x05, 0x15, 0x15, 0x16, 0x0C, 0x04, 0x04};
byte empty[0];

// Cactus & Dinosour
int dino_px = 0, dino_py = 0;
int cactus_px = 0, cactus_py = 0;
int cactus1_px = 0, cactus1_py = 0;
const int delay_dis = 10;

// Photon detect
const int photoPin0 = A0;
const int photoPin1 = A1;
int speed = 0;

// Task Handler
int is_lcd_suspend = 1;
TaskHandle_t LCDTaskHandle;

// Game
int score = 0;
int is_game_over = 0;

void game_over(){
  lcd.setCursor(0, 0);
  lcd.print(F("Game Over"));
  lcd.setCursor(0, 1);
  lcd.print(F("Score: "));
  lcd.print(score);
}

void reset_game(){
  score = 0;
  is_game_over = 0;
  
  dino_px = 0;
  dino_py = 0;
  
  cactus_px = 10;
  cactus_py = 0;
  
  cactus1_px = 7;
  cactus1_py = 0;

  speed = 0;
}

void create_dino_cactus_cahr(){
  lcd.createChar(0, dinosaur);
  lcd.createChar(1, cactus);
  cactus_px = random(8, 15);
  cactus1_px = random(8, 15) - 5;
}

void cactiTask(void *pvParameters){
  static int timer_counter = 0;
  for(;;){
    if(!speed){
      
    }else if(timer_counter % speed){
//      if(is_lcd_suspend){vTaskResume(LCDTaskHandle); is_lcd_suspend = 0;}
      if(is_game_over){
        reset_game();
      }
      
      int cactus_new_px = cactus_px - 1;
      int cactus1_new_px = cactus1_px - 1;
      // For Cactus0
//      Serial.print(cactus_new_px);
      if(cactus_new_px <= MAXCOL && cactus_new_px >= MINCOL){
//          Serial.print("A \n");
          cactus_px = cactus_new_px;
      }else if(cactus_new_px > MAXCOL){
//          Serial.print("B \n");
          cactus_px = MINCOL + (cactus_new_px - MAXCOL);
      }else if(cactus_new_px < MINCOL){
//          Serial.print("HI \n");
          cactus_px = MAXCOL - (MINCOL - cactus_new_px);
      }

//    Count Score
      if(cactus_px == 0){
        if(dino_py != cactus_py){
          score++;
        }else{
          is_game_over = 1;
        }
      }

      // For Cactus1
//      Serial.print(cactus_new_px);
      if(cactus1_new_px <= MAXCOL && cactus1_new_px >= MINCOL){
//          Serial.print("A \n");
          cactus1_px = cactus1_new_px;
      }else if(cactus1_new_px > MAXCOL){
//          Serial.print("B \n");
          cactus1_px = MINCOL + (cactus1_new_px - MAXCOL);
      }else if(cactus1_new_px < MINCOL){
//          Serial.print("HI \n");
          cactus1_px = MAXCOL - (MINCOL - cactus1_new_px);
      }

       //    Count Score
      if(cactus1_px == 0){
        if(dino_py != cactus1_py){
          score++;
        }else{
          is_game_over = 1;
        }
      }
    }
    
    cactus_py = 15;
    cactus1_py = 0;
    timer_counter = (timer_counter + 1) % 10000;

//    Serial.print(F("L PIN0: "));
//    Serial.print(analogRead(photoPin0));
//    Serial.print(F(" | R PIN1: "));
//    Serial.print(analogRead(photoPin1));
//    Serial.print(" | speed: ");
//    Serial.print(speed);
    Serial.print(" | PosX: ");
    Serial.print(cactus_px);
//    Serial.print(" | Counter: ");
//    Serial.print(timer_counter);
//    Serial.print("\n");

    Serial.print(F(" | SCORE: "));
    Serial.print(score);
    Serial.print("\n");
    
    vTaskDelay(delay_dis);
  }
}

void LCDTask(void *pvParameters){
  static int last_cactus_px = 0, last_cactus_py = 0;
  
  for(;;){
    lcd.clear();
    
    if(is_game_over){
      lcd.setCursor(0, 0);
      lcd.print(F("Game Over"));
      lcd.setCursor(0, 1);
      lcd.print(F("Score: "));
      lcd.print(score);

//      score = 0;
//      is_game_over = 0;
//      
//      dino_px = 0;
//      dino_py = 0;
//
//      cactus_px = 10;
//      cactus_py = 0;
//      
//      cactus1_px = 7;
//      cactus1_py = 0;
//      
//      speed = 0;
      Serial.print("IN");
      Serial.print(is_game_over);
      Serial.print("\n");
//      game_over();
//      reset_game();
    }else{
      lcd.setCursor(cactus1_px, cactus1_py); 
      lcd.write(1); 
      lcd.setCursor(cactus_px, cactus_py); 
      lcd.write(1);
      lcd.setCursor(dino_px, dino_py); 
      lcd.write(0);
    }
    
    vTaskDelay(5);
    Serial.print(F("END"));
    Serial.print(is_game_over);
    Serial.print(F("\n"));
  }
}

void leftTask(void *pvParameters) {
  for( ;; ){
      int p0_read = analogRead(photoPin0);
      int p1_read = analogRead(photoPin1);
      if(p1_read - 50 > p0_read){
        if(speed < MAXSPEED){
          speed+=3;
        }
      }else if(p0_read < PHOTONSTOPTHRES && p1_read < PHOTONSTOPTHRES){
        speed = 0;
        if(!is_game_over){
//          vTaskSuspend(LCDTaskHandle); 
//          is_lcd_suspend = 1;
        }
      }
      delay(300); 
   }  
}

void rightTask(void *pvParameters) {
  for( ;; ){
      int p0_read = analogRead(photoPin0);
      int p1_read = analogRead(photoPin1);
      if(p0_read - 50 > p1_read){
        if(MINSPEED < speed){
          speed-=3;
        }
      }else if(p0_read < PHOTONSTOPTHRES && p1_read < PHOTONSTOPTHRES){
        speed = 0;
        if(!is_game_over){
//          vTaskSuspend(LCDTaskHandle); 
//          is_lcd_suspend = 1;
        }
      }
      delay(300); 
   }  
}

void keypadTask(void *pvParameters){
  for( ;; ){
    const char key = myKeypad.getKey();
    
    if(key == '2'){
      dino_py = 0;
      Serial.print(key);
      Serial.print(F("\n"));
    }else if(key == '8'){
      dino_py = 15;
      Serial.print(key);
      Serial.print(F("\n"));
    }
  }
}

void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  create_dino_cactus_cahr();
  
  xTaskCreate(cactiTask, "cactiTask", 72, NULL, 5, NULL );
  xTaskCreate(LCDTask, "LCDTask", 72, NULL, 50, &LCDTaskHandle);
  xTaskCreate(leftTask, "leftTask", 64, NULL, 5, NULL );
  xTaskCreate(rightTask, "rightTask", 64, NULL, 5, NULL );
  xTaskCreate(keypadTask, "keypadTask", 64, NULL, 5, NULL );

  vTaskStartScheduler();
}
void loop(){
//  lcd.setCursor(0,0); lcd.write(0);
//  lcd.setCursor(0,15); lcd.write(1);
  // get_over();
}
