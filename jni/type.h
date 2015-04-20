#ifndef __TYPE_H__
#define __TYPE_H__
#pragma  once

#include<string>
using namespace std;

#define RECT_X                 15              //数据输入板起点横坐标
#define RECT_Y                 25              //数据输入板起点纵坐标
#define DISTANCE               15              //结果显示板与数据输入板的距离
#define RECT_WIDTH             256             //数据输入宽度
#define RECT_HEIGHT            256             //数据输入高度
#define RESULT_HEIGHT          136             //结果显示板高度 
#define DENSITY_X              256             //数据宽度分辨点数
#define DENSITY_Y              256             //数据高度分辨点数
#define MAX_STROKE             30              //最大笔划数
#define MAX_LENGTH             1000            //字符每笔最大点数 
#define SAMPLE_COUNT           3               //样本组个数                                             
//Preprocess Constants Definition
#define MIN_LENGTH             2               //字符每笔最小点数
#define MIN_DIS                pow(10,2)       //两笔间的最小距离
#define SMOOTH_COEF            0.5             //平滑系数
#define PEI                    3.1415926       //圆周率
//Cut Constants Definition
#define WORD_LENGTH            10              //单词长度(含单字符的个数)
#define SECT_LENGTH            50              //每笔划中的分段数
#define MAX_CUT                9               //一个笔画中的切分点数目最大值
#define RADIAL_COUNT           3               //多段型字符切分部件总类别数
//Recognize Constants Definition
#define LETTER_CLASSCOUNT      128             //字符类别总数
#define DOT_LENGTH             40              //点笔划的最大长度
#define MIN_CODEFEATURE        3               //链码特征持续最小点数（相对于抖动噪声）
#define GRID_M_X               7               //字符主体笔划方向特征X网格数
#define GRID_M_Y               7               //字符主体笔划方向特征Y网格数
#define GRID_A_X               5               //字符附加笔划方向特征X网格数
#define GRID_A_Y               5               //字符附加笔划方向特征Y网格数
#define STREAM_M               9               //字符主体笔划方向特征码流维数
#define STREAM_A               4               //字符附加笔划方向特征码流维数
#define CODE_M                 16              //字符主体笔划方向特征链码维数
#define CODE_A                 8               //字符附加笔划方向特征链码维数
#define LETTER_MCOUNT          58              //字符主体笔划总类别数
#define LETTER_ACOUNT          9               //字符附加笔划总类别数
#define LETTER_DCOUNT          8               //字符点笔划总类别数
#define LETTER_SACOUNT         5               //字符单附加笔划总类别数
#define LETTER_FDCOUNT         5               //字符点连笔笔划总类别数 
//RecognizeWord Constants Definition
#define WORD_CLASSCOUNT        500             //单词类别总数
//Postprocess Constants Definition
//Global Variable Types Definition
typedef   struct 
{
	int x;
	int y;
}Point;                                //点坐标
//Preprocess Variable Types Definition
//Cut Variable Types Definition
typedef struct
{
	int stroke;                          //1-30
	int cut_start;                       //1-1000
	int cut_end;                         //1-1000
	int belong;                          //1-30
}CutLabel;                               //标注结构 
typedef struct
{
	int centre;
	int up;
	int down;
}Baseline;                               //基线域
typedef struct
{
	int start;
	int end;
}Width;                                  //宽度
typedef struct
{
	Width width_x;
	Width width_y;
}Domain;                                 //笔划区域
typedef struct
{
	int stroke;                          //笔划序号1-30
	int section;                         //所属连体段1-10
	Baseline baseline;
	Domain domain;
	int length;                          //笔划的长度
	int ismain;                          //ismain:0:不是主体笔划;1:是主体笔划
	Width cutsect[MAX_CUT*2]; 	         //切分段序号(0-999,0-999)
}Stroke_C;                               //切分时的笔划结构
//Recognize Variable Types Definition
typedef enum{Main,Affix,Dot,Void} Sign;
typedef struct
{
	Point pos;
	int fx;
	int size;
}Fxm;                                     //方向码存储
typedef struct 
{
	Sign sign;
	int ms[MAX_LENGTH];
	Fxm fxm[MAX_LENGTH];
	int length;
	Point start;
	Point end;
	Point lu;
	Point rd;
	int baseline;
}Stroke_R;                                //识别时的笔划结构
typedef struct
{
	int mg[GRID_M_X*GRID_M_Y*4];
	int ms[STREAM_M*4];
	int mc[CODE_M];
	int affix;	
	int ag1[GRID_A_X*GRID_A_Y*4];
	int as1[STREAM_A*4];
	int ac1[CODE_A];
	int ag2[GRID_A_X*GRID_A_Y*4];
	int as2[STREAM_A*4];
	int ac2[CODE_A];
	int fdot;
	int dot_num;
	int dot_pos;
	int dot_rel;
}Feature_R;                               //识别时的特征结构
//RecognizeWord Variable Types Definition
typedef struct
{
	int number;      //1-128:字符识别结果;0:尚未识别或识别失败
	int distance;    //识别距离
}Result;
//Global Functions Definition                               
void Initialize(Point data[][MAX_LENGTH]);                                //数据初始化
void ReadFile(Point data[][MAX_LENGTH],string path,int n);               //读文件
void FromDataToPixel(bool pixel[][DENSITY_Y],Point data[][MAX_LENGTH]);   //从联机数据到脱机位图
int sign(int a);                                                          //得到该数据的符号
int getnear(double a);                                                    //得到最近的整数
string Display(int * data,int size);                                     //返回输出字符串
void ReadFeature(string path,string name);                              //读入训练特征

#endif
