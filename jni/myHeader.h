#ifndef __MYHEADER_H__
#define __MYHEADER_H__
#pragma  once

#include "type.h"
#include <cmath>

//Cut Functions Definition                                                
void DecomposeWord(Point wordletter[][MAX_STROKE][MAX_LENGTH],Point data[][MAX_LENGTH],CutLabel label[]);   
//�����зֵ�ָ��
void GetCutpoint(Point cut[],Point wordletter[][MAX_STROKE][MAX_LENGTH],CutLabel label[],int line);          
//���ݱ궨�����зֵ�
Width GetWidth(Point data[][MAX_LENGTH]);                    //�õ��ַ����
Baseline GetBaseline(Point data[][MAX_LENGTH]);              //�õ�����λ��
void Project(int project[],bool pixel[][DENSITY_Y],int direction);
//����ͶӰ,direction,0:��ֱͶӰ;1:ˮƽͶӰ
void ProjectDif(int project[],bool pixel[][DENSITY_Y],int direction);
//������ͶӰ,direction,0:��ֱͶӰ;1:ˮƽͶӰ
Baseline ComputeBaseline(int project[]);                     //��ˮƽͶӰ���������
void GetStroke(Stroke_C pstroke[],Point data[][MAX_LENGTH]); //�ɵ�洢���ʻ��ṹ(�з�)
int WidthRelation(Width width1,Width width2);                //������1�Ϳ��2��λ�ù�ϵ(0:����;1:1����2;2:2����1;3:2��1����ཻ;4:2��1�Ҳ��ཻ)
Width IsThrough(Point data[],int line);                      //�жϱʻ�β���Ƿ���ڴ�Խ�ʻ�,���ش�Խ�ο��(-1��ʾ�޴�Խ��)
int IsLine(Point data[]);                                    //�жϱʻ��Ƿ�Ϊֱ��(0:����ֱ��;1:����ֱ��;2:������бֱ��;3:ˮƽ��۽����б��)
void GetCutSect(Stroke_C pstroke[],Point data[][MAX_LENGTH]);//�õ��ʻ����зֶ�
void CorrectCut(Width cut[],Point data[],int lastretain);    //�зֶ�У��
void FirstCut(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line);
//�����з�(��λȫ�����зֵ㡢ά����д�����µ��зֵ�ɸѡ)
int WidthPosition(Width width,Width widths[],bool inter);    //�����ȵ�������ȶ�(�����ĸ���ȶξ�����С0~29,inter��ʾ��ȶ����,0:����μ�;1:�������)
void Cluster(Stroke_C pstroke[],int line);                   //���ӱʻ�����

int Intersect(Point data1[],Point data2[]);                  //�ҳ����߶εĴ�ֱ���㲢���ؽ������߶�1�е�λ��
int PerpendIntersect(int dot_x,Point data[],int ns,int ne);  //�ҳ�����ʻ��εĴ�ֱ����(ns��ne�����˴�ֱ����ķ�Χ)
void AffixLabel(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH]); 
//�Ը��ӱʻ���ʻ���λ����������ʻ�0~29,����CutLabel
void MainRectify(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line);
//�Զ�����ʻ���������кϲ�
void SecondCut(CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[]);
//�����з�
int IsConnect(int radial[],CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[],int pos);//pos:1-4,�޶���ĸλ��(1:�׶˲���;2:�м䲿��;3:β�˲���;4:��������)   
//�ж��Ƿ�������ĸ���зֲ���(-1:������;0-DENSITY_X:����,���ص�Ⱥ����ĸ�м�εľ���)
int ThreeDot(Point data_m[][MAX_LENGTH],Point data_d[][MAX_LENGTH]);                     
//�жϵ�Ⱥ�Ƿ�Ϊ����ʽ������ĸ����(-1:����;0-DENSITY_X:��,���ص�Ⱥ����ĸ�м�εľ���)
int ConnectMain(Point data_m[][MAX_LENGTH],int pos);         //�ж�����ʻ��Ƿ�����ĳ���������ĸ(-1:������̬����;0:ʶ����״����;1:��)(pos:1-4,�޶���ĸλ��,1:�׶˲���;2:�м䲿��;3:β�˲���;4:��������)
void SequenceDistance_C(int * data_g,int * data_s,int * data_c,int * result,int term);  
//�з�ʶ��ʱ�ľ�������,�������58+3άʸ����(term,0:������������,1:����������������������)
void LetterCuts();                                           //ά�Ĳ����зֶνṹ���ձ�
void Recognize(Result * result,Point data[][MAX_LENGTH]);                     //���ַ�ʶ��
void GetStroke(Stroke_R pstroke[],Point data[][MAX_LENGTH]);                  //�ɵ�洢���ʻ��ṹ
void GetFeature_T(Feature_R * pfeature,Point data[][MAX_LENGTH],int a,int d); //ѵ��ʱ�õ����ַ�ʶ������
void GetFeature_R(Feature_R * pfeature,Point data[][MAX_LENGTH]);             //ʶ��ʱ�õ����ַ�ʶ������
void GetGrid(int grid[],Fxm fxm[],int sizex,int sizey,Point lu,Point rd);     //�ӷ����뵽����������
int FormalizeFxm(Fxm fxm1[],Fxm fxm2[]);                                      //���������
void GetStream(int stream[],int ms[],int size);                               //�ӷ����뵽������������
void GetCode(int code[],int ms[],int size);                                   //�ӷ����뵽����
void FormalizeCode(int * data,int size);                                      //�������
void ShiftCode(int * data1,int * data2, int size);                            //������λƥ��
int RadialOfMinDistance(int * data_g,int * data_s,int * data_c,int * area);   //�õ��������ж�ʱ�ĵ����ӱʻ��͸��ӱʻ���������ʻ���������Ĳ���              
void SequenceDistance_R(int * data_g,int * data_s,int * data_c,int * result,int sign,int term);  //ʶ��ʱ�ľ�������,�������58άʸ����
int Distance(int * data1,int * data2,int size);                               //�������
int DistanceCode(int * data1,int * data2,int size);                           //����������
int DistanceDot(int num,int pos,int rel,int dotf,int de);                     //��������������
void Sequence(int * data,int size);                                           //����
void Sequence(int * data,int * num,int size);
void AntiSequence(double * data,int * num,int size);                          //������,�Դ���С      
int MaxFrequency(int * data,int size);                                        //Ƶ�����
void LetterRadials();                                                         //128��ά���ַ��Ĳ����ֵ� 
void MainRadials();                                                           //ά�����岿��λ��ͼ
void RedundanceOut(Point data[][MAX_LENGTH]);                  //ȥ����
void Connect(Point data[][MAX_LENGTH]);                        //�ϱ�����
void Reverse(Point data[]);                                    //�ʻ���ת
void Unitize(Point data[][MAX_LENGTH]);                        //��С��һ��
void Smooth(Point data[][MAX_LENGTH]);                         //����ƽ��
void SlantCorrect(Point data[][MAX_LENGTH]);                   //��бУ��
int GetSlantAngle(Point data[][MAX_LENGTH]);                   //�õ���б��     
void Hough(int result[][DENSITY_X*4],bool pixel[][DENSITY_Y]); //Hough�任
void PostprocessByHMM();                                //HMMģ�Ͷ��û�ʶ�������к���
void PostprocessByHMM_Fixed();                          //HMMģ�Ͷ�ʶ�������к���(�����ȹ̶�)
void PostprocessByViterbi();                            //Viterbi�㷨��ʶ�������к���(�����ȹ̶�,����,Ѱ�����·��)
void GetT();                                            //�õ���ʼ״̬����ģ��(������ĸ�����ֳ���Ƶ��)
void GetA();                                            //�õ�״̬ת�Ƹ���ģ��
void GetB(double b[],Result oneinresult[]);             //�õ��۲�ֵ���ʷֲ�ģ��
void WordLetters();                                     //ά�ĵ����ַ��ṹ���ձ�
void RecognizeWord(Result result[][LETTER_CLASSCOUNT],Point wordcut[][MAX_STROKE][MAX_LENGTH]);     //ʶ���зֺ󵥴�
void GetWord(int result[],Result inresult[][LETTER_CLASSCOUNT]);       //�ó�����ʶ����    1-500  
void ReadFeature();               //����ѵ������



extern int letterresult[LETTER_CLASSCOUNT];               //���ַ�ʶ����
//Preprocess Variables Reference
extern Point Data_preprocessed[MAX_STROKE][MAX_LENGTH];   //����Ԥ���������(����)
//Cut Variables Reference
//Recognize Variables Reference
int LetterList[LETTER_CLASSCOUNT+24][3];           //ά�Ķ���
int radialposition[LETTER_CLASSCOUNT+RADIAL_COUNT];//ά�����岿��λ��ͼ1-4 
int RGm[LETTER_MCOUNT][GRID_M_X*GRID_M_Y*4];       //�ַ������������洢         
int RGa[LETTER_SACOUNT][GRID_A_X*GRID_A_Y*4];
int RGd[LETTER_FDCOUNT][GRID_A_X*GRID_A_Y*4];
int RSm[LETTER_MCOUNT][STREAM_M*4];                //�ַ��������������洢        
int RSa[LETTER_SACOUNT][STREAM_A*4];
int RSd[LETTER_FDCOUNT][STREAM_A*4];
int RCm[LETTER_MCOUNT][CODE_M];                    //�ַ����뷽�������洢        
int RCa[LETTER_SACOUNT][CODE_A];
int RCd[LETTER_FDCOUNT][CODE_A];
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//Global Variable Definition
int name;                                            //���ļ�ʱ�ı������
string pathname;                                    //������·��
Point Data[MAX_STROKE][MAX_LENGTH];                  //��ʶ������(����) 
int letterresult[LETTER_CLASSCOUNT];                 //���ַ�ʶ����
int wordresult[WORD_CLASSCOUNT];                     //����ʶ����
//Preprocess Variable Definition
Point Data_preprocessed[MAX_STROKE][MAX_LENGTH];     //����Ԥ���������(����)
//Cut Variable Definition
CutLabel strokeFlag[MAX_STROKE];                     //�ֽⵥ�ʵıʻ���ע
Point Letters[WORD_LENGTH][MAX_STROKE][MAX_LENGTH];  //�����зֵĵ�������(����)
Point cutpoint[WORD_LENGTH];                         //�зֵ�洢
int RadialList[4][3];                                //ά�Ĳ����зֶι��ɶ��ձ�                               
int CGm[RADIAL_COUNT][GRID_M_X*GRID_M_Y*4];          //���������������洢         
int CSm[RADIAL_COUNT][STREAM_M*4];                   //�����������������洢        
int CCm[RADIAL_COUNT][CODE_M];                       //�������뷽�������洢        
//RecognizeWord Variable Definition
Result Wordinresult[WORD_LENGTH][LETTER_CLASSCOUNT]; //����ʶ����м���
//Postprocess Variable Definition
int WordList[WORD_CLASSCOUNT][WORD_LENGTH];          //ά�ĵ����ַ����ɶ��ձ� 
double T[LETTER_CLASSCOUNT+1];                       //��ʼ״̬����ģ��(�������ַ�)
double A[LETTER_CLASSCOUNT+1][LETTER_CLASSCOUNT+1];  //ת�Ƹ���ģ��(�������ַ�,A[i][j]�ǵ�������ĸiʱ��������ĸj�ĸ���)
double B[WORD_LENGTH][LETTER_CLASSCOUNT+1];          //�۲�ֵ���ʷֲ�ģ��(�������ַ�,��ģ�����ÿ�鵥�ַ�ʶ����,B[i][j]��ʶ�����ǵ�ǰ��i���ַ�ʱ��ĸ��ֵ��j�ĸ���)
int result[WORD_LENGTH];

#endif