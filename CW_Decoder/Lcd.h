/**************************************************************************
 LCD表示制御　公開ヘッダー
 
 *************************************************************************/
#define LCD_COLUMNS (16)  //LCD桁数
#define LCD_ROWS     (2)  //LCD行数

//初期化
void LCD_init(void);

//メニューを表示
void LCD_openMenu(void);

//メニュー選択
void LCD_selectMenu(void);

//デコードモードに切り替え
void LCD_goToDecode(void);

//LCDに１文字出力
void LCD_printAscii(int asciiNumber);

//デコードした文字をクリアする
void LCD_resetAll();
