#include "myHeader.h"
#include <fstream>
#include <iostream>
#include <jni.h>
#include "net_app_jni_Recognize.h"
#include "com_example_read_MainActivity.h"
int Mydata[30][2000];
int main();

int resultdata[5][10]={0};

int max(int x, int y)
{
	return x>y?x:y;
}
int min(int x, int y)
{
	return x<y?x:y;
}
JNIEXPORT jobjectArray JNICALL Java_net_app_jni_Recognize_recognize
  (JNIEnv * env, jobject object, jobjectArray arraydata){
    int k=0,j=0;
    jintArray myarray =(jintArray)( env->GetObjectArrayElement(arraydata, 0));
    for (k = 0; k < 30; k++){
    	    myarray =(jintArray)( env->GetObjectArrayElement(arraydata, k));
            jint *coldata = env->GetIntArrayElements((jintArray)myarray, 0);
            for (j=0; j<2000; j++) {
            Mydata[k][j]= coldata[j]; //ȡ��JAVA����arrayData������,����ֵ��JNI�е�����
            }
        env->ReleaseIntArrayElements((jintArray)myarray, coldata,0 );
    }
    main();
    jintArray array;
    jobjectArray result;
    jclass intArrcls=env->FindClass("[I");
    result=env->NewObjectArray(5,intArrcls,NULL);
    for(int i=0;i<5;i++){
       //����jint��һλ����
       jintArray intArray=env->NewIntArray(10);
       jint temp[10];
       for(int j=0;j<10;j++){
    		temp[j]=resultdata[i][j];
       }
       //����jint��һά�����ֵ
       env->SetIntArrayRegion(intArray,0,10,temp);
       //��object�������鸳ֵ�������ֶ�jintһά���������
       env->SetObjectArrayElement(result,i,intArray);
       env->DeleteLocalRef(intArray);
    }
    return result;
}
int main()
{
	//clear
	Initialize(Data);
	int i, j;
	for (i=0;i<LETTER_CLASSCOUNT;i++)
		letterresult[i]=1;            //1-128
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=1;              //1-500
	Initialize(Data_preprocessed);
	for (i=0;i<MAX_STROKE;i++)
	{
		strokeFlag[i].stroke=0;
		strokeFlag[i].cut_start=0;
		strokeFlag[i].cut_end=0;
		strokeFlag[i].belong=0;
	}
	for (i=0;i<WORD_LENGTH;i++)
		Initialize(Letters[i]);
	for (i=0;i<WORD_LENGTH;i++)
	{
		cutpoint[i].x=-1;
		cutpoint[i].y=-1;
	}
	for (i=0;i<WORD_LENGTH;i++)
	{
		for (j=0;j<LETTER_CLASSCOUNT;j++)
		{
			Wordinresult[i][j].number=0;
			Wordinresult[i][j].distance=0;
		}
	}
	ReadFeature();                        //ѵ������
	for (int i = 0; i < MAX_STROKE; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			Data[i][j].x=(int)Mydata[i][2*j];
			Data[i][j].y=(int)Mydata[i][2*j+1];
		}
	}
	if (Data[0][0].x==-1)
	{
		cout << "����ȷ��д����·����ͨ����ͼ���������ݣ�" << endl;
		return 0;
	}

	WordLetters();   
	LetterRadials(); 
	MainRadials();
	//Read��ť����
	for (i=0;i<MAX_STROKE;i++)
	{
		for (j=0;j<MAX_LENGTH;j++)     //��Ԥ�������ݸ�ֵ
			Data_preprocessed[i][j]=Data[i][j];//�Ǹ�MAX_STROKE*MAX_LENGTH�ľ���
	}
	//Preprocess��ť����
	Unitize(Data_preprocessed);         //��С��һ��
	Smooth(Data_preprocessed);          //ƽ��
	Smooth(Data_preprocessed);

	//Cut��ť����
	Baseline baseline;
	baseline=GetBaseline(Data_preprocessed);
	Stroke_C strokes[MAX_STROKE];
	GetStroke(strokes,Data_preprocessed);
	GetCutSect(strokes,Data_preprocessed);
	FirstCut(strokeFlag,strokes,Data_preprocessed,baseline.centre);
	SecondCut(strokeFlag,Data,strokes);

	//Recognize��ť����
	DecomposeWord(Letters,Data,strokeFlag);

	RecognizeWord(Wordinresult,Letters);
	GetWord(wordresult,Wordinresult);
	GetT();                       //�趨HMMģ��
	GetA();
	for (i=0;i<WORD_LENGTH;i++)
		GetB(B[i],Wordinresult[i]);
	PostprocessByHMM();
	for (int j = 0; j < 5; j++)
		{
			for (int i = 0 ; i < WORD_LENGTH; i++)
			{
				resultdata[j][i] = -1;
			}
		}
	for (int j = 0; j < 5; j++)
	{
		for (i=0;(i<WORD_LENGTH)&&(WordList[wordresult[j]-1][i]!=0);i++)
		{
			resultdata[j][i] = WordList[wordresult[j]-1][i];
			cout << WordList[wordresult[j]-1][i] << " ";
		}
		cout << endl;
	}
    i++;
    resultdata[j][i]=wordresult[0]-1;
	int g;
	cout << endl;
	cin >> g;
	return 0;
}



void Initialize(Point data[][MAX_LENGTH])                           //���ݳ�ʼ��.��ʼ�����ݶ���-1
{
	for (int i=0;i<MAX_STROKE;i++)//MAX_STROKE���ʻ���
		for (int j=0;j<MAX_LENGTH;j++)//MAX_LENGTH��ÿ����������=1000
		{
			data[i][j].x=-1;
			data[i][j].y=-1;
		}
}

void ReadFile(Point data[][MAX_LENGTH],string path,int n)           //���ļ���MAX_LENGTH��ÿ����������=1000����ȡpath�е�name���ļ����õ�һ��Point��ά����data
{

}//���������ѵ����ݶ��뵽data��

void FromDataToPixel(bool pixel[][DENSITY_Y],Point data[][MAX_LENGTH])     //��������Ϣ������
{
	int i,j,k,dx,dy;
	double x,x1,x2,y,y1,y2;
	for (i=0;i<DENSITY_X;i++)
	{
		for (j=0;j<DENSITY_Y;j++)
		{
			pixel[i][j]=0;
		}
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		pixel[data[i][0].x][data[i][0].y]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			x1=(double)data[i][j-1].x;
			y1=(double)data[i][j-1].y;
			x2=(double)data[i][j].x;
			y2=(double)data[i][j].y;
			dx=data[i][j].x-data[i][j-1].x;
			dy=data[i][j].y-data[i][j-1].y;
			if ((dx!=0)&&(dy!=0))
			{
				if (abs(dx)>abs(dy))
				{
					for (k=1;k<abs(dx);k++)
					{
						x=double(data[i][j-1].x+sign(dx)*k);
						y=(y2-y1)*(x-x1)/(x2-x1)+y1;
						pixel[data[i][j-1].x+sign(dx)*k][getnear(y)]=1;
					}
				}
				else
				{
					for (k=1;k<abs(dy);k++)
					{
						y=double(data[i][j-1].y+sign(dy)*k);
						x=(x2-x1)*(y-y1)/(y2-y1)+x1;
						pixel[getnear(x)][data[i][j-1].y+sign(dy)*k]=1;
					}
				}
			}
			else
			{
				if (dy==0)
				{
					for (k=1;k<abs(dx);k++)
					{
						pixel[data[i][j-1].x+sign(dx)*k][data[i][j].y]=1;
					}
				}
				if (dx==0)
				{
					for (k=1;k<abs(dy);k++)
					{
						pixel[data[i][j].x][data[i][j-1].y+sign(dy)*k]=1;
					}
				}	
			}
			pixel[data[i][j].x][data[i][j].y]=1;
		}
	}
	return;
}                                                      

int sign(int a)                                          //�õ������ݵķ���
{
	if (a>0)
		return 1;
	if (a<0)
		return -1;
	return 0;
}

int getnear(double a)                                   //�õ����������
{
	if (a<(double(int(a))+0.5))
		return int(a);
	else
		return int(a)+1;
}



string Display(int * data,int size)                      //��������ַ���
{
	string str="";
	string temp="";
	for (int i=0;i<size/4;i++)
	{ 
		str+="/";
		for (int j=i*4;j<(i+1)*4;j++)
		{
			//temp.("%d,",*(data+j));
			str+=temp;
		}
	}
	return str;
}

void ReadFeature()               //����ѵ������
{	
	int i,j;
	for (i=0;i<LETTER_MCOUNT;i++)                         //���ַ�ʶ��ѵ��������ʼ��
	{
		for (j=0;j<GRID_M_X*GRID_M_Y*4;j++)
			RGm[i][j]=0;
		for (j=0;j<STREAM_M*4;j++)
			RSm[i][j]=0;
		for (j=0;j<CODE_M;j++)
			RCm[i][j]=0;
	}
	for (i=0;i<LETTER_SACOUNT;i++)
	{
		for (j=0;j<GRID_A_X*GRID_A_Y*4;j++)
			RGa[i][j]=0;
		for (j=0;j<STREAM_A*4;j++)
			RSa[i][j]=0;
		for (j=0;j<CODE_A;j++)
			RCa[i][j]=0;
	}
	for (i=0;i<LETTER_FDCOUNT;i++)
	{
		for (j=0;j<GRID_A_X*GRID_A_Y*4;j++)
			RGd[i][j]=0;
		for (j=0;j<STREAM_A*4;j++)
			RSd[i][j]=0;
		for (j=0;j<CODE_A;j++)
			RCd[i][j]=0;
	}	                  
	for (i=0;i<RADIAL_COUNT;i++)                         //�����ʶ��ѵ��������ʼ��
	{
		for (j=0;j<GRID_M_X*GRID_M_Y*4;j++)
			CGm[i][j]=0;
		for (j=0;j<STREAM_M*4;j++)
			CSm[i][j]=0;
		for (j=0;j<CODE_M;j++)
			CCm[i][j]=0;
	}	
	FILE * fp;
	string filename;
	filename = "/storage/sdcard0/write.dat";
	if ((fp=fopen(filename.c_str(),"rb"))==NULL)
	{
		//printf("Can not create the file.\n");
		return;
	}
	for (i=0;(i<LETTER_MCOUNT)&&(!feof(fp));i++)           //�ַ�����������ȡ
	{
		fread(&RGm[i], sizeof(int), GRID_M_X*GRID_M_Y*4, fp);
	}
	for (i=0;(i<LETTER_SACOUNT)&&(!feof(fp));i++)
	{
		fread(&RGa[i], sizeof(int), GRID_A_X*GRID_A_Y*4, fp);
	}
	for (i=0;(i<LETTER_FDCOUNT)&&(!feof(fp));i++)
	{
		fread(&RGd[i], sizeof(int), GRID_A_X*GRID_A_Y*4, fp);
	}
	for (i=0;(i<LETTER_MCOUNT)&&(!feof(fp));i++)           //�ַ���������������ȡ
	{
		fread(&RSm[i], sizeof(int), STREAM_M*4, fp);
	}
	for (i=0;(i<LETTER_SACOUNT)&&(!feof(fp));i++)                         
	{
		fread(&RSa[i], sizeof(int), STREAM_A*4, fp);
	}
	for (i=0;(i<LETTER_FDCOUNT)&&(!feof(fp));i++)                         
	{
		fread(&RSd[i], sizeof(int), STREAM_A*4, fp);
	}
	for (i=0;(i<LETTER_MCOUNT)&&(!feof(fp));i++)            //�ַ����뷽��������ȡ
	{
		fread(&RCm[i], sizeof(int), CODE_M, fp);
	}
	for (i=0;(i<LETTER_SACOUNT)&&(!feof(fp));i++)                      
	{
		fread(&RCa[i], sizeof(int), CODE_A, fp);
	}
	for (i=0;(i<LETTER_FDCOUNT)&&(!feof(fp));i++)                      
	{
		fread(&RCd[i], sizeof(int), CODE_A, fp);
	}
	for (i=0;(i<RADIAL_COUNT)&&(!feof(fp));i++)              //����������������ȡ
	{
		fread(&CGm[i], sizeof(int), GRID_M_X*GRID_M_Y*4, fp);
	}
	for (i=0;(i<RADIAL_COUNT)&&(!feof(fp));i++)              //������������������ȡ
	{
		fread(&CSm[i], sizeof(int), STREAM_M*4, fp);
	}
	for (i=0;(i<RADIAL_COUNT)&&(!feof(fp));i++)              //�������뷽��������ȡ
	{
		fread(&CCm[i], sizeof(int), CODE_M, fp);
	}
	fclose(fp);
}

void RecognizeWord(Result result[][LETTER_CLASSCOUNT],Point wordcut[][MAX_STROKE][MAX_LENGTH])     //ʶ���зֺ󵥴�
{
	int i,j,k;
	for (i=0;i<WORD_LENGTH;i++)
	{
		for (j=0;j<LETTER_CLASSCOUNT;j++)
		{
			result[i][j].number=0;
			result[i][j].distance=0;
		}
	}
	Result temp_result[LETTER_CLASSCOUNT];
	Point letter[MAX_STROKE][MAX_LENGTH];
	for (i=0;(i<WORD_LENGTH)&&(wordcut[i][0][0].x!=-1);i++)
	{
		for (j=0;j<MAX_STROKE;j++)
		{
			for (k=0;k<MAX_LENGTH;k++)
				letter[j][k]=wordcut[i][j][k];
		}
		Unitize(letter);
		Connect(letter);       
		Smooth(letter);         
		Smooth(letter);
		Recognize(temp_result,letter);
		for (j=0;j<LETTER_CLASSCOUNT;j++)
			result[i][j]=temp_result[j];
	}
	return;
}

void GetWord(int result[],Result inresult[][LETTER_CLASSCOUNT])       //�ó�����ʶ����    1-500      
{
	int i;
	int letters[WORD_LENGTH];
	for (i=0;i<WORD_LENGTH;i++)
		letters[i]=inresult[i][0].number;
	int distance[WORD_CLASSCOUNT];
	for (i=0;i<WORD_CLASSCOUNT;i++)
		distance[i]=DistanceCode(letters,WordList[i],WORD_LENGTH);
	Sequence(distance,result,WORD_CLASSCOUNT);
	for (i=0;i<WORD_CLASSCOUNT;i++)
		result[i]++;
	/*	for (i=0;i<WORD_CLASSCOUNT;i++)
	result[i]=0;
	int j,length;
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
	length=0;
	for (j=0;j<WORD_LENGTH;j++)
	{
	if (letters[j]==WordList[i][j])
	length++;
	}
	if (length==WORD_LENGTH)
	{
	result[0]=i+1;
	break;
	}
	}*/
	return;
}

void RedundanceOut(Point data[][MAX_LENGTH])                   //ȥ����               
{
	int i,j,k;
	//ȥ������̫�ٵıʻ�
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if(data[i][MIN_LENGTH-1].x==-1)
		{
			if (i==MAX_STROKE-1)
			{
				for (k=0;k<MIN_LENGTH-1;k++)
				{
					data[i][k].x=-1;
					data[i][k].y=-1;
				}
			}
			else
			{
				for (j=i;(j<MAX_STROKE-1)&&(data[j][0].x!=-1);j++)
				{
					for (k=0;k<MAX_LENGTH;k++)
					{
						data[j][k].x=data[j+1][k].x;
						data[j][k].y=data[j+1][k].y;
					}
				}
				if (j==MAX_STROKE-1)
				{
					for (k=0;k<MAX_LENGTH;k++)
					{
						data[j][k].x=-1;
						data[j][k].y=-1;
					}
				}
				i--;
			}
		}
	}
	//ȥ���ʻ��е��ظ��㣨Ϊ������ƽ������������㣩
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			if ((data[i][j].x==data[i][j-1].x)&&(data[i][j].y==data[i][j-1].y))
			{
				for (k=j;k<MAX_LENGTH-1;k++)
				{
					data[i][k].x=data[i][k+1].x;
					data[i][k].y=data[i][k+1].y;
				}
				data[i][MAX_LENGTH-1].x=-1;
				data[i][MAX_LENGTH-1].y=-1;
			}
		}
	}
	return;
}

void Connect(Point data[][MAX_LENGTH])                             //�ϱ�����
{
	int i,j,k,x,y;
	int con[MAX_STROKE];	
	int end[MAX_STROKE];
	int num[MAX_STROKE];
	if (data[0][0].x==-1)
		return;
	//���ʻ������ǰ��ĳ���ʻ��յ�ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (k=0;k<i;k++)                                
		{
			x=data[i][0].x-data[k][end[k]].x;
			y=data[i][0].y-data[k][end[k]].y;
			if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
				(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				con[i]=k;
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][end[con[i]]+1+j].x=data[i][j].x;
				data[con[i]][end[con[i]]+1+j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ��յ���ǰ��ĳ���ʻ����ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (k=0;k<i;k++)                                
		{
			x=data[i][end[i]].x-data[k][0].x;
			y=data[i][end[i]].y-data[k][0].y;
			if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
				(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				con[i]=k;
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=end[con[i]];j>=0;j--)
			{
				data[con[i]][end[i]+1+j].x=data[con[i]][j].x;
				data[con[i]][end[i]+1+j].y=data[con[i]][j].y;
			}
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][j].x=data[i][j].x;
				data[con[i]][j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ��յ���ǰ��ĳ���ʻ��յ�ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (k=0;k<i;k++)                                
		{
			x=data[i][end[i]].x-data[k][end[k]].x;
			y=data[i][end[i]].y-data[k][end[k]].y;
			if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
				(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				con[i]=k;
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=end[i];j>=0;j--)
			{
				data[con[i]][end[con[i]]+1+end[i]-j].x=data[i][j].x;
				data[con[i]][end[con[i]]+1+end[i]-j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ������ǰ��ĳ���ʻ����ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (k=0;k<i;k++)                                
		{
			x=data[i][0].x-data[k][0].x;
			y=data[i][0].y-data[k][0].y;
			if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
				(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				con[i]=k;
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=end[con[i]];j>=0;j--)
			{
				data[con[i]][end[i]+1+j].x=data[con[i]][j].x;
				data[con[i]][end[i]+1+j].y=data[con[i]][j].y;
			}
			for (j=end[i];j>=0;j--)
			{
				data[con[i]][end[i]-j].x=data[i][j].x;
				data[con[i]][end[i]-j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ������ǰһ���ʻ���ĳ���ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (con[i]==-1)         
		{
			for (j=0;(j<MAX_LENGTH)&&(data[i-1][j].x!=-1);j++)
			{
				x=data[i][0].x-data[i-1][j].x;
				y=data[i][0].y-data[i-1][j].y;
				if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
					(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				{
					if (!(abs(data[i][0].y-data[i][end[i]].y)>4*abs(data[i][0].x-data[i][end[i]].x)))
					{
						con[i]=i-1;
						break;
					}
				}	
			}
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][end[con[i]]+1+j].x=data[i][j].x;
				data[con[i]][end[con[i]]+1+j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ�ĳ����ǰһ���ʻ��յ�ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (con[i]==-1)
		{                                               
			for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				x=data[i][j].x-data[i-1][end[i-1]].x;
				y=data[i][j].y-data[i-1][end[i-1]].y;
				if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
					(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				{
					con[i]=i-1;
					break;
				}
			}
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][end[con[i]]+1+j].x=data[i][j].x;
				data[con[i]][end[con[i]]+1+j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ��յ���ǰһ���ʻ�ĳ���ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	} 
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (con[i]==-1)
		{                                               
			for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				x=data[i][end[i]].x-data[i-1][j].x;
				y=data[i][end[i]].y-data[i-1][j].y;
				if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
					(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				{
					con[i]=i-1;
					break;
				}
			}
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=end[con[i]];j>=0;j--)
			{
				data[con[i]][end[i]+1+j].x=data[con[i]][j].x;
				data[con[i]][end[i]+1+j].y=data[con[i]][j].y;
			}
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][j].x=data[i][j].x;
				data[con[i]][j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//���ʻ�ĳ����ǰһ���ʻ����ӽ�
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (con[i]==-1)
		{                                               
			for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				x=data[i][j].x-data[i-1][0].x;
				y=data[i][j].y-data[i-1][0].y;
				if (((pow(x,2)+pow(y,2))<=MIN_DIS)||
					(sqrt((pow(x,2)+pow(y,2))<=num[i]/5)&&(sqrt(pow(x,2)+pow(y,2))<=num[k]/5)))
				{
					con[i]=i-1;
					break;
				}
			}
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=end[con[i]];j>=0;j--)
			{
				data[con[i]][end[i]+1+j].x=data[con[i]][j].x;
				data[con[i]][end[i]+1+j].y=data[con[i]][j].y;
			}
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][j].x=data[i][j].x;
				data[con[i]][j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
	//����ʻ����ڵ�һ��ʱ
	for (i=0;i<MAX_STROKE;i++)
	{
		con[i]=-1;
		end[i]=-1;
		num[i]=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{}
		end[i]=j-1;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		num[i]=1;
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			num[i]+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	int max=-1;
	int maxi=0;
	for (i=0;i<MAX_STROKE;i++)
	{
		if (num[i]>max)
		{
			max=num[i];
			maxi=i;
		}
	} 
	if (maxi!=0)
	{
		if (!((maxi==1)&&(end[2]==-1)&&(num[1]<3*num[0]/2)))
		{
			for (i=1;i<=maxi;i++)
				con[i]=i-1;
		}
	}
	for (i=MAX_STROKE-1;i>0;i--)
	{		
		if (con[i]!=-1)
		{
			for (j=0;j<=end[i];j++)
			{
				data[con[i]][end[con[i]]+1+j].x=data[i][j].x;
				data[con[i]][end[con[i]]+1+j].y=data[i][j].y;
				data[i][j].x=-1;
				data[i][j].y=-1;
			}
			end[con[i]]+=(end[i]+1);
			for (k=i+1;(k<MAX_STROKE)&&(data[k][0].x!=-1);k++)
				for (j=0;(j<MAX_LENGTH)&&(data[k][j].x!=-1);j++)
				{
					data[k-1][j].x=data[k][j].x;
					data[k-1][j].y=data[k][j].y;
					data[k][j].x=-1;
					data[k][j].y=-1;
				}
		}
	}
}

void Reverse(Point data[])                                   //�ʻ���ת
{
	int i,j;
	Point temp[MAX_LENGTH];
	for (i=0;i<MAX_LENGTH;i++)
		temp[i]=data[i];
	for (j=0;temp[j].x!=-1;j++)
	{}
	for (i=0;i<j;i++)
		data[i]=temp[j-1-i];
	return;
}

void Unitize(Point data[][MAX_LENGTH])                       //��С��һ��
{
	int i,j;
	double coef=1.0;
	double width,height;
	width=(double)DENSITY_X;//256�����ݿ�ȷֱ����
	height=(double)DENSITY_Y;//256�����ݸ߶ȷֱ����
	int x_start,x_end,y_start,y_end;
	if (data[0][0].x==-1)//û�гɹ���ȡ.dat�ļ�
		return;	
	x_start=data[0][0].x;//data[0][0]��� x_star,y_star;	
	x_end=data[0][0].x;
	y_start=data[0][0].y;	
	y_end=data[0][0].y;
	for(i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
		for(j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{   
			if (data[i][j].x<x_start) //x_start: �����к�������С�ĵ�             
				x_start=data[i][j].x;//x_end�������к��������ĵ�
			if (data[i][j].x>x_end)
				x_end=data[i][j].x;
			if (data[i][j].y<y_start)//y_start:��������������С�ĵ�
				y_start=data[i][j].y;
			if (data[i][j].y>y_end)//y_end:���������������ĵ�
				y_end=data[i][j].y;                
		}
		x_start=x_start-2;x_end=x_end+2;y_start=y_start-2;y_end=y_end+2;
		if ((width/abs(x_start-x_end))<(height/abs(y_start-y_end)))
			coef=width/abs(x_start-x_end);//��Ҫ��չ��С��һ����Ϊ��������
		else
			coef=height/abs(y_start-y_end);
		for(i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)//�������ʾ�ڣ�0����-��256��256��
			for(j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{                                       
				data[i][j].x=(int)((data[i][j].x-x_start)*coef);//�����Ͻ���ʾ����һ��
				data[i][j].y=(int)((data[i][j].y-y_start)*coef);
			} 
}

void Smooth(Point data[][MAX_LENGTH])                         //����ƽ��
{
	int i,j;
	if (data[0][0].x==-1)
		return;
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{	
			data[i][j].x=(int)(SMOOTH_COEF*data[i][j-1].x+(1-SMOOTH_COEF)*data[i][j].x);
			data[i][j].y=(int)(SMOOTH_COEF*data[i][j-1].y+(1-SMOOTH_COEF)*data[i][j].y);
		}
	}
}

void SlantCorrect(Point data[][MAX_LENGTH])           //��бУ��
{
	int angle;
	angle=GetSlantAngle(data);
}

int GetSlantAngle(Point data[][MAX_LENGTH])           //�õ���б��
{
	int i,j;
	bool data_pixel[DENSITY_X][DENSITY_Y];
	FromDataToPixel(data_pixel,data);                 //�������ѻ�����
	int hough[180][DENSITY_X*4];
	Hough(hough,data_pixel);
	int max,maxi,maxj;
	max=0;maxi=-1;maxj=-1;
	for (i=0;i<180;i++)
	{
		for (j=0;j<DENSITY_X*4;j++)
		{
			if (hough[i][j]>=max)
			{
				max=hough[i][j];
				maxi=i;
				maxj=j;
			}
		}
	}
	return maxi;
}

void Hough(int result[][DENSITY_X*4],bool pixel[][DENSITY_Y])        //Hough�任
{
	int i,j,k;
	for (i=0;i<180;i++)
	{
		for (j=0;j<DENSITY_X*4;j++)
		{
			result[i][j]=0;
		}
	}
	for(i=0;i<DENSITY_X;i++)
	{
		for(j=0;j<DENSITY_Y;j++)
		{
			if (pixel[i][j]==1)
			{
				for(k=0;k<180;k++) 
				{
					result[k][getnear(i*sin(k*PEI/180)+j*cos(k*PEI/180))+DENSITY_X*2]+=1;
				}
			}
		}	
	}
}

void PostprocessByHMM()                              //HMMģ�Ͷ��û�ʶ�������к���
{
	int i,j;
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=1;          //1-500
	double p[WORD_CLASSCOUNT];
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
		/*		p[i]=T[WordList[i][0]];      //�����ַ�������ת������
		for (j=1;j<WORD_LENGTH;j++)
		p[i]=p[i]*A[WordList[i][j-1]][WordList[i][j]];*/
		p[i]=1.0;
		for (j=0;j<WORD_LENGTH;j++)
			p[i]=p[i]*B[j][WordList[i][j]];
	}
	AntiSequence(p,wordresult,WORD_CLASSCOUNT);
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=wordresult[i]+1;
}

void PostprocessByHMM_Fixed()                         //HMMģ�Ͷ�ʶ�������к���(�����ȹ̶�)
{
	int i,j,length1,length2;
	double p[WORD_CLASSCOUNT];
	length1=0;
	for (j=0;j<WORD_LENGTH;j++)
	{
		if (Wordinresult[j][0].number!=0) 
			length1++;
	}
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=1;              //1-500
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
		length2=0;
		for (j=0;j<WORD_LENGTH;j++)
		{
			if (WordList[i][j]!=0) 
				length2++;
		}
		if (length1==length2)
		{
			p[i]=T[WordList[i][0]];
			for (j=1;j<WORD_LENGTH;j++)
				p[i]=p[i]*A[WordList[i][j-1]][WordList[i][j]];
			for (j=0;j<WORD_LENGTH;j++)
				p[i]=p[i]*B[j][WordList[i][j]];
		}
		else
			p[i]=0;
	}
	AntiSequence(p,wordresult,WORD_CLASSCOUNT);
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=wordresult[i]+1;
}

void PostprocessByViterbi()                        //Viterbi�㷨��ʶ�������к���(�����ȹ̶�,����,Ѱ�����·��)
{
	int i,j,t;
	double media[WORD_LENGTH][LETTER_CLASSCOUNT];
	double d[LETTER_CLASSCOUNT];
	int distance[WORD_CLASSCOUNT];
	int path[WORD_LENGTH-1][LETTER_CLASSCOUNT];
	int pathend;
	int num[LETTER_CLASSCOUNT];
	int length;
	int letters[WORD_LENGTH];
	for (i=0;i<WORD_LENGTH;i++)
		letters[i]=0;
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=1;              //1-500	
	length=0;
	for (j=0;j<WORD_LENGTH;j++)
	{
		if (Wordinresult[j][0].number!=0) 
			length++;
	}
	for (i=0;i<LETTER_CLASSCOUNT;i++)
	{
		media[0][i]=T[i+1]*B[0][i+1];
	}
	for (t=1;t<length;t++)
	{
		for (i=0;i<LETTER_CLASSCOUNT;i++)
		{
			for (j=0;j<LETTER_CLASSCOUNT;j++)
			{
				d[j]=media[t-1][j]*A[j+1][i+1];
			}	
			AntiSequence(d,num,LETTER_CLASSCOUNT);
			path[t-1][i]=num[0];
			media[t][i]=d[0]*B[t][i+1];
		}
	}
	for (j=0;j<LETTER_CLASSCOUNT;j++)
	{
		d[j]=media[length-1][j];
	}	
	AntiSequence(d,num,LETTER_CLASSCOUNT);
	pathend=num[0];
	letters[length-1]=pathend+1;
	for (i=length-2;i>=0;i--)
	{
		letters[i]=path[i][letters[i+1]-1]+1;
	}
	for (i=0;i<WORD_CLASSCOUNT;i++)
		distance[i]=DistanceCode(letters,WordList[i],WORD_LENGTH);
	Sequence(distance,wordresult,WORD_CLASSCOUNT);
	for (i=0;i<WORD_CLASSCOUNT;i++)
		wordresult[i]=wordresult[i]+1;
}

void GetT()                                   //�õ���ʼ״̬����ģ��(������ĸ�����ֳ���Ƶ��)
{
	int i;
	int t[LETTER_CLASSCOUNT+1];
	for (i=0;i<LETTER_CLASSCOUNT+1;i++)
	{
		T[i]=0.0;
		t[i]=0;
	}
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
		t[WordList[i][0]]+=1;
	}
	for (i=0;i<LETTER_CLASSCOUNT+1;i++)
	{
		T[i]=double(t[i])/WORD_CLASSCOUNT;
	} 
}

void GetA()                                   //�õ�״̬ת�Ƹ���ģ��
{
	int i,j;
	int a[LETTER_CLASSCOUNT+1][LETTER_CLASSCOUNT+1];
	int sum[LETTER_CLASSCOUNT+1];
	for (i=0;i<LETTER_CLASSCOUNT+1;i++)
	{
		for (j=0;j<LETTER_CLASSCOUNT+1;j++)
		{
			A[i][j]=0.0;
			a[i][j]=0;
		}
		sum[i]=0;
	}
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
		for (j=0;j<WORD_LENGTH-1;j++)
		{
			a[WordList[i][j]][WordList[i][j+1]]+=1;	
			sum[WordList[i][j]]+=1;
		}
	}
	for (i=0;i<LETTER_CLASSCOUNT+1;i++)
	{
		for (j=0;j<LETTER_CLASSCOUNT+1;j++)
		{
			if (sum[i]!=0)
				A[i][j]=double(a[i][j])/sum[i];
		}
	} 
}

void GetB(double b[],Result oneinresult[])                     //�õ��۲�ֵ���ʷֲ�ģ��
{
	double multi[LETTER_CLASSCOUNT];
	double sum;
	int i;
	if (oneinresult[0].number==0)
	{
		for (i=1;i<LETTER_CLASSCOUNT+1;i++)
			b[i]=0.001;
		b[0]=1-b[1]*LETTER_CLASSCOUNT;
	}
	else
	{
		b[0]=0.001;
		for (i=0;i<LETTER_CLASSCOUNT;i++)
			multi[i]=1.0/oneinresult[i].distance; 
		sum=0.0;
		for (i=0;i<LETTER_CLASSCOUNT;i++)
			sum+=multi[i];
		for (i=0;i<LETTER_CLASSCOUNT;i++)
			b[oneinresult[i].number]=multi[i]*(1-b[0])/sum;
	}
}

void WordLetters()                                           //ά�ĵ����ַ��ṹ���ձ�
{
	int i,j;
	for (i=0;i<WORD_CLASSCOUNT;i++)
	{
		for (j=0;j<WORD_LENGTH;j++)
			WordList[i][j]=0;
	}
	i=0;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=58;
	i=1;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=5;  WordList[i][4]=58;
	WordList[i][5]=77; WordList[i][6]=57; WordList[i][7]=63; 
	i=2;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=66; WordList[i][4]=120;
	WordList[i][5]=61; WordList[i][6]=25; WordList[i][7]=81;
	i=3;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=66; WordList[i][4]=120;
	WordList[i][5]=111;WordList[i][6]=58; WordList[i][7]=65; WordList[i][8]=37; WordList[i][9]=102;
	i=4;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=118;WordList[i][4]=63;
	i=5;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=58; WordList[i][4]=99;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=98;  
	i=6;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=58; WordList[i][4]=107;
	WordList[i][5]=92; WordList[i][6]=25;  
	i=7;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=112;WordList[i][4]=120;
	WordList[i][5]=83; WordList[i][6]=96; WordList[i][7]=37; WordList[i][8]=78; WordList[i][9]=57;
	i=8;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=8;  WordList[i][3]=9;  WordList[i][4]=37;
	WordList[i][5]=98;  
	i=9;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=8;  WordList[i][3]=100;WordList[i][4]=25;
	WordList[i][5]=6;  
	i=10;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=84; WordList[i][4]=25;
	i=11;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=88; WordList[i][4]=58;
	WordList[i][5]=109;  
	i=12;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=37; WordList[i][4]=78;
	WordList[i][5]=36; WordList[i][6]=76; WordList[i][7]=57;  
	i=13;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=75; WordList[i][3]=57; WordList[i][4]=85;
	i=14;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=63;
	i=15;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=113;
	i=16;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=65; WordList[i][4]=37;
	WordList[i][5]=82; 
	i=17;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=65; WordList[i][4]=25;
	WordList[i][5]=97; 
	i=18;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=99; WordList[i][4]=37;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=98; 
	i=19;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=111;WordList[i][4]=37;
	WordList[i][5]=126;WordList[i][6]=118;  
	i=20;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=36; WordList[i][4]=108;
	WordList[i][5]=37; WordList[i][6]=98;  
	i=21;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=79; WordList[i][3]=57; WordList[i][4]=65;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=98; 
	i=22;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=58; WordList[i][4]=83;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=98; 
	i=23;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=58; WordList[i][4]=83;
	WordList[i][5]=118;WordList[i][6]=113; 
	i=24;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=66; WordList[i][4]=58;
	i=25;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=96; WordList[i][4]=58;
	WordList[i][5]=107;WordList[i][6]=64;
	i=26;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=112;WordList[i][4]=58;
	WordList[i][5]=113; 
	i=27;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=126;
	WordList[i][5]=58;  
	i=28;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=5;  WordList[i][4]=118;
	WordList[i][5]=79;  
	i=29;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=74; WordList[i][4]=58;
	WordList[i][5]=115;WordList[i][6]=58; 
	i=30;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=100;WordList[i][4]=58;
	WordList[i][5]=79; WordList[i][6]=57; WordList[i][7]=113; 
	i=31;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=91; WordList[i][3]=37; WordList[i][4]=116;
	WordList[i][5]=118;  
	i=32;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=95; WordList[i][3]=78; WordList[i][4]=36;
	WordList[i][5]=100;WordList[i][6]=58; 
	i=33;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=127;WordList[i][3]=16; WordList[i][4]=37;
	WordList[i][5]=76; WordList[i][6]=117; 
	i=34;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=107;WordList[i][3]=76; WordList[i][4]=36;
	WordList[i][5]=78; WordList[i][6]=57; WordList[i][7]=85; 
	i=35;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=62; WordList[i][4]=58;
	WordList[i][5]=77; 
	i=36;
	WordList[i][0]=55; WordList[i][1]=58; WordList[i][2]=30; WordList[i][3]=57; WordList[i][4]=77;
	WordList[i][5]=117;
	i=37;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=61; WordList[i][3]=76; WordList[i][4]=57;
	WordList[i][5]=113;  
	i=38;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=65; WordList[i][3]=66; WordList[i][4]=25;
	WordList[i][5]=77; WordList[i][6]=57; 
	i=39;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=73; WordList[i][3]=88; WordList[i][4]=58;
	WordList[i][5]=85; 
	i=40;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=77; WordList[i][3]=14; WordList[i][4]=58;
	WordList[i][5]=113;
	i=41;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=77; WordList[i][3]=111;WordList[i][4]=58;
	WordList[i][5]=113;
	i=42;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=91; WordList[i][3]=25; WordList[i][4]=105;
	i=43;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=25; WordList[i][4]=65; 
	WordList[i][5]=25; WordList[i][6]=85; 
	i=44;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=25; WordList[i][4]=85;
	i=45;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=25; WordList[i][4]=91;
	WordList[i][5]=25; WordList[i][6]=8;  WordList[i][7]=122; 
	i=46;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=127;WordList[i][3]=115;WordList[i][4]=112;
	WordList[i][5]=58; WordList[i][6]=97; 
	i=47;
	WordList[i][0]=55; WordList[i][1]=120;WordList[i][2]=111;WordList[i][3]=120;WordList[i][4]=111;
	WordList[i][5]=122;
	i=48;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=5;  WordList[i][3]=58; WordList[i][4]=75; 
	WordList[i][5]=117;
	i=49;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=78; WordList[i][6]=36; WordList[i][7]=108;WordList[i][8]=104;WordList[i][9]=58;
	i=50;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=84; WordList[i][4]=37;
	WordList[i][5]=97;
	i=51;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=37; WordList[i][4]=110;
	i=52;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=9;  WordList[i][4]=122;
	i=53;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=74; WordList[i][4]=58;
	WordList[i][5]=115;WordList[i][6]=58;
	i=54;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=86; 
	i=55;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=37; WordList[i][4]=102;
	i=56;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=100;WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=76; WordList[i][6]=57; WordList[i][7]=77;
	i=57;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=104;WordList[i][3]=104;WordList[i][4]=122;
	i=58;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=104;WordList[i][3]=104;WordList[i][4]=37;
	WordList[i][5]=116;WordList[i][6]=9;  WordList[i][7]=122; 
	i=59;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=128;WordList[i][3]=67;
	i=60;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=92; WordList[i][4]=58;
	WordList[i][5]=77;  
	i=61;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=17; WordList[i][6]=58; WordList[i][7]=113; 
	i=62;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=104;WordList[i][4]=58;
	WordList[i][5]=115;WordList[i][6]=37; WordList[i][7]=126;WordList[i][8]=118;WordList[i][9]=63;
	i=63;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=58; WordList[i][4]=97;
	i=64;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=66; WordList[i][4]=58;
	WordList[i][5]=125;WordList[i][6]=37; WordList[i][7]=114; 
	i=65;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=84; WordList[i][4]=58;
	WordList[i][5]=113; 
	i=66;
	WordList[i][0]=55; WordList[i][1]=37; WordList[i][2]=126;WordList[i][3]=25; WordList[i][4]=105;
	i=67;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=8;  WordList[i][3]=112;WordList[i][4]=118;
	WordList[i][5]=81;
	i=68;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=77; WordList[i][3]=75; WordList[i][4]=117;
	WordList[i][5]=101; 
	i=69;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=77; WordList[i][3]=28; WordList[i][4]=101;
	i=70;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=79; WordList[i][3]=55; WordList[i][4]=58;
	WordList[i][5]=77; WordList[i][6]=57; 
	i=71;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=79; WordList[i][3]=14; WordList[i][4]=118;
	WordList[i][5]=77; WordList[i][6]=14; WordList[i][7]=29; WordList[i][8]=8;  WordList[i][9]=122;
	i=72;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=14; WordList[i][3]=29; WordList[i][4]=113;
	i=73;
	WordList[i][0]=55; WordList[i][1]=27; WordList[i][2]=107;WordList[i][3]=9;  WordList[i][4]=118;
	WordList[i][5]=109; 
	i=74;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=65; WordList[i][3]=25; WordList[i][4]=97;
	i=75;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=8;  WordList[i][3]=100;WordList[i][4]=58;
	WordList[i][5]=115;WordList[i][6]=76; WordList[i][7]=117;WordList[i][8]=101;
	i=76;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=73; WordList[i][3]=128;WordList[i][4]=85;
	i=77;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=79; WordList[i][3]=24; WordList[i][4]=113;
	i=78;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=83; WordList[i][3]=84; WordList[i][4]=25;
	WordList[i][5]=105; 
	i=79;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=83; WordList[i][3]=25; WordList[i][4]=107;
	WordList[i][5]=122; 
	i=80;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=89;
	i=81;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=115;WordList[i][3]=37; WordList[i][4]=21;
	i=82;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=125;WordList[i][3]=92; WordList[i][4]=25;
	WordList[i][5]=77; 
	i=83;
	WordList[i][0]=55; WordList[i][1]=25; WordList[i][2]=125;WordList[i][3]=92; WordList[i][4]=25;
	WordList[i][5]=113;
	i=84;
	WordList[i][0]=55; WordList[i][1]=29; WordList[i][2]=77; WordList[i][3]=28; WordList[i][4]=111;
	WordList[i][5]=9;  WordList[i][6]=122;
	i=85;
	WordList[i][0]=55; WordList[i][1]=29; WordList[i][2]=79; WordList[i][3]=24; WordList[i][4]=109;
	i=86;
	WordList[i][0]=55; WordList[i][1]=29; WordList[i][2]=115;WordList[i][3]=29; WordList[i][4]=109;
	i=87;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=37; WordList[i][4]=64;
	i=88;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=92; WordList[i][3]=37; WordList[i][4]=78;
	i=89;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=92; WordList[i][3]=37; WordList[i][4]=80;
	i=90;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=128;WordList[i][3]=113;
	i=91;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=37; WordList[i][4]=86;
	i=92;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=37; WordList[i][4]=88;
	WordList[i][5]=37; WordList[i][6]=86;
	i=93;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=35; WordList[i][4]=104;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=78; 
	i=94;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=116;WordList[i][3]=37; WordList[i][4]=98;
	i=95;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=17; WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=112;WordList[i][6]=58; WordList[i][7]=105; 
	i=96;
	WordList[i][0]=55; WordList[i][1]=35; WordList[i][2]=17; WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=126;WordList[i][6]=58; WordList[i][7]=67; 
	i=97;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=78; WordList[i][4]=57;
	WordList[i][5]=2;  
	i=98;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=37; WordList[i][4]=15;
	WordList[i][5]=118;WordList[i][6]=113; 
	i=99;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=118;
	i=100;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=73; WordList[i][3]=112;WordList[i][4]=118;
	WordList[i][5]=97; 
	i=101;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=59;
	i=102;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=117;WordList[i][4]=59;
	i=103;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=66; WordList[i][4]=58;
	WordList[i][5]=125;WordList[i][6]=37; WordList[i][7]=76; WordList[i][8]=36; WordList[i][9]=106;
	i=104;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=104;WordList[i][4]=118;
	WordList[i][5]=77; WordList[i][6]=65; WordList[i][7]=37; WordList[i][8]=86;
	i=105;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=108;WordList[i][4]=122;
	i=106;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=78;
	i=107;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=118;WordList[i][4]=77;
	i=108;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=99; WordList[i][3]=37; WordList[i][4]=106;
	i=109;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=14; WordList[i][3]=37; WordList[i][4]=88;
	WordList[i][5]=37; WordList[i][6]=86; 
	i=110;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=14; WordList[i][3]=118;WordList[i][4]=77;
	i=111;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=127; 
	i=112;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=96; WordList[i][4]=37;
	WordList[i][5]=78; WordList[i][6]=117; 
	i=113;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=31; WordList[i][4]=117;
	WordList[i][5]=65; WordList[i][6]=66; WordList[i][7]=118;  
	i=114;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=37; WordList[i][4]=84;
	WordList[i][5]=118;
	i=115;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=118;WordList[i][4]=81;
	i=116;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=118;WordList[i][4]=107;
	WordList[i][5]=37; WordList[i][6]=124;
	i=117;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=70; WordList[i][4]=29;
	WordList[i][5]=77; 
	i=118;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=84; WordList[i][4]=37;
	WordList[i][5]=78; WordList[i][6]=117;WordList[i][7]=85; 
	i=119;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=15; WordList[i][4]=37;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=126;WordList[i][8]=118; 
	i=120;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=16; WordList[i][3]=112;WordList[i][4]=37;
	WordList[i][5]=126;WordList[i][6]=118;WordList[i][7]=63;
	i=121;
	WordList[i][0]=55; WordList[i][1]=118;WordList[i][2]=30; WordList[i][3]=127;WordList[i][4]=63;
	i=122;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=79; WordList[i][3]=57; WordList[i][4]=77;
	i=123;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=85; 
	i=124;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=100;WordList[i][4]=58;
	i=125;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=100;WordList[i][4]=25;
	WordList[i][5]=77; WordList[i][6]=111;WordList[i][7]=58; WordList[i][8]=97; 
	i=126;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=98;  
	i=127;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=127; 
	i=128;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=115;WordList[i][3]=104;WordList[i][4]=58;
	i=129;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=58; WordList[i][4]=113;
	i=130;
	WordList[i][0]=61; WordList[i][1]=58; WordList[i][2]=16; WordList[i][3]=58;
	i=131;
	WordList[i][0]=61; WordList[i][1]=120;WordList[i][2]=65; WordList[i][3]=25; WordList[i][4]=107;
	WordList[i][5]=104;WordList[i][6]=58; 
	i=132;
	WordList[i][0]=61; WordList[i][1]=120;WordList[i][2]=91; WordList[i][3]=58; WordList[i][4]=107;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=78;  
	i=133;
	WordList[i][0]=61; WordList[i][1]=120;WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=4;
	WordList[i][5]=112;WordList[i][6]=25; 
	i=134;
	WordList[i][0]=61; WordList[i][1]=37; WordList[i][2]=56; WordList[i][3]=58; WordList[i][4]=77;
	WordList[i][5]=57; WordList[i][6]=109; 
	i=135;
	WordList[i][0]=61; WordList[i][1]=37; WordList[i][2]=56; WordList[i][3]=120;WordList[i][4]=107;
	WordList[i][5]=120;WordList[i][6]=14; WordList[i][7]=37; WordList[i][8]=126;WordList[i][9]=118;
	i=136;
	WordList[i][0]=61; WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=75; WordList[i][4]=117;
	WordList[i][5]=109; 
	i=137;
	WordList[i][0]=61; WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=118;WordList[i][4]=113;
	i=138;
	WordList[i][0]=61; WordList[i][1]=27; WordList[i][2]=77; WordList[i][3]=117;
	i=139;
	WordList[i][0]=61; WordList[i][1]=27; WordList[i][2]=107;WordList[i][3]=29; WordList[i][4]=85;
	i=140;
	WordList[i][0]=61; WordList[i][1]=27; WordList[i][2]=107;WordList[i][3]=118;WordList[i][4]=101;
	i=141;
	WordList[i][0]=61; WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=75;
	WordList[i][5]=117;WordList[i][6]=77; 
	i=142;
	WordList[i][0]=61; WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=24; WordList[i][4]=113;
	i=143;
	WordList[i][0]=61; WordList[i][1]=25; WordList[i][2]=91; WordList[i][3]=76; WordList[i][4]=57;
	WordList[i][5]=123; 
	i=144;
	WordList[i][0]=61; WordList[i][1]=29; WordList[i][2]=14; WordList[i][3]=29; WordList[i][4]=113;
	i=145;
	WordList[i][0]=61; WordList[i][1]=35; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=86;
	i=146;
	WordList[i][0]=61; WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=37; WordList[i][4]=110;
	i=147;
	WordList[i][0]=61; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=37; WordList[i][4]=98;
	i=148;
	WordList[i][0]=61; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=118;WordList[i][4]=63;
	i=149;
	WordList[i][0]=61; WordList[i][1]=118;WordList[i][2]=63;
	i=150;
	WordList[i][0]=61; WordList[i][1]=118;WordList[i][2]=73; WordList[i][3]=64;
	i=151;
	WordList[i][0]=61; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=113;
	i=152;
	WordList[i][0]=61; WordList[i][1]=118;WordList[i][2]=79; WordList[i][3]=121;
	i=153;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=66; WordList[i][4]=58;
	i=154;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=22;
	WordList[i][5]=128;WordList[i][6]=85; 
	i=155;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=65; WordList[i][4]=58;
	i=156;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=103;WordList[i][3]=37; WordList[i][4]=64;
	i=157;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=103;WordList[i][3]=37; WordList[i][4]=80;
	i=158;
	WordList[i][0]=4;  WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=66; WordList[i][4]=118;
	WordList[i][5]=73; WordList[i][6]=64; 
	i=159;
	WordList[i][0]=4;  WordList[i][1]=78; WordList[i][2]=119;WordList[i][3]=14; WordList[i][4]=78;
	WordList[i][5]=57; WordList[i][6]=111;WordList[i][7]=112;WordList[i][8]=58; 
	i=160;
	WordList[i][0]=4;  WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=116;
	WordList[i][5]=84; WordList[i][6]=37; WordList[i][7]=3; 
	i=161;
	WordList[i][0]=4;  WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=88; WordList[i][4]=37;
	WordList[i][5]=98; 
	i=162;
	WordList[i][0]=4;  WordList[i][1]=37; WordList[i][2]=104;WordList[i][3]=37; WordList[i][4]=78;
	i=163;
	WordList[i][0]=4;  WordList[i][1]=37; WordList[i][2]=126;WordList[i][3]=58; WordList[i][4]=75;
	WordList[i][5]=117; 
	i=164;
	WordList[i][0]=4;  WordList[i][1]=25; WordList[i][2]=63; 
	i=165;
	WordList[i][0]=4;  WordList[i][1]=25; WordList[i][2]=65; WordList[i][3]=62; WordList[i][4]=120;
	WordList[i][5]=105; 
	i=166;
	WordList[i][0]=4;  WordList[i][1]=25; WordList[i][2]=73; WordList[i][3]=66; WordList[i][4]=58;
	i=167;
	WordList[i][0]=4;  WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=83; WordList[i][4]=118;
	WordList[i][5]=63; 
	i=168;
	WordList[i][0]=4;  WordList[i][1]=25; WordList[i][2]=87; WordList[i][3]=58; WordList[i][4]=125;
	WordList[i][5]=112;WordList[i][6]=58; WordList[i][7]=113;  
	i=169;
	WordList[i][0]=4;  WordList[i][1]=29; WordList[i][2]=65; WordList[i][3]=29; WordList[i][4]=113;
	i=170;
	WordList[i][0]=4;  WordList[i][1]=35; WordList[i][2]=9;  WordList[i][3]=37; WordList[i][4]=116;
	WordList[i][5]=118; 
	i=171;
	WordList[i][0]=4;  WordList[i][1]=35; WordList[i][2]=88; WordList[i][3]=100;WordList[i][4]=118;
	WordList[i][5]=75; WordList[i][6]=117;WordList[i][7]=109;  
	i=172;
	WordList[i][0]=4;  WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=106;
	i=173;
	WordList[i][0]=4;  WordList[i][1]=118;WordList[i][2]=125;WordList[i][3]=88; WordList[i][4]=118;
	WordList[i][5]=115;WordList[i][6]=62; WordList[i][7]=118;  
	i=174;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=66; WordList[i][4]=58;
	i=175;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=65; WordList[i][4]=37;
	WordList[i][5]=86;  
	i=176;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=99; WordList[i][4]=37;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=86;
	i=177;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=111;WordList[i][4]=58;
	WordList[i][5]=97;  
	i=178;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=36; WordList[i][4]=72;
	i=179;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=36; WordList[i][4]=110;
	i=180;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=89;  
	i=181;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=107;WordList[i][3]=128;WordList[i][4]=85;
	i=182;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=58; WordList[i][4]=97;
	i=183;
	WordList[i][0]=65; WordList[i][1]=58; WordList[i][2]=30; WordList[i][3]=24; WordList[i][4]=79;
	i=184;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=4;  WordList[i][3]=128;WordList[i][4]=87;
	WordList[i][5]=112;WordList[i][6]=58; WordList[i][7]=97;  
	i=185;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=4;  WordList[i][3]=128;WordList[i][4]=109;
	i=186;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=87; WordList[i][3]=100;WordList[i][4]=58;
	WordList[i][5]=113; 
	i=187;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=91; WordList[i][3]=78; WordList[i][4]=57;
	i=188;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=58; WordList[i][4]=6;
	i=189;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=100;WordList[i][4]=25;
	WordList[i][5]=79; 
	i=190;
	WordList[i][0]=65; WordList[i][1]=120;WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=99;
	WordList[i][5]=84; WordList[i][6]=37; WordList[i][7]=80; 
	i=191;
	WordList[i][0]=65; WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=88;
	WordList[i][5]=9;  WordList[i][6]=58; WordList[i][7]=113;  
	i=192;
	WordList[i][0]=65; WordList[i][1]=37; WordList[i][2]=106; 
	i=193;
	WordList[i][0]=65; WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=88; WordList[i][4]=25;
	WordList[i][5]=115;WordList[i][6]=58; WordList[i][7]=81;
	i=194;
	WordList[i][0]=65; WordList[i][1]=126;WordList[i][2]=118;WordList[i][3]=115;WordList[i][4]=56;
	WordList[i][5]=118;WordList[i][6]=115;WordList[i][7]=112;WordList[i][8]=37; WordList[i][9]=114;
	i=195;
	WordList[i][0]=65; WordList[i][1]=27; WordList[i][2]=87; WordList[i][3]=29; WordList[i][4]=101;
	i=196;
	WordList[i][0]=65; WordList[i][1]=27; WordList[i][2]=111;WordList[i][3]=29; WordList[i][4]=77;
	i=197;
	WordList[i][0]=65; WordList[i][1]=27; WordList[i][2]=16; WordList[i][3]=5;  WordList[i][4]=118;
	i=198;
	WordList[i][0]=65; WordList[i][1]=27; WordList[i][2]=30; WordList[i][3]=117;WordList[i][4]=113;
	i=199;
	WordList[i][0]=65; WordList[i][1]=25; WordList[i][2]=4;  WordList[i][3]=78; WordList[i][4]=57;
	WordList[i][5]=97; 
	i=200;
	WordList[i][0]=65; WordList[i][1]=25; WordList[i][2]=73; WordList[i][3]=25; WordList[i][4]=109;
	i=201;
	WordList[i][0]=65; WordList[i][1]=25; WordList[i][2]=91; WordList[i][3]=100;WordList[i][4]=58;
	WordList[i][5]=113;  
	i=202;
	WordList[i][0]=65; WordList[i][1]=29; WordList[i][2]=79; WordList[i][3]=28; WordList[i][4]=107;
	WordList[i][5]=15; WordList[i][6]=118;WordList[i][7]=113; 
	i=203;
	WordList[i][0]=65; WordList[i][1]=29; WordList[i][2]=14; WordList[i][3]=37; WordList[i][4]=112;
	WordList[i][5]=118;WordList[i][6]=101;  
	i=204;
	WordList[i][0]=65; WordList[i][1]=35; WordList[i][2]=5;  WordList[i][3]=37; WordList[i][4]=86;
	i=205;
	WordList[i][0]=65; WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=35; WordList[i][4]=96;
	WordList[i][5]=120;WordList[i][6]=113;
	i=206;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=4;  WordList[i][3]=84; WordList[i][4]=37;
	WordList[i][5]=108;WordList[i][6]=122;  
	i=207;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=37; WordList[i][4]=106;
	i=208;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=69; WordList[i][3]=78; WordList[i][4]=36;
	WordList[i][5]=62; WordList[i][6]=118;  
	i=209;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=73; WordList[i][3]=84; WordList[i][4]=37;
	WordList[i][5]=112;WordList[i][6]=58; WordList[i][7]=63;  
	i=210;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=73; WordList[i][3]=112;WordList[i][4]=37;
	WordList[i][5]=116;WordList[i][6]=118;WordList[i][7]=113;  
	i=211;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=65; WordList[i][4]=37;
	WordList[i][5]=5;  WordList[i][6]=108;WordList[i][7]=37; WordList[i][8]=102;
	i=212;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=117;WordList[i][4]=99;
	WordList[i][5]=100;WordList[i][6]=37; WordList[i][7]=126;WordList[i][8]=58; WordList[i][9]=63;
	i=213;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=78; WordList[i][4]=57;
	WordList[i][5]=77; WordList[i][6]=127;WordList[i][7]=85;  
	i=214;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=88; WordList[i][4]=29;
	WordList[i][5]=77; WordList[i][6]=28; WordList[i][7]=85; 
	i=215;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=3;  
	i=216;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=118;WordList[i][4]=2;
	i=217;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=16; WordList[i][3]=76; WordList[i][4]=36;
	WordList[i][5]=64; 
	i=218;
	WordList[i][0]=65; WordList[i][1]=118;WordList[i][2]=16; WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=106;
	i=219;
	WordList[i][0]=69; WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=58; WordList[i][4]=55;
	WordList[i][5]=118;WordList[i][6]=63;  
	i=220;
	WordList[i][0]=69; WordList[i][1]=58; WordList[i][2]=16; WordList[i][3]=37; WordList[i][4]=106;
	i=221;
	WordList[i][0]=69; WordList[i][1]=58; WordList[i][2]=30; WordList[i][3]=57; WordList[i][4]=2;
	i=222;
	WordList[i][0]=69; WordList[i][1]=120;WordList[i][2]=79; WordList[i][3]=57;
	i=223;
	WordList[i][0]=69; WordList[i][1]=120;WordList[i][2]=22; WordList[i][3]=15; WordList[i][4]=25;
	i=224;
	WordList[i][0]=69; WordList[i][1]=37; WordList[i][2]=76; WordList[i][3]=75; WordList[i][4]=121;
	i=225;
	WordList[i][0]=69; WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=21;
	i=226;
	WordList[i][0]=69; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=58; WordList[i][4]=125;
	WordList[i][5]=118;WordList[i][6]=63; 
	i=227;
	WordList[i][0]=69; WordList[i][1]=29; WordList[i][2]=111;WordList[i][3]=108;WordList[i][4]=118;
	i=228;
	WordList[i][0]=69; WordList[i][1]=29; WordList[i][2]=111;WordList[i][3]=118;
	i=229;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=30; WordList[i][4]=117;
	WordList[i][5]=105;  
	i=230;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=125;WordList[i][4]=58;
	WordList[i][5]=113;  
	i=231;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=56; WordList[i][4]=37;
	WordList[i][5]=124;
	i=232;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=37; WordList[i][4]=56;
	WordList[i][5]=118;WordList[i][6]=63;  
	i=233;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=25; WordList[i][4]=2;
	i=234;
	WordList[i][0]=69; WordList[i][1]=118;WordList[i][2]=22; WordList[i][3]=9;  WordList[i][4]=122;
	i=235;
	WordList[i][0]=8;  WordList[i][1]=58; WordList[i][2]=6;  
	i=236;
	WordList[i][0]=8;  WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=66; WordList[i][4]=120;
	WordList[i][5]=65; WordList[i][6]=58;  
	i=237;
	WordList[i][0]=8;  WordList[i][1]=58; WordList[i][2]=89;  
	i=238;
	WordList[i][0]=8;  WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=76; WordList[i][4]=57;
	WordList[i][5]=113;
	i=239;
	WordList[i][0]=8;  WordList[i][1]=120;WordList[i][2]=99; WordList[i][3]=25; WordList[i][4]=109;
	i=240;
	WordList[i][0]=8;  WordList[i][1]=120;WordList[i][2]=103;WordList[i][3]=58;
	i=241;
	WordList[i][0]=8;  WordList[i][1]=120;WordList[i][2]=20;  
	i=242;
	WordList[i][0]=8;  WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=57; WordList[i][4]=123;
	i=243;
	WordList[i][0]=8;  WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=57; WordList[i][4]=125;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=98; 
	i=244;
	WordList[i][0]=8;  WordList[i][1]=37; WordList[i][2]=100;WordList[i][3]=37; WordList[i][4]=86;
	i=245;
	WordList[i][0]=8;  WordList[i][1]=29; WordList[i][2]=85;
	i=246;
	WordList[i][0]=8;  WordList[i][1]=29; WordList[i][2]=115;WordList[i][3]=104;WordList[i][4]=122;
	i=247;
	WordList[i][0]=8;  WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=128;
	i=248;
	WordList[i][0]=8;  WordList[i][1]=35; WordList[i][2]=108;WordList[i][3]=118;WordList[i][4]=101;
	i=249;
	WordList[i][0]=8;  WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=56; WordList[i][4]=118;
	WordList[i][5]=105;
	i=250;
	WordList[i][0]=8;  WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=102;
	i=251;
	WordList[i][0]=73; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=58;
	i=252;
	WordList[i][0]=73; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=37; WordList[i][4]=78;
	WordList[i][5]=117;  
	i=253;
	WordList[i][0]=73; WordList[i][1]=120;WordList[i][2]=69; WordList[i][3]=58;WordList[i][4]=125;
	WordList[i][5]=37; WordList[i][6]=114;
	i=254;
	WordList[i][0]=73; WordList[i][1]=120;WordList[i][2]=87; WordList[i][3]=116;WordList[i][4]=58;
	i=255;
	WordList[i][0]=73; WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=37; WordList[i][4]=126;
	WordList[i][5]=118; 
	i=256;
	WordList[i][0]=73; WordList[i][1]=37; WordList[i][2]=80; WordList[i][3]=111;WordList[i][4]=118;
	WordList[i][5]=63;  
	i=257;
	WordList[i][0]=73; WordList[i][1]=25; WordList[i][2]=127;WordList[i][3]=83; WordList[i][4]=118;
	i=258;
	WordList[i][0]=73; WordList[i][1]=35; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=76;
	WordList[i][5]=57; WordList[i][6]=77;
	i=259;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=118;WordList[i][4]=77;
	WordList[i][5]=107;WordList[i][6]=37; WordList[i][7]=102;
	i=260;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=36; WordList[i][4]=66;
	WordList[i][5]=118;
	i=261;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=98;
	i=262;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=100;WordList[i][4]=56;
	WordList[i][5]=58; WordList[i][6]=77; WordList[i][7]=57; 
	i=263;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=30; WordList[i][3]=2;
	i=264;
	WordList[i][0]=73; WordList[i][1]=118;WordList[i][2]=30; WordList[i][3]=117;WordList[i][4]=77;
	i=265;
	WordList[i][0]=75; WordList[i][1]=57; WordList[i][2]=55; WordList[i][3]=37; WordList[i][4]=78;
	WordList[i][5]=117;  
	i=266;
	WordList[i][0]=75; WordList[i][1]=57; WordList[i][2]=55; WordList[i][3]=37; WordList[i][4]=110;
	i=267;
	WordList[i][0]=75; WordList[i][1]=57; WordList[i][2]=75; WordList[i][3]=57;
	i=268;
	WordList[i][0]=75; WordList[i][1]=57; WordList[i][2]=83; WordList[i][3]=66; WordList[i][4]=58;
	WordList[i][5]=113;
	i=269;
	WordList[i][0]=75; WordList[i][1]=57; WordList[i][2]=30; WordList[i][3]=57; WordList[i][4]=127;
	WordList[i][5]=85; 
	i=270;
	WordList[i][0]=75; WordList[i][1]=119;WordList[i][2]=73; WordList[i][3]=66; WordList[i][4]=25;
	WordList[i][5]=77; 
	i=271;
	WordList[i][0]=75; WordList[i][1]=119;WordList[i][2]=83; WordList[i][3]=104;WordList[i][4]=58;
	i=272;
	WordList[i][0]=75; WordList[i][1]=119;WordList[i][2]=107;WordList[i][3]=100;WordList[i][4]=25;
	WordList[i][5]=113; 
	i=273;
	WordList[i][0]=75; WordList[i][1]=36; WordList[i][2]=56; WordList[i][3]=58; WordList[i][4]=107;
	WordList[i][5]=120;WordList[i][6]=12;  
	i=274;
	WordList[i][0]=75; WordList[i][1]=36; WordList[i][2]=100;WordList[i][3]=100;WordList[i][4]=118;
	WordList[i][5]=63; 
	i=275;
	WordList[i][0]=75; WordList[i][1]=24; WordList[i][2]=75; WordList[i][3]=24; WordList[i][4]=99;
	WordList[i][5]=128;WordList[i][6]=85;  
	i=276;
	WordList[i][0]=75; WordList[i][1]=24; WordList[i][2]=115;WordList[i][3]=126;WordList[i][4]=58;
	i=277;
	WordList[i][0]=75; WordList[i][1]=34; WordList[i][2]=23; WordList[i][3]=37; WordList[i][4]=80;
	i=278;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=4;  WordList[i][3]=66;WordList[i][4]=118;
	WordList[i][5]=77;  
	i=279;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=77; WordList[i][3]=83; WordList[i][4]=74;
	WordList[i][5]=58; WordList[i][6]=115;WordList[i][7]=58;  
	i=280;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=77; WordList[i][3]=125;WordList[i][4]=58;
	i=281;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=77; WordList[i][3]=30; WordList[i][4]=57;
	WordList[i][5]=79; WordList[i][6]=57;  
	i=282;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=83; WordList[i][3]=108;WordList[i][4]=118;
	WordList[i][5]=4;  WordList[i][6]=104;WordList[i][7]=122; 
	i=283;
	WordList[i][0]=75; WordList[i][1]=117;WordList[i][2]=30; WordList[i][3]=77;
	i=284;
	WordList[i][0]=77; WordList[i][1]=57; WordList[i][2]=75; WordList[i][3]=36; WordList[i][4]=56;
	WordList[i][5]=120;  
	i=285;
	WordList[i][0]=77; WordList[i][1]=57; WordList[i][2]=125;WordList[i][3]=120;WordList[i][4]=113;
	i=286;
	WordList[i][0]=77; WordList[i][1]=119;WordList[i][2]=107;WordList[i][3]=122;
	i=287;
	WordList[i][0]=77; WordList[i][1]=36; WordList[i][2]=104;WordList[i][3]=120;WordList[i][4]=77;
	WordList[i][5]=75;  
	i=288;
	WordList[i][0]=77; WordList[i][1]=36; WordList[i][2]=112;WordList[i][3]=120;WordList[i][4]=115;
	WordList[i][5]=64; 
	i=289;
	WordList[i][0]=77; WordList[i][1]=36; WordList[i][2]=31; WordList[i][3]=57; WordList[i][4]=125;
	WordList[i][5]=118;WordList[i][6]=63;  
	i=290;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=83; WordList[i][3]=112;WordList[i][4]=122;
	i=291;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=110;
	i=292;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=87; WordList[i][3]=37; WordList[i][4]=76;
	WordList[i][5]=117; 
	i=293;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=22; WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=102;  
	i=294;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=16; WordList[i][3]=62; WordList[i][4]=118;
	WordList[i][5]=77; WordList[i][6]=107;WordList[i][7]=37; WordList[i][8]=102; 
	i=295;
	WordList[i][0]=77; WordList[i][1]=117;WordList[i][2]=16; WordList[i][3]=112;WordList[i][4]=118;
	WordList[i][5]=63;  
	i=296;
	WordList[i][0]=79; WordList[i][1]=57; WordList[i][2]=111;WordList[i][3]=58; WordList[i][4]=113;
	i=297;
	WordList[i][0]=79; WordList[i][1]=36; WordList[i][2]=78; WordList[i][3]=57; WordList[i][4]=55;
	WordList[i][5]=118;WordList[i][6]=63;  
	i=298;
	WordList[i][0]=79; WordList[i][1]=36; WordList[i][2]=126;WordList[i][3]=58; WordList[i][4]=75;
	WordList[i][5]=117;  
	i=299;
	WordList[i][0]=79; WordList[i][1]=36; WordList[i][2]=126;WordList[i][3]=58; WordList[i][4]=113;
	i=300;
	WordList[i][0]=79; WordList[i][1]=24; WordList[i][2]=103;WordList[i][3]=58; WordList[i][4]=109;
	i=301;
	WordList[i][0]=79; WordList[i][1]=117;WordList[i][2]=115;WordList[i][3]=70; WordList[i][4]=37;
	WordList[i][5]=78; 
	i=302;
	WordList[i][0]=79; WordList[i][1]=117;WordList[i][2]=16; WordList[i][3]=118;WordList[i][4]=77;
	WordList[i][5]=107;WordList[i][6]=37; WordList[i][7]=102; 
	i=303;
	WordList[i][0]=10; WordList[i][1]=24; WordList[i][2]=77; WordList[i][3]=115;WordList[i][4]=58;
	WordList[i][5]=105;  
	i=304;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=55; WordList[i][3]=118;WordList[i][4]=63;
	i=305;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=100;WordList[i][4]=25;
	WordList[i][5]=8;  WordList[i][6]=122;  
	i=306;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=123;
	i=307;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=91; WordList[i][3]=128;WordList[i][4]=109;
	i=308;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=99; WordList[i][3]=9;  WordList[i][4]=122;
	i=309;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=99; WordList[i][3]=128;WordList[i][4]=115;
	WordList[i][5]=112;WordList[i][6]=58; WordList[i][7]=97; 
	i=310;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=127;WordList[i][3]=109;
	i=311;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=30; WordList[i][3]=57; WordList[i][4]=63;
	i=312;
	WordList[i][0]=83; WordList[i][1]=58; WordList[i][2]=30; WordList[i][3]=57; WordList[i][4]=91;
	WordList[i][5]=76; WordList[i][6]=57; WordList[i][7]=85; 
	i=313;
	WordList[i][0]=83; WordList[i][1]=120;WordList[i][2]=4;  WordList[i][3]=25; WordList[i][4]=113;
	i=314;
	WordList[i][0]=83; WordList[i][1]=120;WordList[i][2]=75; WordList[i][3]=36; WordList[i][4]=15;
	WordList[i][5]=118;WordList[i][6]=77;  
	i=315;
	WordList[i][0]=83; WordList[i][1]=120;WordList[i][2]=91; WordList[i][3]=25; WordList[i][4]=97;
	i=316;
	WordList[i][0]=83; WordList[i][1]=37; WordList[i][2]=80; WordList[i][3]=36; WordList[i][4]=100;
	WordList[i][5]=84; WordList[i][6]=37; WordList[i][7]=80; 
	i=317;
	WordList[i][0]=83; WordList[i][1]=37; WordList[i][2]=15; WordList[i][3]=116;WordList[i][4]=58;
	WordList[i][5]=105; 
	i=318;
	WordList[i][0]=83; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=37; WordList[i][4]=3;
	i=319;
	WordList[i][0]=83; WordList[i][1]=37; WordList[i][2]=126;WordList[i][3]=58; WordList[i][4]=83;
	WordList[i][5]=37; WordList[i][6]=124; 
	i=320;
	WordList[i][0]=83; WordList[i][1]=37; WordList[i][2]=23; WordList[i][3]=37; WordList[i][4]=106;
	i=321;
	WordList[i][0]=83; WordList[i][1]=27; WordList[i][2]=79; WordList[i][3]=107;WordList[i][4]=29;
	WordList[i][5]=101; 
	i=322;
	WordList[i][0]=83; WordList[i][1]=29; WordList[i][2]=4;  WordList[i][3]=118;WordList[i][4]=63;
	i=323;
	WordList[i][0]=83; WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=66; WordList[i][4]=78;
	WordList[i][5]=57; 
	i=324;
	WordList[i][0]=83; WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=112;WordList[i][6]=58; 
	i=325;
	WordList[i][0]=83; WordList[i][1]=35; WordList[i][2]=92; WordList[i][3]=37; WordList[i][4]=116;
	WordList[i][5]=37; WordList[i][6]=86; 
	i=326;
	WordList[i][0]=83; WordList[i][1]=35; WordList[i][2]=104;WordList[i][3]=120;WordList[i][4]=115;
	WordList[i][5]=64;  
	i=327;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=4;  WordList[i][3]=118;WordList[i][4]=77;
	i=328;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=36; WordList[i][4]=100;
	WordList[i][5]=118;  
	i=329;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=117;WordList[i][4]=22;
	WordList[i][5]=15; WordList[i][6]=118;  
	i=330;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=104;WordList[i][4]=37;
	WordList[i][5]=80;  
	i=331;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=37; WordList[i][4]=112;
	WordList[i][5]=37; WordList[i][6]=124;
	i=332;
	WordList[i][0]=83; WordList[i][1]=118;WordList[i][2]=113;
	i=333;
	WordList[i][0]=87; WordList[i][1]=58; WordList[i][2]=4;  WordList[i][3]=66; WordList[i][4]=25;
	WordList[i][5]=105;
	i=334;
	WordList[i][0]=87; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=55;
	WordList[i][5]=37; WordList[i][6]=64; 
	i=335;
	WordList[i][0]=87; WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=58; WordList[i][4]=105;
	i=336;
	WordList[i][0]=87; WordList[i][1]=120;WordList[i][2]=4;  WordList[i][3]=25; WordList[i][4]=77;
	i=337;
	WordList[i][0]=87; WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=58; WordList[i][4]=105;
	i=338;
	WordList[i][0]=87; WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=70; WordList[i][4]=58;
	WordList[i][5]=20;  
	i=339;
	WordList[i][0]=87; WordList[i][1]=25; WordList[i][2]=115;WordList[i][3]=76; WordList[i][4]=57;
	WordList[i][5]=99; WordList[i][6]=128; 
	i=340;
	WordList[i][0]=87; WordList[i][1]=25; WordList[i][2]=22; WordList[i][3]=58;
	i=341;
	WordList[i][0]=87; WordList[i][1]=35; WordList[i][2]=56; WordList[i][3]=37; WordList[i][4]=78;
	i=342;
	WordList[i][0]=87; WordList[i][1]=35; WordList[i][2]=104;WordList[i][3]=118;WordList[i][4]=77;
	i=343;
	WordList[i][0]=87; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=63;
	i=344;
	WordList[i][0]=87; WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=37; WordList[i][4]=106;
	i=345;
	WordList[i][0]=87; WordList[i][1]=118;WordList[i][2]=16; WordList[i][3]=118;WordList[i][4]=77;
	i=346;
	WordList[i][0]=91; WordList[i][1]=58; WordList[i][2]=79;  
	i=347;
	WordList[i][0]=91; WordList[i][1]=118;WordList[i][2]=125;WordList[i][3]=78; WordList[i][4]=117;
	WordList[i][5]=63;
	i=348;
	WordList[i][0]=95; WordList[i][1]=120;WordList[i][2]=115;WordList[i][3]=66; WordList[i][4]=58;
	WordList[i][5]=113;  
	i=349;
	WordList[i][0]=95; WordList[i][1]=37; WordList[i][2]=80; WordList[i][3]=36; WordList[i][4]=104;
	WordList[i][5]=58;  
	i=350;
	WordList[i][0]=95; WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=37; WordList[i][4]=110;
	i=351;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=55; WordList[i][3]=37; WordList[i][4]=76;
	WordList[i][5]=117;  
	i=352;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=98; 
	i=353;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=107;WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=82; 
	i=354;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=118;WordList[i][4]=63;
	i=355;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=115;WordList[i][3]=58; WordList[i][4]=55;
	WordList[i][5]=118;WordList[i][6]=63; 
	i=356;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=115;WordList[i][3]=120;WordList[i][4]=115;
	WordList[i][5]=37; WordList[i][6]=126;WordList[i][7]=118;WordList[i][8]=63;  
	i=357;
	WordList[i][0]=99; WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=66; WordList[i][4]=58;
	i=358;
	WordList[i][0]=99; WordList[i][1]=120;WordList[i][2]=91; WordList[i][3]=25; WordList[i][4]=113;
	i=359;
	WordList[i][0]=99; WordList[i][1]=120;WordList[i][2]=127;WordList[i][3]=125;WordList[i][4]=108;
	WordList[i][5]=37; WordList[i][6]=98;  
	i=360;
	WordList[i][0]=99; WordList[i][1]=120;WordList[i][2]=125;WordList[i][3]=25; WordList[i][4]=85;
	i=361;
	WordList[i][0]=99; WordList[i][1]=120;WordList[i][2]=125;WordList[i][3]=25; WordList[i][4]=97;
	i=362;
	WordList[i][0]=97; WordList[i][1]=37; WordList[i][2]=66; WordList[i][3]=56;WordList[i][4]=118;
	i=363;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=80; 
	i=364;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=80; WordList[i][3]=91; WordList[i][4]=37;
	WordList[i][5]=114;  
	i=365;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=100;WordList[i][4]=58;
	i=366;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=98;  
	i=367;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=37; WordList[i][4]=86;
	i=368;
	WordList[i][0]=99; WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=112;WordList[i][4]=118;
	WordList[i][5]=63;  
	i=369;
	WordList[i][0]=99; WordList[i][1]=25; WordList[i][2]=87; WordList[i][3]=100;WordList[i][4]=58;
	WordList[i][5]=6;  
	i=370;
	WordList[i][0]=99; WordList[i][1]=25; WordList[i][2]=111;WordList[i][3]=108;WordList[i][4]=25;
	WordList[i][5]=97; 
	i=371;
	WordList[i][0]=99; WordList[i][1]=25; WordList[i][2]=30; WordList[i][3]=30; WordList[i][4]=117;
	WordList[i][5]=63; 
	i=372;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=61; WordList[i][3]=37; WordList[i][4]=108;
	WordList[i][5]=118; 
	i=373;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=1;
	i=374;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=36; WordList[i][4]=78;
	WordList[i][5]=107;WordList[i][6]=37; WordList[i][7]=102; 
	i=375;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=77;
	i=376;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=109;
	i=377;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=66; WordList[i][4]=118;
	WordList[i][5]=113;  
	i=378;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=60;
	i=379;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=125;WordList[i][3]=84; WordList[i][4]=118;
	WordList[i][5]=77; 
	i=380;
	WordList[i][0]=99; WordList[i][1]=118;WordList[i][2]=30; WordList[i][3]=117;WordList[i][4]=63;
	i=381;
	WordList[i][0]=103;WordList[i][1]=58; WordList[i][2]=75; WordList[i][3]=36; WordList[i][4]=78;
	i=382;
	WordList[i][0]=103;WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=111;
	WordList[i][5]=118;WordList[i][6]=63; 
	i=383;
	WordList[i][0]=103;WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=37; WordList[i][4]=128;
	i=384;
	WordList[i][0]=103;WordList[i][1]=120;WordList[i][2]=8;  WordList[i][3]=58;
	i=385;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=9;  WordList[i][3]=37; WordList[i][4]=102;
	i=386;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=36; WordList[i][4]=86;
	i=387;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=78; WordList[i][3]=117;WordList[i][4]=101;
	i=388;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=88; WordList[i][3]=37; WordList[i][4]=108;
	WordList[i][5]=118;WordList[i][6]=77;  
	i=389;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=116;WordList[i][3]=120;
	i=390;
	WordList[i][0]=103;WordList[i][1]=37; WordList[i][2]=126;WordList[i][3]=37; WordList[i][4]=110;
	i=391;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=4;  WordList[i][3]=9;  WordList[i][4]=37;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=102; 
	i=392;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=4;  WordList[i][3]=118;WordList[i][4]=125;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=86; 
	i=393;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=8;  WordList[i][3]=112;WordList[i][4]=118;
	i=394;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=77; WordList[i][3]=83; WordList[i][4]=37;
	WordList[i][5]=66; WordList[i][6]=37; WordList[i][7]=86;  
	i=395;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=77; WordList[i][3]=103;WordList[i][4]=118;
	WordList[i][5]=109;
	i=396;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=79;  
	i=397;
	WordList[i][0]=103;WordList[i][1]=27; WordList[i][2]=22; WordList[i][3]=29; WordList[i][4]=105;
	i=398;
	WordList[i][0]=103;WordList[i][1]=25; WordList[i][2]=115;WordList[i][3]=25; WordList[i][4]=4;
	WordList[i][5]=104;WordList[i][6]=58;  
	i=399;
	WordList[i][0]=103;WordList[i][1]=29; WordList[i][2]=8;  WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=102; 
	i=400;
	WordList[i][0]=103;WordList[i][1]=29; WordList[i][2]=107;WordList[i][3]=78; WordList[i][4]=117;
	WordList[i][5]=20; 
	i=401;
	WordList[i][0]=103;WordList[i][1]=29; WordList[i][2]=111;WordList[i][3]=29; WordList[i][4]=85;
	i=402;
	WordList[i][0]=103;WordList[i][1]=35; WordList[i][2]=84; WordList[i][3]=118;WordList[i][4]=105;
	i=403;
	WordList[i][0]=103;WordList[i][1]=35; WordList[i][2]=23; WordList[i][3]=37; WordList[i][4]=126;
	WordList[i][5]=37; WordList[i][6]=86;
	i=404;
	WordList[i][0]=103;WordList[i][1]=118;WordList[i][2]=6; 
	i=405;
	WordList[i][0]=103;WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=3;
	i=406;
	WordList[i][0]=103;WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=15; WordList[i][4]=29;
	WordList[i][5]=83; WordList[i][6]=122;  
	i=407;
	WordList[i][0]=103;WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=5;  WordList[i][4]=37;
	WordList[i][5]=64; 
	i=408;
	WordList[i][0]=103;WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=9;  WordList[i][4]=37;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=102;  
	i=409;
	WordList[i][0]=14; WordList[i][1]=58; WordList[i][2]=79;  
	i=410;
	WordList[i][0]=14; WordList[i][1]=58; WordList[i][2]=16; WordList[i][3]=37; WordList[i][4]=76;
	WordList[i][5]=57; 
	i=411;
	WordList[i][0]=14; WordList[i][1]=37; WordList[i][2]=56; WordList[i][3]=120;WordList[i][4]=107;
	WordList[i][5]=120;WordList[i][6]=12;  
	i=412;
	WordList[i][0]=14; WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=24; WordList[i][4]=4;
	WordList[i][5]=5;  WordList[i][6]=58;  
	i=413;
	WordList[i][0]=14; WordList[i][1]=29; WordList[i][2]=77; WordList[i][3]=26; WordList[i][4]=6;
	i=414;
	WordList[i][0]=14; WordList[i][1]=29; WordList[i][2]=79; WordList[i][3]=117;WordList[i][4]=105;
	i=415;
	WordList[i][0]=127;WordList[i][1]=79; WordList[i][2]=36; WordList[i][3]=110;
	i=416;
	WordList[i][0]=107;WordList[i][1]=35; WordList[i][2]=104;WordList[i][3]=37; WordList[i][4]=114;
	i=417;
	WordList[i][0]=111;WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=120;WordList[i][4]=77;
	i=418;
	WordList[i][0]=111;WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=37; WordList[i][4]=112;
	WordList[i][5]=58; WordList[i][6]=65; WordList[i][7]=37; WordList[i][8]=104;WordList[i][9]=58;
	i=419;
	WordList[i][0]=111;WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=103;WordList[i][4]=58;
	i=420;
	WordList[i][0]=111;WordList[i][1]=58; WordList[i][2]=99; WordList[i][3]=58; WordList[i][4]=127;
	i=421;
	WordList[i][0]=111;WordList[i][1]=58; WordList[i][2]=99; WordList[i][3]=25; WordList[i][4]=105;
	i=422;
	WordList[i][0]=111;WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=108;WordList[i][4]=118;
	WordList[i][5]=63; 
	i=423;
	WordList[i][0]=111;WordList[i][1]=25; WordList[i][2]=55; WordList[i][3]=118;WordList[i][4]=107;
	WordList[i][5]=108;WordList[i][6]=37; WordList[i][7]=110; 
	i=424;
	WordList[i][0]=111;WordList[i][1]=25; WordList[i][2]=127;WordList[i][3]=79; WordList[i][4]=36;
	WordList[i][5]=112;WordList[i][6]=118;WordList[i][7]=63;
	i=425;
	WordList[i][0]=111;WordList[i][1]=29; WordList[i][2]=87; WordList[i][3]=29; WordList[i][4]=101;
	i=426;
	WordList[i][0]=111;WordList[i][1]=35; WordList[i][2]=116;WordList[i][3]=37; WordList[i][4]=21;
	i=427;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=73; WordList[i][3]=84; WordList[i][4]=25;
	WordList[i][5]=81; 
	i=428;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=75; WordList[i][3]=117;WordList[i][4]=115;
	WordList[i][5]=37; WordList[i][6]=126;WordList[i][7]=118;WordList[i][8]=63;  
	i=429;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=79; WordList[i][3]=111;WordList[i][4]=120;
	WordList[i][5]=113; 
	i=430;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=56; WordList[i][4]=25;
	WordList[i][5]=105;  
	i=431;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=108;
	WordList[i][5]=118;  
	i=432;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=108;WordList[i][4]=118;
	WordList[i][5]=113; 
	i=433;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=87; WordList[i][3]=17; WordList[i][4]=120;
	WordList[i][5]=77;  
	i=434;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=103;WordList[i][3]=66; WordList[i][4]=118;
	WordList[i][5]=2;  
	i=435;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=109;
	i=436;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=111;
	WordList[i][5]=58; WordList[i][6]=63; 
	i=437;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=113; 
	i=438;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=56; WordList[i][4]=37;
	WordList[i][5]=124; 
	i=439;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=115;WordList[i][3]=96; WordList[i][4]=122;
	i=440;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=125;WordList[i][3]=76; WordList[i][4]=57;
	WordList[i][5]=113;  
	i=441;
	WordList[i][0]=111;WordList[i][1]=118;WordList[i][2]=30; WordList[i][3]=69; WordList[i][4]=29;
	WordList[i][5]=63;
	i=442;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=58; WordList[i][4]=125;
	WordList[i][5]=37; WordList[i][6]=114;  
	i=443;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=120;WordList[i][4]=115;
	WordList[i][5]=25; WordList[i][6]=85; 
	i=444;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=8;  WordList[i][3]=58; WordList[i][4]=77;
	i=445;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=78; WordList[i][4]=57;
	WordList[i][5]=63;  
	i=446;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=113;  
	i=447;
	WordList[i][0]=115;WordList[i][1]=58; WordList[i][2]=16; WordList[i][3]=37; WordList[i][4]=126;
	WordList[i][5]=118; 
	i=448;
	WordList[i][0]=115;WordList[i][1]=120;WordList[i][2]=4;  WordList[i][3]=25; WordList[i][4]=81;
	i=449;
	WordList[i][0]=115;WordList[i][1]=120;WordList[i][2]=111;WordList[i][3]=25; WordList[i][4]=77;
	i=450;
	WordList[i][0]=115;WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=5;  WordList[i][4]=118;
	WordList[i][5]=65; WordList[i][6]=118;WordList[i][7]=113;
	i=451;
	WordList[i][0]=115;WordList[i][1]=37; WordList[i][2]=112;WordList[i][3]=118;
	i=452;
	WordList[i][0]=115;WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=91; WordList[i][4]=25;
	WordList[i][5]=113; 
	i=453;
	WordList[i][0]=115;WordList[i][1]=35; WordList[i][2]=96; WordList[i][3]=37; WordList[i][4]=64;
	i=454;
	WordList[i][0]=115;WordList[i][1]=118;WordList[i][2]=65; WordList[i][3]=37; WordList[i][4]=70;
	WordList[i][5]=118;  
	i=455;
	WordList[i][0]=115;WordList[i][1]=118;WordList[i][2]=8;  WordList[i][3]=9;  WordList[i][4]=118;
	i=456;
	WordList[i][0]=115;WordList[i][1]=118;WordList[i][2]=87; WordList[i][3]=5;  WordList[i][4]=29;
	WordList[i][5]=63; 
	i=457;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=65; WordList[i][3]=58; WordList[i][4]=97;
	i=458;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=73; WordList[i][3]=88; WordList[i][4]=122;
	i=459;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=75; WordList[i][4]=117;
	WordList[i][5]=109; 
	i=460;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=79; WordList[i][3]=91; WordList[i][4]=25;
	WordList[i][5]=8;  WordList[i][6]=122;
	i=461;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=85; 
	i=462;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=91; WordList[i][3]=58; WordList[i][4]=8;
	WordList[i][5]=9;  WordList[i][6]=122; 
	i=463;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=107;WordList[i][3]=31; WordList[i][4]=24;
	WordList[i][5]=77; WordList[i][6]=24; WordList[i][7]=85; 
	i=464;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=111;WordList[i][3]=92; WordList[i][4]=25;
	WordList[i][5]=77;  
	i=465;
	WordList[i][0]=125;WordList[i][1]=58; WordList[i][2]=22; WordList[i][3]=58; WordList[i][4]=97;
	i=466;
	WordList[i][0]=125;WordList[i][1]=120;WordList[i][2]=105;  
	i=467;
	WordList[i][0]=125;WordList[i][1]=120;WordList[i][2]=107;WordList[i][3]=25; WordList[i][4]=8;
	WordList[i][5]=122;  
	i=468;
	WordList[i][0]=125;WordList[i][1]=37; WordList[i][2]=92; WordList[i][3]=37; WordList[i][4]=114;
	i=469;
	WordList[i][0]=125;WordList[i][1]=37; WordList[i][2]=92; WordList[i][3]=37; WordList[i][4]=116;
	WordList[i][5]=76; WordList[i][6]=57; 
	i=470;
	WordList[i][0]=125;WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=66; WordList[i][4]=37;
	WordList[i][5]=80; 
	i=471;
	WordList[i][0]=125;WordList[i][1]=37; WordList[i][2]=108;WordList[i][3]=108;WordList[i][4]=37;
	WordList[i][5]=98;   
	i=472;
	WordList[i][0]=125;WordList[i][1]=27; WordList[i][2]=65; WordList[i][3]=104;WordList[i][4]=118;
	WordList[i][5]=85;  
	i=473;
	WordList[i][0]=125;WordList[i][1]=25; WordList[i][2]=77; WordList[i][3]=63;
	i=474;
	WordList[i][0]=125;WordList[i][1]=29; WordList[i][2]=77; WordList[i][3]=117;WordList[i][4]=101;
	i=475;
	WordList[i][0]=125;WordList[i][1]=35; WordList[i][2]=80; WordList[i][3]=57;
	i=476;
	WordList[i][0]=125;WordList[i][1]=35; WordList[i][2]=100;WordList[i][3]=37; WordList[i][4]=114;
	i=477;
	WordList[i][0]=125;WordList[i][1]=118;WordList[i][2]=83; WordList[i][3]=108;WordList[i][4]=122;
	i=478;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=77; WordList[i][3]=57; WordList[i][4]=97;
	i=479;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=79; WordList[i][3]=36; WordList[i][4]=78;
	i=480;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=83; WordList[i][3]=37; WordList[i][4]=106;
	i=481;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=87; WordList[i][3]=58; WordList[i][4]=77;
	WordList[i][5]=117;WordList[i][6]=63;  
	i=482;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=58; WordList[i][4]=63;
	i=483;
	WordList[i][0]=16; WordList[i][1]=58; WordList[i][2]=125;WordList[i][3]=31; WordList[i][4]=57;
	WordList[i][5]=113;
	i=484;
	WordList[i][0]=16; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=58; WordList[i][4]=2;
	i=485;
	WordList[i][0]=16; WordList[i][1]=37; WordList[i][2]=84; WordList[i][3]=58; WordList[i][4]=4;
	WordList[i][5]=128;WordList[i][6]=85; 
	i=486;
	WordList[i][0]=16; WordList[i][1]=27; WordList[i][2]=69; WordList[i][3]=70; WordList[i][4]=118;
	WordList[i][5]=63;  
	i=487;
	WordList[i][0]=16; WordList[i][1]=27; WordList[i][2]=103;WordList[i][3]=29; WordList[i][4]=111;
	WordList[i][5]=118;WordList[i][6]=63; 
	i=488;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=77; 
	i=489;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=61; WordList[i][4]=37;
	WordList[i][5]=124;  
	i=490;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=99; WordList[i][4]=58;
	WordList[i][5]=125;WordList[i][6]=84; WordList[i][7]=122; 
	i=491;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=77; WordList[i][3]=103;WordList[i][4]=118;
	WordList[i][5]=63; 
	i=492;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=99; WordList[i][3]=37; WordList[i][4]=100;
	WordList[i][5]=37; WordList[i][6]=124; 
	i=493;
	WordList[i][0]=16; WordList[i][1]=118;WordList[i][2]=111;WordList[i][3]=112;WordList[i][4]=118;
	i=494;
	WordList[i][0]=30; WordList[i][1]=57; WordList[i][2]=99; WordList[i][3]=37; WordList[i][4]=64;
	i=495;
	WordList[i][0]=30; WordList[i][1]=57; WordList[i][2]=107;WordList[i][3]=37; WordList[i][4]=62;
	WordList[i][5]=120;WordList[i][6]=105; 
	i=496;
	WordList[i][0]=30; WordList[i][1]=36; WordList[i][2]=128;WordList[i][3]=125;WordList[i][4]=118;
	WordList[i][5]=63; 
	i=497;
	WordList[i][0]=30; WordList[i][1]=36; WordList[i][2]=31; WordList[i][3]=36; WordList[i][4]=84;
	WordList[i][5]=104;WordList[i][6]=58; 
	i=498;
	WordList[i][0]=30; WordList[i][1]=117;WordList[i][2]=75; WordList[i][3]=117;
	i=499;
	WordList[i][0]=30; WordList[i][1]=117;WordList[i][2]=79; WordList[i][3]=36; WordList[i][4]=5;
	WordList[i][5]=118; 
}

void DecomposeWord(Point wordletter[][MAX_STROKE][MAX_LENGTH],Point data[][MAX_LENGTH],CutLabel label[])
{
	int i,j,k;
	//wordletter��ʼ��
	for (i=0;i<WORD_LENGTH;i++)
	{
		for (j=0;j<MAX_STROKE;j++)
			for (k=0;k<MAX_LENGTH;k++)
			{
				wordletter[i][j][k].x=-1;
				wordletter[i][j][k].y=-1;
			}
	}
	//���ʲ��
	for (i=0;(i<MAX_STROKE)&&(label[i].stroke!=0);i++)
	{
		if (label[i].belong<=WORD_LENGTH)
		{
			for (j=0;(j<MAX_STROKE)&&(wordletter[label[i].belong-1][j][0].x!=-1);j++)
			{}
			if (j<MAX_STROKE)
			{
				for (k=label[i].cut_start-1;k<label[i].cut_end;k++)         //�����зֵ�ָ��
					wordletter[label[i].belong-1][j][k-(label[i].cut_start-1)]=data[label[i].stroke-1][k];
			}	
		}	
	}
} 

Width GetWidth(Point data[][MAX_LENGTH])                     //�õ��ַ����
{
	Width width;
	width.start=-1;
	width.end=-1;
	if (data[0][0].x==-1)
		return width;
	int i,j,k;
	Domain stroke_domain[MAX_STROKE];
	for (i=0;i<MAX_STROKE;i++)
	{
		stroke_domain[i].width_x.start=DENSITY_X-1;
		stroke_domain[i].width_x.end=0;
		stroke_domain[i].width_y.start=DENSITY_Y-1;
		stroke_domain[i].width_y.end=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		stroke_domain[i].width_x.start=data[i][0].x;
		stroke_domain[i].width_x.end=data[i][0].x;
		stroke_domain[i].width_y.start=data[i][0].y;
		stroke_domain[i].width_y.end=data[i][0].y;
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			if (data[i][j].x<stroke_domain[i].width_x.start)
				stroke_domain[i].width_x.start=data[i][j].x;
			if (data[i][j].x>stroke_domain[i].width_x.end)
				stroke_domain[i].width_x.end=data[i][j].x;
			if (data[i][j].y<stroke_domain[i].width_y.start)
				stroke_domain[i].width_y.start=data[i][j].y;
			if (data[i][j].y>stroke_domain[i].width_y.end)
				stroke_domain[i].width_y.end=data[i][j].y;
		}
	}
	Point data_selected[MAX_STROKE][MAX_LENGTH];  //��ȥ������Ҫ�Ĳ���                
	Initialize(data_selected);
	int mean=0;                                   //�������бʻ���ƽ�����
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
		mean=mean+(stroke_domain[i].width_x.end-stroke_domain[i].width_x.start)
		*(stroke_domain[i].width_y.end-stroke_domain[i].width_y.start);
	mean=mean/i;
	k=0;                                          //ɾȥС��ƽ������ıʻ�
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (((stroke_domain[i].width_x.end-stroke_domain[i].width_x.start)
			*(stroke_domain[i].width_y.end-stroke_domain[i].width_y.start))>mean/3)   
		{
			for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				data_selected[k][j]=data[i][j];
			}
			k++;
		}
	}
	width.start=DENSITY_X-1;
	width.end=0;
	for (i=0;(i<MAX_STROKE)&&(data_selected[i][0].x!=-1);i++)
	{
		for (j=0;(j<MAX_LENGTH)&&(data_selected[i][j].x!=-1);j++)
		{
			if (data_selected[i][j].x<width.start)
				width.start=data_selected[i][j].x;
			if (data_selected[i][j].x>width.end)
				width.end=data_selected[i][j].x;
		}
	}
	return width;
}

Baseline GetBaseline(Point data[][MAX_LENGTH])                     //�õ�����λ��                        
{
	Baseline line;
	line.centre=-1;
	line.up=-1;
	line.down=-1;
	int i,j,k;
	Domain stroke_domain[MAX_STROKE];
	for (i=0;i<MAX_STROKE;i++)
	{
		stroke_domain[i].width_x.start=DENSITY_X-1;
		stroke_domain[i].width_x.end=0;
		stroke_domain[i].width_y.start=DENSITY_Y-1;
		stroke_domain[i].width_y.end=0;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		stroke_domain[i].width_x.start=data[i][0].x;
		stroke_domain[i].width_x.end=data[i][0].x;
		stroke_domain[i].width_y.start=data[i][0].y;
		stroke_domain[i].width_y.end=data[i][0].y;
		for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{
			if (data[i][j].x<stroke_domain[i].width_x.start)
				stroke_domain[i].width_x.start=data[i][j].x;
			if (data[i][j].x>stroke_domain[i].width_x.end)
				stroke_domain[i].width_x.end=data[i][j].x;
			if (data[i][j].y<stroke_domain[i].width_y.start)
				stroke_domain[i].width_y.start=data[i][j].y;
			if (data[i][j].y>stroke_domain[i].width_y.end)
				stroke_domain[i].width_y.end=data[i][j].y;
		}
	}
	Point data_selected[MAX_STROKE][MAX_LENGTH];  //��ȥ������Ҫ�Ĳ���                
	Initialize(data_selected);
	int mean=0;                                   //�������бʻ���ƽ�����
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
		mean=mean+(stroke_domain[i].width_x.end-stroke_domain[i].width_x.start)
		*(stroke_domain[i].width_y.end-stroke_domain[i].width_y.start);
	mean=mean/i;
	k=0;                                          //ɾȥС��ƽ������ıʻ�
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (((stroke_domain[i].width_x.end-stroke_domain[i].width_x.start)
			*(stroke_domain[i].width_y.end-stroke_domain[i].width_y.start))>mean)
		{
			for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				data_selected[k][j]=data[i][j];
			}
			k++;
		}
	}
	bool pixel[DENSITY_X][DENSITY_Y];
	FromDataToPixel(pixel,data_selected);  
	int project[DENSITY_Y];
	Project(project,pixel,1);
	line=ComputeBaseline(project);
	return line; 
}

void Project(int project[],bool pixel[][DENSITY_Y],int direction)       //����ͶӰ
{                      //direction,0:��ֱͶӰ;1:ˮƽͶӰ
	int i,j;
	if (direction==0)
	{
		for (i=0;i<DENSITY_X;i++)
		{
			project[i]=0;
			for (j=0;j<DENSITY_Y;j++)
			{
				if (pixel[i][j]==1)
					project[i]++;
			}
		}	
	}
	else
	{
		for (i=0;i<DENSITY_Y;i++)
		{
			project[i]=0;
			for (j=0;j<DENSITY_X;j++)
			{
				if (pixel[j][i]==1)
					project[i]++;
			}
		}
	}
	return;
}

void ProjectDif(int project[],bool pixel[][DENSITY_Y],int direction)               //������ͶӰ
{                       //direction,0:��ֱͶӰ;1:ˮƽͶӰ
	int i,j;
	int start,end;
	if (direction==0)
	{
		for (i=0;i<DENSITY_X;i++)
		{
			start=-1;
			end=-1;
			for (j=0;j<DENSITY_Y;j++)
			{
				if (pixel[i][j]==1)
				{
					if (start==-1)
					{
						start=j;
						end=j;
					}
					else
						end=j;
				}
			}
			if (start!=-1)
				project[i]=end-start;
			else
				project[i]=-1;

		}
	}
	else
	{
		for (i=0;i<DENSITY_Y;i++)
		{
			start=-1;
			end=-1;
			for (j=0;j<DENSITY_X;j++)
			{
				if (pixel[j][i]==1)
				{
					if (start==-1)
					{
						start=j;
						end=j;
					}
					else
						end=j;
				}
			}
			if (start!=-1)
				project[i]=end-start;
			else
				project[i]=-1;
		}	
	}
	return;
}

Baseline ComputeBaseline(int project[])          //���������ˮƽͶӰ�����ֵ���Ӧ����λ�ã��Ի���Ϊ�ֽ�ֱ����
{                                               
	int i;                                       
	int up,down,have_up,sum,sum_now;
	Baseline line;
	line.centre=-1;
	line.up=-1;
	line.down=-1;
	int num[DENSITY_Y];
	int seq[DENSITY_Y];
	have_up=0;
	up = 0;
	for (i=0;i<DENSITY_Y;i++)
	{
		seq[i]=project[i];
		if (have_up==0)
		{
			if (project[i]!=0)
			{
				up=i;
				have_up=1;
			}
		}
		if (project[i]!=0)
			down=i;
	}
	Sequence(seq,num,DENSITY_Y);
	line.centre=num[DENSITY_Y-1];
	sum=0;
	for (i=line.centre;i>=up;i--)
	{
		sum=sum+project[i];
	}
	sum_now=0;
	for (i=line.centre;i>=up;i--)
	{
		sum_now=sum_now+project[i];
		if (sum_now>sum/2)
		{
			line.up=i-1;
			break;
		}
	}
	sum=0;
	for (i=line.centre;i<=down;i++)
	{
		sum=sum+project[i];
	}
	sum_now=0;
	for (i=line.centre;i<=down;i++)
	{
		sum_now=sum_now+project[i];
		if (sum_now>sum/2)
		{
			line.down=i+1;
			break;
		}
	}
	return line; 
}

void Recognize(Result * result,Point data[][MAX_LENGTH])      //���ַ�ʶ��
{
	int i;
	for (i=0;i<LETTER_CLASSCOUNT;i++)
	{
		(*(result+i)).number=0;
		(*(result+i)).distance=0;
	}
	int num[LETTER_CLASSCOUNT];
	int temp_dis[LETTER_SACOUNT];
	int distance0[LETTER_CLASSCOUNT],main0[LETTER_MCOUNT],affix0[LETTER_ACOUNT],dot0[LETTER_DCOUNT];
	int distance1[LETTER_CLASSCOUNT],main1[LETTER_MCOUNT],affix1[LETTER_ACOUNT],dot1[LETTER_DCOUNT];
	int distance=0;
	double degree0,degree1;
	Feature_R feature;
	int f0[GRID_A_X*GRID_A_Y*4],f1[STREAM_A*4];
	int d0,d1;
	for (i=0;i<GRID_A_X*GRID_A_Y*4;i++)
	{
		f0[i]=0;
	}
	d0=12*Distance(f0,RGa[0],GRID_A_X*GRID_A_Y*4);
	for (i=0;i<STREAM_A*4;i++)
	{
		f1[i]=0;
	}
	d1=4*Distance(f1,RSa[0],STREAM_A*4);                //����Ϊ��󸽼�������0�ľ���
	//����������ʶ��
	void * p;
	int * temp;
	if ((p=malloc(sizeof(int)*LETTER_CLASSCOUNT))!=0)
		temp=(int *)p;
	else
		return;	
	GetFeature_R(&feature,data);                        //�õ������ַ�������
	SequenceDistance_R(feature.mg,feature.ms,feature.mc,main0,0,0);
	SequenceDistance_R(feature.mg,feature.ms,feature.mc,main1,0,1);
	if (feature.affix==0)                               //�޸��ӱʻ������
	{
		affix0[0]=0;
		for (i=1;i<LETTER_ACOUNT;i++)
			affix0[i]=d0;
		affix1[0]=0;
		for (i=1;i<LETTER_ACOUNT;i++)
			affix1[i]=d1;
	}
	if (feature.affix==1)                              //��һ�����ӱʻ������
	{
		SequenceDistance_R(feature.ag1,feature.as1,feature.ac1,temp_dis,1,0);
		affix0[0]=d0;
		for (i=1;i<=LETTER_SACOUNT;i++)
			affix0[i]=temp_dis[i-1];
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix0[i]=temp_dis[0]+d0/2;
		SequenceDistance_R(feature.ag1,feature.as1,feature.ac1,temp_dis,1,1);
		affix1[0]=d1;
		for (i=1;i<=LETTER_SACOUNT;i++)
			affix1[i]=temp_dis[i-1];
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix1[i]=temp_dis[0]+d1/2;
	}  
	if (feature.affix==2)                             //���������ӱʻ������
	{
		for (i=0;i<=LETTER_SACOUNT;i++)
			affix0[i]=d0;
		SequenceDistance_R(feature.ag1,feature.as1,feature.ac1,temp_dis,1,0);
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix0[i]=temp_dis[0];
		SequenceDistance_R(feature.ag2,feature.as2,feature.ac2,temp_dis,1,0);
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix0[i]+=temp_dis[i-LETTER_SACOUNT];
		for (i=0;i<=LETTER_SACOUNT;i++)
			affix1[i]=d1;
		SequenceDistance_R(feature.ag1,feature.as1,feature.ac1,temp_dis,1,1);
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix1[i]=temp_dis[0];
		SequenceDistance_R(feature.ag2,feature.as2,feature.ac2,temp_dis,1,1);
		for (i=LETTER_SACOUNT+1;i<LETTER_ACOUNT;i++)
			affix1[i]+=temp_dis[i-LETTER_SACOUNT];
	}
	for (i=0;i<LETTER_DCOUNT;i++)
	{
		dot0[i]=DistanceDot(feature.dot_num,feature.dot_pos,feature.dot_rel,i,5*d0);
		dot1[i]=DistanceDot(feature.dot_num,feature.dot_pos,feature.dot_rel,i,5*d1);
	}	
	for (i=0;i<LETTER_CLASSCOUNT;i++)
	{	
		distance0[i]=main0[LetterList[i][0]-1]+affix0[LetterList[i][1]]+dot0[LetterList[i][2]];
		if ((i==11)||(i==12)||(i==13)||(i==14))
		{                             //ĳЩ�ַ����������ϲ�ͬ�Ĳ����ֽ�ṹ
			distance=main0[LetterList[i+117][0]-1]+affix0[LetterList[i+117][1]]+dot0[LetterList[i+117][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		if ((i==27)||(i==28))
		{
			distance=main0[LetterList[i+105][0]-1]+affix0[LetterList[i+105][1]]+dot0[LetterList[i+105][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		if ((i==70)||(i==71)||(i==72)||(i==73))
		{
			distance=main0[LetterList[i+64][0]-1]+affix0[LetterList[i+64][1]]+dot0[LetterList[i+64][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		if ((i==78)||(i==79))
		{
			distance=main0[LetterList[i+60][0]-1]+affix0[LetterList[i+60][1]]+dot0[LetterList[i+60][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		if ((i==88)||(i==89)||(i==90)||(i==91)||(i==92)||(i==93)||(i==94)||(i==95))
		{
			distance=main0[LetterList[i+52][0]-1]+affix0[LetterList[i+52][1]]+dot0[LetterList[i+52][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		if ((i==112)||(i==113)||(i==114)||(i==115))
		{
			distance=main0[LetterList[i+36][0]-1]+affix0[LetterList[i+36][1]]+dot0[LetterList[i+36][2]];
			if (distance0[i]>distance)
				distance0[i]=distance;
		}
		*(temp+i)=distance0[i];
	}
	Sequence(temp,LETTER_CLASSCOUNT);	
	degree0=double(*(temp+1)-*(temp+0))/double(*(temp+0));
	for (i=0;i<LETTER_CLASSCOUNT;i++)
	{
		distance1[i]=main1[LetterList[i][0]-1]+affix1[LetterList[i][1]]+dot1[LetterList[i][2]];
		if ((i==11)||(i==12)||(i==13)||(i==14))
		{
			distance=main1[LetterList[i+117][0]-1]+affix1[LetterList[i+117][1]]+dot1[LetterList[i+117][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		if ((i==27)||(i==28))
		{
			distance=main1[LetterList[i+105][0]-1]+affix1[LetterList[i+105][1]]+dot1[LetterList[i+105][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		if ((i==70)||(i==71)||(i==72)||(i==73))
		{
			distance=main1[LetterList[i+64][0]-1]+affix1[LetterList[i+64][1]]+dot1[LetterList[i+64][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		if ((i==78)||(i==79))
		{
			distance=main1[LetterList[i+60][0]-1]+affix1[LetterList[i+60][1]]+dot1[LetterList[i+60][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		if ((i==88)||(i==89)||(i==90)||(i==91)||(i==92)||(i==93)||(i==94)||(i==95))
		{
			distance=main1[LetterList[i+52][0]-1]+affix1[LetterList[i+52][1]]+dot1[LetterList[i+52][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		if ((i==112)||(i==113)||(i==114)||(i==115))
		{
			distance=main1[LetterList[i+36][0]-1]+affix1[LetterList[i+36][1]]+dot1[LetterList[i+36][2]];
			if (distance1[i]>distance)
				distance1[i]=distance;
		}
		*(temp+i)=distance1[i];
	}
	Sequence(temp,LETTER_CLASSCOUNT);	
	degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
	if (degree0>=degree1)
	{
		Sequence(distance0,num,LETTER_CLASSCOUNT);
		for (i=0;i<LETTER_CLASSCOUNT;i++)
		{
			(*(result+i)).number=*(num+i)+1;                   //���Ϊ��1-128
			(*(result+i)).distance=distance0[i];
		}
	}
	else
	{
		Sequence(distance1,num,LETTER_CLASSCOUNT);
		for (i=0;i<LETTER_CLASSCOUNT;i++)
		{
			(*(result+i)).number=*(num+i)+1;                  
			(*(result+i)).distance=distance1[i];
		}
	}
	free(p);
}


int WidthRelation(Width width1,Width width2)           //����ʻ�1�ͱʻ�2��λ��:0,����;1,�ʻ�1�����ʻ�2;                                                      
{                                                      //2,�ʻ�2�����ʻ�1;3,�ʻ�2�ڱʻ�1����ཻ;4,�ʻ�2�ڱʻ�1�Ҳ��ཻ
	if ((width2.end<=width1.start)||(width2.start>=width1.end))
		return 0;
	if ((width2.start>=width1.start)&&(width2.end<=width1.end))
		return 1;
	if ((width2.start<width1.start)&&(width2.end>width1.end))
		return 2;
	if ((width2.start<width1.start)&&(width1.start<width2.end)&&(width2.end<width1.end))
	{
		if ((width2.end-width2.start)<(width1.end-width1.start))
		{
			if ((width2.end-width1.start)<(width2.end-width2.start)/3)                   //B��A����ཻ,���ཻ��ΧС,�ɽ�����Ϊ���ཻ
				return 0;
			else if ((width2.end-width1.start)>(width2.end-width2.start)*3/4)            //B��A����ཻ,�����ཻ��ΧС,�ɽ�����ΪA����B 
				return 1;
			else
				return 3;
		}
		else
		{
			if ((width2.end-width1.start)<(width1.end-width1.start)/3)
				return 0;
			else if ((width2.end-width1.start)>(width1.end-width1.start)*3/4)            
				return 2;
			else
				return 3;
		}
	}
	if ((width1.start<width2.start)&&(width2.start<width1.end)&&(width1.end<width2.end))
	{
		if ((width2.end-width2.start)<(width1.end-width1.start))
		{
			if ((width1.end-width2.start)<(width2.end-width2.start)/3)
				return 0;
			else if ((width1.end-width2.start)>(width2.end-width2.start)*3/4)            
				return 1;
			else
				return 4;
		}
		else
		{
			if ((width1.end-width2.start)<(width1.end-width1.start)/3)
				return 0;
			else if ((width1.end-width2.start)>(width1.end-width1.start)*3/4)            
				return 2;
			else
				return 4;
		}
	}
	return 0;
}

Width IsThrough(Point data[],int line)      //�жϱʻ�β���Ƿ���ڴ�Խ�ʻ�,���ش�Խ�ο��(-1��ʾ�޴�Խ��)
{
	Width width;
	int i,j;
	int high,refer;
	width.start=-1;
	width.end=-1;
	for (i=0;(i<MAX_LENGTH)&&(data[i].x!=-1);i++)
	{}
	i--;
	if (i<2)
		return width;
	high=-1;
	refer=i;
	for (j=i-1;j>=0;j--)
	{
		if (data[j].y<(data[refer].y-5))
		{
			high=j;
			refer=j;
		}
		else if (data[j].y>(data[refer].y+5))
			break;
	}
	if (high!=-1)
	{
		if ((data[i].y>line)&&(data[high].y<=line))             //������ߴ�Խ
		{ 
			if (data[i].x<data[high].x)
			{                                                   //������д����
				width.start=data[i].x;
				width.end=data[high].x;	
			}	
		}
	}
	return width;
}

int IsLine(Point data[])                 //�жϱʻ��Ƿ�Ϊֱ��(0:����ֱ��;1:����ֱ��;2:������бֱ��;3:ˮƽ��۽����б��)
{
	int i,last,centre,length,temp;
	for (i=0;(i<MAX_LENGTH)&&(data[i].x!=-1);i++)
	{}
	last=i-1;
	length=0;
	for (i=1;i<=last;i++)
	{
		length+=(int)sqrt(pow(data[i].y-data[i-1].y,2)+pow(data[i].x-data[i-1].x,2));
	}
	centre=0;
	temp=0;
	for (i=1;i<=last;i++)
	{
		temp+=(int)sqrt(pow(data[i].y-data[i-1].y,2)+pow(data[i].x-data[i-1].x,2));
		if (temp>=length/2)
		{
			centre=i;
			break;
		}
	}
	if ((pow(data[centre].y-(data[0].y+data[last].y)/2,2)+pow(data[centre].x-(data[0].x+data[last].x)/2,2))<MIN_DIS)
	{
		if (data[0].x==data[last].x)
			return 1;
		else if ((double(abs(data[0].y-data[last].y))/double(abs(data[0].x-data[last].x)))>tan(80*PEI/180))
			return 1;
		if (data[0].x<data[last].x)
		{
			if (data[0].y<data[last].y)
				return 2;
			else
				return 3;
		}
		else
		{
			if ((data[0].y<data[last].y)&&((abs(data[0].x-data[last].x)>DENSITY_X/25)))
				return 3;
		}
	}
	return 0;
}

void GetCutSect(Stroke_C pstroke[],Point data[][MAX_LENGTH])            //�õ��ʻ����зֶ�
{
	int i,j,k;
	int high1,high2,low,direction,last;
	Width cutsects[MAX_CUT*4];
	int retain;
	int next,tail,min,min_p;
	Domain domain;                   //�ҳ��������ʵĿ�Ⱥ͸߶�
	domain.width_x.start=pstroke[0].domain.width_x.start;
	domain.width_x.end=pstroke[0].domain.width_x.end;
	domain.width_y.start=pstroke[0].domain.width_y.start;
	domain.width_y.end=pstroke[0].domain.width_y.end;
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (pstroke[i].domain.width_x.start<domain.width_x.start)
			domain.width_x.start=pstroke[i].domain.width_x.start;
		if (pstroke[i].domain.width_x.end>domain.width_x.end)
			domain.width_x.end=pstroke[i].domain.width_x.end;
		if (pstroke[i].domain.width_y.start<domain.width_y.start)
			domain.width_y.start=pstroke[i].domain.width_y.start;
		if (pstroke[i].domain.width_y.end>domain.width_y.end)
			domain.width_y.end=pstroke[i].domain.width_y.end;
	}
	for (i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if ((pstroke[i].ismain==1)&&((pstroke[i].domain.width_x.end-pstroke[i].domain.width_x.start)>(domain.width_x.end-domain.width_x.start)/10))
		{                          //���ڿ�Ⱥ�С�ıʻ������з�
			for (j=0;j<MAX_CUT*4;j++)
			{
				cutsects[j].start=-1;
				cutsects[j].end=-1;
			}
			k=0;
			direction=0;           //�趨��ʼ����
			high1=0;
			low=-1;
			high2=-1;
			for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
			{
				if (direction==0)
				{
					if (low==-1)
					{
						if (data[i][j].y<data[i][high1].y)
							high1=j;
						else if (data[i][j].y>data[i][high1].y)
						{
							direction=1;
							low=j;
						}
						else
							high1=j;
					}
					else
					{
						if (data[i][j].y<data[i][high2].y)
							high2=j;
						else if (data[i][j].y>data[i][high2].y)
						{
							if (k<MAX_CUT*4)
							{
								cutsects[k].start=high1;
								cutsects[k].end=high2;
							}
							k++;
							direction=1;
							high1=high2;
							low=j;
							high2=-1;
						}
						else
							high2=j;	
					}
				}
				else
				{
					if (data[i][j].y<data[i][low].y)
					{
						direction=0;
						high2=j;
					}
					else if (data[i][j].y>data[i][low].y)
						low=j;
					else
						low=j;	
				}
			}
			if (high2!=-1)
			{
				if (k<MAX_CUT*4)
				{
					cutsects[k].start=high1;
					cutsects[k].end=high2;
				}
				k++;
			}
			retain=0;                        //�Ա��ʻ��յ����һ������ʻ��ľ����ж�����зֶ��Ƿ���
			for (j=0;(j<MAX_CUT*4)&&(cutsects[j].start!=-1);j++)
			{}
			last=j-1;
			if ((i<(MAX_STROKE-1))&&(last!=-1))
			{
				for (next=i+1;next<MAX_STROKE;next++)
				{
					if (pstroke[next].ismain==1)
						break;
				}
				if (next<MAX_STROKE)
				{
					for (j=0;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
					{}
					tail=j-1;
					min=int(pow(data[next][0].x-data[i][tail].x,2)
						+pow(data[next][0].y-data[i][tail].y,2));
					for (j=1;(j<MAX_LENGTH)&&(data[next][j].x!=-1);j++)
					{
						if (int(pow(data[next][j].x-data[i][tail].x,2)         //ŷʽ����
							+pow(data[next][j].y-data[i][tail].y,2))<min)
							min=int(pow(data[next][j].x-data[i][tail].x,2)
							+pow(data[next][j].y-data[i][tail].y,2));
					}
					if (min<MIN_DIS)
						retain=1;
				}
				for (next=i+1;next<MAX_STROKE;next++)
				{
					if (pstroke[next].ismain==0)
						break;
				}
				if (next<MAX_STROKE)
				{
					if ((IsLine(data[next])==3)&&(pstroke[next].length>DOT_LENGTH/3))
					{
						for (j=0;(j<MAX_CUT*4)&&(data[next][j].x!=-1);j++)
						{}
						tail=j-1;
						if (data[next][0].x>data[next][tail].x)
						{
							if ((data[next][tail].x-data[i][cutsects[last].start].x)<DENSITY_X/15)
							{
								min=int(pow(data[next][tail].x-data[i][0].x,2)
									+pow(data[next][tail].y-data[i][0].y,2));
								min_p=0;
								for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
								{
									if (int(pow(data[next][tail].x-data[i][j].x,2)
										+pow(data[next][tail].y-data[i][j].y,2))<min)
									{
										min=int(pow(data[next][tail].x-data[i][j].x,2)
											+pow(data[next][tail].y-data[i][j].y,2));
										min_p=j;
									}
								}
								for (j=0;j<=tail;j++)
								{
									if (int(pow(data[next][j].x-data[i][min_p].x,2)
										+pow(data[next][j].y-data[i][min_p].y,2))<min)
									{
										min=int(pow(data[next][j].x-data[i][min_p].x,2)
											+pow(data[next][j].y-data[i][min_p].y,2));
									}
								}
								if (min<pow(pstroke[next].length/3,2))
									retain=2;
							}	
						}
						else
						{
							if ((data[next][0].x-data[i][cutsects[last].start].x)<DENSITY_X/15)
							{
								min=int(pow(data[next][0].x-data[i][0].x,2)
									+pow(data[next][0].y-data[i][0].y,2));
								min_p=0;
								for (j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
								{
									if (int(pow(data[next][0].x-data[i][j].x,2)
										+pow(data[next][0].y-data[i][j].y,2))<min)
									{
										min=int(pow(data[next][0].x-data[i][j].x,2)
											+pow(data[next][0].y-data[i][j].y,2));
										min_p=j;
									}
								}
								for (j=0;j<=tail;j++)
								{
									if (int(pow(data[next][j].x-data[i][min_p].x,2)
										+pow(data[next][j].y-data[i][min_p].y,2))<min)
									{
										min=int(pow(data[next][j].x-data[i][min_p].x,2)
											+pow(data[next][j].y-data[i][min_p].y,2));
									}
								}
								if (min<pow(pstroke[next].length/3,2))
									retain=2;
							}
						}
					}
				}
			}
			CorrectCut(cutsects,data[i],retain);
			for (j=0;j<MAX_CUT*2;j++)
				pstroke[i].cutsect[j]=cutsects[j];
		}
	}
	return;
}

void CorrectCut(Width cut[],Point data[],int lastretain)            //�зֶ�У��
{   
	int i,j,k;
	int low,high;
	double hwratio;	                                                 
	Width cut_temp[MAX_CUT*4];
	for (i=0;i<MAX_CUT*4;i++)                                        //�����ݺ�ȶԶ�����ɵ��з�������У��                
	{
		cut_temp[i]=cut[i];
		cut[i].start=-1;
		cut[i].end=-1;
	}
	k=0;
	for (i=0;(i<MAX_CUT*4)&&(cut_temp[i].start!=-1);i++)         
	{
		low=cut_temp[i].start;
		for (j=cut_temp[i].start+1;j<=cut_temp[i].end;j++)
		{
			if (data[j].y>data[low].y)
				low=j;
		}
		if (data[cut_temp[i].start].y<data[cut_temp[i].end].y)
		{
			for (j=cut_temp[i].start;j<=low;j++)
			{
				if (data[j].y>data[cut_temp[i].end].y)
				{
					high=j-1;
					break;
				}
			}	
		}
		else
		{
			for (j=cut_temp[i].end;j>=low;j--)
			{
				if (data[j].y>=data[cut_temp[i].start].y)
				{
					high=j;
					break;
				}
			}	
		}
		if (min(abs(data[cut_temp[i].start].x-data[cut_temp[i].end].x),abs(data[high].x-data[cut_temp[i].end].x))!=0)
			hwratio=double(data[low].y-data[high].y)/
			double(min(abs(data[cut_temp[i].start].x-data[cut_temp[i].end].x),abs(data[high].x-data[cut_temp[i].end].x)));
		else
			hwratio=1.0;
		if ((hwratio>0.1)&&min((data[low].y-data[high].y),
			max(abs(data[cut_temp[i].start].x-data[cut_temp[i].end].x),abs(data[high].x-data[cut_temp[i].end].x)))>1)
		{
			cut[k]=cut_temp[i];
			k++;			
		}
		else
		{
			if (i==0)
			{
				if (((i+1)<MAX_CUT*4)&&(cut_temp[i+1].start!=-1)
					&&((data[cut_temp[i].end].y-data[cut_temp[i].start].y)>(data[low].y-data[cut_temp[i].end].y)))
				{                                        //�зֶκϲ�
					cut_temp[i].end=cut_temp[i+1].end;   
					for (j=i+1;j<MAX_CUT*4-1;j++)
						cut_temp[j]=cut_temp[j+1];
					cut_temp[MAX_CUT*4-1].start=-1;
					cut_temp[MAX_CUT*4-1].end=-1;
					i--;
				}
			}
			else
			{
				if (data[cut_temp[i].start].y<=data[cut_temp[i].end].y)
				{
					if (((i+1)<MAX_CUT*4)&&(cut_temp[i+1].start!=-1))
					{                                    //�зֶκϲ�
						cut_temp[i].end=cut_temp[i+1].end;     
						for (j=i+1;j<MAX_CUT*4-1;j++)
							cut_temp[j]=cut_temp[j+1];
						cut_temp[MAX_CUT*4-1].start=-1;
						cut_temp[MAX_CUT*4-1].end=-1;
						i--;
					}
				}
				else
				{
					if (k>0)
					{                                            //��ͬ�ϲ���������
						cut[k-1].end=cut_temp[i].end;          
					}
				}
			}
		}
	}                                                      
	Width temp;                                                   //ɾ���ʻ�������ȷ���зֶ�
	for (i=0;(i<MAX_CUT*4)&&(cut[i].start!=-1);i++)         
	{
		temp=cut[i];
		low=temp.start;
		for (j=temp.start+1;j<=temp.end;j++)
		{
			if (data[j].y>data[low].y)
				low=j;
		}
		if (data[temp.start].y<data[temp.end].y)
		{
			for (j=temp.start;j<=low;j++)
			{
				if (data[j].y>data[temp.end].y)
				{
					temp.start=j-1;
					break;
				}
			}	
		}
		else
		{
			for (j=temp.end;j>=low;j--)
			{
				if (data[j].y>=data[temp.start].y)
				{
					temp.end=j;
					break;
				}
			}	
		}
		for (j=low;j>=temp.start;j--)
		{
			if ((data[low].y-data[j].y)>DENSITY_Y/50)
				temp.start=j;
		}
		for (j=low;j<=temp.end;j++)
		{
			if ((data[low].y-data[j].y)>DENSITY_Y/50)
				temp.end=j;
		}
		if (data[temp.start].x<data[temp.end].x)
		{                             //ɾ�����зֶ�
			if (i<(MAX_CUT*4-1))
			{
				for (j=i;j<MAX_CUT*4-1;j++)	
					cut[j]=cut[j+1];
			} 
			cut[MAX_CUT*4-1].start=-1;
			cut[MAX_CUT*4-1].end=-1;
			i--;
		}
	}                                            
	int lastp,inflect,direction,rp,refer;                            //���һ�ʵ�����������д���                             
	for (i=0;(i<MAX_CUT*4)&&(cut[i].start!=-1);i++)    
	{}
	if ((i!=0)&&(lastretain==0))
	{
		i--;
		low=cut[i].start;
		for (j=cut[i].start+1;j<=cut[i].end;j++)
		{
			if (data[j].y>data[low].y)
				low=j;
		}
		inflect=-1; 
		direction=0;                       //�趨��ʼ����
		rp=-1;
		refer=low;
		for (j=low-1;j>=cut[i].start;j--)
		{
			if (data[j].x>data[refer].x+5)
			{	
				if (direction==1)
				{
					inflect=refer;
					break;
				}
				refer=j;
			}
			else if (data[j].x<data[refer].x-3)
			{
				if (direction==0)
				{
					direction=1;
					rp=refer;
				}
				refer=j;
			}
		}
		for (j=cut[i].end;(j<MAX_LENGTH)&&(data[j].x!=-1);j++)
		{}
		lastp=j-1;
		if (inflect==-1)
		{
			if ((lastp-cut[i].end)<3)
			{
				if (data[cut[i].start].x<=data[cut[i].end].x)
				{               //ȥ�����зֶ�
					cut[i].start=-1;    
					cut[i].end=-1;
				}
				else
				{
					if ((data[cut[i].start].y-data[cut[i].end].y)<(data[low].y-data[cut[i].start].y)*2/3)
					{               //ȥ�����зֶ�
						cut[i].start=-1;    
						cut[i].end=-1;
					}
					else
					{
						if ((double(data[cut[i].start].y-data[cut[i].end].y)
							/double(data[cut[i].start].x-data[cut[i].end].x))<tan(30*PEI/180))
						{               //ȥ�����зֶ�
							cut[i].start=-1;    
							cut[i].end=-1;
						}
					}
				}
			}
		}
		else
		{
			if (((lastp-cut[i].end)>=3)&&((data[lastp].y>data[low].y)||(abs(data[lastp].y-data[low].y)<DENSITY_Y/25)))
				lastretain=2;
			else
			{
				if (cut[i].start>3)
				{
					cut[i].end=inflect;
					lastretain=2;
				}
				else if ((data[cut[i].start].y!=data[cut[i].start+5].y)
					&&((double(abs(data[cut[i].start].x-data[cut[i].start+5].x))
					/double(abs(data[cut[i].start].y-data[cut[i].start+5].y)))<tan(15*PEI/180)))
				{
					cut[i].end=inflect;
					lastretain=2;
				}
				else if ((data[inflect].y-data[cut[i].end].y)>(data[low].y-data[inflect].y)*2/3)
					lastretain=2;
				else
				{               //ȥ�����зֶ�
					cut[i].start=-1;    
					cut[i].end=-1;
				}
			}
		}
	}
	int length;                                                      //ɾȥȦ���зֶ�
	for (i=0;(i<MAX_CUT*4)&&(cut[i].start!=-1);i++)         
	{
		if (((i==(MAX_CUT*4-1))||(cut[i+1].start==-1))&&(lastretain==2))
			continue;
		low=cut[i].start;
		for (j=cut[i].start+1;j<=cut[i].end;j++)
		{
			if (data[j].y>data[low].y)
				low=j;
		}
		length=0;
		for (j=cut[i].start+1;j<=cut[i].end;j++)
		{
			length+=(int)sqrt(pow(data[j].y-data[j-1].y,2)+pow(data[j].x-data[j-1].x,2));
		}
		if ((length<PEI*DENSITY_X/10)&&((int)sqrt(pow(data[cut[i].start].y-data[cut[i].end].y,2)
			+pow(data[cut[i].start].x-data[cut[i].end].x,2))<length/3))
		{
			if (((data[cut[i].start].y<=data[cut[i].end].y)
				&&((data[cut[i].end].y-data[cut[i].start].y)<(data[low].y-data[cut[i].end].y)))
				||((data[cut[i].start].y>data[cut[i].end].y)
				&&((data[cut[i].start].y-data[cut[i].end].y)<(data[low].y-data[cut[i].start].y))))
			{
				if (i<(MAX_CUT*4-1))
				{
					for (j=i;j<MAX_CUT*4-1;j++)
						cut[j]=cut[j+1];
				}
				cut[MAX_CUT*4-1].start=-1;
				cut[MAX_CUT*4-1].end=-1;
				i--;
			}
		}
	}
	Point data_selected[MAX_STROKE][MAX_LENGTH];                     //ɾ���зֿռ��ϲ����ϵ��зֶ�
	bool pixel[DENSITY_X][DENSITY_Y];
	int project[DENSITY_X];
	Initialize(data_selected);
	for(j=0;j<MAX_LENGTH;j++)
		data_selected[0][j]=data[j];
	FromDataToPixel(pixel,data_selected);
	ProjectDif(project,pixel,0);
	Width leftsect,rightsect;
	int count;
	for (i=0;(i<MAX_CUT*4)&&(cut[i].start!=-1);i++)         
	{ 
		if (((i==(MAX_CUT*4-1))||(cut[i+1].start==-1))&&(lastretain==2))
			continue;
		leftsect.start=-1;
		leftsect.end=-1;
		rightsect.start=-1;
		rightsect.end=-1;
		count=0;
		temp=cut[i];
		low=temp.start;
		for (j=temp.start+1;j<=temp.end;j++)
		{
			if (data[j].y>data[low].y)
				low=j;
		}
		if (data[temp.start].y<data[temp.end].y)
		{
			for (j=temp.start;j<=low;j++)
			{
				if (data[j].y>data[temp.end].y)
				{
					temp.start=j-1;
					break;
				}
			}	
		}
		else
		{
			for (j=temp.end;j>=low;j--)
			{
				if (data[j].y>=data[temp.start].y)
				{
					temp.end=j;
					break;
				}
			}	
		}
		for (j=low;j>=temp.start;j--)
		{
			if ((data[low].y-data[j].y)>DENSITY_Y/25)
				temp.start=j;
		}
		for (j=low;j<=temp.end;j++)
		{
			if ((data[low].y-data[j].y)>DENSITY_Y/25)
				temp.end=j;
		}
		for (j=data[temp.end].x;j<=data[temp.start].x;j++)
		{
			if (leftsect.start==-1)       
			{
				if (project[j]<3)
				{
					count++;
					if (count==4)   
					{
						leftsect.start=j-3;
						count=0;
					}
				}
				else
					count=0;
			}
			else
			{
				if (project[j]>=3)
				{
					leftsect.end=j-1;
					break;	
				}
			}
		}
		if ((leftsect.start!=-1)&&(leftsect.end==-1))
			leftsect.end=j-1;
		count=0;
		for (j=data[temp.start].x;j>=data[temp.end].x;j--)
		{
			if (rightsect.end==-1)
			{
				if (project[j]<3)
				{
					count++;
					if (count==4)
					{
						rightsect.end=j+3;
						count=0;
					}
				}
				else
					count=0;
			}
			else
			{
				if (project[j]>=3)
				{
					rightsect.start=j+1;
					break;	
				}
			}
		}
		if ((rightsect.end!=-1)&&(rightsect.start==-1))
			rightsect.start=j+1;
		if ((leftsect.start==rightsect.start)&&(leftsect.end==rightsect.end))
			temp=leftsect;
		else if (leftsect.start==-1)
			temp=rightsect;
		else if (rightsect.start==-1)
			temp=leftsect;
		else
		{
			temp.start=-1;
			temp.end=-1;
		}	
		if (temp.start==-1)
		{
			if (i<(MAX_CUT*4-1))
			{
				for (j=i;j<MAX_CUT*4-1;j++)
					cut[j]=cut[j+1];
			}
			cut[MAX_CUT*4-1].start=-1;
			cut[MAX_CUT*4-1].end=-1;
			i--;
		}	
	}
	return;
}

void FirstCut(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line)     //�����з�
{
	int i,j,k;
	for (i=0;i<MAX_STROKE;i++)
	{
		label[i].stroke=0;             //1-30
		label[i].cut_start=0;          //1-1000
		label[i].cut_end=0;            //1-1000
		label[i].belong=0;             //1-30
	}
	Width affix;                                                     //����η���
	Width sect_widths[WORD_LENGTH];
	for (i=0;i<WORD_LENGTH;i++)
	{
		sect_widths[i].start=-1;
		sect_widths[i].end=-1;
	}
	k=0;                                                              
	for (i=0;(i<MAX_STROKE)&&(pstroke[i].stroke!=0);i++)                      
	{
		if (pstroke[i].ismain==1)
		{
			if (k<WORD_LENGTH)
			{
				pstroke[i].section=k+1;
				sect_widths[k]=pstroke[i].domain.width_x;
				k++;
			}	
		}
	}
	for (i=0;(i<MAX_STROKE)&&(pstroke[i].stroke!=0);i++)                      
	{
		if (pstroke[i].ismain==0)
		{
			affix=pstroke[i].domain.width_x;
			pstroke[i].section=WidthPosition(affix,sect_widths,0)+1;
			if (pstroke[i].domain.width_x.end>sect_widths[pstroke[i].section-1].end)   //����ο��У��
				sect_widths[pstroke[i].section-1].end=pstroke[i].domain.width_x.end;
			if (pstroke[i].domain.width_x.start<sect_widths[pstroke[i].section-1].start)
				sect_widths[pstroke[i].section-1].start=pstroke[i].domain.width_x.start; 
		}
	} 
	Stroke_C sections[MAX_STROKE];                                   //���ӱʻ�����
	for (i=0;i<WORD_LENGTH;i++)
	{
		for (j=0;j<MAX_STROKE;j++)
		{
			sections[j].stroke=0;
			sections[j].section=0;
			sections[j].domain.width_x.start=DENSITY_X-1;
			sections[j].domain.width_x.end=0;
			sections[j].domain.width_y.start=DENSITY_Y-1;
			sections[j].domain.width_y.end=0;
			sections[j].length=0;
			sections[j].ismain=0;
			for (k=0;k<MAX_CUT*2;k++)
			{
				sections[j].cutsect[k].start=-1;
				sections[j].cutsect[k].end=-1;
			}
		}
		k=0;
		for (j=0;(j<MAX_STROKE)&&(pstroke[j].stroke!=0);j++) 
		{
			if ((pstroke[j].section==(i+1))&&(pstroke[j].ismain==0))
			{
				sections[k]=pstroke[j];
				k++;
			}
		}
		if (k>1)
		{
			Cluster(sections,line);   //����
			for (j=0;(j<MAX_STROKE)&&(sections[j].stroke!=0);j++) 
				pstroke[sections[j].stroke-1]=sections[j];
		}
	}
	int low,count;                                                   //�зֶι��� 
	Point data_cut[MAX_LENGTH],data_tail[MAX_LENGTH];
	for (i=0;(i<MAX_STROKE)&&(pstroke[i].stroke!=0);i++)                          
	{
		if (pstroke[i].ismain==1)
		{
			for (j=0;(j<MAX_CUT*2)&&(pstroke[i].cutsect[j].start!=-1);j++)
			{                         
				low=pstroke[i].cutsect[j].start; //�����зֶγ���
				for (k=pstroke[i].cutsect[j].start+1;k<=pstroke[i].cutsect[j].end;k++)
				{
					if (data[i][k].y>data[i][low].y)
						low=k;
				}
				for (k=low;k>=pstroke[i].cutsect[j].start;k--)
				{
					if ((data[i][low].y-data[i][k].y)>DENSITY_Y/25)
						pstroke[i].cutsect[j].start=k;
				}
				for (k=low;k<=pstroke[i].cutsect[j].end;k++)
				{
					if ((data[i][low].y-data[i][k].y)>DENSITY_Y/25)
						pstroke[i].cutsect[j].end=k;
				}
				for (k=0;k<MAX_LENGTH;k++)      //�зֶε���
				{
					data_cut[k].x=-1;
					data_cut[k].y=-1;
				}
				for (k=pstroke[i].cutsect[j].end;k>=pstroke[i].cutsect[j].start;k--)
					data_cut[pstroke[i].cutsect[j].end-k]=data[i][k];
				for (k=0;k<MAX_LENGTH;k++)
				{
					data_tail[k].x=-1;
					data_tail[k].y=-1;
				}
				for (k=0;k<pstroke[i].cutsect[j].start;k++)
					data_tail[k]=data[i][k];
				count=Intersect(data_cut,data_tail);
				if (count!=-1)
					pstroke[i].cutsect[j].start=pstroke[i].cutsect[j].end-count;
				for (k=0;k<MAX_LENGTH;k++)
				{
					data_cut[k].x=-1;
					data_cut[k].y=-1;
				}
				for (k=pstroke[i].cutsect[j].start;k<=pstroke[i].cutsect[j].end;k++)
					data_cut[k-pstroke[i].cutsect[j].start]=data[i][k];
				for (k=0;k<MAX_LENGTH;k++)
				{
					data_tail[k].x=-1;
					data_tail[k].y=-1;
				}
				for (k=pstroke[i].cutsect[j].end+1;k<=MAX_LENGTH;k++)
					data_tail[k-(pstroke[i].cutsect[j].end+1)]=data[i][k];
				count=Intersect(data_cut,data_tail);
				if (count!=-1)
					pstroke[i].cutsect[j].end=pstroke[i].cutsect[j].start+count;
			}	
		}
	}
	int n;                                                           //�з�
	k=0;                                                              
	n=0;
	for (i=0;(i<MAX_STROKE)&&(pstroke[i].stroke!=0);i++)      
	{	
		if (pstroke[i].ismain==1)
		{
			count=1;
			for (j=0;(j<MAX_CUT*2)&&(pstroke[i].cutsect[j].start!=-1);j++) 
			{
				label[k].stroke=i+1;
				label[k].cut_start=count;
				label[k].cut_end=PerpendIntersect((data[i][pstroke[i].cutsect[j].start].x+data[i][pstroke[i].cutsect[j].end].x)/2,
					data[i],pstroke[i].cutsect[j].start,pstroke[i].cutsect[j].end)+1;
				count=label[k].cut_end+1;
				label[k].belong=n+1;
				n++;
				k++;		
			}
			label[k].stroke=i+1;
			label[k].cut_start=count;
			label[k].cut_end=MAX_LENGTH;
			label[k].belong=n+1;
			n++;
			k++;
		}
		else
		{
			label[k].stroke=i+1;
			label[k].cut_start=1;
			label[k].cut_end=MAX_LENGTH;
			k++;
		}
	}
	AffixLabel(label,pstroke,data);              //���ӱʻ��ķ����ע
	MainRectify(label,pstroke,data,line);        //�Զ�����ʻ���������кϲ�
	return;
}

int WidthPosition(Width width,Width widths[],bool inter)                             
{                                     //�����ȵ�������ȶ�0~29
	int result;
	int i,dis,dis_now;
	if (inter==0)
	{               //�μ����ѡ���widths�Ŀ��
		if (widths[0].start!=-1)
		{
			for (i=1;(i<MAX_STROKE)&&(widths[i].start!=-1);i++)
			{
				if (widths[i-1].start<widths[i].end)
				{
					if ((widths[i-1].end-widths[i].end)<DENSITY_X/10)
						widths[i-1].start=(widths[i-1].end+widths[i].end)/2;
					else
						widths[i-1].start=widths[i].end+DENSITY_X/20;

				}
			}
		}
	}
	else
	{              //���ڵ������Զ�width�Ŀ��(����д����Ưϰ�ߵ�У��)
		width.start+=3;
		width.end+=3;
	}
	if (width.end>=widths[0].end)
		dis=width.end-widths[0].end;
	else if (width.start<=widths[0].start)
		dis=widths[0].start-width.start;
	else
		dis=0;
	result=0;
	for (i=1;(i<MAX_STROKE)&&(widths[i].start!=-1);i++)
	{
		if (width.end>=widths[i].end)
			dis_now=width.end-widths[i].end;
		else if (width.start<=widths[i].start)
			dis_now=widths[i].start-width.start;
		else
			dis_now=0;
		if (dis_now<dis)
		{
			dis=dis_now;
			result=i;
		}
	}
	return result;
}

void Cluster(Stroke_C pstroke[],int line)
{                                                 //���ӱʻ�����
	int i,j,k;
	int c[10];
	for (j=0;j<10;j++)
		c[j]=-1;
	Domain domain;
	int dis_x,dis_y;
	Point centre_refer,centre;
	c[0]=0;
	k=1;
	for (i=1;(i<MAX_STROKE)&&(pstroke[i].stroke!=0);i++)
	{
		centre.x=(pstroke[i].domain.width_x.start+pstroke[i].domain.width_x.end)/2;
		centre.y=(pstroke[i].domain.width_y.start+pstroke[i].domain.width_y.end)/2;
		centre_refer.y=(pstroke[i-1].domain.width_y.start+pstroke[i-1].domain.width_y.end)/2;
		if (((centre_refer.y<=line)&&(centre.y<=line))||((centre_refer.y>=line)&&(centre.y>=line)))
		{                              //�������ȿ�������ߵ����λ���Լ���д���������
			centre_refer.x=(pstroke[c[0]].domain.width_x.start+pstroke[c[0]].domain.width_x.end)/2;
			dis_x=centre_refer.x-centre.x;
			dis_y=abs(centre_refer.y-centre.y);
			for (j=1;j<k;j++)
			{
				centre_refer.x=(pstroke[c[j]].domain.width_x.start+pstroke[c[j]].domain.width_x.end)/2;
				if ((centre_refer.x-centre.x)<dis_x)
				{
					dis_x=centre_refer.x-centre.x;
					dis_y=abs(centre_refer.y-centre.y);
				}
			}
			if ((dis_x<DENSITY_X/15)&&(dis_y<DENSITY_Y/4))
			{
				c[k]=i;
				k++;
				continue;
			}
		}
		if (k>1)
		{
			domain=pstroke[c[0]].domain;
			for (j=1;(j<10)&&(c[j]!=-1);j++)
			{
				if (pstroke[c[j]].domain.width_x.start<domain.width_x.start)
					domain.width_x.start=pstroke[c[j]].domain.width_x.start;
				if (pstroke[c[j]].domain.width_x.end>domain.width_x.end)
					domain.width_x.end=pstroke[c[j]].domain.width_x.end;
				if (pstroke[c[j]].domain.width_y.start<domain.width_y.start)
					domain.width_y.start=pstroke[c[j]].domain.width_y.start;
				if (pstroke[c[j]].domain.width_y.end>domain.width_y.end)
					domain.width_y.end=pstroke[c[j]].domain.width_y.end;
			}
			for (j=0;(j<10)&&(c[j]!=-1);j++)
				pstroke[c[j]].domain=domain;
		}
		for (j=0;j<10;j++)
			c[j]=-1;
		c[0]=i;
		k=1;		
	}
	if (k>1)
	{
		domain=pstroke[c[0]].domain;
		for (j=1;(j<10)&&(c[j]!=-1);j++)
		{
			if (pstroke[c[j]].domain.width_x.start<domain.width_x.start)
				domain.width_x.start=pstroke[c[j]].domain.width_x.start;
			if (pstroke[c[j]].domain.width_x.end>domain.width_x.end)
				domain.width_x.end=pstroke[c[j]].domain.width_x.end;
			if (pstroke[c[j]].domain.width_y.start<domain.width_y.start)
				domain.width_y.start=pstroke[c[j]].domain.width_y.start;
			if (pstroke[c[j]].domain.width_y.end>domain.width_y.end)
				domain.width_y.end=pstroke[c[j]].domain.width_y.end;
		}
		for (j=0;(j<10)&&(c[j]!=-1);j++)
			pstroke[c[j]].domain=domain;
	}
	return;
}

int Intersect(Point data1[],Point data2[])          //�ҳ����߶εĴ�ֱ���㲢���ؽ������߶�1�е�λ��
{
	int i,j;
	for (i=1;(i<MAX_LENGTH)&&(data1[i].x!=-1);i++)
	{
		for (j=1;(j<MAX_LENGTH)&&(data2[j].x!=-1);j++)
		{
			if (((data2[j-1].x+data2[j].x)/2>=min(data1[i-1].x,data1[i].x))
				&&((data2[j-1].x+data2[j].x)/2<max(data1[i-1].x,data1[i].x))
				&&((data1[i-1].y+data1[i].y)/2>=min(data2[j-1].y,data2[j].y))
				&&((data1[i-1].y+data1[i].y)/2<max(data2[j-1].y,data2[j].y)))
			{
				return i;      //�ҵ��߶�1�ʻ�����ĵ�һ����                 
			}
		}
	}
	return -1;
}

int PerpendIntersect(int dot_x,Point data[],int ns,int ne)        //�ҳ�����ʻ��εĴ�ֱ����
{
	int result=-1;
	int i,min;
	min=abs(data[ns].x-dot_x);
	result=ns;
	for (i=ns+1;i<=ne;i++)
	{
		if (abs(data[i].x-dot_x)<min)
		{
			min=abs(data[i].x-dot_x);
			result=i;
		}
	}
	if ((dot_x>data[result].x)&&(result!=0))
		result--;
	return result;
}

void AffixLabel(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH])    
{                                //�Ը��ӱʻ���ʻ���λ�������ʻ�0~29,����CutLabel
	int i,j,k,l;
	Width affix; 
	Width mains[MAX_STROKE];
	int main_num[MAX_STROKE];
	Width width;
	for (i=0;i<WORD_LENGTH;i++)
	{
		for (j=0;j<MAX_STROKE;j++)
		{
			mains[j].start=-1;
			mains[j].end=-1;
			main_num[j]=0;
		}
		k=0;
		for (j=0;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)             
		{
			if ((label[j].belong!=0)&&(pstroke[label[j].stroke-1].section==(i+1)))
			{
				width.start=data[label[j].stroke-1][label[j].cut_start-1].x;
				width.end=data[label[j].stroke-1][label[j].cut_start-1].x;
				for (l=label[j].cut_start+1;(l<=label[j].cut_end)&&(data[label[j].stroke-1][l-1].x!=-1);l++)
				{
					if (data[label[j].stroke-1][l-1].x<width.start)
						width.start=data[label[j].stroke-1][l-1].x;
					if (data[label[j].stroke-1][l-1].x>width.end)
						width.end=data[label[j].stroke-1][l-1].x;
				}
				mains[k]=width;
				main_num[k]=label[j].belong;
				k++;
			}
		}
		if (k!=0)
		{
			for (j=0;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)             
			{
				if ((label[j].belong==0)&&(pstroke[label[j].stroke-1].section==(i+1)))
				{
					affix=pstroke[label[j].stroke-1].domain.width_x;
					label[j].belong=main_num[WidthPosition(affix,mains,1)];
				}
			}
		}
	}
	return;
}

void MainRectify(CutLabel label[],Stroke_C pstroke[],Point data[][MAX_LENGTH],int line)     //�Զ�����ʻ���������кϲ�
{                                                             
	int i,j;
	int next,min,dis,lastp;
	int have;
	for (i=0;(i<MAX_STROKE)&&(label[i].stroke!=0);i++)       //ĳ�ַ������������ʻ�ʱ�ĺϲ�
	{
		if ((pstroke[label[i].stroke-1].ismain==1)&&(label[i].cut_start!=1)&&(label[i].cut_end==MAX_LENGTH))
		{
			next=-1;                 //�ҵ����ڵ���һ����ʻ�
			for (j=i+1;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)
			{
				if ((pstroke[label[j].stroke-1].ismain==1)&&(label[j].belong==(label[i].belong+1)))
				{
					next=j;
					break;
				}
			}
			if (next!=-1)
			{
				have=0;
				for (j=0;(j<MAX_LENGTH)&&(data[label[i].stroke-1][j].x!=-1);j++)
				{}
				lastp=j-1;          //���㱾�ʻ�β������һ�ʻ��ľ���
				min=int(pow(data[label[i].stroke-1][lastp].x-data[label[next].stroke-1][0].x,2)
					+pow(data[label[i].stroke-1][lastp].y-data[label[next].stroke-1][0].y,2));
				for (j=1;(j<MAX_LENGTH)&&(data[label[next].stroke-1][j].x!=-1);j++)
				{
					dis=int(pow(data[label[i].stroke-1][lastp].x-data[label[next].stroke-1][j].x,2)
						+pow(data[label[i].stroke-1][lastp].y-data[label[next].stroke-1][j].y,2));
					if (dis<min)
						min=dis;
				}
				if (min<MIN_DIS/2)
				{
					if ((lastp-label[i].cut_start+1)<30)
						have=1;    //̫���Ķβ����ϲ�
				}
				else if (min<MIN_DIS)
				{
					if ((lastp-label[i].cut_start+1)<15)
						have=1;    
				}
				else
				{ 
					if (IsThrough(data[label[i].stroke-1],line).start==-1)
					{              //���㱾�ʻ�����һ�ʻ�ʼ��ľ���
						min=int(pow(data[label[i].stroke-1][0].x-data[label[next].stroke-1][0].x,2)
							+pow(data[label[i].stroke-1][0].y-data[label[next].stroke-1][0].y,2));
						for (j=1;(j<MAX_LENGTH)&&(data[label[i].stroke-1][j].x!=-1);j++)
						{
							dis=int(pow(data[label[i].stroke-1][j].x-data[label[next].stroke-1][0].x,2)
								+pow(data[label[i].stroke-1][j].y-data[label[next].stroke-1][0].y,2));
							if (dis<min)
								min=dis;
						}
						if (min<MIN_DIS)
							have=1;
					}
				}
				if (have==1)
				{                      
					for (j=0;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)
					{
						if (label[j].belong>label[i].belong)
							label[j].belong--;
					}	
				}
			}
		}
	}
	return;
}

void SecondCut(CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[])      //�����з�,�������ĸ�����зֶμ�ĺϲ�
{
	int i,j;
	for (i=0;(i<MAX_STROKE)&&(label[i].stroke!=0);i++)       //У������ƽ��Ԥ������γɵ��зִ�λ          
	{
		if (label[i].cut_start!=1)
			label[i].cut_start--;
		if (label[i].cut_end!=MAX_LENGTH)
			label[i].cut_end--;
	}
	int radial[3];
	int before_dis,before_pos,dis,pos;
	before_dis=-1;
	bool havemerge;
	for (i=0;(i<MAX_STROKE)&&(label[i].stroke!=0);i++)              
	{
		if (pstroke[label[i].stroke-1].ismain==1)
		{
			for (j=0;j<3;j++)
				radial[j]=-1;
			havemerge=0;
			dis=-1;
			radial[0]=i;
			for (j=radial[0]+1;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)
			{
				if ((pstroke[label[j].stroke-1].ismain==1)&&(label[j].belong==(label[radial[0]].belong+1))
					&&(label[j].stroke==label[radial[0]].stroke))
				{
					radial[1]=j;
					break;
				}
			}
			if (radial[1]!=-1)
			{
				for (j=radial[1]+1;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)
				{
					if ((pstroke[label[j].stroke-1].ismain==1)&&(label[j].belong==(label[radial[1]].belong+1))
						&&(label[j].stroke==label[radial[1]].stroke))
					{
						radial[2]=j;
						break;
					}
				}	
			}
			if (radial[2]!=-1)
			{                          //�ҵ��������ĸ��3���зֶ�
				if (label[radial[0]].cut_start==1)
				{
					if (label[radial[2]].cut_end==MAX_LENGTH)
						pos=4;
					else
						pos=1;
				}
				else
				{
					if (label[radial[2]].cut_end==MAX_LENGTH)
						pos=3;
					else
						pos=2;
				}
				dis=IsConnect(radial,label,data,pstroke,pos);
			}
			if (dis==-1)
			{
				if (before_dis!=-1)
				{
					havemerge=1;
					radial[0]--;
					radial[1]=radial[0]+1;
					radial[2]=radial[1]+1;	
				}
			}
			else
			{
				if (pos==4)
					havemerge=1;
				if (pos==1)
				{
					havemerge=0;
					before_dis=dis;
					before_pos=pos;
				}
				if (pos==3)
				{
					havemerge=1;
					if (before_dis!=-1)
					{
						if (dis>before_dis)
						{
							for (j=0;j<3;j++)
								radial[j]--;
						}
					}
				}
				if (pos==2)
				{
					if (before_dis==-1)
					{
						havemerge=0;
						before_dis=dis;
						before_pos=pos;
					}
					else
					{
						havemerge=1;
						if (dis>before_dis)
						{
							for (j=0;j<3;j++)
								radial[j]--;	
						}
						else if (dis==before_dis)
						{
							if (before_pos==1)
							{
								for (j=0;j<3;j++)
									radial[j]--;
							}
							else
							{
								if (label[radial[0]-2].cut_start!=1)
								{
									for (j=0;j<3;j++)
										radial[j]--;
								}
							}
						}
					}	
				}
			}
			if (havemerge)        //�ϲ�
			{
				label[radial[0]].cut_end=label[radial[2]].cut_end;
				for (j=radial[1];j<MAX_STROKE-2;j++)
					label[j]=label[j+2];
				for (j=MAX_STROKE-2;j<MAX_STROKE;j++)
				{
					label[j].stroke=0;             
					label[j].cut_start=0;          
					label[j].cut_end=0;           
					label[j].belong=0;             
				}
				for (j=0;(j<MAX_STROKE)&&(label[j].stroke!=0);j++)
				{
					if (label[j].belong==(label[radial[0]].belong+1))
						label[j].belong=label[j].belong-1;
					if (label[j].belong>(label[radial[0]].belong+1))
						label[j].belong=label[j].belong-2;
				}
				before_dis=-1;
			}
		}
	}
	return;
}

int IsConnect(int radial[],CutLabel label[],Point data[][MAX_LENGTH],Stroke_C pstroke[],int pos) //�ж��Ƿ�������ĸ���зֲ���
{
	int i,j,k;
	int letter[3][10];
	for (i=0;i<3;i++)
		for (j=0;j<10;j++)
			letter[i][j]=-1;
	for (i=0;(i<MAX_STROKE)&&(label[i].stroke!=0);i++)             
	{                                           //��ĸ���ʻ�����
		for (j=0;j<3;j++)
		{
			if (label[i].belong==label[radial[j]].belong)
			{
				for (k=0;(k<10)&&(letter[j][k]!=-1);k++)
				{}
				letter[j][k]=i;
				break;
			}
		}
	}
	for (i=0;i<3;i++)
	{                                           //�����зֶδ��������¸��ӱʻ�,�����зֶ�
		if ((letter[i][1]!=-1)&&(pstroke[label[letter[i][1]].stroke-1].domain.width_y.end>pstroke[label[letter[i][0]].stroke-1].baseline.centre))
			return -1;	
	}
	int have=0;                                 //����һ�������зֶδ����ӱʻ�,�����зֶ�
	Point main[3][MAX_LENGTH];
	Point dot[10][MAX_LENGTH];
	for (i=0;i<3;i++)
	{
		for (j=0;j<MAX_LENGTH;j++)
		{
			main[i][j].x=-1;
			main[i][j].y=-1;
		}
	}
	for (i=0;i<10;i++)
	{
		for (j=0;j<MAX_LENGTH;j++)
		{
			dot[i][j].x=-1;
			dot[i][j].y=-1;
		}
	}
	for (j=label[letter[0][0]].cut_start;j<=label[letter[0][0]].cut_end;j++)
		main[0][j-label[letter[0][0]].cut_start]=data[label[letter[0][0]].stroke-1][j-1];
	for (i=1;(i<10)&&(letter[0][i]!=-1);i++)
	{}
	if (i>1)
	{
		have=1;
		for (j=1;j<i;j++)
			for (k=0;k<MAX_LENGTH;k++)
				dot[j-1][k]=data[label[letter[0][j]].stroke-1][k];
	}
	for (j=label[letter[1][0]].cut_start;j<=label[letter[1][0]].cut_end;j++)
		main[1][j-label[letter[1][0]].cut_start]=data[label[letter[1][0]].stroke-1][j-1];
	for (i=1;(i<10)&&(letter[1][i]!=-1);i++)
	{}
	if (i>1)
	{
		if (have)
			return -1;
		else
		{
			have=1;
			for (j=1;j<i;j++)
				for (k=0;k<MAX_LENGTH;k++)
					dot[j-1][k]=data[label[letter[1][j]].stroke-1][k];
		}
	}
	for (j=label[letter[2][0]].cut_start;j<=label[letter[2][0]].cut_end;j++)
		main[2][j-label[letter[2][0]].cut_start]=data[label[letter[2][0]].stroke-1][j-1];
	for (i=1;(i<10)&&(letter[2][i]!=-1);i++)
	{}
	if (i>1)
	{
		if (have)
			return -1;
		else
		{
			have=1;
			for (j=1;j<i;j++)
				for (k=0;k<MAX_LENGTH;k++)
					dot[j-1][k]=data[label[letter[2][j]].stroke-1][k];	
		}
	}
	int temp;
	if (have)
	{
		temp=ThreeDot(main,dot);
		if (temp!=-1)      //���ӱʻ�������ʽ,��������зֶ�
		{
			if (ConnectMain(main,pos)!=-1) 
				return temp;
		}		     
	}
	else
	{
		if (ConnectMain(main,pos)==1)     //��������ʻ�����ĳ���������ĸ,�����зֶ�
			return DENSITY_X;             //�þ���DENSITY_X��ʾû�е㸽�ӵ����
	}
	return -1;
}

int ThreeDot(Point data_m[][MAX_LENGTH],Point data_d[][MAX_LENGTH])  //�жϵ�Ⱥ�Ƿ�Ϊ����ʽ������ĸ����
{
	int i,j,k;
	//ȥ������̫�ٵĵ�ʻ�
	for (i=0;(i<10)&&(data_d[i][0].x!=-1);i++)
	{
		if(data_d[i][MIN_LENGTH-1].x==-1)
		{
			if (i==9)
			{
				for (k=0;k<MIN_LENGTH-1;k++)
				{
					data_d[i][k].x=-1;
					data_d[i][k].y=-1;
				}
			}
			else
			{
				for (j=i;(j<9)&&(data_d[j][0].x!=-1);j++)
				{
					for (k=0;k<MAX_LENGTH;k++)
					{
						data_d[j][k].x=data_d[j+1][k].x;
						data_d[j][k].y=data_d[j+1][k].y;
					}
				}
				for (k=0;k<MAX_LENGTH;k++)
				{
					data_d[9][k].x=-1;
					data_d[9][k].y=-1;
				}
				i--;
			}
		}
	}
	for (i=0;(i<10)&&(data_d[i][0].x!=-1);i++)
	{}
	if (i<3)
		return -1;
	Domain domain_d[10];
	for (i=0;i<10;i++)
	{
		if (data_d[i][0].x==-1)
		{
			domain_d[i].width_x.start=-1;
			domain_d[i].width_x.end=-1;
			domain_d[i].width_y.start=-1;
			domain_d[i].width_y.end=-1;
		}
		else
		{
			domain_d[i].width_x.start=data_d[i][0].x;	
			domain_d[i].width_x.end=data_d[i][0].x;
			domain_d[i].width_y.start=data_d[i][0].y;	
			domain_d[i].width_y.end=data_d[i][0].y;
			for(j=1;(j<MAX_LENGTH)&&(data_d[i][j].x!=-1);j++)
			{	
				if (data_d[i][j].x<domain_d[i].width_x.start)                    
					domain_d[i].width_x.start=data_d[i][j].x;
				if (data_d[i][j].x>domain_d[i].width_x.end)
					domain_d[i].width_x.end=data_d[i][j].x;
				if (data_d[i][j].y<domain_d[i].width_y.start)
					domain_d[i].width_y.start=data_d[i][j].y;
				if (data_d[i][j].y>domain_d[i].width_y.end)
					domain_d[i].width_y.end=data_d[i][j].y;
			}
		}
	}
	int num;
	num=1;
	for (i=1;(i<10)&&(domain_d[i].width_x.start!=-1);i++)   //�������ص�ʱ,ֻ����һ��
	{
		num++;
		for (j=0;j<i;j++)
		{
			if (int(sqrt(pow(((domain_d[i].width_x.start+domain_d[i].width_x.end)/2)
				-((domain_d[j].width_x.start+domain_d[j].width_x.end)/2),2)
				+pow(((domain_d[i].width_y.start+domain_d[i].width_y.end)/2)
				-((domain_d[j].width_y.start+domain_d[j].width_y.end)/2),2)))<3)
			{
				num--;
				break;
			}
		}	
	}
	if (num<3)                                             //����С��3��������ʽ
		return -1;
	Width width_m,width_d;
	width_m.start=data_m[1][0].x;             //������ĸ�м�εĿ�ȷ�Χ	
	width_m.end=data_m[1][0].x;
	for(i=1;(i<MAX_LENGTH)&&(data_m[1][i].x!=-1);i++)
	{	
		if (data_m[1][i].x<width_m.start)                    
			width_m.start=data_m[1][i].x;
		if (data_m[1][i].x>width_m.end)
			width_m.end=data_m[1][i].x;
	}
	width_d=domain_d[0].width_x;              //�����Ⱥ�Ŀ�ȷ�Χ
	for (i=1;(i<10)&&(domain_d[i].width_x.start!=-1);i++)
	{
		if (domain_d[i].width_x.start<width_d.start)                    
			width_d.start=domain_d[i].width_x.start;
		if (domain_d[i].width_x.end>width_d.end)                    
			width_d.end=domain_d[i].width_x.end;
	}
	return abs((width_m.end+width_m.start)/2-(width_d.end+width_d.start)/2);
}

int ConnectMain(Point data_m[][MAX_LENGTH],int pos)            //�ж�����ʻ��Ƿ�����ĳ���������ĸ
{
	int i,j,k;
	Width width[3];
	for (i=0;i<3;i++)
	{
		width[i].start=data_m[i][0].y;	
		width[i].end=data_m[i][0].y;
		for(j=1;(j<MAX_LENGTH)&&(data_m[i][j].x!=-1);j++)
		{	
			if (data_m[i][j].y<width[i].start)
				width[i].start=data_m[i][j].y;
			if (data_m[i][j].y>width[i].end)
				width[i].end=data_m[i][j].y;
		}
	}
	Width min_width;
	min_width=width[0];
	for (i=1;i<3;i++)
	{
		if ((width[i].end-width[i].start)<(min_width.end-min_width.start))
			min_width=width[i];
	}
	int letter;
	switch (pos)
	{
	case 1:  //�׶˲���
		letter=0;
		break;
	case 2:  //�м䲿��
		letter=1;
		break;
	case 3:  //β�˲���
		letter=3;
		break;
	case 4:  //��������
		letter=2;
		break;
	default:
		letter=0;
		break;
	}
	for (i=0;i<3;i++)                           //�Ը߶Ȳ��ų����������ĸ
	{
		if (RadialList[letter][i]!=3)
		{
			if ((width[i].end-width[i].start)>3*(min_width.end-min_width.start))
				return -1;	  //������̬����
		}
		else
		{
			if ((min_width.start-width[i].start)>2*(min_width.end-min_width.start))
				return -1;	 //������̬����
		}
	}
	int result[3][LETTER_MCOUNT+RADIAL_COUNT];
	int distance0[LETTER_MCOUNT+RADIAL_COUNT],distance1[LETTER_MCOUNT+RADIAL_COUNT];
	double degree0,degree1;
	int temp[LETTER_MCOUNT+RADIAL_COUNT],num[LETTER_MCOUNT+RADIAL_COUNT];
	int mg[GRID_M_X*GRID_M_Y*4];
	int ms[STREAM_M*4];
	int mc[CODE_M];
	Stroke_R strokes[MAX_STROKE];
	Point data_selected[MAX_STROKE][MAX_LENGTH];
	for (i=0;i<3;i++)
	{
		Initialize(data_selected);
		for (j=0;j<MAX_LENGTH;j++)
			data_selected[0][j]=data_m[i][j];
		Unitize(data_selected);         
		Smooth(data_selected);         
		Smooth(data_selected);
		GetStroke(strokes,data_selected);
		GetGrid(mg,strokes[0].fxm,GRID_M_X,GRID_M_Y,strokes[0].lu,strokes[0].rd);//��������ʻ�
		GetStream(ms,strokes[0].ms,STREAM_M);  
		GetCode(mc,strokes[0].ms,CODE_M);
		SequenceDistance_C(mg,ms,mc,distance0,0);
		SequenceDistance_C(mg,ms,mc,distance1,1);
		for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
			temp[j]=distance0[j];
		Sequence(temp,LETTER_MCOUNT+RADIAL_COUNT);	
		degree0=double(temp[1]-temp[0])/double(temp[0]);
		for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
			temp[j]=distance1[j];
		Sequence(temp,LETTER_MCOUNT+RADIAL_COUNT);	
		degree1=double(temp[1]-temp[0])/double(temp[0]);
		if (degree0>=degree1)
		{
			Sequence(distance0,num,LETTER_MCOUNT+RADIAL_COUNT);
			for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
				result[i][j]=num[j]+1;                   //���Ϊ��1-61
		}
		else
		{
			Sequence(distance1,num,LETTER_MCOUNT+RADIAL_COUNT);
			for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
				result[i][j]=num[j]+1;                  
		}
		for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
		{
			if (result[i][j]==5)
				result[i][j]=LETTER_MCOUNT+1;
			if (result[i][j]==6)
				result[i][j]=LETTER_MCOUNT+2;
		}			
	}
	int count;
	count=0;
	int k1;
	for (i=0;i<3;i++)
	{
		k=0;
		for (j=0;j<LETTER_MCOUNT+RADIAL_COUNT;j++)
		{
			if (radialposition[result[i][j]-1]==radialposition[RadialList[letter][i]+LETTER_MCOUNT-1])
			{
				if (result[i][j]!=(RadialList[letter][i]+LETTER_MCOUNT))
					k++;
				else
				{
					k1=j;
					break;
				}
			}
		}
		if (k<5)
			count++;
		if (RadialList[letter][i]==1)       //�ų�����������ĸ��������������ʶ
		{
			for (j=0;j<k1;j++)
			{
				if ((result[i][j]==55)||((result[i][j]==43)))
					count--;
			}
		}
		if (RadialList[letter][i]==2)
		{
			for (j=0;j<k1;j++)
			{
				if (result[i][j]==44)
					count--;
			}	
		}
	}
	if (count==3)
		return 1;   //���ξ�����Χ�ڷ���,���ж�Ϊ�����
	return 0;       //ʶ����״����
}

void SequenceDistance_C(int * data_g,int * data_s,int * data_c,int * result,int term)
{
	int i;
	double degree1,degree2;
	int temp[LETTER_MCOUNT+RADIAL_COUNT],temp1[LETTER_MCOUNT+RADIAL_COUNT],temp2[LETTER_MCOUNT+RADIAL_COUNT];
	if (term==0)
	{
		for (i=0;i<LETTER_MCOUNT;i++)
			*(result+i)=Distance(data_g,RGm[i],GRID_M_X*GRID_M_Y*4);
		for (i=0;i<RADIAL_COUNT;i++)
			*(result+i+LETTER_MCOUNT)=Distance(data_g,CGm[i],GRID_M_X*GRID_M_Y*4);
	}
	else
	{
		for (i=0;i<LETTER_MCOUNT;i++)
		{
			*(temp1+i)=Distance(data_s,RSm[i],STREAM_M*4);
			*(temp+i)=*(temp1+i);
		}
		for (i=0;i<RADIAL_COUNT;i++)
		{
			*(temp1+i+LETTER_MCOUNT)=Distance(data_s,CSm[i],STREAM_M*4);
			*(temp+i+LETTER_MCOUNT)=*(temp1+i+LETTER_MCOUNT);
		}
		Sequence(temp,LETTER_MCOUNT+RADIAL_COUNT);	
		degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
		for (i=0;i<LETTER_MCOUNT;i++)
		{
			*(temp2+i)=Distance(data_s,RSm[i],STREAM_M*4)+DistanceCode(data_c,RCm[i],CODE_M)*1000;
			*(temp+i)=*(temp2+i);
		}
		for (i=0;i<RADIAL_COUNT;i++)
		{
			*(temp2+i+LETTER_MCOUNT)=Distance(data_s,CSm[i],STREAM_M*4)+DistanceCode(data_c,CCm[i],CODE_M)*1000;
			*(temp+i+LETTER_MCOUNT)=*(temp2+i+LETTER_MCOUNT);
		}
		Sequence(temp,LETTER_MCOUNT+RADIAL_COUNT);	
		degree2=double(*(temp+1)-*(temp+0))/double(*(temp+0));
		if (degree1>=degree2)
		{
			for (i=0;i<LETTER_MCOUNT+RADIAL_COUNT;i++)
				*(result+i)=*(temp1+i);
		}
		else
		{
			for (i=0;i<LETTER_MCOUNT+RADIAL_COUNT;i++)
				*(result+i)=*(temp2+i);
		}
	}
	return;
}

void LetterCuts()                                                     
{
	//ά�Ĳ����зֶνṹ���ձ�
	//�зֶ�:1-3
	int i=0;
	RadialList[i][0]=1;RadialList[i][1]=2;RadialList[i][2]=2;
	i=1;
	RadialList[i][0]=2;RadialList[i][1]=2;RadialList[i][2]=2;
	i=2;
	RadialList[i][0]=1;RadialList[i][1]=2;RadialList[i][2]=3;
	i=3;
	RadialList[i][0]=2;RadialList[i][1]=2;RadialList[i][2]=3;
}

void GetFeature_R(Feature_R * pfeature,Point data[][MAX_LENGTH])                //ʶ��ʱ�õ����ַ�ʶ������
{          
	int i,temp;	
	int dot1=-1,dot2=-1,affix1=-1,affix2=-1;
	int tempf[GRID_A_X*GRID_A_Y*4];
	int tempfz[STREAM_A*4];
	int tempfs[CODE_A];
	int area[LETTER_SACOUNT+LETTER_FDCOUNT];
	Stroke_R strokes[MAX_STROKE];
	GetStroke(strokes,data);
	for (i=0;i<GRID_M_X*GRID_M_Y*4;i++)        //������ʼ��
		pfeature->mg[i]=0;
	for (i=0;i<STREAM_M*4;i++)
		pfeature->ms[i]=0;
	for (i=0;i<CODE_M;i++)
		pfeature->mc[i]=0;
	pfeature->affix=0;                        //affix:0�޸��ӱʻ�,1��һ�����ӱʻ�,2���������ӱʻ�
	for (i=0;i<GRID_A_X*GRID_A_Y*4;i++)
	{
		pfeature->ag1[i]=0;
		pfeature->ag2[i]=0;
	}
	for (i=0;i<STREAM_A*4;i++)
	{
		pfeature->as1[i]=0;
		pfeature->as2[i]=0;
	}
	for (i=0;i<CODE_A;i++)
	{
		pfeature->ac1[i]=0;
		pfeature->ac2[i]=0;
	}
	pfeature->fdot=0;                          //fdot:0�޵�����,1�������,2������,3��������,4��������,5һ���
	pfeature->dot_num=0;
	pfeature->dot_pos=-1;
	pfeature->dot_rel=-1;
	for (i=0;i<MAX_STROKE;i++)                 //�����ʻ�                       
	{
		if (strokes[i].sign==Dot)                       
		{
			if 	(pfeature->dot_num<3)
				pfeature->dot_num++;                     
			if (pfeature->dot_pos==-1)                   
			{
				if (strokes[i].baseline<strokes[0].baseline)
					pfeature->dot_pos=1;     
				else
					pfeature->dot_pos=0;
			}
			if (dot1==-1)
				dot1=i;
			else if (dot2==-1)
				dot2=i;
		}
	}
	for (i=0;i<MAX_STROKE;i++)                           //�����ӱʻ�(��һ��),���ڻ����·��ĸ��ӱʻ�
	{
		if ((strokes[i].sign==Affix)&&(strokes[i].baseline>=strokes[0].baseline))
		{  
			strokes[i].sign=Void;                        //�Ѿ�������Ĳ��ٴ���ڶ���
			if (pfeature->dot_num==0)                    //δ���������
			{ 
				if (((strokes[i].rd.y-strokes[i].lu.y)==0)
					||((strokes[i].rd.x-strokes[i].lu.x)/(strokes[i].rd.y-strokes[i].lu.y)>=3))
				{                                        //���ں᳤��״�ĸ��ӱʻ�
					pfeature->dot_num=2;                 //��Ϊ�������
					pfeature->dot_pos=0;
					pfeature->dot_rel=0;
				}
				else if(((strokes[i].rd.x-strokes[i].lu.x)==0)
					||((strokes[i].rd.y-strokes[i].lu.y)/(strokes[i].rd.x-strokes[i].lu.x)>=3))
				{                                        //����������״�ĸ��ӱʻ�
					pfeature->dot_num=2;                 //��Ϊ��������
					pfeature->dot_pos=0;
					pfeature->dot_rel=1;
				}
				else 
				{
					GetGrid(tempf,strokes[i].fxm,GRID_A_X,GRID_A_Y,strokes[i].lu,strokes[i].rd);
					GetStream(tempfz,strokes[i].ms,STREAM_A);
					GetCode(tempfs,strokes[i].ms,CODE_A);
					area[0]=0;area[1]=0;area[2]=0;area[3]=0;area[4]=0;
					area[5]=1;area[6]=1;area[7]=0;area[8]=1;area[9]=1;
					switch (RadialOfMinDistance(tempf,tempfz,tempfs,area))
					{
					case 5:                         //�����������
						pfeature->dot_num=2;                
						pfeature->dot_pos=0;
						pfeature->dot_rel=0;
						break;
					case 6:                         //������������
						pfeature->dot_num=2;                
						pfeature->dot_pos=0;
						pfeature->dot_rel=1;
						break;
					case 8:                         //������������	
						pfeature->dot_num=3;                
						pfeature->dot_pos=0;
						pfeature->dot_rel=-1;
						break;
					case 9:	                        //����Ϊһ��
						pfeature->dot_num=1;                
						pfeature->dot_pos=0;
						pfeature->dot_rel=-1;
						dot1=i;
						break;
					}
				}              
			}
			else if ((pfeature->dot_num==1)&&(pfeature->dot_pos==0))                    
			{                                            //���һ���µ�����
				if (abs((strokes[i].rd.y+strokes[i].lu.y)-(strokes[dot1].rd.y+strokes[dot1].lu.y))
	>abs((strokes[i].rd.x+strokes[i].lu.x)-(strokes[dot1].rd.x+strokes[dot1].lu.x)))
				{
					if ((strokes[i].rd.y+strokes[i].lu.y)<(strokes[dot1].rd.y+strokes[dot1].lu.y))
						pfeature->dot_num=3;             //��Ϊ�������
					else
					{
						pfeature->dot_num=2;             //��Ϊһ��         
						pfeature->dot_rel=1;
						dot2=i;
					}
				}
				else
				{
					pfeature->dot_num=2;                 //��Ϊһ��         
					pfeature->dot_rel=0;
					dot2=i;
				}
			}                                            //��������µ�����
			else if ((pfeature->dot_num==2)&&(pfeature->dot_pos==0))                                             	                 
			{                                            //��Ϊһ��
				pfeature->dot_num=3;                
				pfeature->dot_rel=-1;
			}
		}
	}
	for (i=0;i<MAX_STROKE;i++)                           //�����ӱʻ�(�ڶ���),���������Ϸ��ĸ��ӱʻ�
	{
		if (strokes[i].sign==Affix)                      //����������ӱʻ� 
		{
			strokes[i].sign=Void;
			if (affix1==-1)
				affix1=i;
			else if (affix2==-1)
				affix2=i;
		}
	}
	if ((affix1!=-1)&&(affix2!=-1))                      //���������ӱʻ��ĵ���
	{
		if (abs((strokes[affix1].rd.y+strokes[affix1].lu.y)-(strokes[affix2].rd.y+strokes[affix2].lu.y))
	>abs((strokes[affix1].rd.x+strokes[affix1].lu.x)-(strokes[affix2].rd.x+strokes[affix2].lu.x)))
		{
			if ((strokes[affix1].rd.y+strokes[affix1].lu.y)<(strokes[affix2].rd.y+strokes[affix2].lu.y))
			{                                            //����λ��ʱ���µ�Ϊ1
				temp=affix1;
				affix1=affix2;
				affix2=temp;
			}
		}
		else
		{
			if ((strokes[affix1].rd.x+strokes[affix1].lu.x)<(strokes[affix2].rd.x+strokes[affix2].lu.x))
			{                                            //����λ��ʱ���ҵ�Ϊ1
				temp=affix1;
				affix1=affix2;
				affix2=temp;
			}
		}
	}    //�����ӱʻ�(������),��Ի����Ϸ��ĸ��ӱʻ�
	if ((affix1!=-1)&&(affix2==-1))                      //��ֻ��һ�����ӱʻ�
	{
		if (pfeature->dot_num==0)	                          
		{
			if (((strokes[affix1].rd.y-strokes[affix1].lu.y)==0)
				||((strokes[affix1].rd.x-strokes[affix1].lu.x)/(strokes[affix1].rd.y-strokes[affix1].lu.y)>=3))
			{                                           //���ں᳤��״�ĸ��ӱʻ�
				if (strokes[affix1].start.x<strokes[affix1].end.x)
				{                                       //��Ϊ�������
					pfeature->dot_num=2;                    
					pfeature->dot_pos=1;
					affix1=-1;
				}
			}
			else
			{
				GetGrid(tempf,strokes[affix1].fxm,GRID_A_X,GRID_A_Y,strokes[affix1].lu,strokes[affix1].rd);
				GetStream(tempfz,strokes[affix1].ms,STREAM_A);
				GetCode(tempfs,strokes[affix1].ms,CODE_A);
				area[0]=1;area[1]=1;area[2]=1;area[3]=1;area[4]=1;
				area[5]=1;area[6]=0;area[7]=1;area[8]=0;area[9]=1;
				switch (RadialOfMinDistance(tempf,tempfz,tempfs,area))
				{
				case 5:                                 //�����������
					pfeature->dot_num=2;                
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					affix1=-1;
					break;
				case 7:                                 //������������	
					pfeature->dot_num=3;                
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					affix1=-1;
					break;
				case 9:	                                //����Ϊһ��
					pfeature->dot_num=1;                
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					dot1=affix1;
					affix1=-1;
					break;
				}			
			}
		}
		else if ((pfeature->dot_num==1)&&(pfeature->dot_pos==1))
		{
			if (abs((strokes[affix1].rd.y+strokes[affix1].lu.y)-(strokes[dot1].rd.y+strokes[dot1].lu.y))
				>abs((strokes[affix1].rd.x+strokes[affix1].lu.x)-(strokes[dot1].rd.x+strokes[dot1].lu.x)))
			{                                            //���븽�ӱʻ�Ϊ����λ��
				if (strokes[dot1].lu.x>strokes[affix1].lu.x)
				{                                        //���븽�ӱʻ���಻Զ                                     
					pfeature->dot_num=3;                
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					affix1=-1;
				}
			}
			else                                         //���븽�ӱʻ�Ϊ����λ��
			{                                            
				affix2=dot1;                             //�����гɵ�ĸ��ӱʻ��ָ�
				dot1=-1;
				pfeature->dot_num=0;
				pfeature->dot_pos=-1;
				GetGrid(tempf,strokes[affix1].fxm,GRID_A_X,GRID_A_Y,strokes[affix1].lu,strokes[affix1].rd);
				GetStream(tempfz,strokes[affix1].ms,STREAM_A);
				GetCode(tempfs,strokes[affix1].ms,CODE_A);
				area[0]=1;area[1]=1;area[2]=1;area[3]=1;area[4]=1;
				area[5]=0;area[6]=0;area[7]=0;area[8]=0;area[9]=1;
				switch (RadialOfMinDistance(tempf,tempfz,tempfs,area))
				{
				case 9:	                                //����Ϊһ��
					pfeature->dot_num=2;                
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					dot1=affix2;
					dot2=affix1;
					affix1=-1;
					affix2=-1;
					break;
				}
			}		
		}
	}
	else if ((affix1!=-1)&&(affix2!=-1))                 //�����������ӱʻ�
	{
		if (pfeature->dot_num==0)	                          
		{
			if (abs((strokes[affix2].rd.y+strokes[affix2].lu.y)-(strokes[affix1].rd.y+strokes[affix1].lu.y))
			>abs((strokes[affix2].rd.x+strokes[affix2].lu.x)-(strokes[affix1].rd.x+strokes[affix1].lu.x)))
			{                                            //�����ӱʻ�Ϊ����λ��
				GetGrid(tempf,strokes[affix2].fxm,GRID_A_X,GRID_A_Y,strokes[affix2].lu,strokes[affix2].rd);
				GetStream(tempfz,strokes[affix2].ms,STREAM_A);
				GetCode(tempfs,strokes[affix2].ms,CODE_A);	
				area[0]=1;area[1]=1;area[2]=1;area[3]=1;area[4]=1;
				area[5]=0;area[6]=0;area[7]=1;area[8]=0;area[9]=1;
				switch (RadialOfMinDistance(tempf,tempfz,tempfs,area))
				{
				case 7:	                                //������������
					pfeature->dot_num=3;                     
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					break;
				case 9:                                 //����Ϊһ��
					pfeature->dot_num=3;                     
					pfeature->dot_pos=1;
					pfeature->dot_rel=-1;
					affix1=-1;
					break;
				default: 
					affix1=affix2;                      
					pfeature->dot_num=0;                     
					pfeature->dot_pos=-1;
					pfeature->dot_rel=-1;
					break;
				}		
				affix2=-1;
			}
		}
		else if ((pfeature->dot_num==1)&&(pfeature->dot_pos==1))
		{
			if (abs((strokes[dot2].rd.y+strokes[dot2].lu.y)-(strokes[dot1].rd.y+strokes[dot1].lu.y))
					>abs((strokes[dot2].rd.x+strokes[dot2].lu.x)-(strokes[dot1].rd.x+strokes[dot1].lu.x)))
			{                                            //���븽�ӱʻ�Ϊ����λ��	                          
				pfeature->dot_num=3;                
				pfeature->dot_pos=1;
				pfeature->dot_rel=-1;
				affix2=-1;
			}
		}
	}
	if ((pfeature->dot_num==2)&&(pfeature->dot_pos==0))  //��������λ����
	{
		if (pfeature->dot_rel==-1)
		{
			if (abs(strokes[dot1].start.y-strokes[dot2].start.y)
				>abs(strokes[dot1].start.x-strokes[dot2].start.x))
				pfeature->dot_rel=1;
			else
				pfeature->dot_rel=0;
		}
	}
	if ((pfeature->dot_num==1)&&(pfeature->dot_pos==1)&&(pfeature->affix==0))  
	{                                                    //�Ե�򸽼ӱʻ����ж�
		if (((abs(strokes[dot1].start.y-strokes[dot1].end.y)
			>5*abs(strokes[dot1].start.x-strokes[dot1].end.x))
			||((strokes[dot1].rd.x+strokes[dot1].lu.x)!=(strokes[dot1].start.x+strokes[dot1].end.x)))
			&&(strokes[dot1].length>DOT_LENGTH/3))
		{                                                //�����гɵ�ĸ��ӱʻ��ָ�
			affix1=dot1;
			dot1=-1;
			pfeature->dot_num=0;
			pfeature->dot_pos=-1;
		}
	}
	if ((pfeature->dot_num==2)&&(pfeature->dot_pos==1))  //��������������̫�̵����
	{
		if (abs((strokes[dot1].rd.y+strokes[dot1].lu.y)-(strokes[dot2].rd.y+strokes[dot2].lu.y))
	>2*abs((strokes[dot1].rd.x+strokes[dot1].lu.x)-(strokes[dot2].rd.x+strokes[dot2].lu.x))) 
	pfeature->dot_num=3;
	}
	if (affix1!=-1)                                      //�����ӱʻ�(���ı�),д��
	{
		GetGrid(pfeature->ag1,strokes[affix1].fxm,GRID_A_X,GRID_A_Y,strokes[affix1].lu,strokes[affix1].rd);
		GetStream(pfeature->as1,strokes[affix1].ms,STREAM_A);
		GetCode(pfeature->ac1,strokes[affix1].ms,CODE_A);
		pfeature->affix=1;
	}
	if (affix2!=-1)
	{
		GetGrid(pfeature->ag2,strokes[affix2].fxm,GRID_A_X,GRID_A_Y,strokes[affix2].lu,strokes[affix2].rd);
		GetStream(pfeature->as2,strokes[affix2].ms,STREAM_A);
		GetCode(pfeature->ac2,strokes[affix2].ms,CODE_A);
		pfeature->affix=2;
	}
	GetGrid(pfeature->mg,strokes[0].fxm,GRID_M_X,GRID_M_Y,strokes[0].lu,strokes[0].rd);//��������ʻ�
	GetStream(pfeature->ms,strokes[0].ms,STREAM_M);  
	GetCode(pfeature->mc,strokes[0].ms,CODE_M);
}

void GetGrid(int f[],Fxm fxm[],int sizex,int sizey,Point lu,Point rd)   //�ӷ����뵽����������
{
	int i,j,n,sum,count,pos;    
	for (i=0;i<sizex*sizey*4;i++)
		f[i]=0;
	void * p1;
	int * logx;
	if ((p1=malloc(sizeof(int)*(sizex+1)))!=0)
		logx=(int *)p1;
	void * p2;
	int * logy;
	if ((p2=malloc(sizeof(int)*(sizex+1)))!=0)
		logy=(int *)p2;
	for (i=0;i<(sizex+1);i++)
		*(logx+i)=0;
	for (i=0;i<(sizey+1);i++)
		*(logy+i)=0;
	*(logx+0)=lu.x;
	*(logx+sizex)=rd.x+1;
	*(logy+0)=lu.y;
	*(logy+sizey)=rd.y+1;
	Fxm temp_fxm[DENSITY_X*10];
	sum=FormalizeFxm(temp_fxm,fxm);
	n=sum/sizex;
	count=0;
	for (i=1;i<sizex;i++)
	{
		pos=*(logx+i-1);
		while(true)
		{	
			int flag = 0;
			for (j=0;(j<DENSITY_X*10)&&(temp_fxm[j].fx!=0);j++)
			{
				if (temp_fxm[j].pos.x==pos)
				{
					count++;
					flag = 1;
				}
			}
			if (count<n && flag)
				pos++;
			else
			{
				*(logx+i)=pos+1;
				count=count-n;
				break;
			}
		}
	}
	n=sum/sizey;
	count=0;
	for (i=1;i<sizey;i++)
	{
		pos=*(logy+i-1);
		while(true)
		{
			int flag = 0;
			for (j=0;(j<DENSITY_X*10)&&(temp_fxm[j].fx!=0);j++)
			{
				if (temp_fxm[j].pos.y==pos)
				{
					count++;
					flag = 1;
				}
			}
			if (count<n && flag)
				pos++;
			else
			{
				*(logy+i)=pos+1;
				count=count-n;
				break;
			}
		}
	}
	int posx=0,posy=0;
	for (i=0;(i<DENSITY_X*10)&&(temp_fxm[i].fx!=0);i++)
	{
		for (j=0;j<sizex;j++)
		{
			if ((temp_fxm[i].pos.x>=*(logx+j))&&(temp_fxm[i].pos.x<*(logx+j+1)))
			{
				posx=j;
				break;
			}
		}
		for (j=0;j<sizey;j++)
		{
			if ((temp_fxm[i].pos.y>=*(logy+j))&&(temp_fxm[i].pos.y<*(logy+j+1)))
			{
				posy=j;
				break;
			}
		}
		if (temp_fxm[i].fx==1)
			f[(posx*sizey+posy)*4+0]++;
		if (temp_fxm[i].fx==2)
			f[(posx*sizey+posy)*4+1]++;
		if (temp_fxm[i].fx==3)
			f[(posx*sizey+posy)*4+2]++;
		if (temp_fxm[i].fx==4)
			f[(posx*sizey+posy)*4+3]++;
	}
	free(p1);
	free(p2);
}

int FormalizeFxm(Fxm fxm1[],Fxm fxm2[])           //���������    
{
	int i,k,count;
	for (i=0;i<DENSITY_X*10;i++)
	{
		fxm1[i].pos.x=-1;
		fxm1[i].pos.y=-1;
		fxm1[i].fx=0;
		fxm1[i].size=0;
	}
	int stepx,stepy;
	k=0;
	for (i=0;(i<MAX_LENGTH)&&(fxm2[i].fx!=0);i++)
	{
		count=fxm2[i].size;
		fxm1[k].pos=fxm2[i].pos;
		fxm1[k].fx=fxm2[i].fx;
		fxm1[k].size=1;
		count--;
		k++;
		if (k>=DENSITY_X*10)
			return DENSITY_X*10;
		stepx=0;
		stepy=0;
		if (fxm2[i+1].fx!=0)
		{
			stepx=fxm2[i+1].pos.x-fxm2[i].pos.x;
			stepy=fxm2[i+1].pos.y-fxm2[i].pos.y;
		}
		else
		{
			if (fxm2[i].fx==1)
				stepy=fxm2[i].size;
			if (fxm2[i].fx==2)
				stepx=-fxm2[i].size;
			if (fxm2[i].fx==3)
				stepy=-fxm2[i].size;
			if (fxm2[i].fx==4)
				stepx=fxm2[i].size;
		}
		while (((stepx!=0)||(stepy!=0))&&(count!=0))
		{
			if (stepx>0)
			{
				fxm1[k].pos.x=fxm1[k-1].pos.x+1;
				fxm1[k].pos.y=fxm1[k-1].pos.y;
				fxm1[k].fx=fxm1[k-1].fx;
				fxm1[k].size=1;
				stepx--;
				count--;
				k++;
				if (k>=DENSITY_X*10)
					return DENSITY_X*10;
			}
			else if (stepx<0)
			{
				fxm1[k].pos.x=fxm1[k-1].pos.x-1;
				fxm1[k].pos.y=fxm1[k-1].pos.y;
				fxm1[k].fx=fxm1[k-1].fx;
				fxm1[k].size=1;
				stepx++;
				count--;
				k++;
				if (k>=DENSITY_X*10)
					return DENSITY_X*10;
			}
			if (count==0)
				break;
			if (stepy>0)
			{
				fxm1[k].pos.x=fxm1[k-1].pos.x;
				fxm1[k].pos.y=fxm1[k-1].pos.y+1; 
				fxm1[k].fx=fxm1[k-1].fx;
				fxm1[k].size=1;	
				stepy--;
				count--;
				k++;
				if (k>=DENSITY_X*10)
					return DENSITY_X*10;
			}
			else if (stepy<0)
			{
				fxm1[k].pos.x=fxm1[k-1].pos.x;
				fxm1[k].pos.y=fxm1[k-1].pos.y-1;
				fxm1[k].fx=fxm1[k-1].fx;
				fxm1[k].size=1;
				stepy++;
				count--;
				k++;
				if (k>=DENSITY_X*10)
					return DENSITY_X*10;
			}
			if (count==0)
				break;
		}
	}
	return k;
}

void GetStream(int f[],int ms[],int size)                  //�ӷ����뵽������������
{                           
	int i,j,k,n,t;                       
	for (i=0;i<size*4;i++)
		f[i]=0;
	if (ms[0]==0)
		return;      
	void * p;
	int * temp;
	if ((p=malloc(sizeof(int)*MAX_LENGTH))!=0)
		temp=(int *)p;
	else
		return;
	for (i=0;i<MAX_LENGTH;i++)
		*(temp+i)=0; 
	for (i=0;(i<MAX_LENGTH)&&(ms[i]!=0);i++) 
		*(temp+i)=ms[i];
	k=i;
	n=1;                //��ȥ��������������ÿn������������ͬ�ķ�����ż�¼
	k=0;
	t=ms[0]; 
	for (i=1;((i<MAX_LENGTH)&&(ms[i]!=0));i++)
	{
		if (ms[i]==t)
			n++;
		else
		{
			if ((n>=MIN_CODEFEATURE )&&(k<MAX_LENGTH))
			{ 
				for (j=0;j<n;j++)
				{
					*(temp+k)=t;
					k++;
				}
			}
			t=ms[i];
			n=1;
		}
	}
	if ((n>=MIN_CODEFEATURE )&&(k<MAX_LENGTH))
	{
		for (j=0;j<n;j++)
		{
			*(temp+k)=t;
			k++;
		}
	}    
	n=k/size;                  //д����������
	t=k-k/size*size;
	int n1=0;
	for (i=0;i<size;i++)
	{
		for (j=n1;j<n1+n;j++)
		{
			if (*(temp+j)==1)
				f[4*i+0]++;	
			if (*(temp+j)==2)
				f[4*i+1]++;
			if (*(temp+j)==3)
				f[4*i+2]++;
			if (*(temp+j)==4)
				f[4*i+3]++;
		}
		if (t!=0)
		{
			if (*(temp+n1+n)==1)
				f[4*i+0]++;	
			if (*(temp+n1+n)==2)
				f[4*i+1]++;
			if (*(temp+n1+n)==3)
				f[4*i+2]++;
			if (*(temp+n1+n)==4)
				f[4*i+3]++;
			t--;
			n1=n1+n+1;
		}
		else
			n1=n1+n;
	}
	free(p);
}

void GetCode(int f[],int ms[],int size)        //�ӷ����뵽����
{
	int i,k;                       
	for (i=0;i<size;i++)
		f[i]=0;
	if (ms[0]==0)
		return; 
	k=0;
	f[k]=ms[0];
	k++;
	for (i=1;((i<MAX_LENGTH)&&(ms[i]!=0));i++)
	{
		if ((ms[i]!=f[k-1])&&(k<size))
		{
			f[k]=ms[i];
			k++;
		}
	}
	FormalizeCode(f,size);      //��������������
	return;
}

void FormalizeCode(int * data,int size)                       //�������
{
	int i,k=0;
	int * temp;
	void * p;
	if ((p=malloc(sizeof(int)*size))!=NULL)
		temp=(int *)p;
	for (i=0;i<size;i++)
	{
		*(temp+i)=*(data+i);
		*(data+i)=0;
	}
	int v13[2];             //��������������
	int v24[2];
	for (i=0;i<2;i++)
	{
		v13[i]=0; 
		v24[i]=0;
	}
	if ((*(temp+0)==1)||(*(temp+0)==3))
	{
		v13[0]=*(temp+0);
		*(data+k)=*(temp+0);
		k++;
		if ((*(temp+1)!=0)&&(*(temp+1)!=*(data+k-1)))
		{
			v24[0]=*(temp+1);
			v13[1]=*(temp+1);
			*(data+k)=*(temp+1);
			k++;
		}
	}
	else if ((*(temp+0)==2)||(*(temp+0)==4))
	{
		v24[0]=*(temp+0);
		*(data+k)=*(temp+0);
		k++;
		if ((*(temp+1)!=0)&&(*(temp+1)!=*(data+k-1)))
		{
			v13[0]=*(temp+1);
			v24[1]=*(temp+1);
			*(data+k)=*(temp+1);
			k++;
		}
	}
	for (i=2;(i<size)&&(*(temp+i)!=0);i++)          
	{
		if (*(temp+i)==1)
		{
			v24[1]=*(temp+i);
			v24[0]=*(temp+i-1);
			if (v13[0]==3)
			{
				if ((*(data+k-1)!=v13[1])&&(k<size))
				{
					*(data+k)=v13[1];
					k++;
				}
				if ((*(data+k-1)!=1)&&(k<size))
				{
					*(data+k)=1;
					k++;
				}
				v13[0]=1;
				v13[1]=0;
			}
		}
		if (*(temp+i)==2)
		{
			v13[1]=*(temp+i);
			v13[0]=*(temp+i-1);
			if (v24[0]==4)
			{
				if ((*(data+k-1)!=v24[1])&&(k<size))
				{
					*(data+k)=v24[1];
					k++;
				}
				if ((*(data+k-1)!=2)&&(k<size))
				{
					*(data+k)=2;
					k++;
				}
				v24[0]=2;
				v24[1]=0;
			}
		}	
		if (*(temp+i)==3)
		{
			v24[1]=*(temp+i);
			v24[0]=*(temp+i-1);
			if (v13[0]==1)
			{
				if ((*(data+k-1)!=v13[1])&&(k<size))
				{
					*(data+k)=v13[1];
					k++;
				}
				if ((*(data+k-1)!=3)&&(k<size))
				{
					*(data+k)=3;
					k++;
				}
				v13[0]=3;
				v13[1]=0;
			}
		}
		if (*(temp+i)==4)
		{
			v13[1]=*(temp+i);
			v13[0]=*(temp+i-1);
			if (v24[0]==2)
			{
				if ((*(data+k-1)!=v24[1])&&(k<size))
				{
					*(data+k)=v24[1];
					k++;
				}
				if ((*(data+k-1)!=4)&&(k<size))
				{
					*(data+k)=4;
					k++;
				}
				v24[0]=4;
				v24[1]=0;
			}
		}
	}
	if ((v13[1]!=0)&&(v24[1]==0))
	{
		if ((*(data+k-1)!=v13[1])&&(k<size))
		{
			*(data+k)=v13[1];
			k++;
		}
	}
	if ((v24[1]!=0)&&(v13[1]==0))
	{
		if ((*(data+k-1)!=v24[1])&&(k<size))
		{
			*(data+k)=v24[1];
			k++;
		}
	}
	if ((v13[1]!=0)&&(v24[1]!=0))
	{
		if ((*(data+k-1)!=*(temp+i-1))&&(k<size)&&((i-1)<size))
		{
			*(data+k)=*(temp+i-1);
			k++;
		}
	}
	free(p);
	return;
}

int RadialOfMinDistance(int * data_g,int * data_s,int * data_c,int * area)     //�õ��������ж�ʱ�ĵ����ӱʻ��͸��ӱʻ���������ʻ���������Ĳ���
{
	int n,pr=0;                                       //�ȽϽ��
	void * p;
	int * pnum;                                       //��űȽ�˳��
	if ((p=malloc(sizeof(int)*(LETTER_SACOUNT+LETTER_FDCOUNT)))!=0)
		pnum=(int *)p;
	else
		return -1; 
	int sl = 0;
	for (sl=0;(sl<CODE_A)&&(*(data_c+sl)!=0);sl++)
	{}
	if (*(area+0)==0)
	{
		SequenceDistance_R(data_g,data_s,data_c,pnum,2,0);
		n=0;
		pr=*(pnum+n)+LETTER_SACOUNT;
		n++;
		while (1)
		{
			if (*(area+pr)!=0)
			{
				if (((pr==7)||(pr==8))&&(sl>4))
				{
					pr=*(pnum+n)+LETTER_SACOUNT;
					n++;
					continue;
				}
				else
					break;
			}
			else
			{
				pr=*(pnum+n)+LETTER_SACOUNT;
				n++;
				continue;
			}
		}
	}
	else
	{
		SequenceDistance_R(data_g,data_s,data_c,pnum,3,0);
		n=0;
		pr=*(pnum+n);
		n++;
		while (1)
		{
			if (*(area+pr)!=0)
			{
				if (((pr==7)||(pr==8))&&(sl>4))
				{
					pr=*(pnum+n)+LETTER_SACOUNT;
					n++;
					continue;
				}
				else
					break;
			}
			else
			{
				pr=*(pnum+n);
				n++;
				continue;
			}
		}
	}
	return pr; 
}

void SequenceDistance_R(int * data_g,int * data_s,int * data_c,int * result,int sign,int term)                 //ʶ��ʱ�ľ�������,�������58άʸ����
{
	int i;
	double degree1,degree2;
	void * p;
	int * temp;
	if ((p=malloc(sizeof(int)*LETTER_MCOUNT))!=0)
		temp=(int *)p;
	else
		return;
	void * p1;
	int * temp1;
	if ((p1=malloc(sizeof(int)*LETTER_MCOUNT))!=0)
		temp1=(int *)p1;
	else
		return;
	void * p2;
	int * temp2;
	if ((p2=malloc(sizeof(int)*LETTER_MCOUNT))!=0)
		temp2=(int *)p2;
	else
		return;
	switch (sign)                  
	{
	case 0:                   //��������ʻ��ľ���
		if (term==0)
		{
			for (i=0;i<LETTER_MCOUNT;i++)
				*(result+i)=Distance(data_g,RGm[i],GRID_M_X*GRID_M_Y*4);
		}
		if (term==1)
		{
			for (i=0;i<LETTER_MCOUNT;i++)
			{
				*(temp1+i)=Distance(data_s,RSm[i],STREAM_M*4);
				*(temp+i)=*(temp1+i);
			}
			Sequence(temp,LETTER_MCOUNT);	
			degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			for (i=0;i<LETTER_MCOUNT;i++)
			{
				*(temp2+i)=Distance(data_s,RSm[i],STREAM_M*4)+DistanceCode(data_c,RCm[i],CODE_M)*1000;
				*(temp+i)=*(temp2+i);
			}
			Sequence(temp,LETTER_MCOUNT);	
			degree2=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			if (degree1>=degree2)
			{
				for (i=0;i<LETTER_MCOUNT;i++)
					*(result+i)=*(temp1+i);
			}
			else
			{
				for (i=0;i<LETTER_MCOUNT;i++)
					*(result+i)=*(temp2+i);
			}
		}
		break;
	case 1:                //�������ӱʻ��ľ���
		if (term==0)
		{
			for (i=0;i<LETTER_SACOUNT;i++)
				*(result+i)=Distance(data_g,RGa[i],GRID_A_X*GRID_A_Y*4);
		}
		if (term==1)
		{
			for (i=0;i<LETTER_SACOUNT;i++)
			{
				*(temp1+i)=Distance(data_s,RSa[i],STREAM_A*4);
				*(temp+i)=*(temp1+i);
			}
			Sequence(temp,LETTER_SACOUNT);	
			degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			for (i=0;i<LETTER_SACOUNT;i++)
			{
				*(temp2+i)=Distance(data_s,RSa[i],STREAM_A*4)+DistanceCode(data_c,RCa[i],CODE_A)*500;
				*(temp+i)=*(temp2+i);
			}
			Sequence(temp,LETTER_SACOUNT);	
			degree2=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			if (degree1>=degree2)
			{
				for (i=0;i<LETTER_SACOUNT;i++)
					*(result+i)=*(temp1+i);
			}
			else
			{
				for (i=0;i<LETTER_SACOUNT;i++)
					*(result+i)=*(temp2+i);
			}
		}
		break;
	case 2:                              //�����ʱʻ�����
		if (term==0)
		{
			for (i=0;i<LETTER_FDCOUNT;i++)
				*(temp+i)=Distance(data_g,RGd[i],GRID_A_X*GRID_A_Y*4);
			Sequence(temp,result,LETTER_FDCOUNT);
		}
		if (term==1)
		{
			for (i=0;i<LETTER_FDCOUNT;i++)
			{
				*(temp1+i)=Distance(data_s,RSd[i],STREAM_A*4);
				*(temp+i)=*(temp1+i);
			}
			Sequence(temp,LETTER_FDCOUNT);	
			degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			for (i=0;i<LETTER_FDCOUNT;i++)
			{
				*(temp2+i)=Distance(data_s,RSd[i],STREAM_A*4)+DistanceCode(data_c,RCd[i],CODE_A)*500;
				*(temp+i)=*(temp2+i);
			}
			Sequence(temp,LETTER_FDCOUNT);	
			degree2=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			if (degree1>=degree2)
				Sequence(temp1,result,LETTER_FDCOUNT);
			else
				Sequence(temp2,result,LETTER_FDCOUNT);
		}
		break;
	case 3:             //���ӱʻ��������ʱʻ�����
		if (term==0)
		{
			for (i=0;i<LETTER_SACOUNT;i++)
				*(temp+i)=Distance(data_g,RGa[i],GRID_A_X*GRID_A_Y*4);
			for (i=0;i<LETTER_FDCOUNT;i++)
				*(temp+i+LETTER_SACOUNT)=Distance(data_g,RGd[i],GRID_A_X*GRID_A_Y*4);
			Sequence(temp,result,LETTER_SACOUNT+LETTER_FDCOUNT);
		}
		if (term==1)
		{
			for (i=0;i<LETTER_SACOUNT;i++)
			{
				*(temp1+i)=Distance(data_s,RSa[i],STREAM_A*4);
				*(temp+i)=*(temp1+i);
			}
			for (i=0;i<LETTER_FDCOUNT;i++)
			{
				*(temp1+i+LETTER_SACOUNT)=Distance(data_s,RSd[i],STREAM_A*4);
				*(temp+i+LETTER_SACOUNT)=*(temp1+i+LETTER_SACOUNT);
			}
			Sequence(temp,LETTER_SACOUNT+LETTER_FDCOUNT);
			degree1=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			for (i=0;i<LETTER_SACOUNT;i++)
			{
				*(temp2+i)=Distance(data_s,RSa[i],STREAM_A*4)+DistanceCode(data_c,RCa[i],CODE_A)*500;
				*(temp+i)=*(temp2+i);
			}
			for (i=0;i<LETTER_FDCOUNT;i++)
			{
				*(temp2+i+LETTER_SACOUNT)=Distance(data_s,RSd[i],STREAM_A*4)+DistanceCode(data_c,RCd[i],CODE_A)*500;
				*(temp+i+LETTER_SACOUNT)=*(temp2+i+LETTER_SACOUNT);
			}
			Sequence(temp,LETTER_SACOUNT+LETTER_FDCOUNT);	
			degree2=double(*(temp+1)-*(temp+0))/double(*(temp+0));
			if (degree1>=degree2)
				Sequence(temp1,result,LETTER_SACOUNT+LETTER_FDCOUNT);
			else
				Sequence(temp2,result,LETTER_SACOUNT+LETTER_FDCOUNT);
		}
		break;
	}
	free(p);
	free(p1);
	free(p2);
	return;
}

int Distance(int * data1,int * data2,int size)                     //�������
{
	int i;
	int result=0;
	for (i=0;i<size;i++)
		result+=int(pow(*(data1+i)-*(data2+i),2));
	return result;
}

int DistanceCode(int * data1,int * data2,int size)                 //����������
{
	int i,result=0;
	void * p1;
	int * temp1;
	if ((p1=malloc(sizeof(int)*size))!=0)
		temp1=(int *)p1;
	void * p2;
	int * temp2;
	if ((p2=malloc(sizeof(int)*size))!=0)
		temp2=(int *)p2;
	for (i=0;i<size;i++)
	{	
		*(temp1+i)=*(data1+i);
		*(temp2+i)=*(data2+i);
	}
	ShiftCode(temp1,temp2,size);
	for (i=0;i<size;i++)
	{	
		if ((*(temp1+i))!=(*(temp2+i)))
			result++;
	}
	free(p1);
	free(p2);
	return result;
}

int DistanceDot(int num,int pos,int rel,int dotf,int de)           //��������������
{
	int dis=0;
	if ((num==0)&&(pos==-1)&&(rel==-1))
	{
		if (dotf==0)
			dis=0;
		else
			dis=de;
	}
	else if ((num==1)&&(pos==1)&&(rel==-1))
	{
		if (dotf==1)
			dis=0;
		else if ((dotf==2)||(dotf==3))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==2)&&(pos==1)&&(rel==-1))
	{
		if (dotf==2)
			dis=0;
		else if ((dotf==1)||(dotf==3))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==3)&&(pos==1)&&(rel==-1))
	{
		if (dotf==3)
			dis=0;
		else if ((dotf==1)||(dotf==2))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==1)&&(pos==0)&&(rel==-1))
	{
		if (dotf==4)
			dis=0;
		else if ((dotf==5)||(dotf==6)||(dotf==7))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==2)&&(pos==0)&&(rel==0))
	{
		if (dotf==5)
			dis=0;
		else if ((dotf==4)||(dotf==6)||(dotf==7))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==2)&&(pos==0)&&(rel==1))
	{
		if (dotf==6)
			dis=0;
		else if ((dotf==4)||(dotf==5)||(dotf==7))
			dis=de/2;
		else
			dis=de;
	}
	else if ((num==3)&&(pos==0)&&(rel==-1))
	{
		if (dotf==7)
			dis=0;
		else if ((dotf==4)||(dotf==5)||(dotf==6))
			dis=de/2;
		else
			dis=de;
	}
	else
		dis=de;
	return dis;
}

void Sequence(int * data,int size)                   //����           
{
	int temp;
	for (int i=0;i<size-1;i++)
		for (int j=0;j<size-1-i;j++)
			if (*(data+j)>*(data+j+1))
			{
				temp=*(data+j);
				*(data+j)=*(data+j+1);
				*(data+j+1)=temp;
			}
}

void Sequence(int * data,int * num,int size)
{
	int i,j,t;
	int temp;
	for (i=0;i<size;i++)
		*(num+i)=i;
	for (i=0;i<size-1;i++)
	{
		for (j=0;j<size-1-i;j++)
			if (*(data+j)>*(data+j+1))
			{
				temp=*(data+j);
				*(data+j)=*(data+j+1);
				*(data+j+1)=temp;
				t=*(num+j);
				*(num+j)=*(num+j+1);
				*(num+j+1)=t;
			}
	}
}

void AntiSequence(double * data,int * num,int size)         //������,�Դ���С
{
	int i,j,t;
	double temp;
	for (i=0;i<size;i++)
		*(num+i)=i;
	for (i=0;i<size-1;i++)
		for (j=0;j<size-1-i;j++)
			if (*(data+j)<*(data+j+1))
			{
				temp=*(data+j);
				*(data+j)=*(data+j+1);
				*(data+j+1)=temp;
				t=*(num+j);
				*(num+j)=*(num+j+1);
				*(num+j+1)=t;
			}
}
void ShiftCode(int * data1,int * data2, int size)                  //������λƥ��
{
	int i,j;
	void * p1;
	int * temp1;
	if ((p1=malloc(sizeof(int)*size))!=0)
		temp1=(int *)p1;
	void * p2;
	int * temp2;
	if ((p2=malloc(sizeof(int)*size))!=0)
		temp2=(int *)p2;
	for (i=0;i<size;i++)
	{
		*(temp1+i)=*(data1+i);
		*(temp2+i)=*(data2+i);
	}
	int max=0;
	int t=0;
	int s=0;       //��ʾ���ĸ����ݴ���λ��
	int n=0;       //��ʾ�ƶ���λ��
	for (i=0;i<size/2;i++)
	{
		for (j=0;j<size-i;j++)
		{
			if((*(temp1+j)==*(temp2+j+i))&&(*(temp1+j)!=0))
			{
				t++;
				if (j==(size-i-1))
				{
					if (t>max)
					{
						max=t;
						s=1;
						n=i;
					}
					t=0;
				}
			}
			else
			{	
				if (t>max)
				{
					max=t;
					s=1;
					n=i;
				}
				t=0;
			}		
		}
	}
	for (i=0;i<size/2;i++)
	{
		for (j=0;j<size-i;j++)
		{
			if((*(temp2+j)==*(temp1+j+i))&&(*(temp2+j)!=0))
			{
				t++;
				if (j==(size-i-1))
				{
					if (t>max)
					{
						max=t;
						s=2;
						n=i;
					}
					t=0;
				}
			}
			else
			{
				if (t>max)
				{
					max=t;
					s=2;
					n=i;
				}
				t=0;
			}
		}
	}
	if (s==1)
	{
		for (i=0;i<n;i++)
			*(data1+i)=*(temp1+0);
		for (i=n;i<size;i++)
			*(data1+i)=*(temp1+i-n);
	}
	else if (s==2)
	{
		for (i=0;i<n;i++)
			*(data2+i)=*(temp2+0);
		for (i=n;i<size;i++)
			*(data2+i)=*(temp2+i-n);
	}
	free(p1);
	free(p2);
}

void GetStroke(Stroke_R pstroke[],Point data[][MAX_LENGTH])        //�ɵ�洢���ʻ��ṹ
{
	int i,j,k,k1,l;
	for (i=0;i<MAX_STROKE;i++)
	{
		pstroke[i].sign=Void;	
		for (j=0;j<MAX_LENGTH;j++)
		{
			pstroke[i].ms[j]=0;
			pstroke[i].fxm[j].pos.x=-1;
			pstroke[i].fxm[j].pos.y=-1;
			pstroke[i].fxm[j].fx=0;
			pstroke[i].fxm[j].size=0;
		}
		pstroke[i].length=0;
		pstroke[i].start.x=-1;
		pstroke[i].start.y=-1;
		pstroke[i].end.x=-1;
		pstroke[i].end.y=-1;
		pstroke[i].lu.x=-1;
		pstroke[i].lu.y=-1;
		pstroke[i].rd.x=-1;
		pstroke[i].rd.y=-1;
		pstroke[i].baseline=0;
	} 
	int x_start,x_end,y_start,y_end;
	int line[MAX_LENGTH]; 
	int n=0,bn=0,have=0,now=0;
	for(i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (i==0)
		{
			for (j=0;j<MAX_LENGTH;j++)
				line[j]=0;
		}
		bn=0;	
		pstroke[i].start.x=data[i][0].x;                 //�õ�ÿ�ʵ����
		pstroke[i].start.y=data[i][0].y;
		pstroke[i].end.x=data[i][0].x;
		pstroke[i].end.y=data[i][0].y;
		x_start=data[i][0].x;	
		x_end=data[i][0].x;
		y_start=data[i][0].y;	
		y_end=data[i][0].y;
		k=0;
		k1=0;
		now=0;
		for(j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{                                                //�õ�ÿ�ʵ��յ�
			pstroke[i].end.x=data[i][j].x;
			pstroke[i].end.y=data[i][j].y;
			if (data[i][j].x<x_start)                    //�õ�ÿ�ʵķ�Χ
				x_start=data[i][j].x;
			if (data[i][j].x>x_end)
				x_end=data[i][j].x;
			if (data[i][j].y<y_start)
				y_start=data[i][j].y;
			if (data[i][j].y>y_end)
				y_end=data[i][j].y;                      //�õ�ÿ�ʵķ�����
			n=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
			if (n>0)
			{
				if (abs(data[i][j].y-data[i][j-1].y)>=abs(data[i][j].x-data[i][j-1].x))
				{
					if ((data[i][j].y-data[i][j-1].y)>=0)
					{
						if ((now==3)&&(data[i][j+1].x!=-1)&&(j>=3))
						{
							if (data[i][j+1].x-5<data[i][j-3].x)
							{
								pstroke[i].ms[k]=2;
								k++;
							}
							else 
							{
								pstroke[i].ms[k]=4;
								k++;
							}
						}
						now=1;
						for (l=0;l<n;l++)                    //��ֵ
						{
							pstroke[i].ms[k]=1;
							k++;
							bn=0;	
						}
						pstroke[i].fxm[k1].pos=data[i][j-1];
						pstroke[i].fxm[k1].fx=1;
						pstroke[i].fxm[k1].size=n;
						k1++;
					}
					else
					{
						if ((now==1)&&(data[i][j+1].x!=-1)&&(j>=3))
						{
							if (data[i][j+1].x-5<data[i][j-3].x)
							{
								pstroke[i].ms[k]=2;
								k++;
							}
							else 
							{
								pstroke[i].ms[k]=4;
								k++;
							}
						}
						now=3;	
						for (l=0;l<n;l++)
						{
							pstroke[i].ms[k]=3;
							k++;
							bn=0;
						}
						pstroke[i].fxm[k1].pos=data[i][j-1];
						pstroke[i].fxm[k1].fx=3;
						pstroke[i].fxm[k1].size=n;
						k1++;
					}
				}
				else
				{
					if ((data[i][j].x-data[i][j-1].x)<=0)
					{
						if ((now==4)&&(data[i][j+1].x!=-1)&&(j>=3))
						{
							if (data[i][j+1].y+5<data[i][j-3].y)
							{
								pstroke[i].ms[k]=3;
								k++;
							}
							else 
							{
								pstroke[i].ms[k]=1;
								k++;
							}
						}
						now=2;	
						for (l=0;l<n;l++)
						{
							pstroke[i].ms[k]=2;
							k++;
							if ((i==0)&&(!have))
							{
								line[bn]=data[i][j].y;
								bn++;
							}
							if (bn>(DENSITY_X/6))
								have=1;
						}
						pstroke[i].fxm[k1].pos=data[i][j-1];
						pstroke[i].fxm[k1].fx=2;
						pstroke[i].fxm[k1].size=n;
						k1++;
					}
					else
					{
						if ((now==2)&&(data[i][j+1].x!=-1)&&(j>=3))
						{
							if (data[i][j+1].y+5<data[i][j-3].y)
							{
								pstroke[i].ms[k]=3;
								k++;
							}
							else 
							{
								pstroke[i].ms[k]=1;
								k++;
							}
						}
						now=4;	
						for (l=0;l<n;l++)
						{
							pstroke[i].ms[k]=4;
							k++;
							if ((i==0)&&(!have))
							{
								line[bn]=data[i][j].y;
								bn++;
							}
							if (bn>(DENSITY_X/6))
								have=1;
						}
						pstroke[i].fxm[k1].pos=data[i][j-1];
						pstroke[i].fxm[k1].fx=4;
						pstroke[i].fxm[k1].size=n;
						k1++;
					}
				}
			}
		}
		pstroke[i].length=k;                            //��¼ÿ�ʵ���(��ֵ��)
		pstroke[i].lu.x=x_start;           
		pstroke[i].lu.y=y_start;
		pstroke[i].rd.x=x_end;
		pstroke[i].rd.y=y_end;
		if ((pstroke[i].length>MIN_LENGTH)&&(pstroke[i].length<=DOT_LENGTH))         //�ж�ÿ�ʵıʻ���־�ͻ���
		{
			pstroke[i].sign=Dot;
			pstroke[i].baseline=pstroke[i].start.y;
		}
		else if ((pstroke[i].length>DOT_LENGTH)&&(pstroke[i].length<=MAX_LENGTH)) 
		{
			pstroke[i].sign=Affix;
			if (((pstroke[i].rd.x-pstroke[i].lu.x)<DOT_LENGTH*2/3)&&((pstroke[i].rd.y-pstroke[i].lu.y)<DOT_LENGTH*2/3))
				pstroke[i].sign=Dot;                    //���Ϳ�̫С�ĵĲ��Ǹ��ӱʻ�,������ʻ�
			pstroke[i].baseline=y_start;
		}
	}		
	pstroke[0].sign=Main;
	if (have)
	{
		for(j=0;((j<MAX_LENGTH)&&(line[j]!=0));j++)
			pstroke[0].baseline+=line[j];
		pstroke[0].baseline/=j;
	}
	else
		pstroke[0].baseline=pstroke[0].start.y;
}

void GetStroke(Stroke_C pstroke[],Point data[][MAX_LENGTH])             //�ɵ�洢���ʻ��ṹ
{
	int i,j,k;
	for (i=0;i<MAX_STROKE;i++)
	{
		pstroke[i].stroke=0;
		pstroke[i].section=0;
		pstroke[i].domain.width_x.start=DENSITY_X-1;
		pstroke[i].domain.width_x.end=0;
		pstroke[i].domain.width_y.start=DENSITY_Y-1;
		pstroke[i].domain.width_y.end=0;
		pstroke[i].length=0;
		pstroke[i].ismain=0;
		for (j=0;j<MAX_CUT*2;j++)
		{
			pstroke[i].cutsect[j].start=-1;
			pstroke[i].cutsect[j].end=-1;
		}
	}
	Baseline line;
	line=GetBaseline(data);
	bool pixel[DENSITY_X][DENSITY_Y];
	Point data_selected[MAX_STROKE][MAX_LENGTH];
	int project[DENSITY_Y];	
	Initialize(data_selected);
	for(i=0;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)                     //�õ�ÿ�ʵķ�Χ�ͻ���
	{
		pstroke[i].stroke=i+1;
		for(j=0;j<MAX_LENGTH;j++)
			data_selected[0][j]=data[i][j];
		FromDataToPixel(pixel,data_selected);
		for (k=0;k<DENSITY_Y;k++)
		{
			project[k]=0;
			for (j=0;j<DENSITY_X;j++)
			{
				if (pixel[j][k]==1)
					project[k]++;
			}
		}
		pstroke[i].baseline=ComputeBaseline(project);
		pstroke[i].domain.width_x.start=data[i][0].x;	
		pstroke[i].domain.width_x.end=data[i][0].x;
		pstroke[i].domain.width_y.start=data[i][0].y;	
		pstroke[i].domain.width_y.end=data[i][0].y;
		pstroke[i].length=0;
		for(j=1;(j<MAX_LENGTH)&&(data[i][j].x!=-1);j++)
		{	
			if (data[i][j].x<pstroke[i].domain.width_x.start)                    
				pstroke[i].domain.width_x.start=data[i][j].x;
			if (data[i][j].x>pstroke[i].domain.width_x.end)
				pstroke[i].domain.width_x.end=data[i][j].x;
			if (data[i][j].y<pstroke[i].domain.width_y.start)
				pstroke[i].domain.width_y.start=data[i][j].y;
			if (data[i][j].y>pstroke[i].domain.width_y.end)
				pstroke[i].domain.width_y.end=data[i][j].y;
			pstroke[i].length+=(int)sqrt(pow(data[i][j].y-data[i][j-1].y,2)+pow(data[i][j].x-data[i][j-1].x,2));
		}
	}
	Domain domain;                   //�ҳ��������ʵĿ�Ⱥ͸߶�
	domain.width_x.start=pstroke[0].domain.width_x.start;
	domain.width_x.end=pstroke[0].domain.width_x.end;
	domain.width_y.start=pstroke[0].domain.width_y.start;
	domain.width_y.end=pstroke[0].domain.width_y.end;
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)
	{
		if (pstroke[i].domain.width_x.start<domain.width_x.start)
			domain.width_x.start=pstroke[i].domain.width_x.start;
		if (pstroke[i].domain.width_x.end>domain.width_x.end)
			domain.width_x.end=pstroke[i].domain.width_x.end;
		if (pstroke[i].domain.width_y.start<domain.width_y.start)
			domain.width_y.start=pstroke[i].domain.width_y.start;
		if (pstroke[i].domain.width_y.end>domain.width_y.end)
			domain.width_y.end=pstroke[i].domain.width_y.end;
	}
	pstroke[0].ismain=1;                                    //�ҳ�����ʻ�
	int refer=0;                                            //referʼ��Ϊ����ʻ�
	Width width,width_refer,width_temp;
	width_refer=pstroke[0].domain.width_x;
	int temp;
	for (i=1;(i<MAX_STROKE)&&(data[i][0].x!=-1);i++)              
	{
		if (((pstroke[i].domain.width_x.end-pstroke[i].domain.width_x.start)>(domain.width_x.end-domain.width_x.start)/10)
			||((pstroke[i].domain.width_y.end-pstroke[i].domain.width_y.start)>(domain.width_y.end-domain.width_y.start)/3)
			||(pstroke[i].length>DOT_LENGTH*2/3))
		{	                                                //��Ⱥͳ��ȶ���С�ҵ�����С�ıʻ���������ʻ�
			width=pstroke[i].domain.width_x; 
			temp=WidthRelation(width_refer,width);
			switch (temp)  //�Ը����ʻ������ĺ���λ�ù�ϵ����,�ж�����ʻ�
			{              //����ʻ�refer��i��λ��,
			case 0:        //0,����
				width_temp=IsThrough(data[refer],line.centre);
				if ((width_temp.start==-1)||(IsLine(data[i])!=2))
				{
					pstroke[i].ismain=1;
					width_refer=width;
					refer=i;
				}
				break;
			case 1:        //1,refer����i
				width_temp=IsThrough(data[refer],line.centre);
				if ((width_temp.start!=-1)&&(WidthRelation(width_temp,width)==1))
				{
					if ((pstroke[i].baseline.down>line.up)&&(IsLine(data[i])!=2))
						pstroke[i].ismain=1;
				}		
				break;
			case 2:        //2,i����refer
				pstroke[i].ismain=1;
				pstroke[refer].ismain=0;
				width_temp=IsThrough(data[refer],line.centre);
				if ((width_temp.start!=-1)&&(pstroke[refer].baseline.down>line.up))                                
					pstroke[refer].ismain=1;
				width_refer=width;
				refer=i;
				break;
			case 3:        //3,i��refer����ཻ
				width_temp=IsThrough(data[refer],line.centre);
				if (((width_temp.start==-1)||(IsLine(data[i])!=2))&&(pstroke[i].baseline.down>line.up))
				{
					pstroke[i].ismain=1;
					refer=i;
				}
				width_refer.start=width.start;
				break;
			case 4:        //4,i��refer�Ҳ��ཻ
				width_refer.end=width.end;
				break;
			}
		}
	}
	return;
}

void LetterRadials()                                //128��ά���ַ��Ĳ����ֵ�
{
	//���岿��:1-58,��������:1-8,�㲿��:1-7,0:�ղ���
	int i=0;
	LetterList[i][0]=54;LetterList[i][1]=1;LetterList[i][2]=0;
	i=1;
	LetterList[i][0]=15;LetterList[i][1]=0;LetterList[i][2]=7;
	i=2;
	LetterList[i][0]=16;LetterList[i][1]=0;LetterList[i][2]=7;
	i=3;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=7;
	i=4;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=7;
	i=5;
	LetterList[i][0]=17;LetterList[i][1]=0;LetterList[i][2]=7;
	i=6;
	LetterList[i][0]=18;LetterList[i][1]=0;LetterList[i][2]=7;
	i=7;
	LetterList[i][0]=19;LetterList[i][1]=0;LetterList[i][2]=7;
	i=8;
	LetterList[i][0]=20;LetterList[i][1]=0;LetterList[i][2]=7;
	i=9;
	LetterList[i][0]=9;LetterList[i][1]=0;LetterList[i][2]=3;
	i=10;
	LetterList[i][0]=10;LetterList[i][1]=0;LetterList[i][2]=3;
	i=11;
	LetterList[i][0]=35;LetterList[i][1]=5;LetterList[i][2]=0;
	i=12;
	LetterList[i][0]=36;LetterList[i][1]=5;LetterList[i][2]=0;
	i=13;
	LetterList[i][0]=33;LetterList[i][1]=5;LetterList[i][2]=0;
	i=14;
	LetterList[i][0]=34;LetterList[i][1]=5;LetterList[i][2]=0;
	i=15;
	LetterList[i][0]=55;LetterList[i][1]=0;LetterList[i][2]=0;
	i=16;
	LetterList[i][0]=56;LetterList[i][1]=0;LetterList[i][2]=0;
	i=17;
	LetterList[i][0]=50;LetterList[i][1]=1;LetterList[i][2]=6;
	i=18;
	LetterList[i][0]=50;LetterList[i][1]=1;LetterList[i][2]=0;
	i=19;
	LetterList[i][0]=13;LetterList[i][1]=1;LetterList[i][2]=3;
	i=20;
	LetterList[i][0]=14;LetterList[i][1]=1;LetterList[i][2]=3;
	i=21;
	LetterList[i][0]=33;LetterList[i][1]=0;LetterList[i][2]=3;
	i=22;
	LetterList[i][0]=34;LetterList[i][1]=0;LetterList[i][2]=3;
	i=23;
	LetterList[i][0]=41;LetterList[i][1]=3;LetterList[i][2]=0;
	i=24;
	LetterList[i][0]=42;LetterList[i][1]=3;LetterList[i][2]=0;
	i=25;
	LetterList[i][0]=41;LetterList[i][1]=2;LetterList[i][2]=0;
	i=26;
	LetterList[i][0]=42;LetterList[i][1]=2;LetterList[i][2]=0;
	i=27;
	LetterList[i][0]=41;LetterList[i][1]=4;LetterList[i][2]=0;
	i=28;
	LetterList[i][0]=42;LetterList[i][1]=4;LetterList[i][2]=0;
	i=29;
	LetterList[i][0]=41;LetterList[i][1]=0;LetterList[i][2]=3;
	i=30;
	LetterList[i][0]=42;LetterList[i][1]=0;LetterList[i][2]=3;
	i=31;
	LetterList[i][0]=51;LetterList[i][1]=0;LetterList[i][2]=6;
	i=32;
	LetterList[i][0]=52;LetterList[i][1]=0;LetterList[i][2]=6;
	i=33;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=6;
	i=34;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=6;
	i=35;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=0;
	i=36;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=0;
	i=37;
	LetterList[i][0]=3;LetterList[i][1]=1;LetterList[i][2]=0;
	i=38;
	LetterList[i][0]=4;LetterList[i][1]=1;LetterList[i][2]=0;
	i=39;
	LetterList[i][0]=21;LetterList[i][1]=1;LetterList[i][2]=0;
	i=40;
	LetterList[i][0]=22;LetterList[i][1]=1;LetterList[i][2]=0;
	i=41;
	LetterList[i][0]=47;LetterList[i][1]=1;LetterList[i][2]=0;
	i=42;
	LetterList[i][0]=48;LetterList[i][1]=1;LetterList[i][2]=0;
	i=43;
	LetterList[i][0]=47;LetterList[i][1]=7;LetterList[i][2]=0;
	i=44;
	LetterList[i][0]=48;LetterList[i][1]=7;LetterList[i][2]=0;
	i=45;
	LetterList[i][0]=47;LetterList[i][1]=6;LetterList[i][2]=0;
	i=46;
	LetterList[i][0]=48;LetterList[i][1]=6;LetterList[i][2]=0;
	i=47;
	LetterList[i][0]=47;LetterList[i][1]=8;LetterList[i][2]=0;
	i=48;
	LetterList[i][0]=48;LetterList[i][1]=8;LetterList[i][2]=0;
	i=49;
	LetterList[i][0]=53;LetterList[i][1]=1;LetterList[i][2]=6;
	i=50;
	LetterList[i][0]=54;LetterList[i][1]=1;LetterList[i][2]=6;
	i=51;
	LetterList[i][0]=49;LetterList[i][1]=1;LetterList[i][2]=6;
	i=52;
	LetterList[i][0]=53;LetterList[i][1]=1;LetterList[i][2]=0;
	i=53;
	LetterList[i][0]=49;LetterList[i][1]=1;LetterList[i][2]=0;
	i=54;
	LetterList[i][0]=5;LetterList[i][1]=1;LetterList[i][2]=0;
	i=55;
	LetterList[i][0]=6;LetterList[i][1]=1;LetterList[i][2]=0;
	i=56;
	LetterList[i][0]=1;LetterList[i][1]=0;LetterList[i][2]=0;
	i=57;
	LetterList[i][0]=2;LetterList[i][1]=0;LetterList[i][2]=0;
	i=58;
	LetterList[i][0]=15;LetterList[i][1]=0;LetterList[i][2]=4;
	i=59;
	LetterList[i][0]=16;LetterList[i][1]=0;LetterList[i][2]=4;
	i=60;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=4;
	i=61;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=4;
	i=62;
	LetterList[i][0]=15;LetterList[i][1]=0;LetterList[i][2]=2;
	i=63;
	LetterList[i][0]=16;LetterList[i][1]=0;LetterList[i][2]=2;
	i=64;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=2;
	i=65;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=2;
	i=66;
	LetterList[i][0]=17;LetterList[i][1]=0;LetterList[i][2]=4;
	i=67;
	LetterList[i][0]=18;LetterList[i][1]=0;LetterList[i][2]=4;
	i=68;
	LetterList[i][0]=19;LetterList[i][1]=0;LetterList[i][2]=4;
	i=69;
	LetterList[i][0]=20;LetterList[i][1]=0;LetterList[i][2]=4;
	i=70;
	LetterList[i][0]=17;LetterList[i][1]=0;LetterList[i][2]=1;
	i=71;
	LetterList[i][0]=18;LetterList[i][1]=0;LetterList[i][2]=1;
	i=72;
	LetterList[i][0]=19;LetterList[i][1]=0;LetterList[i][2]=1;
	i=73;
	LetterList[i][0]=20;LetterList[i][1]=0;LetterList[i][2]=1;
	i=74;
	LetterList[i][0]=7;LetterList[i][1]=0;LetterList[i][2]=0;
	i=75;
	LetterList[i][0]=8;LetterList[i][1]=0;LetterList[i][2]=0;
	i=76;
	LetterList[i][0]=9;LetterList[i][1]=0;LetterList[i][2]=0;
	i=77;
	LetterList[i][0]=10;LetterList[i][1]=0;LetterList[i][2]=0;
	i=78;
	LetterList[i][0]=9;LetterList[i][1]=0;LetterList[i][2]=1;
	i=79;
	LetterList[i][0]=10;LetterList[i][1]=0;LetterList[i][2]=1;
	i=80;
	LetterList[i][0]=27;LetterList[i][1]=0;LetterList[i][2]=0;
	i=81;
	LetterList[i][0]=28;LetterList[i][1]=0;LetterList[i][2]=0;
	i=82;
	LetterList[i][0]=25;LetterList[i][1]=0;LetterList[i][2]=0;
	i=83;
	LetterList[i][0]=26;LetterList[i][1]=0;LetterList[i][2]=0;
	i=84;
	LetterList[i][0]=27;LetterList[i][1]=0;LetterList[i][2]=3;
	i=85;
	LetterList[i][0]=28;LetterList[i][1]=0;LetterList[i][2]=3;
	i=86;
	LetterList[i][0]=25;LetterList[i][1]=0;LetterList[i][2]=3;
	i=87;
	LetterList[i][0]=26;LetterList[i][1]=0;LetterList[i][2]=3;
	i=88;
	LetterList[i][0]=31;LetterList[i][1]=0;LetterList[i][2]=1;
	i=89;
	LetterList[i][0]=32;LetterList[i][1]=0;LetterList[i][2]=1;
	i=90;
	LetterList[i][0]=29;LetterList[i][1]=0;LetterList[i][2]=1;
	i=91;
	LetterList[i][0]=30;LetterList[i][1]=0;LetterList[i][2]=1;
	i=92;
	LetterList[i][0]=39;LetterList[i][1]=0;LetterList[i][2]=1;
	i=93;
	LetterList[i][0]=40;LetterList[i][1]=0;LetterList[i][2]=1;
	i=94;
	LetterList[i][0]=37;LetterList[i][1]=0;LetterList[i][2]=1;
	i=95;
	LetterList[i][0]=38;LetterList[i][1]=0;LetterList[i][2]=1;
	i=96;
	LetterList[i][0]=39;LetterList[i][1]=0;LetterList[i][2]=2;
	i=97;
	LetterList[i][0]=40;LetterList[i][1]=0;LetterList[i][2]=2;
	i=98;
	LetterList[i][0]=37;LetterList[i][1]=0;LetterList[i][2]=2;
	i=99;
	LetterList[i][0]=38;LetterList[i][1]=0;LetterList[i][2]=2;
	i=100;
	LetterList[i][0]=13;LetterList[i][1]=1;LetterList[i][2]=0;
	i=101;
	LetterList[i][0]=14;LetterList[i][1]=1;LetterList[i][2]=0;
	i=102;
	LetterList[i][0]=33;LetterList[i][1]=0;LetterList[i][2]=0;
	i=103;
	LetterList[i][0]=34;LetterList[i][1]=0;LetterList[i][2]=0;
	i=104;
	LetterList[i][0]=13;LetterList[i][1]=0;LetterList[i][2]=0;
	i=105;
	LetterList[i][0]=14;LetterList[i][1]=0;LetterList[i][2]=0;
	i=106;
	LetterList[i][0]=11;LetterList[i][1]=0;LetterList[i][2]=0;
	i=107;
	LetterList[i][0]=12;LetterList[i][1]=0;LetterList[i][2]=0;
	i=108;
	LetterList[i][0]=45;LetterList[i][1]=0;LetterList[i][2]=0;
	i=109;
	LetterList[i][0]=46;LetterList[i][1]=0;LetterList[i][2]=0;
	i=110;
	LetterList[i][0]=43;LetterList[i][1]=0;LetterList[i][2]=0;
	i=111;
	LetterList[i][0]=44;LetterList[i][1]=0;LetterList[i][2]=0;
	i=112;
	LetterList[i][0]=15;LetterList[i][1]=0;LetterList[i][2]=1;
	i=113;
	LetterList[i][0]=16;LetterList[i][1]=0;LetterList[i][2]=1;
	i=114;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=1;
	i=115;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=1;
	i=116;
	LetterList[i][0]=23;LetterList[i][1]=0;LetterList[i][2]=0;
	i=117;
	LetterList[i][0]=24;LetterList[i][1]=0;LetterList[i][2]=0;
	i=118;
	LetterList[i][0]=41;LetterList[i][1]=0;LetterList[i][2]=0;
	i=119;
	LetterList[i][0]=42;LetterList[i][1]=0;LetterList[i][2]=0;
	i=120;
	LetterList[i][0]=51;LetterList[i][1]=0;LetterList[i][2]=0;
	i=121;
	LetterList[i][0]=52;LetterList[i][1]=0;LetterList[i][2]=0;
	i=122;
	LetterList[i][0]=51;LetterList[i][1]=0;LetterList[i][2]=5;
	i=123;
	LetterList[i][0]=52;LetterList[i][1]=0;LetterList[i][2]=5;
	i=124;
	LetterList[i][0]=5;LetterList[i][1]=0;LetterList[i][2]=5;
	i=125;
	LetterList[i][0]=6;LetterList[i][1]=0;LetterList[i][2]=5;
	i=126;
	LetterList[i][0]=57;LetterList[i][1]=0;LetterList[i][2]=0;
	i=127;
	LetterList[i][0]=58;LetterList[i][1]=0;LetterList[i][2]=0;
	i=128;
	LetterList[i][0]=35;LetterList[i][1]=0;LetterList[i][2]=2;
	i=129;
	LetterList[i][0]=36;LetterList[i][1]=0;LetterList[i][2]=2;
	i=130;
	LetterList[i][0]=33;LetterList[i][1]=0;LetterList[i][2]=2;
	i=131;
	LetterList[i][0]=34;LetterList[i][1]=0;LetterList[i][2]=2;
	i=132;
	LetterList[i][0]=41;LetterList[i][1]=0;LetterList[i][2]=1;
	i=133;
	LetterList[i][0]=42;LetterList[i][1]=0;LetterList[i][2]=1;
	i=134;
	LetterList[i][0]=17;LetterList[i][1]=4;LetterList[i][2]=0;
	i=135;
	LetterList[i][0]=18;LetterList[i][1]=4;LetterList[i][2]=0;
	i=136;
	LetterList[i][0]=19;LetterList[i][1]=4;LetterList[i][2]=0;
	i=137;
	LetterList[i][0]=20;LetterList[i][1]=4;LetterList[i][2]=0;
	i=138;
	LetterList[i][0]=9;LetterList[i][1]=4;LetterList[i][2]=0;
	i=139;
	LetterList[i][0]=10;LetterList[i][1]=4;LetterList[i][2]=0;
	i=140;
	LetterList[i][0]=31;LetterList[i][1]=4;LetterList[i][2]=0;
	i=141;
	LetterList[i][0]=32;LetterList[i][1]=4;LetterList[i][2]=0;
	i=142;
	LetterList[i][0]=29;LetterList[i][1]=4;LetterList[i][2]=0;
	i=143;
	LetterList[i][0]=30;LetterList[i][1]=4;LetterList[i][2]=0;
	i=144;
	LetterList[i][0]=39;LetterList[i][1]=4;LetterList[i][2]=0;
	i=145;
	LetterList[i][0]=40;LetterList[i][1]=4;LetterList[i][2]=0;
	i=146;
	LetterList[i][0]=37;LetterList[i][1]=4;LetterList[i][2]=0;
	i=147;
	LetterList[i][0]=38;LetterList[i][1]=4;LetterList[i][2]=0;
	i=148;
	LetterList[i][0]=15;LetterList[i][1]=4;LetterList[i][2]=0;
	i=149;
	LetterList[i][0]=16;LetterList[i][1]=4;LetterList[i][2]=0;
	i=150;
	LetterList[i][0]=5;LetterList[i][1]=4;LetterList[i][2]=0;
	i=151;
	LetterList[i][0]=6;LetterList[i][1]=4;LetterList[i][2]=0;
}

void MainRadials()                             //ά�����岿��λ��ͼ                                            
{
	//1:�׶˲���;2:�м䲿��;3:β�˲���;4:��������
	int i=0;
	radialposition[i]=4;
	i=1;
	radialposition[i]=3;
	i=2;
	radialposition[i]=4;
	i=3;
	radialposition[i]=3;
	i=4;
	radialposition[i]=1;
	i=5;
	radialposition[i]=2;
	i=6;
	radialposition[i]=4;
	i=7;
	radialposition[i]=3;
	i=8;
	radialposition[i]=4;
	i=9;
	radialposition[i]=3;
	i=10;
	radialposition[i]=1;
	i=11;
	radialposition[i]=2;
	i=12;
	radialposition[i]=4;
	i=13;
	radialposition[i]=3;
	i=14;
	radialposition[i]=4;
	i=15;
	radialposition[i]=3;
	i=16;
	radialposition[i]=4;
	i=17;
	radialposition[i]=3;
	i=18;
	radialposition[i]=1;
	i=19;
	radialposition[i]=2;
	i=20;
	radialposition[i]=4;
	i=21;
	radialposition[i]=3;
	i=22;
	radialposition[i]=4;
	i=23;
	radialposition[i]=3;
	i=24;
	radialposition[i]=1;
	i=25;
	radialposition[i]=2;
	i=26;
	radialposition[i]=4;
	i=27;
	radialposition[i]=3;
	i=28;
	radialposition[i]=1;
	i=29;
	radialposition[i]=2;
	i=30;
	radialposition[i]=4;
	i=31;
	radialposition[i]=3;
	i=32;
	radialposition[i]=1;
	i=33;
	radialposition[i]=2;
	i=34;
	radialposition[i]=4;
	i=35;
	radialposition[i]=3;
	i=36;
	radialposition[i]=1;
	i=37;
	radialposition[i]=2;
	i=38;
	radialposition[i]=4;
	i=39;
	radialposition[i]=3;
	i=40;
	radialposition[i]=4;
	i=41;
	radialposition[i]=3;
	i=42;
	radialposition[i]=1;
	i=43;
	radialposition[i]=2;
	i=44;
	radialposition[i]=4;
	i=45;
	radialposition[i]=3;
	i=46;
	radialposition[i]=4;
	i=47;
	radialposition[i]=3;
	i=48;
	radialposition[i]=1;
	i=49;
	radialposition[i]=2;
	i=50;
	radialposition[i]=4;
	i=51;
	radialposition[i]=3;
	i=52;
	radialposition[i]=4;
	i=53;
	radialposition[i]=3;
	i=54;
	radialposition[i]=1;
	i=55;
	radialposition[i]=2;
	i=56;
	radialposition[i]=4;
	i=57;
	radialposition[i]=3;
	i=58;
	radialposition[i]=1;
	i=59;
	radialposition[i]=2;
	i=60;
	radialposition[i]=3;
}


