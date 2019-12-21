/*
ESP32-CAM CameraWebServer
Author : ChungYi Fu (Kaohsiung, Taiwan)  2019-12-22 01:30
https://www.facebook.com/francefu

http://192.168.xxx.xxx             //網頁首頁管理介面
http://192.168.xxx.xxx:81/stream   //取得串流影像
http://192.168.xxx.xxx/capture     //取得影像
http://192.168.xxx.xxx/status      //取得狀態設定值

//設定視訊參數
http://192.168.xxx.xxx/control?var=framesize&val=value    // value = 10->UXGA(1600x1200), 9->SXGA(1280x1024), 8->XGA(1024x768) ,7->SVGA(800x600), 6->VGA(640x480), 5 selected=selected->CIF(400x296), 4->QVGA(320x240), 3->HQVGA(240x176), 0->QQVGA(160x120)
http://192.168.xxx.xxx/control?var=quality&val=value    // value = 10 ~ 63
http://192.168.xxx.xxx/control?var=brightness&val=value    // value = -2 ~ 2
http://192.168.xxx.xxx/control?var=contrast&val=value    // value = -2 ~ 2
http://192.168.xxx.xxx/control?var=saturation&val=value    // value = -2 ~ 2 
http://192.168.xxx.xxx/control?var=gainceiling&val=value    // value = 0 ~ 6
http://192.168.xxx.xxx/control?var=colorbar&val=value    // value = 0 or 1
http://192.168.xxx.xxx/control?var=awb&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=agc&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=aec&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=hmirror&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=vflip&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=awb_gain&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=agc_gain&val=value    // value = 0 ~ 30
http://192.168.xxx.xxx/control?var=aec_value&val=value    // value = 0 ~ 1200
http://192.168.xxx.xxx/control?var=aec2&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=dcw&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=bpc&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=wpc&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=raw_gma&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=lenc&val=value    // value = 0 or 1 
http://192.168.xxx.xxx/control?var=special_effect&val=value    // value = 0 ~ 6
http://192.168.xxx.xxx/control?var=wb_mode&val=value    // value = 0 ~ 4
http://192.168.xxx.xxx/control?var=ae_level&val=value    // value = -2 ~ 2   
*/

//輸入WIFI連線帳號密碼
const char* ssid = "*****";
const char* password = "*****";

//輸入AP端連線帳號密碼
const char* apssid = "ESP32-CAM";
const char* appassword = "12345678";         //AP密碼至少要8個字以上

#include "esp_camera.h"  //視訊函式
#include <WiFi.h>
#include "soc/soc.h"             //用於電源不穩不重開機 
#include "soc/rtc_cntl_reg.h"    //用於電源不穩不重開機 

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

//CAMERA_MODEL_AI_THINKER  指定安可信ESP32-CAM模組腳位設定
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void startCameraServer();

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //關閉電源不穩就重開機的設定
    
  Serial.begin(115200);
  Serial.setDebugOutput(true);  //開啟診斷輸出
  Serial.println();

  //視訊組態設定
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  //視訊初始化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //可動態改變視訊框架大小(解析度大小)
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);  //96x96|QQVGA|QQVGA2|QCIF|HQVGA|240x240|QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA|QXGA|INVALID

  WiFi.mode(WIFI_AP_STA);  //其他模式 WiFi.mode(WIFI_AP); WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);    //執行網路連線

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  WiFi.softAP((WiFi.localIP().toString()+"_"+(String)apssid).c_str(), appassword);   //設定SSID顯示客戶端IP 

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
