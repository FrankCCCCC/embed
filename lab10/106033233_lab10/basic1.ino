#include <Arduino_FreeRTOS.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#define LEFT 0
#define STOP 1
#define RIGHT 2
#define MAXCOL 15
#define MINCOL 0
#define MAXSPEED 15
#define MINSPEED 0
#define PHOTONSTOPTHRES 550

//LCD 
LiquidCrystal_I2C lcd(0x27,16,2);
byte dinosaur[8]  = {0x07, 0x05, 0x06, 0x07, 0x14, 0x17, 0x0E, 0x0A};
byte cactus[8] = {0x04, 0x05, 0x15, 0x15, 0x16, 0x0C, 0x04, 0x04};
byte empty[0];

// Cactus & Dinosour
int dino_px = 0, dino_py = 0;
int cactus_px = 0, cactus_py = 0, cactus_mov_dir = 0;
const int delay_dis = 10;

// Photon detect
const int photoPin0 = A0;
const int photoPin1 = A1;
int speed = 0;

// Task Handler
int is_lcd_suspend = 1;
TaskHandle_t LCDTaskHandle;

void create_dino_cactus_cahr(){
  lcd.createChar(0, dinosaur);
  lcd.createChar(1, cactus);
  cactus_px = random(8, 15);
}

void cactiTask(void *pvParameters){
  static int timer_counter = 0;
  for(;;){
//    if(cactus_mov_dir > 0){
//      if(cactus_px < MAXCOL){
//        cactus_px++;
//      }
//    }else if(cactus_mov_dir < 0){
//      if(cactus_px > 0){
//        cactus_px--;
//      }
//    }
    if(!speed){
      
      
    }else if(timer_counter % speed){
      if(is_lcd_suspend){vTaskResume(LCDTaskHandle); is_lcd_suspend = 0;}
      
      int cactus_new_px = cactus_px - 1;
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
    }
    cactus_py = 15;
    timer_counter = (timer_counter + 1) % 10000;
    
//    Jump Speed Control
//    int cactus_new_px = cactus_px - speed;
//    if(cactus_new_px < MAXCOL && cactus_new_px > MINCOL){
//        cactus_px = cactus_new_px;
//    }else if(cactus_new_px > MAXCOL){
//        cactus_px = MINCOL + (cactus_new_px - MAXCOL);
//    }else if(cactus_new_px < MINCOL){
//        cactus_px = MAXCOL - (MINCOL - cactus_new_px);
//    }

//    else{
//      vTaskSuspend()
//    }

    Serial.print("L PIN0: ");
    Serial.print(analogRead(photoPin0));
    Serial.print(" | R PIN1: ");
    Serial.print(analogRead(photoPin1));
    Serial.print(" | speed: ");
    Serial.print(speed);
    Serial.print(" | PosX: ");
    Serial.print(cactus_px);
    Serial.print(" | Counter: ");
    Serial.print(timer_counter);
    Serial.print("\n");
    
    
    vTaskDelay(delay_dis);
  }
}

void LCDTask(void *pvParameters){
  static int last_cactus_px = 0, last_cactus_py = 0;
  
  for(;;){
      lcd.clear();
//    lcd.setCursor(dino_px, dino_py); 
//    lcd.write(0);
//    if(cactus_px != last_cactus_px || cactus_py != last_cactus_py){
//      lcd.setCursor(cactus_px, cactus_py); 
//      lcd.write(1); 

      lcd.setCursor(cactus_px, 0); 
      lcd.write(1); 
      lcd.setCursor(cactus_px, cactus_py); 
      lcd.write(1); 
//      last_cactus_px = cactus_px;
//      last_cactus_py = cactus_py;
//    }
    vTaskDelay(delay_dis);
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
        vTaskSuspend(LCDTaskHandle);
        is_lcd_suspend = 1;
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
        vTaskSuspend(LCDTaskHandle);
        is_lcd_suspend = 1;
      }
      delay(300); 
   }  
}


void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  create_dino_cactus_cahr();
  
  xTaskCreate(cactiTask, "cactiTask", 128, NULL, 1, NULL );
  xTaskCreate(LCDTask, "LCDTask", 128, NULL, 1, &LCDTaskHandle);
  xTaskCreate(leftTask, "leftTask", 128, NULL, 1, NULL );
  xTaskCreate(rightTask, "rightTask", 128, NULL, 1, NULL );
}
void loop(){
//  lcd.setCursor(0,0); lcd.write(0);
//  lcd.setCursor(0,15); lcd.write(1);
}
