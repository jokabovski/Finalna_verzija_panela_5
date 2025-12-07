/**************************CrowPanel ESP32 HMI Display Example Code************************
Version     :	1.1
Suitable for:	CrowPanel ESP32 HMI Display
Product link:	https://www.elecrow.com/esp32-display-series-hmi-touch-screen.html
Code	  link:	https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File
Lesson	link:	https://www.youtube.com/watch?v=WHfPH-Kr9XU
Description	:	The code is currently available based on the course on YouTube, 
				        if you have any questions, please refer to the course video: Introduction 
				        to ask questions or feedback.
**************************************************************/


#include <Wire.h>
#include <SPI.h>

/**************************LVGL and UI************************
if you want to use the LVGL demo. you need to include <demos/lv_demos.h> and <examples/lv_examples.h>. 
if not, please do not include it. It will waste your Flash space.
**************************************************************/
#include <lvgl.h>
#include "ui.h"

//moje stvari
#include "screen_timeout.h"
#include "xl9535_relay.h"
#include "ui_input_helpers.h" //za promenjive da se vide na displeju kad se izmene


// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>
/**************************LVGL and UI END************************/

/*******************************************************************************
   Config the display panel and touch panel in gfx_conf.h
 ******************************************************************************/
#include "gfx_conf.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf1[screenWidth * screenHeight / 10];
static lv_color_t disp_draw_buf2[screenWidth * screenHeight / 10];
static lv_disp_drv_t disp_drv;


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = ( area->x2 - area->x1 + 1 );
   uint32_t h = ( area->y2 - area->y1 + 1 );

   tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);

   lv_disp_flush_ready( disp );

}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
   uint16_t touchX, touchY;
   bool touched = tft.getTouch( &touchX, &touchY);
   if( !touched )
   {
      data->state = LV_INDEV_STATE_REL;
   }
   else
   {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;

      /*
      Serial.print( "Data x " );
      Serial.println( touchX );

      Serial.print( "Data y " );
      Serial.println( touchY );
      */
      // Reset screen timeout on touch -> dodata f-ja
      screenTimeout.resetTimeout();
   }
}

//Moj kod za I2C

void scanI2C() {
    Serial.println("Scanning I2C bus...");
    
    int deviceCount = 0;
    
    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");
            deviceCount++;
            
            // Identify common devices
            if (address == 0x14) Serial.println("  -> GT911 Touch Controller");
            if (address == 0x20) Serial.println("  -> This could be your XL9535 relay board!");
            if (address >= 0x20 && address <= 0x27) Serial.println("  -> XL9535 address range");
        }
    }
    
    if (deviceCount == 0) {
        Serial.println("\n*** NO I2C DEVICES FOUND ***");
    } else {
        Serial.print("\nTotal devices found: ");
        Serial.println(deviceCount);
    }
    
    Serial.println("=== Scan Complete ===\n");
}

//Pomcne f-je za manuelnu kontrolu releja

// Bridge functions for C code to call relay functions
extern "C" {
    void relay_on_bridge(uint8_t relay) {
        relayBoard.relayOn(relay);
    }
    
    void relay_off_bridge(uint8_t relay) {
        relayBoard.relayOff(relay);
    }
    
    void relay_all_off_bridge(void) {
        relayBoard.allOff();
    }
}




void setup()
{
  Serial.begin(9600);
  

  delay(2000);  // Wait for Serial Monitor
  Serial.println("LVGL Widgets Demo");

  //GPIO init
#if defined (CrowPanel_50) || defined (CrowPanel_70)
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(17, OUTPUT);
  digitalWrite(17, LOW);
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
  pinMode(42, OUTPUT);
  digitalWrite(42, LOW);
#elif defined (CrowPanel_43)
  pinMode(20, OUTPUT);
  digitalWrite(20, LOW);
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
  pinMode(35, OUTPUT);
  digitalWrite(35, LOW);
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);//TOUCH-CS
#endif

  //Display Prepare
  tft.begin();
  Serial.println("Display started");
  
  // MANUALLY INITIALIZE I2C - this is the fix!
  Wire.begin(19, 20);  // SDA=19, SCL=20 for CrowPanel 50
  delay(100);
  Serial.println("I2C manually initialized on pins SDA=19, SCL=20");

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  delay(200);

  lv_init();

  delay(100);

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, screenWidth * screenHeight/10);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.full_refresh = 1;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  tft.fillScreen(TFT_BLACK);

  //please do not use LVGL Demo and UI export from Squareline Studio in the same time.
  // lv_demo_widgets();    // LVGL demo
  ui_init();
  
  Serial.println( "Setup done" );


  //Ovde stavljas svoj kod!

   // NOW scan I2C - after tft.begin() initialized it!
    Serial.println("\n=== Scanning I2C Bus ===");
    scanI2C();

    // Initialize screen timeout manager
    screenTimeout.begin(30000, 0, 255);

    // Initialize relay board
    Serial.println("\n=== Initializing Relay Board ===");
    if (relayBoard.begin(0x20)) {
      Serial.println("✓ Relay board ready!");
      //relayBoard.relayOn(5);
    } else {
      Serial.println("✗ Relay board initialization failed!");
    }
}

void loop()
{
    lv_timer_handler();

    relayBoard.relayOn(5);

    // Update screen timeout manager
    screenTimeout.update();  
    //
    delay(5);
}
