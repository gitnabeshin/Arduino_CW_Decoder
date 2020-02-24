/**************************************************************************
 LCD表示制御
  
  0. メニュー
  1. モード切り替え（和文、英文）
  2. サンプリング周期変更
  3. //TODO   テストモード * -
  4. //TODO   リファレンス音の再生
  
 *************************************************************************/
#include "Arduino.h"
#include "DecoderConfig.h"
#include "Lcd.h"
#include "Decode.h"
#include <LiquidCrystal_I2C.h>

#define MAX_MENU_INDEX (2) //menu[] の最大インデックス

LiquidCrystal_I2C lcd(0x27, 16, 2);

int mIndex1 = 0;        //現在のメニュー位置
int lcdindex = 0;       //LCD 現在のカーソル位置
int line1[LCD_COLUMNS]; //LCD 1行目の文字列バッファ
int line2[LCD_COLUMNS]; //LCD 2行目の文字列バッファ

typedef enum MENU_MODE {
  MENU_INIT,          //メニュー選択状態
  MENU_LANG,          //和文英文選択状態
  MENU_INTERVAL,      //サンプリング周期設定状態
  MENU_INTERVAL_SUB,  //サンプリング周期変更中状態

} MENU_MODE_T;

//コントロールモード
MENU_MODE_T menuMode;

//メニューに表示する文字列
String menu[] = {
  "MENU (1-2)",     //0
  "1.[LANG](0-1)",  //1
  "2.[INTERVAL]",   //2
};

//ボタン種別
typedef enum {
  BUTTON_MENU,    //メニューボタン
  BUTTON_SELECT,  //選択ボタン
  BUTTON_UP,      //UPボタン
  BUTTON_DOWN,    //DOWNボタン

  BUTTON_NONE
} BUTTONS;

//初期化
void LCD_init() {
  lcd.begin();    //LCDの桁数と行数をセット
}

//MENUモードに入る
void LCD_openMenu() {
  menuMode = MENU_INIT;
  mIndex1 = 0;
  tone( SPEAKER_PIN, 1319, 300 );

  lcd.clear();
  lcd.setCursor( 0, 0 );
  lcd.print( menu[ mIndex1 ] );
}

//デコードモードに入る
void LCD_goToDecode() {
  //line1とline2を消去 space(" ":32)でパディング
  for (int index = 0; index < LCD_COLUMNS; index++ ) {
    line1[index] = 32;  //32=" "
    line2[index] = 32;
  }

  //スタートアップ表示
  tone( SPEAKER_PIN, 1319, 300 );
  lcd.setCursor( 0, 0 );
  lcd.print( "CW Decorder" );
  lcd.setCursor( 0, 1 );
  lcd.print( "BaseTime " );
  lcd.print( DECODE_getInterval() );
  lcd.print( "(ms)" );
  delay( 1300 );
  tone( SPEAKER_PIN, 784, 500 );
  delay(180);
  tone( SPEAKER_PIN, 1047, 100 );
  delay( 90 );
  lcd.clear();
  lcdindex = 0;
  if ( DECODE_isWabun() ) {
    lcd.print("Japanese >>");
  } else {
    lcd.print("English >>");
  }
}

//押されたボタンIDを返す
int getButtonEvent() {
  if ( !digitalRead( SELECT_PIN ) ) {
    Serial.println("BUTTON_SELECT");
    tone( SPEAKER_PIN, 1200, 120 );
    return BUTTON_SELECT;
  } else if ( !digitalRead( UP_PIN ) ) {
    Serial.println("BUTTON_UP");
    tone( SPEAKER_PIN, 900, 120 );
    return BUTTON_UP;
  } else if ( !digitalRead( DOWN_PIN ) ) {
    Serial.println("BUTTON_DOWN");
    tone( SPEAKER_PIN, 500, 120 );
    return BUTTON_DOWN;
  } else {
    return BUTTON_NONE;
  }
}


// 1.[MENU_LANG]モード
//   SELECTボタンで和文、英文モードを選択
void selectLang() {
  //選択ボタンが押されたかチェック
  switch ( getButtonEvent() ) {
    case BUTTON_SELECT:
      if ( DECODE_isWabun() ) {
        //和文モードだったら、英文モードにスイッチ
        DECODE_setEnglishMode();
        lcd.setCursor( 0, 1 );
        lcd.print( " English    " );
      } else {
        //英文モードだったら、和文モードにスイッチ
        DECODE_setWabunMode();
        lcd.setCursor( 0, 1 );
        lcd.print( " Japanese   " );
      }
      break;
    default:
      //何もしないでリターン
      return;
      break;
  }
}

// 2.[MENU_INTERVAL]モード
//   サンプリング周期を変更する
void selectInterval() {
  if( menuMode == MENU_INTERVAL_SUB ){
    //変更後のインターバル
    int nextInterval = DECODE_getInterval();
    //選択ボタンが押されたかチェック
    switch ( getButtonEvent() ) {
      case BUTTON_UP:
        //10ms 長くする
        nextInterval = nextInterval + 10;
        break;
      case BUTTON_DOWN:
        //10ms 長くする
        nextInterval = nextInterval - 10;
        break;
      default:
        //何もしないでリターン
        return;
        break;
    }
  
    DECODE_setInterval(nextInterval);
    lcd.setCursor( 0, 1 );
    for (int index = 0; index < LCD_COLUMNS; index++ ) {
      lcd.write(32);  //32=" "
    }
    lcd.setCursor( 0, 1 );
    lcd.print( " " );
    lcd.print( DECODE_getInterval() );
    lcd.print( " ms" );
   
  } else {
     //選択ボタンが押されたかチェック
    switch ( getButtonEvent() ) {
      case BUTTON_SELECT:
        if ( menuMode != MENU_INTERVAL_SUB ) {
          //和文モードだったら、英文モードにスイッチ
          menuMode = MENU_INTERVAL_SUB;
        }
        break;
      default:
        //何もしないでリターン
        return;
      break;
    }
  }
}

//上下ボタンでMenuを選択
void selectMenu() {

  //上下ボタンを数字増減に使いたいので特別扱い　
  if( menuMode == MENU_INTERVAL_SUB ){
    return;
  }
  
  switch ( getButtonEvent() ) {
    case BUTTON_UP:
      if ( mIndex1 + 1 < MAX_MENU_INDEX ) {
        mIndex1++;
      } else {
        mIndex1 = MAX_MENU_INDEX;
      }
      break;
    case BUTTON_DOWN:
      if ( mIndex1 - 1 > 0 ) {
        mIndex1--;
      } else {
        mIndex1 = 0;
      }
      break;
    default:
      //何もしないでリターン
      return;
      break;
  }

  lcd.clear();
  lcd.setCursor( 0, 0 );
  lcd.print( menu[ mIndex1 ] );

  //モード切り替え
  switch ( mIndex1 ) {
    case 0:
      menuMode = MENU_INIT;
      break;
    case 1:
      menuMode = MENU_LANG;
      break;
    case 2:
      menuMode = MENU_INTERVAL;
      break;
    default:
      break;
  }
}


//MENU制御メイン
void LCD_selectMenu() {

  //上下ボタンでメニューを選択
  selectMenu();

  //選択したモードに応じた処理を呼び出す
  switch ( menuMode ) {
    case MENU_INIT:
      Serial.println("    --MENU INIT---");   
      break;
    case MENU_LANG:
      Serial.println("    --MENU LANG---");
      lcd.setCursor( 0, 1 );
      if ( DECODE_isWabun() ) {
        lcd.print( " Japanese   " );
      } else {
        lcd.print( " English    " );
      }
      selectLang();
      break;
    case MENU_INTERVAL:
      Serial.println("    --MENU INTERVAL---");
      lcd.setCursor( 0, 1 );
      lcd.print( " " );
      lcd.print( DECODE_getInterval() );
      lcd.print( " change?" );
      selectInterval();
      break;
    case MENU_INTERVAL_SUB:
      Serial.println("    --MENU INTERVAL SUB---");
      selectInterval();
      break;
    default:
      //
      break;
  }
}

// LCDにデコードした文字を出力する
void LCD_printAscii( int asciiNumber ) {

  //2行目に出力中の文字が最大文字数を超えた場合、改行（１行目に文字をコピーして２行目を空に）する
  if ( lcdindex > LCD_COLUMNS - 1 ) {
    lcdindex = 0;
    for ( int i = 0; i <= LCD_COLUMNS - 1; i++ ) {
      lcd.setCursor( i, LCD_ROWS - 2 );
      lcd.write( line1[i] );
      lcd.setCursor( i, LCD_ROWS - 1 );
      lcd.write(32);
    }
  }

  //通常表示(LCD最大文字数以下の場合)
  line1[lcdindex] = asciiNumber;
  lcd.setCursor( lcdindex, LCD_ROWS - 1 );
  lcd.write( asciiNumber );
  lcdindex += 1;
}

// デコードした文字をクリアする
void LCD_resetAll() {
  lcd.clear();
  lcdindex = 0;
    if ( DECODE_isWabun() ) {
    lcd.print("Japanese >>");
  } else {
    lcd.print("English >>");
  }
}
