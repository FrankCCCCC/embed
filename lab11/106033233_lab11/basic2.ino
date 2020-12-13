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
#define EGG_ARRAY_LEN 3

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
byte egg[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};
byte broken[8] = {B00100, B10101, B01110, B11111, B11111, B01110, B10101, B00100};
byte empty[0];

// Cactus & Dinosour
typedef struct{
  char dino_x;
  char dino_y;
  char cactus_x;
  char cactus_y;
  char broken_idx;
  char num_eggs;
  char egg_xs[EGG_ARRAY_LEN];
  char egg_ys[EGG_ARRAY_LEN];
}Char_p;
char dino_px = 0, dino_py = 0;
char cactus_px = 0, cactus_py = 0;
char broken_idx = -1;
char num_eggs = 0;
char egg_xs[EGG_ARRAY_LEN] = {0};
char egg_ys[EGG_ARRAY_LEN] = {0};
const int delay_dis = 10;
const char cactus_dir[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, 1}};

// Task Handler
QueueHandle_t Global_Queue_Handle; //Global Handler
int is_lcd_suspend = 1;
TaskHandle_t LCDTaskHandle;

// Game
int score = 0;
int is_game_over = 0;
char is_success = 0;

void success(){
  lcd.setCursor(0, 0);
  lcd.print(F("Succeed!"));
}

void fail(){
  lcd.setCursor(0, 0);
  lcd.print(F("Game Over"));
}

void reset_game(){
  score = 0;
  is_game_over = 0;
  
  dino_px = 0;
  dino_py = 0;
  cactus_px = random(8, 10);
  cactus_py = random(0, 2)? 15 : 0;
  broken_idx = -1;
  num_eggs = 0;
  for(int i = 0; i < EGG_ARRAY_LEN; i++){
    egg_xs[i] = 0;
    egg_ys[i] = 0;
  }

  Char_p cp;
  cp.dino_x = dino_px;
  cp.dino_y = dino_py;
  cp.cactus_x = cactus_px;
  cp.cactus_y = cactus_py;
  cp.broken_idx = broken_idx;
  cp.num_eggs = num_eggs;
  for(int i = 0; i < EGG_ARRAY_LEN; i++){
    cp.egg_xs[i] = egg_xs[i];
    cp.egg_ys[i] = egg_ys[i];
  }
  if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
//        Serial.println("Failed to send to queue");
  }
}

void create_dino_cactus_char(){
  lcd.createChar(0, err_dragon);
  lcd.createChar(1, cactus);
  lcd.createChar(2, egg);
  lcd.createChar(3, broken);
}

void displayTask(void *pvParameters){
  Char_p cp, new_cp;
  
  for(;;){
    lcd.clear();
    
    if(xQueueReceive(Global_Queue_Handle,&new_cp,0)){
//        Serial.println("receive value:");
//        Serial.println(rx_int);
        cp = new_cp;
    }

    lcd.setCursor(cp.cactus_x, cp.cactus_y); 
    lcd.write(1);
    lcd.setCursor(cp.dino_x, cp.dino_y); 
    lcd.write(0);
    for(int i = 0; i < cp.num_eggs; i++){
      lcd.setCursor(cp.egg_xs[i], cp.egg_ys[i]);
      lcd.write(2);
    }
    if(cp.broken_idx >= 0){
      lcd.setCursor(cp.egg_xs[broken_idx], cp.egg_ys[broken_idx]);
      lcd.write(3);
    }

    if(is_game_over){
      vTaskDelay(90);
      lcd.clear();
      if(is_success){
        success();
      }else{
        fail();
      }
      vTaskDelay(30);
    }
    vTaskDelay(5);
  }
}

void controlTask(void *pvParameters){
  for( ;; ){
    const char key = myKeypad.getKey();
    Char_p cp;

//  Control Dino
    if(key == '2'){
      dino_py = 0;
    }else if(key == '8'){
      dino_py = 15;
    }else if(key == '4'){
      dino_px = dino_px - 1 >= MINCOL? dino_px - 1 : dino_px;
    }else if(key == '6'){
      dino_px = dino_px + 1 <= MAXCOL? dino_px + 1 : dino_px;
    }else if(key == '5'){
      char is_repeat = 0;
      for(char i = 0; i < num_eggs; i++){
        if(dino_px == egg_xs[num_eggs] && dino_py == egg_ys[num_eggs]){
          is_repeat = 1;
          break;
        }
      }
      if(num_eggs < EGG_ARRAY_LEN && is_repeat == 0){
        egg_xs[num_eggs] = dino_px;
        egg_ys[num_eggs] = dino_py;
        num_eggs += 1;
      }
    }

//  For Cactus0
//  Control Cactus  
    static int timer_counter = 0;
    if(is_game_over){
      reset_game();
    }

//  Detect Egg
    for(int i = 0; i < num_eggs; i++){
      if(egg_xs[i] == cactus_px && egg_ys[i] == cactus_py){
        is_game_over = 1;
        is_success = 0;
        broken_idx = i;
        break;
      }
    }
    if(num_eggs == 3 && is_game_over == 0){is_game_over = 1; is_success = 1;}
//  Update Cactus position
    cactus_px = random(8, 10);
    cactus_py = random(0, 2)? 15 : 0;
    
    cp.dino_x = dino_px;
    cp.dino_y = dino_py;
    cp.cactus_x = cactus_px;
    cp.cactus_y = cactus_py;
    cp.broken_idx = broken_idx;
    cp.num_eggs = num_eggs;
    for(char i = 0; i < EGG_ARRAY_LEN; i++){
      cp.egg_xs[i] = egg_xs[i];
      cp.egg_ys[i] = egg_ys[i];
    }
    if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
//        Serial.println("Failed to send to queue");
    }
    timer_counter = (timer_counter + 1) % 100;
    
    vTaskDelay(10);
  }
}

void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  create_dino_cactus_char();

  Global_Queue_Handle = xQueueCreate(3, sizeof(Char_p));
  
  // Char_p cp;
  // cp.dino_x = dino_px;
  // cp.dino_y = dino_py;
  // cp.cactus_x = cactus_px;
  // cp.cactus_y = cactus_py;
  // cp.broken_idx = broken_idx;
  // cp.num_eggs = num_eggs;
  // for(int i = 0; i < EGG_ARRAY_LEN; i++){
  //   cp.egg_xs[i] = egg_xs[i];
  //   cp.egg_ys[i] = egg_ys[i];
  // }
  // if(!xQueueSend(Global_Queue_Handle, &cp, 10)){
  //       // Serial.println("Failed to send to queue");
  // }
  reset_game();
  xTaskCreate(displayTask, "displayTask", 128, NULL, 5, &LCDTaskHandle);
  xTaskCreate(controlTask, "controlTask", 128, NULL, 5, NULL );

  vTaskStartScheduler();
  Serial.println("HI");
}
void loop(){
}
