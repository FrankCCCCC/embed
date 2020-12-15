#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <queue.h>
#include <semphr.h>

// Keypad
#define KEY_ROWS 4
#define KEY_COLS 4

#define LEFT 0
#define STOP 1
#define RIGHT 2
#define MAXCOL 15
#define MINCOL 0
#define BUFXSIZE 16
#define BUFYSIZE 2
#define MAXSPEED 15
#define MINSPEED 0
#define PHOTONSTOPTHRES 550
#define EGG_ARRAY_LEN 3

#define ERR_DRAGON_ID 1
#define CACTUS_ID 2
#define EGG_ID 3
#define BROKEN_ID 4

#define button 1
#define xAxis A0
#define yAxis A1

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
char buf[BUFYSIZE][BUFXSIZE] = {0};

// Cactus & Dinosour
// typedef struct{
//   char dino_x;
//   char dino_y;
//   char cactus_x;
//   char cactus_y;
//   char broken_idx;
//   char num_eggs;
//   char egg_xs[EGG_ARRAY_LEN];
//   char egg_ys[EGG_ARRAY_LEN];
// }Char_p;
char dino_px = 0, dino_py = 0;
char cactus_px = 0, cactus_py = 0;
// char broken_idx = -1;
char num_eggs = 0;
char egg_xs[EGG_ARRAY_LEN] = {0};
char egg_ys[EGG_ARRAY_LEN] = {0};
const int delay_dis = 10;
const char cactus_dir[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, 1}};

// Task Handler
SemaphoreHandle_t binary_sem; //Global handler
SemaphoreHandle_t  gatekeeper = 0; /* global handler */
int is_lcd_suspend = 1;
TaskHandle_t LCDTaskHandle;

// Game
int score = 0;
int is_game_over = 0;
char is_success = 0;
char is_dino_move = 1;

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
  cactus_py = random(0, 2)? 1 : 0;
//   broken_idx = -1;
  num_eggs = 0;
  for(int i = 0; i < EGG_ARRAY_LEN; i++){
    egg_xs[i] = 0;
    egg_ys[i] = 0;
  }
}

void create_dino_cactus_char(){
  lcd.createChar(ERR_DRAGON_ID, err_dragon);
  lcd.createChar(CACTUS_ID, cactus);
  lcd.createChar(EGG_ID, egg);
  lcd.createChar(BROKEN_ID, broken);
}

void displayTask(void *pvParameters){
//   Char_p cp, new_cp;
  
  for(;;){
    // if(!is_game_over){
    lcd.clear();
    for(int x = 0; x < BUFXSIZE; x++){
        for(int y = 0; y < BUFYSIZE; y++){
            if(buf[y][x]){
                lcd.setCursor(x, y > 0? 15:0);
                lcd.write(buf[y][x]);
            }
        }
    }
    // }
    // lcd.clear();

    // for(int x = 0; x < BUFXSIZE; x++){
    //     for(int y = 0; y < BUFYSIZE; y++){
    //         if(buf[y][x]){
    //             lcd.setCursor(x, y > 0? 15:0);
    //             lcd.write(buf[y][x]);
    //         }
    //     }
    // }

    // if(is_game_over){
    //   vTaskDelay(90);
    //   lcd.clear();
    //   if(is_success){
    //     success();
    //   }else{
    //     fail();
    //   }
    //   vTaskDelay(30);
    // }
    vTaskDelay(5);
  }
}

void dinoTask(void *pvParameters){
  for( ;; ){
    const char key = myKeypad.getKey();

//  Control Dino
    int xVal = analogRead(xAxis);
    int yVal = analogRead(yAxis);
    // if(key == '2'){
    if(yVal < 400){
        dino_py = 0;
        is_dino_move = 1;
        xSemaphoreGiveFromISR(binary_sem, NULL);
    // }else if(key == '8'){
    }else if(yVal > 600){
        dino_py = 1;
        is_dino_move = 1;
        xSemaphoreGiveFromISR(binary_sem, NULL);
    // }else if(key == '4'){
    }else if(xVal < 400){
        dino_px = dino_px - 1 >= MINCOL? dino_px - 1 : dino_px;
        is_dino_move = 1;
        xSemaphoreGiveFromISR(binary_sem, NULL);
    // }else if(key == '6'){
    }else if(xVal > 600){
        dino_px = dino_px + 1 <= MAXCOL? dino_px + 1 : dino_px;
        is_dino_move = 1;
        xSemaphoreGiveFromISR(binary_sem, NULL);
    // }else if(key == '5'){
    }else if(digitalRead(button) == 0){
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
        is_dino_move = 1;
        xSemaphoreGiveFromISR(binary_sem, NULL);
    }else{
        is_dino_move = 0;
    }

//  For Cactus0
//  Control Cactus  
    // static int timer_counter = 0;
    // if(is_game_over){
    //   reset_game();
    // }

//  Detect Egg
    // for(int i = 0; i < num_eggs; i++){
    //   if(egg_xs[i] == cactus_px && egg_ys[i] == cactus_py){
    //     is_game_over = 1;
    //     is_success = 0;
    //     break;
    //   }
    // }
    if(xSemaphoreTake(gatekeeper, 100)){
        for(char x = 0; x < BUFXSIZE; x++){
            for(char y = 0; y < BUFYSIZE; y++){
                buf[y][x] = 0;
            }
        }

        buf[dino_py][dino_px] = ERR_DRAGON_ID;
        for(char i = 0; i < num_eggs; i++){
            buf[egg_ys[i]][egg_xs[i]] = EGG_ID;
        }
        xSemaphoreGive(gatekeeper); 
    }

    if(num_eggs == 3 && is_game_over == 0){
        is_game_over = 1; 
        is_success = 1;
        vTaskDelay(100);
    }
    if(is_success == 1 && is_game_over == 1){
        vTaskSuspend(LCDTaskHandle);
        vTaskDelay(90);
        lcd.clear();
        success();
        vTaskDelay(30);
        reset_game();
        vTaskResume(LCDTaskHandle);
    }
    // timer_counter = (timer_counter + 1) % 100;
    
    vTaskDelay(10);
  }
}

void cactusTask(void *pvParameters){
    // static char _cactus_px, _cactus_py;
    // _cactus_px = cactus_px;
    // _cactus_py = cactus_py;

    for(;;){
        if(xSemaphoreTake(gatekeeper, 100)){
            buf[cactus_py][cactus_px] = CACTUS_ID;
            //  Update Cactus position
            for(int i = 0; i < num_eggs; i++){
                if(egg_xs[i] == cactus_px && egg_ys[i] == cactus_py){
                    buf[egg_ys[i]][egg_xs[i]] = BROKEN_ID;
                    is_game_over = 1;
                    is_success = 0;
                    vTaskDelay(100);
                    break;
                }
            }
            xSemaphoreGive(gatekeeper); 
        }
        if(is_success == 0 && is_game_over == 1){
            vTaskSuspend(LCDTaskHandle);
            vTaskDelay(90);
            lcd.clear();
            fail();
            vTaskDelay(30);
            reset_game();
            vTaskResume(LCDTaskHandle);
        }

        if(xSemaphoreTake(binary_sem, 0)){
            char is_moved = 0;

            if(cactus_px + 1 < MAXCOL){
                if((!random(0, 3)) && (!is_moved)){
                cactus_px = cactus_px + 1;
                is_moved = 1;
                }
            }
            if(cactus_px - 1 > MINCOL){
                if((!random(0, 2)) && (!is_moved)){
                cactus_px = cactus_px - 1;
                is_moved = 1;
                }
            }
            if(cactus_py == 0){
                if(!is_moved){
                cactus_py = 1;
                is_moved = 1;
                }
            }
            if(cactus_py == 1){
                if(!is_moved){
                cactus_py = 0;
                is_moved = 1;
                }
            }
            is_dino_move = 0;
        }
    }

}

void setup(){
    pinMode(button, INPUT_PULLUP); //return LOW when down
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    create_dino_cactus_char();
    
    vSemaphoreCreateBinary(binary_sem);
    gatekeeper = xSemaphoreCreateMutex();
    
    reset_game();
    xTaskCreate(displayTask, "displayTask", 128, NULL, 5, &LCDTaskHandle);
    xTaskCreate(dinoTask, "dinoTask", 128, NULL, 5, NULL );
    xTaskCreate(cactusTask, "cactusTask", 128, NULL, 5, NULL );

    vTaskStartScheduler();
    Serial.println("HI");
}
void loop(){
}
