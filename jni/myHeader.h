#ifndef __MYHEADER_H__
#define __MYHEADER_H__
#pragma  once

#include "type.h"
#include <cmath>

//Cut Functions Definition                                                
void DecomposeWord(Point wordletter[][MAX_STROKE][MAX_LENGTH],Point data[][MAX_LENGTH],CutLabel label[]);   
//依据切分点分割单词
void GetCutpoint(Point cut[],Point wordletter[][MAX_STROKE][MAX_LENGTH],CutLabel label[],int line);          
//依据标定汇总切分点
Width GetWidth(Point data[][MAX_LENGTH]);                    //得到字符宽度
Baseline GetBaseline(Point data[][MAX_LENGTH]);              //得到基线位置
void Project(int project[],bool pixel[][DENSITY_Y],int direction);
//计算投影,direction,0:垂直投影;1:水平投影
void ProjectDif(int project[],bool pixel[][DENSITY_Y],int direction);
//计算差分投影,direction,0:垂直投影;1:水平投影
Baseline ComputeBaseline(int project[]);                     //由水平投影计算基线域
void GetStroke(Stroke_C pstroke[],Point data[][MAX_LENGTH]); //由点存储到笔划结构(切分)
int WidthRelation(Width width1,Width width2);                //计算宽度1和宽度2的位置关系(0:相离;1:1包含2;2:2包含1;3:2在1左侧相交;4:2在1右侧相交)
Width IsThrough(Point data[],int line);                      //判断笔划尾部是否存在穿越笔划,返回穿越段宽度(-1表示无穿越段)
int IsLine(Point data[]);                                    //判断笔划是否为直线(0:不是直线;1:是竖直线;2:锐角倾角斜直线;3:水平或钝角倾角斜线)
void GetCutSect(Stroke_C pstroke[],Point data[][MAX_LENGTH]);//得到笔划的切分段
void CorrectCut(Width cut[],Point data[],int lastretain);    //切分段校正
void FirstCut(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line);
//初次切分(定位全部过切分点、维文书写规则下的切分点筛选)
int WidthPosition(Width width,Width widths[],bool inter);    //计算宽度的所属宽度段(即与哪个宽度段距离最小0~29,inter表示宽度段类别,0:连体段间;1:连体段内)
void Cluster(Stroke_C pstroke[],int line);                   //附加笔划聚类

int Intersect(Point data1[],Point data2[]);                  //找出两线段的垂直交点并返回交点在线段1中的位置
int PerpendIntersect(int dot_x,Point data[],int ns,int ne);  //找出点与笔划段的垂直交点(ns和ne限制了垂直交点的范围)
void AffixLabel(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH]); 
//对附加笔划点笔划定位其所属主体笔划0~29,完善CutLabel
void MainRectify(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line);
//对多主体笔划的情况进行合并
void SecondCut(CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[]);
//二次切分
int IsConnect(int radial[],CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[],int pos);//pos:1-4,限定字母位置(1:首端部件;2:中间部件;3:尾端部件;4:独立部件)   
//判断是否多段型字母的切分部件(-1:不相连;0-DENSITY_X:相连,返回点群至字母中间段的距离)
int ThreeDot(Point data_m[][MAX_LENGTH],Point data_d[][MAX_LENGTH]);                     
//判断点群是否为三点式且在字母正中(-1:不是;0-DENSITY_X:是,返回点群至字母中间段的距离)
int ConnectMain(Point data_m[][MAX_LENGTH],int pos);         //判断主体笔划是否属于某个连体段字母(-1:大致形态不符;0:识别形状不符;1:是)(pos:1-4,限定字母位置,1:首端部件;2:中间部件;3:尾端部件;4:独立部件)
void SequenceDistance_C(int * data_g,int * data_s,int * data_c,int * result,int term);  
//切分识别时的距离排序,结果存在58+3维矢量内(term,0:仅含网格特征,1:包含码流特征和链码特征)
void LetterCuts();                                           //维文部件切分段结构对照表
void Recognize(Result * result,Point data[][MAX_LENGTH]);                     //单字符识别
void GetStroke(Stroke_R pstroke[],Point data[][MAX_LENGTH]);                  //由点存储到笔划结构
void GetFeature_T(Feature_R * pfeature,Point data[][MAX_LENGTH],int a,int d); //训练时得到单字符识别特征
void GetFeature_R(Feature_R * pfeature,Point data[][MAX_LENGTH]);             //识别时得到单字符识别特征
void GetGrid(int grid[],Fxm fxm[],int sizex,int sizey,Point lu,Point rd);     //从方向码到网格方向特征
int FormalizeFxm(Fxm fxm1[],Fxm fxm2[]);                                      //方向码规整
void GetStream(int stream[],int ms[],int size);                               //从方向码到码流方向特征
void GetCode(int code[],int ms[],int size);                                   //从方向码到链码
void FormalizeCode(int * data,int size);                                      //链码规整
void ShiftCode(int * data1,int * data2, int size);                            //链码移位匹配
int RadialOfMinDistance(int * data_g,int * data_s,int * data_c,int * area);   //得到点连笔判断时的单附加笔划和附加笔划中与输入笔划距离最近的部件              
void SequenceDistance_R(int * data_g,int * data_s,int * data_c,int * result,int sign,int term);  //识别时的距离排序,结果存在58维矢量内
int Distance(int * data1,int * data2,int size);                               //距离计算
int DistanceCode(int * data1,int * data2,int size);                           //链码距离计算
int DistanceDot(int num,int pos,int rel,int dotf,int de);                     //点特征间距离计算
void Sequence(int * data,int size);                                           //排序
void Sequence(int * data,int * num,int size);
void AntiSequence(double * data,int * num,int size);                          //反排序,自大至小      
int MaxFrequency(int * data,int size);                                        //频率最大
void LetterRadials();                                                         //128类维文字符的部件字典 
void MainRadials();                                                           //维文主体部件位置图
void RedundanceOut(Point data[][MAX_LENGTH]);                  //去冗余
void Connect(Point data[][MAX_LENGTH]);                        //断笔连接
void Reverse(Point data[]);                                    //笔画翻转
void Unitize(Point data[][MAX_LENGTH]);                        //大小归一化
void Smooth(Point data[][MAX_LENGTH]);                         //数据平滑
void SlantCorrect(Point data[][MAX_LENGTH]);                   //倾斜校正
int GetSlantAngle(Point data[][MAX_LENGTH]);                   //得到倾斜角     
void Hough(int result[][DENSITY_X*4],bool pixel[][DENSITY_Y]); //Hough变换
void PostprocessByHMM();                                //HMM模型对用基识别结果进行后处理
void PostprocessByHMM_Fixed();                          //HMM模型对识别结果进行后处理(串长度固定)
void PostprocessByViterbi();                            //Viterbi算法对识别结果进行后处理(串长度固定,遍历,寻找最佳路径)
void GetT();                                            //得到初始状态概率模型(各个字母的首字出现频率)
void GetA();                                            //得到状态转移概率模型
void GetB(double b[],Result oneinresult[]);             //得到观察值概率分布模型
void WordLetters();                                     //维文单词字符结构对照表
void RecognizeWord(Result result[][LETTER_CLASSCOUNT],Point wordcut[][MAX_STROKE][MAX_LENGTH]);     //识别切分后单词
void GetWord(int result[],Result inresult[][LETTER_CLASSCOUNT]);       //得出单词识别结果    1-500  
void ReadFeature();               //读入训练特征



extern int letterresult[LETTER_CLASSCOUNT];               //单字符识别结果
//Preprocess Variables Reference
extern Point Data_preprocessed[MAX_STROKE][MAX_LENGTH];   //经过预处理的数据(联机)
//Cut Variables Reference
//Recognize Variables Reference
int LetterList[LETTER_CLASSCOUNT+24][3];           //维文对照
int radialposition[LETTER_CLASSCOUNT+RADIAL_COUNT];//维文主体部件位置图1-4 
int RGm[LETTER_MCOUNT][GRID_M_X*GRID_M_Y*4];       //字符网格方向特征存储         
int RGa[LETTER_SACOUNT][GRID_A_X*GRID_A_Y*4];
int RGd[LETTER_FDCOUNT][GRID_A_X*GRID_A_Y*4];
int RSm[LETTER_MCOUNT][STREAM_M*4];                //字符码流方向特征存储        
int RSa[LETTER_SACOUNT][STREAM_A*4];
int RSd[LETTER_FDCOUNT][STREAM_A*4];
int RCm[LETTER_MCOUNT][CODE_M];                    //字符链码方向特征存储        
int RCa[LETTER_SACOUNT][CODE_A];
int RCd[LETTER_FDCOUNT][CODE_A];
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//Global Variable Definition
int name;                                            //读文件时的编号名称
string pathname;                                    //样本库路径
Point Data[MAX_STROKE][MAX_LENGTH];                  //待识别数据(联机) 
int letterresult[LETTER_CLASSCOUNT];                 //单字符识别结果
int wordresult[WORD_CLASSCOUNT];                     //单词识别结果
//Preprocess Variable Definition
Point Data_preprocessed[MAX_STROKE][MAX_LENGTH];     //经过预处理的数据(联机)
//Cut Variable Definition
CutLabel strokeFlag[MAX_STROKE];                     //分解单词的笔划标注
Point Letters[WORD_LENGTH][MAX_STROKE][MAX_LENGTH];  //经过切分的单词数据(联机)
Point cutpoint[WORD_LENGTH];                         //切分点存储
int RadialList[4][3];                                //维文部件切分段构成对照表                               
int CGm[RADIAL_COUNT][GRID_M_X*GRID_M_Y*4];          //部件网格方向特征存储         
int CSm[RADIAL_COUNT][STREAM_M*4];                   //部件码流方向特征存储        
int CCm[RADIAL_COUNT][CODE_M];                       //部件链码方向特征存储        
//RecognizeWord Variable Definition
Result Wordinresult[WORD_LENGTH][LETTER_CLASSCOUNT]; //单词识别的中间结果
//Postprocess Variable Definition
int WordList[WORD_CLASSCOUNT][WORD_LENGTH];          //维文单词字符构成对照表 
double T[LETTER_CLASSCOUNT+1];                       //初始状态概率模型(包含空字符)
double A[LETTER_CLASSCOUNT+1][LETTER_CLASSCOUNT+1];  //转移概率模型(包含空字符,A[i][j]是当出现字母i时其后跟随字母j的概率)
double B[WORD_LENGTH][LETTER_CLASSCOUNT+1];          //观察值概率分布模型(包含空字符,此模型针对每组单字符识别结果,B[i][j]是识别结果是当前第i个字符时字母真值是j的概率)
int result[WORD_LENGTH];

#endif