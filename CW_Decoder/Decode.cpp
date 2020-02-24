/**************************************************************************
   モールス信号デコード処理

    refferd from http://jh7ubc.web.fc2.com/arduino/CW_decoder.html
 **************************************************************************/
#include "Arduino.h"
#include "DecoderConfig.h"
#include "Decode.h"
#include "Lcd.h"

long startLOW;      //LOW状態が始まった時刻
long startHIGH;     //HIGH状態が始まった時刻
long lowTime;       //LOW状態が続いた時間（ms）
long noSigTime;     //無信号時の経過時間(ms)

int preState = HIGH;//入力ピンの前の状態（初期値HIGH）

byte Mcode = 1;     //モールス符号内部コード
byte data;          //dot（短点）のときdata=0,dash（長点）のときdata=1
byte bitcount = 0;  //入力bit数

boolean space   = false;  //spaceを送出するかどうかのflag
boolean isWabun = false;   //和文モードflag

int dotTime = DEFAULT_DOT_TIME; //モールス信号の短点（・）と長点（－）のしきい値 ms 
int Ltime = 100;                //dot(lowTime)の一時記憶,初期値100ms

void decodeEn(void);
void decodeJP(void);

//和文モードに切り替える
void DECODE_setWabunMode() {
  isWabun = true;
}

//英文モードに切り替える
void DECODE_setEnglishMode() {
  isWabun = false;
}

//現在のモードが和文モードの場合はtrue, 英文モードならfalseを返す
boolean DECODE_isWabun() {
  return isWabun;
}

//モールス信号の短点（・）と長点（－）のしきい値 ms を返す
int DECODE_getInterval(void){
  return dotTime;
}
//モールス信号の短点（・）と長点（－）のしきい値 ms をセットする
void DECODE_setInterval(int interval){
  dotTime = interval;
}

//英語モードで初期化
void DECODE_init() {
  DECODE_setWabunMode();
}

//デコード
void DECODE_execDecode() {

  //入力ピンの状態を読む
  int state = digitalRead( SIGNAL_PIN );

  //押している間だけSpeaker音を鳴らす
  if ( state == HIGH ) {
    noTone( SPEAKER_PIN );
    Serial.println(1);
  } else {
    tone( SPEAKER_PIN, 523, 10 );
    Serial.println(0);
  }

  //スイッチが離れたら(信号がLOWからHIGHに変わったら)
  if ( state == HIGH && preState == LOW ) {
    startHIGH = millis();               //HIGH状態が始まった時刻
    lowTime = millis() - startLOW;      //LOW状態が続いた時間(押していた時間)(ms)
    if (lowTime < dotTime) {            //押していた時間が基準時間（dotTime）より短ければ
      data = 0;                         // * :dot: 「トン」
      Ltime = lowTime;                  //lowTimeを記憶
    } else {
      data = 1;                         // - :dash: 「ツー」
    }
    bitcount++;
    Mcode = Mcode << 1;
    Mcode = Mcode | data;

    if ( bitcount > 8 ) {    //8 bit 以上はerror
      Mcode = 1;
      bitcount = 0;
    }
  }

  //スイッチが押されたら(信号がHIGHからLOWに変わったら)
  if ( state == LOW && preState == HIGH ) {
    startLOW = millis();
  }

  //入力が終わったらデコード(HIGH状態のまま)
  if ( state == HIGH && preState == HIGH ) {
    noSigTime = millis() - startHIGH;
    if ( bitcount != 0 ) {
      //入力なし時間が基準時間（dotTime）の3倍より大きければ、入力完了とみなす
      if ( noSigTime > (3 * Ltime) ) {
        if ( DECODE_isWabun() ) {
          decodeJP(); //和文デコード
        } else {
          decodeEn(); //英文デコード
        }
        //入力バッファを初期化
        Mcode = 1;
        bitcount = 0;
        space = true;//空白を出力
      }
    }

    //空白を出力するのは１文字だけ
    if ( space == true && noSigTime > 7 * Ltime ) {
      LCD_printAscii(32); //" "
      space = false;
    }
  }
  preState = state;
}

/*****************************************
   英文をデコード、１文字出力する
 *****************************************/
void decodeEn() {
  switch ( Mcode ) {
    case B00000101:
      LCD_printAscii(65);//"A"
      break;
    case B00011000:
      LCD_printAscii(66);//"B"
      break;
    case B00011010:
      LCD_printAscii(67);//"C"
      break;
    case B00001100:
      LCD_printAscii(68);//"D"
      break;
    case B00000010:
      LCD_printAscii(69);//"E"
      break;
    case B00010010:
      LCD_printAscii(70);//"F"
      break;
    case B00001110:
      LCD_printAscii(71);//"G"
      break;
    case B00010000:
      LCD_printAscii(72);//"H"
      break;
    case B00000100:
      LCD_printAscii(73);//"I"
      break;
    case B00010111:
      LCD_printAscii(74);//"J"
      break;
    case B00001101:
      LCD_printAscii(75);//"K"
      break;
    case B00010100:
      LCD_printAscii(76);//"L"
      break;
    case B00000111:
      LCD_printAscii(77);//"M"
      break;
    case B00000110:
      LCD_printAscii(78);//"N"
      break;
    case B00001111:
      LCD_printAscii(79);//"O"
      break;
    case B00010110:
      LCD_printAscii(80);//"P"
      break;
    case B00011101:
      LCD_printAscii(81);//"Q"
      break;
    case B00001010:
      LCD_printAscii(82);//"R"
      break;
    case B00001000:
      LCD_printAscii(83);//"S"
      break;
    case B00000011:
      LCD_printAscii(84);//"T"
      break;
    case B00001001:
      LCD_printAscii(85);//"U"
      break;
    case B00010001:
      LCD_printAscii(86);//"V"
      break;
    case B00001011:
      LCD_printAscii(87);//"W"
      break;
    case B00011001:
      LCD_printAscii(88);//"X"
      break;
    case B00011011:
      LCD_printAscii(89);//"Y"
      break;
    case B00011100:
      LCD_printAscii(90);//"Z"
      break;
    case B00101111:
      LCD_printAscii(49);//"1"
      break;
    case B00100111:
      LCD_printAscii(50);//"2"
      break;
    case B00100011:
      LCD_printAscii(51);//"3"
      break;
    case B00100001:
      LCD_printAscii(52);//"4"
      break;
    case B00100000:
      LCD_printAscii(53);//"5"
      break;
    case B00110000:
      LCD_printAscii(54);//"6"
      break;
    case B00111000:
      LCD_printAscii(55);//"7"
      break;
    case B00111100:
      LCD_printAscii(56);//"8"
      break;
    case B00111110:
      LCD_printAscii(57);//"9"
      break;
    case B00111111:
      LCD_printAscii(48);//"0"
      break;
    case B00110010:
      LCD_printAscii(47);//"/"
      break;
    case B01001100:
      LCD_printAscii(63);//"?"
      break;
    case B01110011:
      LCD_printAscii(44);//","
      break;
    case B01010101:
      LCD_printAscii(46);//"."
      break;
    case B01011010:
      LCD_printAscii(64);//"@"
      break;
    case B00110110:
      LCD_printAscii(40);//"("
      break;
    case B01101101:
      LCD_printAscii(41);//")"
      break;
    case B00101010:
      LCD_printAscii(91);//"["
      LCD_printAscii(65);//"A"
      LCD_printAscii(82);//"R"
      LCD_printAscii(93);//"]"
      break;
    case B01000101:
      LCD_printAscii(91);//"["
      LCD_printAscii(86);//"V"
      LCD_printAscii(65);//"A"
      LCD_printAscii(93);//"]"
      break;
    case B00110001:
      LCD_printAscii(91);//"["
      LCD_printAscii(66);//"B"
      LCD_printAscii(84);//"T"
      LCD_printAscii(93);//"]"
      break;
    case B01100111://"ホレ"
      DECODE_setWabunMode();
      LCD_printAscii(60);//"<"
      break;
    default:
      LCD_printAscii(42);//"*"
  }
}

/*****************************************
   和文をデコードし、１文字出力する
 *****************************************/
void decodeJP() {
  switch ( Mcode ) {
    case B00010111:
      LCD_printAscii(166);//"ｦ"
      break;
    case B00101101:
      LCD_printAscii(176);//"-"
      break;
    case B00111011:
      LCD_printAscii(177);//"ｱ"
      break;
    case B00000101:
      LCD_printAscii(178);//"ｲ"
      break;
    case B00001001:
      LCD_printAscii(179);//"ｳ"
      break;
    case B00110111:
      LCD_printAscii(180);//"ｴ"
      break;
    case B00101000:
      LCD_printAscii(181);//"ｵ"
      break;
    case B00010100:
      LCD_printAscii(182);//"ｶ"
      break;
    case B00110100:
      LCD_printAscii(183);//"ｷ"
      break;
    case B00010001:
      LCD_printAscii(184);//"ｸ"
      break;
    case B00011011:
      LCD_printAscii(185);//"ｹ"
      break;
    case B00011111:
      LCD_printAscii(186);//"ｺ"
      break;
    case B00110101:
      LCD_printAscii(187);//"ｻ"
      break;
    case B00111010:
      LCD_printAscii(188);//"ｼ"
      break;
    case B00111101:
      LCD_printAscii(189);//"ｽ"
      break;
    case B00101110:
      LCD_printAscii(190);//"ｾ"
      break;
    case B00011110:
      LCD_printAscii(191);//"ｿ"
      break;
    case B00000110:
      LCD_printAscii(192);//"ﾀ"
      break;
    case B00010010:
      LCD_printAscii(193);//"ﾁ"
      break;
    case B00010110:
      LCD_printAscii(194);//"ﾂ"
      break;
    case B00101011:
      LCD_printAscii(195);//"ﾃ"
      break;
    case B00100100:
      LCD_printAscii(196);//"ﾄ"
      break;
    case B00001010:
      LCD_printAscii(197);//"ﾅ"
      break;
    case B00011010:
      LCD_printAscii(198);//"ﾆ"
      break;
    case B00010000:
      LCD_printAscii(199);//"ﾇ"
      break;
    case B00011101:
      LCD_printAscii(200);//"ﾈ"
      break;
    case B00010011:
      LCD_printAscii(201);//"ﾉ"
      break;
    case B00011000:
      LCD_printAscii(202);//"ﾊ"
      break;
    case B00111001:
      LCD_printAscii(203);//"ﾋ"
      break;
    case B00011100:
      LCD_printAscii(204);//"ﾌ"
      break;
    case B00000010:
      LCD_printAscii(205);//"ﾍ"
      break;
    case B00001100:
      LCD_printAscii(206);//"ﾎ"
      break;
    case B00011001:
      LCD_printAscii(207);//"ﾏ"
      break;
    case B00100101:
      LCD_printAscii(208);//"ﾐ"
      break;
    case B00000011:
      LCD_printAscii(209);//"ﾑ"
      break;
    case B00110001:
      LCD_printAscii(210);//"ﾒ"
      break;
    case B00110010:
      LCD_printAscii(211);//"ﾓ"
      break;
    case B00001011:
      LCD_printAscii(212);//"ﾔ"
      break;
    case B00110011:
      LCD_printAscii(213);//"ﾕ"
      break;
    case B00000111:
      LCD_printAscii(214);//"ﾖ"
      break;
    case B00001000:
      LCD_printAscii(215);//"ﾗ"
      break;
    case B00001110:
      LCD_printAscii(216);//"ﾘ"
      break;
    case B00110110:
      LCD_printAscii(217);//"ﾙ"
      break;
    case B00001111:
      LCD_printAscii(218);//"ﾚ"
      break;
    case B00010101:
      LCD_printAscii(219);//"ﾛ"
      break;
    case B00001101:
      LCD_printAscii(220);//"ﾜ"
      break;
    case B00101010:
      LCD_printAscii(221);//"ﾝ"
      break;
    case B00000100:
      LCD_printAscii(222);//"ﾞ"
      break;
    case B00100110:
      LCD_printAscii(223);//"ﾟ"
      break;
    case B00101111:
      LCD_printAscii(49);//"1"
      break;
    case B00100111:
      LCD_printAscii(50);//"2"
      break;
    case B00100011:
      LCD_printAscii(51);//"3"
      break;
    case B00100001:
      LCD_printAscii(52);//"4"
      break;
    case B00100000:
      LCD_printAscii(53);//"5"
      break;
    case B00110000:
      LCD_printAscii(54);//"6"
      break;
    case B00111000:
      LCD_printAscii(55);//"7"
      break;
    case B00111100:
      LCD_printAscii(56);//"8"
      break;
    case B00111110:
      LCD_printAscii(57);//"9"
      break;
    case B00111111:
      LCD_printAscii(48);//"0"
      break;
    case B01001100:
      LCD_printAscii(63);//"?"
      break;
    case B01010101:
      LCD_printAscii(46);//"."
      break;
    case B01110011:
      LCD_printAscii(44);//","
      break;
    case B01010100:
      LCD_printAscii(163);//"｣"
      break;
    case B00100010://"ラタ"
      DECODE_setEnglishMode();
      LCD_printAscii(62);//">"
      break;
    case B01101101:
      DECODE_setEnglishMode();
      LCD_printAscii(40);//"("
      break;
    case B01010010:
      DECODE_setWabunMode();
      LCD_printAscii(41);//")"
      break;
    default:
      LCD_printAscii(42);//"*"
  }
}
