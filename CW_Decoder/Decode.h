/************************************************************************** 
 モールス信号デコード処理　公開ヘッダー
  
 **************************************************************************/
#define DEFAULT_DOT_TIME   (120)  //短点と長点を分ける時間(ms)の初期値

//初期化
void DECODE_init(void);

//デコード
void DECODE_execDecode(void);

//和文モードに切り替える
void DECODE_setWabunMode(void);

//英文モードに切り替える
void DECODE_setEnglishMode(void);

//現在のモードが和文モードの場合はtrue, 英文モードならfalseを返す
boolean DECODE_isWabun(void);

//モールス信号の短点（・）と長点（－）のしきい値 ms を返す
int DECODE_getInterval(void);

//モールス信号の短点（・）と長点（－）のしきい値 ms をセットする
void DECODE_setInterval(int interval);
