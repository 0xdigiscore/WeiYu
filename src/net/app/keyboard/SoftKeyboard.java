package net.app.keyboard;
import java.util.ArrayList;
import java.util.List;
import net.app.util.WeiFormat;
import net.app.view.CandidateView;
import net.app.view.DrawViewTest;

import com.example.weiyu.R;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.inputmethodservice.InputMethodService;
import android.inputmethodservice.Keyboard;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode.VmPolicy;
import android.text.InputType;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.CompletionInfo;
import android.view.inputmethod.CorrectionInfo;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.ExtractedText;
import android.view.inputmethod.ExtractedTextRequest;
import android.view.inputmethod.InputConnection;
import android.widget.FrameLayout;
import android.widget.HorizontalScrollView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.RelativeLayout;
import android.widget.TableRow;
import android.widget.Toast;
import android.inputmethodservice.KeyboardView;

public class SoftKeyboard extends InputMethodService implements 
    KeyboardView. OnKeyboardActionListener{
	private LatinKeyboard mWeiYuKeyboard;
	private LatinKeyboard mSymbolsKeyboard;
	private LatinKeyboard mQuertKeyboard;
    private LatinKeyboard mCurKeyboard;
    private DrawViewTest draw;
	/*******键盘View对象**********/
	private LatinKeyboardView mInputView;
	CandidatesContainer candidateContainer;
	private CandidateView candidateView;
	public StringBuilder mComposing = new StringBuilder();
	private CompletionInfo[] mCompletions;
	private CandidatesContainer candidatesContainer;
	private String mWordSeparators;
	public static Handler Writehandler;
	public static final int CANDIDATE_VIEW_MEAT=1; 
	private boolean mCapsLock;
	
	//决定能不能有候选栏
	private boolean mPredictionOn=false;
	private StringBuilder result=new StringBuilder();
	private StringBuilder result2;
	private String result3;
	private final static int CWJ_HEAP_SIZE=6*1024*1024;
	
	public void onCreate(){
		super.onCreate();
	    mWordSeparators=getResources().getString(R.string.word_separators);
		Writehandler=new Handler(){
            public void handleMessage(Message msg) {
			       switch (msg.what) {
			          case 98:
			             int a[][]=(int[][])msg.obj;
			             draw.clearnDraw();
			             List<String>coda=new ArrayList<String>();
			             coda=format2(a);
			             setSuggestions(coda, true, true);
			             getCurrentInputConnection().setComposingText(coda.get(0),coda.get(0).length());
			            	
			            }
		  	  }
		  };
	}
	public List<String> format2(int[][] a){
		StringBuilder s=new StringBuilder();
		List<String> formatResult = new ArrayList<String>();
		for(int i=0;i<5;i++){
			for(int j=0;j<10;j++){
				s.append(WeiFormat.WeiYu(a[i][j]));
				//Log.i("MMMMMMMMMMMM",""+a[i][j]);
				
			}
			formatResult.add(s.toString());
			s.setLength(0);
		}
		return formatResult;
	}
	public void onInitializeInterface(){
		mWeiYuKeyboard=new LatinKeyboard(this,R.xml.uyghur_single);
		mQuertKeyboard=new LatinKeyboard(this, R.xml.uyghur_latin_multi);
		mSymbolsKeyboard = new LatinKeyboard(this, R.xml.symbols_before);
	}
	public View onCreateInputView(){
	     View view=getLayoutInflater().inflate(R.layout.input2, null);
		 mInputView=(LatinKeyboardView)view.findViewById(R.id.keyboard);
		 mInputView.setOnKeyboardActionListener(this);
		 draw=(DrawViewTest)view.findViewById(R.id.draw);
		// setLatinKeyboardView(mWeiYuKeyboard);
		 return view;
	}
	public View onCreateCandidatesView(){
		   candidateContainer=(CandidatesContainer)getLayoutInflater().inflate(R.layout.candidatestest, null);
		   candidateContainer.initialize();
		   candidateContainer.setService(this);
		   candidateView=(CandidateView)candidateContainer.findViewById(R.id.candidate_view1);
		   candidateView.setService(this);
		   setCandidatesViewShown(true);
		   return candidateContainer;
		}
	public void setLatinKeyboardView(LatinKeyboard keyboard){
		   mInputView.setKeyboard(keyboard);
    }
	public void changeKeyboard(){
		if(mInputView.getKeyboard()==mWeiYuKeyboard){
			setLatinKeyboardView(mQuertKeyboard);
			draw.setVisibility(View.VISIBLE);
		}else if(mInputView.getKeyboard()==mQuertKeyboard){
			setLatinKeyboardView(mWeiYuKeyboard);
			draw.setVisibility(View.GONE);
		}else{
			setLatinKeyboardView(mQuertKeyboard);
			draw.setVisibility(View.VISIBLE);
		}
	}
	/*决定何时弹出输入法*/
	@Override
	public void onStartInput(EditorInfo attribute, boolean restarting){
		   super.onStartInput(attribute, restarting);
		   mComposing.setLength(0);
		   switch (attribute.inputType & InputType.TYPE_MASK_CLASS) {
	            case InputType.TYPE_CLASS_NUMBER:
	            case InputType.TYPE_CLASS_DATETIME:
	            case InputType.TYPE_CLASS_PHONE:
	            case InputType.TYPE_CLASS_TEXT:
	                mCurKeyboard = mWeiYuKeyboard;
	                break;
	             default:
	                // For all unknown input types, default to the alphabetic
	                // keyboard with no special features.
	                mCurKeyboard = mWeiYuKeyboard;
	        }
	        
	        // Update the label on the enter key, depending on what the application
	        // says it will do.
	        //根据输入目标设置回车键
	        mCurKeyboard.setImeOptions(getResources(), attribute.imeOptions);
	}
	@Override
	public void onStartInputView(EditorInfo attribute,boolean restarting){
		super.onStartInputView(attribute, restarting);
		if(mCurKeyboard==mQuertKeyboard){
			draw.setVisibility(View.VISIBLE);
		}else{
			draw.setVisibility(View.GONE);
		}
		mInputView.setKeyboard(mCurKeyboard);
		
	}
	/**
	    * Helper function to commit any text being composed in to the editor.
	    */
	   public void commitTyped(StringBuilder content) {
		      InputConnection ic =getCurrentInputConnection();
		      content=mComposing;
		      if(ic!=null){
		         if (content.length() > 1) {
	    	         //后面的参数决定了光标应有的位置
		        	 ic.beginBatchEdit();
		        	 updateCandidates();
		             ic.commitText(content, mComposing.length());
		             ic.endBatchEdit();
			        
	           }else{
	               ic.commitText(content, mComposing.length());
	               updateCandidates();
	               
	           }
		   }
	   }
	public void hideKeyboard()
	{
	    requestHideSelf(0);
	}
	//处理光标移动
	@Override 
	public void onUpdateSelection(int oldSelStart, int oldSelEnd,
	       int newSelStart, int newSelEnd,
	       int candidatesStart, int candidatesEnd) {
	       super.onUpdateSelection(oldSelStart, oldSelEnd, newSelStart, newSelEnd,
	               candidatesStart, candidatesEnd);
	       if (mComposing.length() > 0 && (newSelStart != candidatesEnd
	               || newSelEnd != candidatesEnd)) {
	    
	       	/*mComposing.setLength(0);
	       	//候选栏置空
	           updateCandidates();*/
	           //这个语句和下面的if里面的那个决定了结束输入的全过程
	           InputConnection ic = getCurrentInputConnection();
	           if (ic != null) {
	           	//这个语句的作用是，让输入目标内的下划线去掉，完成一次编辑
	            //   ic.finishComposingText();
	               
	           }
	       }
	   }
	  /**
	    * Update the list of available candidates from the current composing
	    * text.  This will need to be filled in by however you are determining
	    * candidates.
	    */
	   private void updateCandidates() {
	       if (mComposing.length() >0) {
	               ArrayList<String> list = new ArrayList<String>();
	               list.add(mComposing.toString());
	               setSuggestions(list, true, true);
	         
	           } else {
	               setSuggestions(null, false, false);
	           }
	   }
	
	   public void setSuggestions(List<String> suggestions, boolean completions,
	           boolean typedWordValid) {
	           candidateContainer.showCandidates(suggestions);
	   }
	 /**
	    * Helper to update the shift state of our keyboard based on the initial
	    * editor state.
	    */
	   private void updateShiftKeyState(EditorInfo attr) {
	
	       if (attr != null 
	               && mInputView != null && mQuertKeyboard == mInputView.getKeyboard()) {
	           int caps = 0;
	           EditorInfo ei = getCurrentInputEditorInfo();
	           if (ei != null && ei.inputType != InputType.TYPE_NULL) {
	               caps = getCurrentInputConnection().getCursorCapsMode(attr.inputType);
	           }
	           mInputView.setShifted(mCapsLock || caps != 0);
	       }
	   }
	   /*
	    * 看看是不是字母
	    * */
	   private boolean isAlphabet(int code){
		   if(Character.isLetter(code)){
			   return true;
		   }else{
			   return false;
		   }
	   }
	   /*
	    * Helper to send a key down / key up pair to the current editor.
	    **/
	   private void keyDownUp(int keyEventCode){
		   //这个函数是用来进行特殊输出的，就好像前面定义的"android"输出，但如果从键盘上输入字符，是不会经过这一步
		   getCurrentInputConnection().sendKeyEvent(
				   new KeyEvent(KeyEvent.ACTION_DOWN,keyEventCode)
				   );
		   getCurrentInputConnection().sendKeyEvent(
				   new KeyEvent(KeyEvent.ACTION_UP,keyEventCode
				   ));
	   }
	   
	   /*
	    *Helper to send a character to the editor as raw key events 
	    * */
	   private void sendKey(int keyCode){//传入的参数是ascl字母表
		   //处理中断符
		   switch(keyCode){
		    /*case '\n':
	          keyDownUp(KeyEvent.KEYCODE_ENTER);
	          //又是"特别输入"或称为强制输入
	          break;*/
	        default:
	        	if(keyCode>='0'&&keyCode<='9'){
	        		keyDownUp(keyCode-'0'+KeyEvent.KEYCODE_0);
	        	}else{
	        		commitTyped(mComposing);
	        		getCurrentInputConnection().commitText(String.valueOf((char)keyCode),mComposing.length());
	        		mComposing.setLength(0);
	        		updateCandidates();
	        	}
	          break;
		   }
	   }
	 private String getWordSeparators(){
		   return mWordSeparators;
	 }
	 //下面这些函数是用来继承的吧？
	 public boolean isWordSeparator(int code){
		   String separators=getWordSeparators();
		   //检查所输入的字符有没有这些字符在里面
	
		   return separators.contains(String.valueOf((char)code));
	   }
	 private void handleBackspace(){
		   //删除一个字，用的就是他
		   final int length=mComposing.length();
		   if(length>1){
			   mComposing.delete(length-1, length);
			 }else if(length>0){
			   //就是说等于1的时候
			   mComposing.setLength(0);
		   }else{
			 keyDownUp(KeyEvent.KEYCODE_DEL);
		   }
		   getCurrentInputConnection().setComposingText(mComposing, mComposing.length());
		   updateCandidates();
		}
	 private void handleShift(){
	   //这才是大小写的切换，是正常切换(通过转换键)
	   if(mInputView==null){
	     return;
	    }
	    Keyboard currenKeyboard=mInputView.getKeyboard();
	    if(currenKeyboard==currenKeyboard){
	      checkToggleCapsLock();
	      mInputView.setShifted(mCapsLock||!mInputView.isShifted());
	    }
	 }
	 private void handleLanguageSwitch(){
	    if(mInputView==null){
	       return;
	     }
	 }
	 private void checkToggleCapsLock(){
	    	//记录上次变幻的时间
	    	long now=System.currentTimeMillis();
	}
	 private void handleCharacter(int primaryCode,int[] keyCodes){
		 if(isAlphabet(primaryCode)){
			 mComposing.append((char)primaryCode);
			 getCurrentInputConnection().setComposingText(mComposing,1);
			 updateCandidates();
		 }else{
			 getCurrentInputConnection().commitText(mComposing, 1);
		 }
	  }
   private void handleClose(){
	    	//关闭键盘的键
	    	requestHideSelf(0);
	    	mInputView.closing();
	}
   public void onKey(int primaryCode, int[] keyCodes) {
	    if (isWordSeparator(primaryCode)) {
            sendKey(primaryCode);
            updateShiftKeyState(getCurrentInputEditorInfo());
        } else if (primaryCode == Keyboard.KEYCODE_DELETE) {
            handleBackspace();
        } else if (primaryCode == Keyboard.KEYCODE_SHIFT) {
        	setLatinKeyboardView(mQuertKeyboard);
        	draw.setVisibility(View.VISIBLE);
        }else if(primaryCode ==-15){
        	setLatinKeyboardView(mWeiYuKeyboard);
        } 
        else if (primaryCode == Keyboard.KEYCODE_CANCEL) {
            handleClose();
            return;
        } else if (primaryCode == LatinKeyboardView.KEYCODE_LANGUAGE_SWITCH) {
        	handleLanguageSwitch();
            return;
        } else if (primaryCode == LatinKeyboardView.KEYCODE_OPTIONS) {
        
            // Show a menu or somethin'
        }else if(primaryCode ==-112
                && mInputView != null){
        	setLatinKeyboardView(mSymbolsKeyboard);
        } else if(primaryCode==-7){
        	if(mCapsLock){
        		mInputView.setShifted(true);
        	}
        	handleShift();
        }
        else {
            handleCharacter(primaryCode, keyCodes);
        }
   }
	@Override
	public void onPress(int arg0) {
		// TODO Auto-generated method stub
	}

	@Override
	public void onRelease(int arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onText(CharSequence arg0) {
		// TODO Auto-generated method stub
		
	}
    @Override
	public void swipeDown() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void swipeLeft() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void swipeRight() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void swipeUp() {
		// TODO Auto-generated method stub
		
	}

	@Override public void onDisplayCompletions(CompletionInfo[] completions) {
    	//当需要在候选栏里面显示auto的内容
    	//此函数作用，猜测：当全屏幕模式的时候，mCompletionOn置true,可以通过候选栏来显示auto

         //必须这个变量允许
            mCompletions = completions; //赋值给本来里面专门记录候选值的变量
            if (completions == null) {
                setSuggestions(null, false, false); //如果没有候选词，就这样处置
                return;
            }
            
            List<String> stringList = new ArrayList<String>();
            for (int i=0; i<(completions != null ? completions.length : 0); i++) {
                CompletionInfo ci = completions[i];
                if (ci != null) stringList.add(ci.getText().toString());
            }
            setSuggestions(stringList, true, true);
        
    }
    public void pickSuggestionManually(int index) {

        if ( mCompletions != null && index >= 0
                && index < mCompletions.length) {
            CompletionInfo ci = mCompletions[index];
            getCurrentInputConnection().commitCompletion(ci);
            if (candidateView != null) {
                candidateView.clear();
            }
            updateShiftKeyState(getCurrentInputEditorInfo());
        } else if (mComposing.length() > 0) {
            // If we were generating candidate suggestions for the current
            // text, we would commit one of them here.  But for this sample,
            // we will just commit the current text.
            commitTyped(mComposing);
            mComposing.setLength(0);
            candidateView.clear();
            updateCandidates();
        }
    }
}