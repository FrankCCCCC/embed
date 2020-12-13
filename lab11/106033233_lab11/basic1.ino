#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <queue.h>

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
byte err_dragon[8]  = {B00111, B00101, B00110, B00111, B10100, B10111, B01110, B01010};
byte dinosaur[8]  = {0x07, 0x05, 0x06, 0x07, 0x14, 0x17, 0x0E, 0x0A};
byte cactus[8] = {0x04, 0x05, 0x15, 0x15, 0x16, 0x0C, 0x04, 0x04};
byte empty[0];

// Cactus & Dinosour
typedef struct{
  int dino_x;
  int dino_y;
  int cactus_x;
  int cactus_y;
  int cactus1_x;
  int cactus1_y;
}Char_p;
int dino_px = 0, dino_py = 0;
int cactus_px = 0, cactus_py = 0;
int cactus1_px = 0, cactus1_py = 0;
const int delay_dis = 10;

// Photon detect
const int photoPin0 = A0;
const int photoPin1 = A1;
int speed = 0;

// Task Handler
QueueHandle_t Global_Queue_Handle; //Global Handler
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
  
  cactus_px = 13;
  cactus_py = 15;
  
  cactus1_px = 10;
  cactus1_py = 0;

  Char_p cp;
  cp.dino_x = 0;
  cp.dino_y = 0;
  cp.cactus_x = 13;
  cp.cactus_y = 15;
  cp.cactus1_x = 10;
  cp.cactus1_y = 0;
  if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
//        Serial.println("Failed to send to queue");
  }

  speed = 2;
}

void create_dino_cactus_cahr(){
  lcd.createChar(0, err_dragon);
  lcd.createChar(1, cactus);
  cactus_px = random(8, 15) + 2;
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
//    Count Score
      if(!(dino_py == cactus_py && dino_px == cactus_px)){
        score++;
      }else{
        is_game_over = 1;
      }
      
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

// For Cactus1
//    Count Score
      if(!(dino_py == cactus1_py && dino_px == cactus1_px)){
        score++;
      }else{
        is_game_over = 1;
      }
      
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
//    Serial.print(" | PosX: ");
//    Serial.print(cactus_px);
//    Serial.print(" | Counter: ");
//    Serial.print(timer_counter);
//    Serial.print("\n");

//    Serial.print(F(" | SCORE: "));
//    Serial.print(score);
//    Serial.print("\n");
    
    vTaskDelay(delay_dis);
  }
}

void displayTask(void *pvParameters){
  static int last_cactus_px = 0, last_cactus_py = 0;
  Char_p cp, new_cp;
  cp.dino_x = 0;
  cp.dino_y = 0;
  cp.cactus_x = random(8, 15) + 2;
  cp.cactus_y = 15;
  cp.cactus1_x = random(8, 15) - 5;;
  cp.cactus1_y = 0;
  
  for(;;){
    lcd.clear();
    
    if(is_game_over){
      lcd.setCursor(0, 0);
      lcd.print(F("Game Over"));
      lcd.setCursor(0, 1);
      lcd.print(F("Score: "));
      lcd.print(score);

      vTaskDelay(30);
    }else{
      if(xQueueReceive(Global_Queue_Handle,&new_cp,0)){
//        Serial.println("receive value:");
//        Serial.println(rx_int);
          cp = new_cp;
      }

      lcd.setCursor(cp.cactus1_x, cp.cactus1_y); 
      lcd.write(1); 
      lcd.setCursor(cp.cactus_x, cp.cactus_y); 
      lcd.write(1);
      lcd.setCursor(cp.dino_x, cp.dino_y); 
      lcd.write(0);
    }

    vTaskDelay(5);
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

void controlTask(void *pvParameters){
  for( ;; ){
    const char key = myKeypad.getKey();
    Char_p cp;

//    Control Dino
    if(key == '2'){
      dino_py = 0;
    }else if(key == '8'){
      dino_py = 15;
    }else if(key == '4'){
      dino_px = dino_px - 1 >= MINCOL? dino_px - 1 : dino_px;
    }else if(key == '6'){
      dino_px = dino_px + 1 <= MAXCOL? dino_px + 1 : dino_px;
    }
    
//  Control Cactus  
    static int timer_counter = 0;
    if(!speed){
      
    }else if(timer_counter % speed){
//      if(is_lcd_suspend){vTaskResume(LCDTaskHandle); is_lcd_suspend = 0;}
      if(is_game_over){
        reset_game();
      }
      
      int cactus_new_px = cactus_px - 1;
      int cactus1_new_px = cactus1_px - 1;

// For Cactus0
//    Count Score
      if(!(dino_py == cactus_py && dino_px == cactus_px)){
        score++;
      }else{
        is_game_over = 1;
      }
      
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

// For Cactus1
//    Count Score
      if(!(dino_py == cactus1_py && dino_px == cactus1_px)){
        score++;
      }else{
        is_game_over = 1;
      }
      
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
    }
    
    cactus_py = 15;
    cactus1_py = 0;

    cp.dino_x = dino_px;
    cp.dino_y = dino_py;
    cp.cactus_x = cactus_px;
    cp.cactus_y = cactus_py;
    cp.cactus1_x = cactus1_px;
    cp.cactus1_y = cactus1_py;
    if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
//        Serial.println("Failed to send to queue");
    }
    timer_counter = (timer_counter + 1) % 10000;
    
    vTaskDelay(10);
  }
}

void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  create_dino_cactus_cahr();

  Global_Queue_Handle = xQueueCreate(5, sizeof(Char_p));
  
//  xTaskCreate(cactiTask, "cactiTask", 64, NULL, 5, NULL );
  xTaskCreate(displayTask, "displayTask", 128, NULL, 5, &LCDTaskHandle);
//  xTaskCreate(leftTask, "leftTask", 64, NULL, 5, NULL );
//  xTaskCreate(rightTask, "rightTask", 64, NULL, 5, NULL );
  xTaskCreate(controlTask, "controlTask", 64, NULL, 5, NULL );

  vTaskStartScheduler();

  Char_p cp;
  cp.dino_x = 0;
  cp.dino_y = 0;
  cp.cactus_x = random(8, 15) + 2;
  cp.cactus_y = 15;
  cp.cactus1_x = random(8, 15) - 5;;
  cp.cactus1_y = 0;
  if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
        Serial.println("Failed to send to queue");
  }
}
void loop(){
}
