package net.app.activity;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings.Secure;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.inputmethod.InputMethodInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.List;

import com.example.weiyu.R;
import com.example.weiyu.R.array;

public class openImeActivity extends Activity
  implements View.OnClickListener
{
  private static final int DISABLE_COLOR = -10329965;
  private static final int ENABLE_COLOR = -1;
  private Button mSelectButton;
  private Button mSwitchButton;
  ProgressDialog proDialog;
  Handler handler=new Handler(){
	  public void handleMessage(Message msg){
		  super.handleMessage(msg);
		  switch(msg.what){
		  case 1:
			  proDialog.dismiss();
		  }
	  }
  };
  private boolean getIsSelected()
  {
    List localList = ((InputMethodManager)getSystemService("input_method")).getEnabledInputMethodList();
    for (int i = 0; ; ++i)
    {
      if (i >= localList.size())
        return false;
      if (((InputMethodInfo)localList.get(i)).getPackageName().contains("com.example.weiyu"))
        return true;
    }
  }
  
  

  private boolean getIsSwitched()
  {
    ((InputMethodManager)getSystemService("input_method")).getEnabledInputMethodList();
     return false;
  }


  private void goSelectIME()
  {
    Intent localIntent = new Intent("android.settings.INPUT_METHOD_SETTINGS");
    localIntent.setFlags(268435456);
    startActivity(localIntent);
  }

  private void goSwitchIME()
  {
    ((InputMethodManager)getSystemService("input_method")).showInputMethodPicker();
  }

  private void setupView()
  {
    this.mSelectButton = ((Button)findViewById(R.id.open_select_button));
    this.mSwitchButton = ((Button)findViewById(R.id.open_switch_button));
    this.mSelectButton.setOnClickListener(this);
    this.mSwitchButton.setOnClickListener(this);
  }

  private void updateView()
  {
    if (getIsSelected())
    {
      this.mSelectButton.setText("�ѹ�ѡ");
      this.mSelectButton.setEnabled(false);
      this.mSelectButton.setTextColor(Color.RED);
    }
    if(getIsSwitched())
    {
      this.mSwitchButton.setText("���л�");
      this.mSwitchButton.setEnabled(false);
      this.mSwitchButton.setTextColor(Color.RED);
     }
  }

  public void onClick(View paramView)
  {
    switch (paramView.getId())
    {
    default:
      return;
    case R.id.open_select_button:
      goSelectIME();
      return;
    case R.id.open_switch_button:
    }
    goSwitchIME();
  }

  @SuppressWarnings({ "unchecked", "static-access" })
protected void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    setContentView(R.layout.open_ime_activity);
    setupView();
    proDialog=new ProgressDialog(openImeActivity.this).show(
    		openImeActivity.this, "���Ժ�", "���ڵ������");
    new Thread(new Runnable() {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			CopyFile();
			Message message=new Message();
			message.what=1;
			handler.handleMessage(message);
		}
	}).start();
    
  }

  protected void onResume()
  {
    super.onResume();
  }

  public void onWindowFocusChanged(boolean paramBoolean)
  {
    super.onWindowFocusChanged(paramBoolean);
    updateView();
  }
  public void CopyFile(){
	  try{
		  InputStream in=getResources().openRawResource(R.raw.write);
		  FileOutputStream fout = new FileOutputStream("/storage/sdcard0/write.dat"); 
		  int length=in.available();
		  byte[] buffer=new byte[length];
		  in.read(buffer);
		  fout.write(buffer);
		  in.close();
		  fout.close();
		  
	  }catch(Exception e){
		  e.printStackTrace();
	  }
  }
}



