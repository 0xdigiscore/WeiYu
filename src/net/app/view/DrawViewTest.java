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
	public int MaxStrokes = 30;//一个字符最大笔划数
	public int MaxPoints = 1000;//每一笔画最多点数
	public int MaxPointsxy = 2000;//每笔画最多点数的xy个数
	private int view_width = 0;	//屏幕的宽度
	private int view_height = 0;	//屏幕的高度
	private int width=0;
	private int height=0;
	private int preX;	//起始点的x坐标值
	private int preY;//起始点的y坐标值
	private Path path;	//路径
	public Paint paint = null;	//画笔
	Bitmap cacheBitmap = null;// 定义一个内存中的图片，该图片将作为缓冲区
	Canvas cacheCanvas = null;// 定义cacheBitmap上的Canvas对象
	public Point [][] word = new Point[MaxStrokes][MaxPoints];
	public int[][] array = new int[MaxStrokes][];
	public int stroke_num = 0;//笔划数
	private int i = -1;//笔画数初始化
	private int j = 0;//每个笔画点数初始化
	public int[] points_stroke = new int[MaxPoints];//每笔实际点数
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
	    * 与手写识别相关的代码变量
	    * */
	    public DrawViewTest draw = null;
	    double end;
		public int CLASSCOUNT = 128;//字符类别总数
		public int[][] resultData_P =new int[MaxStrokes][MaxPointsxy];
		public int[] resultData_R = new int[CLASSCOUNT];
		public String[] resultData = new String[CLASSCOUNT];
		public int[] result_show = new int[10];
		public int flag = 0;//结果标签
		public int tag = 0;//为more添加的标签
		long finish=0;
		Bitmap[] bm = new Bitmap[CLASSCOUNT];
       //重写构造方法
	    public DrawViewTest(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		//获取屏幕宽度、高度
		view_width = context.getResources().getDisplayMetrics().widthPixels;
		view_height = context.getResources().getDisplayMetrics().heightPixels;
		System.out.println("屏幕高度" + view_height);//手机整个屏幕的实际尺寸
		System.out.println("屏幕宽度" + view_width);
		ViewTreeObserver vto = getViewTreeObserver();//获取视图观测者
        
        vto.addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener()//设置视图生成监听器
        {
            public boolean onPreDraw()
            {
          
				if (hasMeasured == false)
                {

                	height = getMeasuredHeight();
                    width = getMeasuredWidth(); //获取DrawView实际的宽度和高度后，可用于计算或设置画布大小   
                    System.out.println("draw width->"+width);
                    System.out.println("draw heiht->"+height);
                    hasMeasured = true;

                }
                return true;
            }
        });
		//创建一个与该view相同大小的缓存
        //int canvas_h = height*2/3;
		cacheBitmap = Bitmap.createBitmap(view_width, view_height,Config.ARGB_8888);
		cacheCanvas = new Canvas();//创建新画布
		path = new Path();//新路径
		cacheCanvas.setBitmap(cacheBitmap);// 在cacheCanvas上绘制cacheBitmap
		paint = new Paint(Paint.DITHER_FLAG);
		paint.setColor(Color.BLACK); // 设置默认的画笔颜色
		// 设置画笔风格
		paint.setStyle(Paint.Style.STROKE);	//设置填充方式为描边
		paint.setStrokeJoin(Paint.Join.ROUND);		//设置笔刷的图形样式
		paint.setStrokeCap(Paint.Cap.ROUND);	//设置画笔转弯处的连接风格
		paint.setStrokeWidth(5); // 设置默认笔触的宽度为1像素
		paint.setAntiAlias(true); // 使用抗锯齿功能
		paint.setDither(true); // 使用抖动效果
		//创建存点坐标的二维数组
		//Point [][] word = new Point[MaxStrokes][];
		for(int m=0;m<MaxStrokes;m++){
			for(int n = 0; n<MaxPoints; n++){
			word[m][n] =new Point(-1,-1);//i：笔划数，j：点坐标
			}
			
		}
		 timer = new Timer(true);
	
     }
	    /**
	     * 处理屏幕显示
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
	     * 发送消息给handler更新ACTIVITY       
	     */
	    TimerTask task = new TimerTask() {
	        public void run() {
	            Message message = new Message();
	            message.what=1;
	            handler.sendMessage(message);
	        }
	    };
	    //手按下时
	    private void touch_start() {
	     if(task != null)
	            task.cancel();//取消之前的任务
	        task = new TimerTask() {
	             @Override
	            public void run() {
	                Message message = new Message();
	                message.what=1;
	                handler.sendMessage(message);
	                
	            }
	        };
	        
	    }
	    //手移动时
	    private void touch_move() {
	       
	            if(task != null)
	                task.cancel();//取消之前的任务
	            task = new TimerTask() {
	                 
	                @Override
	                public void run() {
	                	
	                    Message message = new Message();
	                    message.what=1;
	                    handler.sendMessage(message);
	                }
	            };
	       
	  
	    }
	    //手抬起时
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
	                timer.schedule(task, 2000);               //2200秒后发送消息给handler更新Activity
	            }
	        }else {
	     
	            timer = new Timer(true);
	            timer.schedule(task, 2000);                   //2200秒后发送消息给handler更新Activity
	        }
	         
	    }
	     
	//重写Draw方法
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		canvas.drawColor(Color.TRANSPARENT);	//设置背景颜色
		Paint bmpPaint = new Paint();	//采用默认设置创建一个画笔
		canvas.drawBitmap(cacheBitmap, 0, 0, bmpPaint); //绘制cacheBitmap
		canvas.drawPath(path, paint);	//绘制路径
		canvas.save(Canvas.ALL_SAVE_FLAG);	//保存canvas的状态
		canvas.restore();	//恢复canvas之前保存的状态，防止保存后对canvas执行的操作对后续的绘制有影响	
		canvas.drawRect(0, 0, width,height ,paint);//画出绘图区域边界
	  
	}

	@SuppressLint("NewApi") @Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		// 获取触摸事件的发生位置
		int x = (int)event.getX();
		int y = (int)event.getY();
		//Log.i("System.out", "(x,y)" + "("+x+","+y+")");
		long start = 0;
        
		//画线
		 
		switch (event.getActionMasked()) {
		   case MotionEvent.ACTION_DOWN:
			++i;
			j = 0;
			path.moveTo(x, y); // 将绘图的起始点移到（x,y）坐标点的位置
			preX = x;
			preY = y;
			Point point_start =new Point((int) preX,(int) preY);
			if(i<30&&j<=1000){
			  word[i][j] = point_start;
			}
			//System.out.println("第" + (i+1) + "笔");
			//System.out.println("word["+i+"]["+j+"]" + (word[i][j]) );//起点坐标存入
			touch_start();
			break;
		case MotionEvent.ACTION_MOVE:
			int dx = Math.abs(x - preX);//abs是求绝对值的
			int dy = Math.abs(y - preY);
			if (dx > 0 || dy > 0) { // 判断是否在允许的范围内
			    ++j;
				path.quadTo(preX, preY, x, y);
				preX = x;  //将绘图的起始点移到（x,y）坐标点的位置
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
			cacheCanvas.drawPath(path, paint); //绘制路径
			path.reset();
			++j;
		     if(i<30&&j<1000&&i>0){
		     	points_stroke[i] = j;
		     }
			touch_up();
		    
			break;
	    }
		invalidate();       //更新视图
		return true;		// 返回true表明处理方法已经处理该事件
	}
	
	//显示保存在数组中的点坐标
	public void show_points(){

		System.out.println("笔划数:" + stroke_num);
		for(int m=0; m < MaxStrokes  ;m++){
			if(word[m][0].x == -1)
				break;		
			for(int n=0; n <= MaxPoints;n++)
				if(word[m][n].x == -1)
					break;
				
				
		}

		
	}
	
	//清空上一个绘图数据
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

	//清除画布
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
	//把Point转化为int
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
