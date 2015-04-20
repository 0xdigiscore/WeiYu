package net.app.view;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import android.os.Environment;

public class fileWriter {
	public static void write(int a[][]){
        if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
        	File sdCardDir=Environment.getExternalStorageDirectory();
        	File saveFile=new File(sdCardDir,"abc123dlike.txt");
        	FileWriter fw = null;
			try {
				fw = new FileWriter(saveFile);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
        	StringBuilder sb=new StringBuilder();
        	for(int i=0;i<30;i++){
        		for(int j=0;j<2000;j++){
        			sb.append(a[i][j]+" ");
        		}
        	}
        	try {
				fw.write(sb.toString());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        }		
	}
}