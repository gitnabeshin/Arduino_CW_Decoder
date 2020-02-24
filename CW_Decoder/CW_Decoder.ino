/**************************************************************************
  CW DECODER

***************************************************************************/
#include "DecoderConfig.h"
#include "Decode.h"
#include "Lcd.h"

typedef enum CTRL_MODE {
  DECODE_MODE,  //Decode
  MENU_MODE     //Menu
} CTRL_MODE_T;

//コントロールモード
CTRL_MODE_T ctrlMode;

int menuPinState = HIGH;//入力ピンの状態（初期値HIGH）

void setup() {

  Serial.begin( 9600 );

  //電鍵の接続PINを初期化
  pinMode( SIGNAL_PIN, INPUT_PULLUP );

  //ボタン初期化
  pinMode( MENU_PIN,    INPUT_PULLUP );
  pinMode( SELECT_PIN,  INPUT_PULLUP );
  pinMode( UP_PIN,      INPUT_PULLUP );
  pinMode( DOWN_PIN,    INPUT_PULLUP );

  LCD_init();
  DECODE_init();

  //モード初期化
  ctrlMode = DECODE_MODE;

  //スタートアップ表示
  LCD_goToDecode();
}

/*
 * 押されたボタンに応じた処理を実行する
 */
void loop() {

  //MENUボタン押下CHECK
  menuPinState = digitalRead( MENU_PIN );

  //モード毎の処理
  switch ( ctrlMode ) {
    case DECODE_MODE:
      if ( menuPinState == LOW ) { //Menu押下
        ctrlMode = MENU_MODE; //Menuモードに切り替え
        Serial.println("-- Go To MENU MODE---");
        LCD_openMenu();  //Menu 表示
        delay(300);
      } else {
        if( !digitalRead( SELECT_PIN ) ){
          //文字バッファをクリア
          tone( SPEAKER_PIN, 1200, 120 );
          LCD_resetAll();
        } else {
          //モールス信号デコード
          DECODE_execDecode();           
        }
      }
      break;
    case MENU_MODE:
      if ( menuPinState == LOW ) {   //Menu(決定ボタン)押下
        ctrlMode = DECODE_MODE; //デコードモードへ切り替え
        Serial.println("-- Go To DECODE MODE---");
        LCD_goToDecode(); //デコードモードに復帰
        delay(300);
      } else {
        Serial.println("--MENU MODE---");
        //メニューモード
        LCD_selectMenu();
        delay(100);
      }
      break;
    default:
      break;
  }
}
