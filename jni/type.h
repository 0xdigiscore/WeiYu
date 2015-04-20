#ifndef __TYPE_H__
#define __TYPE_H__
#pragma  once

#include<string>
using namespace std;

#define RECT_X                 15              //�����������������
#define RECT_Y                 25              //������������������
#define DISTANCE               15              //�����ʾ�������������ľ���
#define RECT_WIDTH             256             //����������
#define RECT_HEIGHT            256             //��������߶�
#define RESULT_HEIGHT          136             //�����ʾ��߶� 
#define DENSITY_X              256             //���ݿ�ȷֱ����
#define DENSITY_Y              256             //���ݸ߶ȷֱ����
#define MAX_STROKE             30              //���ʻ���
#define MAX_LENGTH             1000            //�ַ�ÿ�������� 
#define SAMPLE_COUNT           3               //���������                                             
//Preprocess Constants Definition
#define MIN_LENGTH             2               //�ַ�ÿ����С����
#define MIN_DIS                pow(10,2)       //���ʼ����С����
#define SMOOTH_COEF            0.5             //ƽ��ϵ��
#define PEI                    3.1415926       //Բ����
//Cut Constants Definition
#define WORD_LENGTH            10              //���ʳ���(�����ַ��ĸ���)
#define SECT_LENGTH            50              //ÿ�ʻ��еķֶ���
#define MAX_CUT                9               //һ���ʻ��е��зֵ���Ŀ���ֵ
#define RADIAL_COUNT           3               //������ַ��зֲ����������
//Recognize Constants Definition
#define LETTER_CLASSCOUNT      128             //�ַ��������
#define DOT_LENGTH             40              //��ʻ�����󳤶�
#define MIN_CODEFEATURE        3               //��������������С����������ڶ���������
#define GRID_M_X               7               //�ַ�����ʻ���������X������
#define GRID_M_Y               7               //�ַ�����ʻ���������Y������
#define GRID_A_X               5               //�ַ����ӱʻ���������X������
#define GRID_A_Y               5               //�ַ����ӱʻ���������Y������
#define STREAM_M               9               //�ַ�����ʻ�������������ά��
#define STREAM_A               4               //�ַ����ӱʻ�������������ά��
#define CODE_M                 16              //�ַ�����ʻ�������������ά��
#define CODE_A                 8               //�ַ����ӱʻ�������������ά��
#define LETTER_MCOUNT          58              //�ַ�����ʻ��������
#define LETTER_ACOUNT          9               //�ַ����ӱʻ��������
#define LETTER_DCOUNT          8               //�ַ���ʻ��������
#define LETTER_SACOUNT         5               //�ַ������ӱʻ��������
#define LETTER_FDCOUNT         5               //�ַ������ʱʻ�������� 
//RecognizeWord Constants Definition
#define WORD_CLASSCOUNT        500             //�����������
//Postprocess Constants Definition
//Global Variable Types Definition
typedef   struct 
{
	int x;
	int y;
}Point;                                //������
//Preprocess Variable Types Definition
//Cut Variable Types Definition
typedef struct
{
	int stroke;                          //1-30
	int cut_start;                       //1-1000
	int cut_end;                         //1-1000
	int belong;                          //1-30
}CutLabel;                               //��ע�ṹ 
typedef struct
{
	int centre;
	int up;
	int down;
}Baseline;                               //������
typedef struct
{
	int start;
	int end;
}Width;                                  //���
typedef struct
{
	Width width_x;
	Width width_y;
}Domain;                                 //�ʻ�����
typedef struct
{
	int stroke;                          //�ʻ����1-30
	int section;                         //���������1-10
	Baseline baseline;
	Domain domain;
	int length;                          //�ʻ��ĳ���
	int ismain;                          //ismain:0:��������ʻ�;1:������ʻ�
	Width cutsect[MAX_CUT*2]; 	         //�зֶ����(0-999,0-999)
}Stroke_C;                               //�з�ʱ�ıʻ��ṹ
//Recognize Variable Types Definition
typedef enum{Main,Affix,Dot,Void} Sign;
typedef struct
{
	Point pos;
	int fx;
	int size;
}Fxm;                                     //������洢
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
}Stroke_R;                                //ʶ��ʱ�ıʻ��ṹ
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
}Feature_R;                               //ʶ��ʱ�������ṹ
//RecognizeWord Variable Types Definition
typedef struct
{
	int number;      //1-128:�ַ�ʶ����;0:��δʶ���ʶ��ʧ��
	int distance;    //ʶ�����
}Result;
//Global Functions Definition                               
void Initialize(Point data[][MAX_LENGTH]);                                //���ݳ�ʼ��
void ReadFile(Point data[][MAX_LENGTH],string path,int n);               //���ļ�
void FromDataToPixel(bool pixel[][DENSITY_Y],Point data[][MAX_LENGTH]);   //���������ݵ��ѻ�λͼ
int sign(int a);                                                          //�õ������ݵķ���
int getnear(double a);                                                    //�õ����������
string Display(int * data,int size);                                     //��������ַ���
void ReadFeature(string path,string name);                              //����ѵ������

#endif
