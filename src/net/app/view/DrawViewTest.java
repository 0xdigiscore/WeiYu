package net.app.view;

import java.util.Arrays;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import net.app.jni.Recognize;
import net.app.keyboard.SoftKeyboard;


import android.R.interpolator;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Bitmap.Config;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.text.format.Time;
import android.text.style.DynamicDrawableSpan;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.Toast;
public class DrawViewTest extends View{
	Recognize recongize=new Recognize();
	public int WIDTH = 256;
	public int HEIGHT = 256;
	public int MaxStrokes = 30;//һ���ַ����ʻ���
	public int MaxPoints = 1000;//ÿһ�ʻ�������
	public int MaxPointsxy = 2000;//ÿ�ʻ���������xy����
	private int view_width = 0;	//��Ļ�Ŀ��
	private int view_height = 0;	//��Ļ�ĸ߶�
	private int width=0;
	private int height=0;
	private int preX;	//��ʼ���x����ֵ
	private int preY;//��ʼ���y����ֵ
	private Path path;	//·��
	public Paint paint = null;	//����
	Bitmap cacheBitmap = null;// ����һ���ڴ��е�ͼƬ����ͼƬ����Ϊ������
	Canvas cacheCanvas = null;// ����cacheBitmap�ϵ�Canvas����
	public Point [][] word = new Point[MaxStrokes][MaxPoints];
	public int[][] array = new int[MaxStrokes][];
	public int stroke_num = 0;//�ʻ���
	private int i = -1;//�ʻ�����ʼ��
	private int j = 0;//ÿ���ʻ�������ʼ��
	public int[] points_stroke = new int[MaxPoints];//ÿ��ʵ�ʵ���
	private boolean hasMeasured=false;
	Recognize recognize=new Recognize();
	int[][] result;
	private boolean isGoOn=true;
	Timer timer;
	boolean readflag=false;
	boolean threadflag=false;
	   
	   ExecutorService exec=Executors.newSingleThreadScheduledExecutor();
			   //Executors.newCachedThreadPool();
	   /**
	    * ����дʶ����صĴ������
	    * */
	    public DrawViewTest draw = null;
	    double end;
		public int CLASSCOUNT = 128;//�ַ��������
		public int[][] resultData_P =new int[MaxStrokes][MaxPointsxy];
		public int[] resultData_R = new int[CLASSCOUNT];
		public String[] resultData = new String[CLASSCOUNT];
		public int[] result_show = new int[10];
		public int flag = 0;//�����ǩ
		public int tag = 0;//Ϊmore��ӵı�ǩ
		long finish=0;
		Bitmap[] bm = new Bitmap[CLASSCOUNT];
       //��д���췽��
	    public DrawViewTest(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		//��ȡ��Ļ��ȡ��߶�
		view_width = context.getResources().getDisplayMetrics().widthPixels;
		view_height = context.getResources().getDisplayMetrics().heightPixels;
		System.out.println("��Ļ�߶�" + view_height);//�ֻ�������Ļ��ʵ�ʳߴ�
		System.out.println("��Ļ���" + view_width);
		ViewTreeObserver vto = getViewTreeObserver();//��ȡ��ͼ�۲���
        
        vto.addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener()//������ͼ���ɼ�����
        {
            public boolean onPreDraw()
            {
          
				if (hasMeasured == false)
                {

                	height = getMeasuredHeight();
                    width = getMeasuredWidth(); //��ȡDrawViewʵ�ʵĿ�Ⱥ͸߶Ⱥ󣬿����ڼ�������û�����С   
                    System.out.println("draw width->"+width);
                    System.out.println("draw heiht->"+height);
                    hasMeasured = true;

                }
                return true;
            }
        });
		//����һ�����view��ͬ��С�Ļ���
        //int canvas_h = height*2/3;
		cacheBitmap = Bitmap.createBitmap(view_width, view_height,Config.ARGB_8888);
		cacheCanvas = new Canvas();//�����»���
		path = new Path();//��·��
		cacheCanvas.setBitmap(cacheBitmap);// ��cacheCanvas�ϻ���cacheBitmap
		paint = new Paint(Paint.DITHER_FLAG);
		paint.setColor(Color.BLACK); // ����Ĭ�ϵĻ�����ɫ
		// ���û��ʷ��
		paint.setStyle(Paint.Style.STROKE);	//������䷽ʽΪ���
		paint.setStrokeJoin(Paint.Join.ROUND);		//���ñ�ˢ��ͼ����ʽ
		paint.setStrokeCap(Paint.Cap.ROUND);	//���û���ת�䴦�����ӷ��
		paint.setStrokeWidth(5); // ����Ĭ�ϱʴ��Ŀ��Ϊ1����
		paint.setAntiAlias(true); // ʹ�ÿ���ݹ���
		paint.setDither(true); // ʹ�ö���Ч��
		//�����������Ķ�ά����
		//Point [][] word = new Point[MaxStrokes][];
		for(int m=0;m<MaxStrokes;m++){
			for(int n = 0; n<MaxPoints; n++){
			word[m][n] =new Point(-1,-1);//i���ʻ�����j��������
			}
			
		}
		 timer = new Timer(true);
	
     }
	    /**
	     * ������Ļ��ʾ
	     */
	    Handler handler = new Handler(){
	        public void handleMessage(Message msg) {
	        	super.handleMessage(msg);
	            switch (msg.what) {         
	            case 1: 
	            	//clearnDraw();
	               if(word[0][0].x!=-1){
	            	        /*new Thread(new Runnable(){
	            	        	  @Override
								  public void run() {
									// TODO Auto-generated method stub
	            	        		 resultData_P=pointtoarray(word, resultData_P);
	      							 result= recognize.recognize(resultData_P);
	      							 clearAll();
	      					         Message read=new Message();
	    							 read.what=98;
	    							 read.obj=result;
	    							 SoftKeyboard.Writehandler.sendMessage(read);
								   }
								}
	            	        );*/
	            	        exec.execute(new WriteThread());
                    }
	            }
	            
	           
	        }
	            
	    };
	
	    
	    /**
	     * ������Ϣ��handler����ACTIVITY       
	     */
	    TimerTask task = new TimerTask() {
	        public void run() {
	            Message message = new Message();
	            message.what=1;
	            handler.sendMessage(message);
	        }
	    };
	    //�ְ���ʱ
	    private void touch_start() {
	     if(task != null)
	            task.cancel();//ȡ��֮ǰ������
	        task = new TimerTask() {
	             @Override
	            public void run() {
	                Message message = new Message();
	                message.what=1;
	                handler.sendMessage(message);
	                
	            }
	        };
	        
	    }
	    //���ƶ�ʱ
	    private void touch_move() {
	       
	            if(task != null)
	                task.cancel();//ȡ��֮ǰ������
	            task = new TimerTask() {
	                 
	                @Override
	                public void run() {
	                	
	                    Message message = new Message();
	                    message.what=1;
	                    handler.sendMessage(message);
	                }
	            };
	       
	  
	    }
	    //��̧��ʱ
	    private void touch_up() {
	         
	        if (timer!=null) {
	            if (task!=null) {
	                task.cancel();
	                task = new TimerTask() {
	                    public void run() {
	                        Message message = new Message();
	                        message.what = 1;
	                        handler.sendMessage(message);
	                    }
	                };
	                timer.schedule(task, 2000);               //2200�������Ϣ��handler����Activity
	            }
	        }else {
	     
	            timer = new Timer(true);
	            timer.schedule(task, 2000);                   //2200�������Ϣ��handler����Activity
	        }
	         
	    }
	     
	//��дDraw����
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		canvas.drawColor(Color.TRANSPARENT);	//���ñ�����ɫ
		Paint bmpPaint = new Paint();	//����Ĭ�����ô���һ������
		canvas.drawBitmap(cacheBitmap, 0, 0, bmpPaint); //����cacheBitmap
		canvas.drawPath(path, paint);	//����·��
		canvas.save(Canvas.ALL_SAVE_FLAG);	//����canvas��״̬
		canvas.restore();	//�ָ�canvas֮ǰ�����״̬����ֹ������canvasִ�еĲ����Ժ����Ļ�����Ӱ��	
		canvas.drawRect(0, 0, width,height ,paint);//������ͼ����߽�
	  
	}

	@SuppressLint("NewApi") @Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		// ��ȡ�����¼��ķ���λ��
		int x = (int)event.getX();
		int y = (int)event.getY();
		//Log.i("System.out", "(x,y)" + "("+x+","+y+")");
		long start = 0;
        
		//����
		 
		switch (event.getActionMasked()) {
		   case MotionEvent.ACTION_DOWN:
			++i;
			j = 0;
			path.moveTo(x, y); // ����ͼ����ʼ���Ƶ���x,y��������λ��
			preX = x;
			preY = y;
			Point point_start =new Point((int) preX,(int) preY);
			if(i<30&&j<=1000){
			  word[i][j] = point_start;
			}
			//System.out.println("��" + (i+1) + "��");
			//System.out.println("word["+i+"]["+j+"]" + (word[i][j]) );//����������
			touch_start();
			break;
		case MotionEvent.ACTION_MOVE:
			int dx = Math.abs(x - preX);//abs�������ֵ��
			int dy = Math.abs(y - preY);
			if (dx > 0 || dy > 0) { // �ж��Ƿ�������ķ�Χ��
			    ++j;
				path.quadTo(preX, preY, x, y);
				preX = x;  //����ͼ����ʼ���Ƶ���x,y��������λ��
				preY = y;
				Point point_move = new Point((int)preX,(int)preY);
				try{
				 if(i<=30&&j<1000){
				   word[i][j] = point_move;
				  }
				 }catch(ArrayIndexOutOfBoundsException e){
					
				  e.printStackTrace();	
				}
			}
			stroke_num = i + 1 ;
			touch_move();
			break;
		case MotionEvent.ACTION_UP:
		    threadflag=true;
			cacheCanvas.drawPath(path, paint); //����·��
			path.reset();
			++j;
		     if(i<30&&j<1000&&i>0){
		     	points_stroke[i] = j;
		     }
			touch_up();
		    
			break;
	    }
		invalidate();       //������ͼ
		return true;		// ����true�����������Ѿ�������¼�
	}
	
	//��ʾ�����������еĵ�����
	public void show_points(){

		System.out.println("�ʻ���:" + stroke_num);
		for(int m=0; m < MaxStrokes  ;m++){
			if(word[m][0].x == -1)
				break;		
			for(int n=0; n <= MaxPoints;n++)
				if(word[m][n].x == -1)
					break;
				
				
		}

		
	}
	
	//�����һ����ͼ����
	public void clearnPointCach()
	{
		stroke_num = 0;
	/*	for(int m=0; m <= i ;m++)
		{			
			for(int n=0;n<= points_stroke[m];n++)
			{
				word[m][n].set(-1, -1);
			}
			points_stroke[m]=0;
		}*/
		for(int m=0;m<30;m++){
			for(int n=0;n<1000;n++){
				word[m][n].set(-1, -1);
			}
			points_stroke[m]=0;
		}
		isGoOn=true;
		i=-1;
		j=0;
	}

	//�������
	public void clearnDraw()
	{
		Paint paint= new Paint();
		paint.setXfermode(new PorterDuffXfermode(android.graphics.PorterDuff.Mode.CLEAR));
		cacheCanvas.drawPaint(paint);
		paint.setXfermode(new PorterDuffXfermode(android.graphics.PorterDuff.Mode.SRC));
		invalidate();
		path.reset();
		clearnPointCach(); 
	}
	public void cleanD(){
		Paint paint= new Paint();
		paint.setXfermode(new PorterDuffXfermode(android.graphics.PorterDuff.Mode.CLEAR));
		cacheCanvas.drawPaint(paint);
		paint.setXfermode(new PorterDuffXfermode(android.graphics.PorterDuff.Mode.SRC));
		invalidate();
		path.reset();
	}
	//��Pointת��Ϊint
	public int[][] pointtoarray(Point[][] word ,int[][] array){
		
		for(int m = 0; m < MaxStrokes; m++)
			
			for(int n = 0; n <  MaxPoints ; n++){
				array[m][n*2] = word[m][n].x;
				array[m][n*2+1] = word[m][n].y;
			}
		return array;
		
	}
	public void clearAll(){
		for(int i=0;i<30;i++){
			for(int j=0;j<2000;j++){
				resultData_P[i][j]=-1;
			}
		}
	}
	class WriteThread implements Runnable{
		public void run(){
			// TODO Auto-generated method stub
   		     resultData_P=pointtoarray(word, resultData_P);
		     result= recognize.recognize(resultData_P);
		     for(int i=0;i<5;i++){
		    	 for(int j=0;j<10;j++){
		    		 Log.i("FFFFFFFFFFFFFFFFFFFFFFFF",""+result[i][j]);
		    	 }
		     }
		   
		     Message read=new Message();
			 read.what=98;
			 read.obj=result;
			 
			 SoftKeyboard.Writehandler.sendMessage(read);
			
		}
	}
}
