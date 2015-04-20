package net.app.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.util.Log;

public class WeiFormat {
	 /**
     * 汉字转Unicode
     * @param s
     * @return
     */
    public static StringBuilder gbEncoding(final String s){
        StringBuilder str = new StringBuilder();
        for (int i = 0; i < s.length(); i++) {
        int ch = (int) s.charAt(i);
        str.append("\\u" + Integer.toHexString(ch));
        
        }
        return str;
    }
    /**
     * Unicode转汉字
     * @param str
     * @return
     */
    public static String encodingtoStr(String str){
        Pattern pattern = Pattern.compile("(\\\\u(\\p{XDigit}{4}))");
        Matcher matcher = pattern.matcher(str);
        char ch;
        while (matcher.find()) {
        ch = (char) Integer.parseInt(matcher.group(2), 16);
        str = str.replace(matcher.group(1), ch + "");
        }
        return str;
    }
   public static String WeiYu(int num){
     String result = null; 
     num--;
   //  Log.i("EEEEEEEEEEEEEEEEEEEEE",""+num);
     if(num>=0){
	 switch(num){
	 case 0:case 18:case 52:case 53: result = "ئى";
	 break;
	 case 1:case 2:case 3:case 4: result = "پ";
	 break;
	 case 5:case 6:case 7:case 8: result = "چ";
	 break;
	 case 9:case 10: result = "ژ";
	 break;
	 case 11:case 12:case 13:case 14: result = "گ";
	 break;
	 case 15:case 16: result = "ھ";
	 break;
	 case 17:case 49:case 50:case 51: result = "ئې";
	 break;
	 case 19:case 20:case 21:case 22: result = "ڭ";
	 break;
	 case 23:case 24: result = "ۇ";
	 break;
	 case 25:case 26: result = "ۆ";
	 break;
	 case 27:case 28: result = "ۈ";
	 break;
	 case 29:case 30: result = "ۋ";
	 break;
	 case 31:case 32:case 33:case 34: result = "ې";
	 break;
	 case 35:case 36:case 120:case 121: result = "ى";
	 break;
	 case 37:case 38: result = "ئا";
	 break;
	 case 39:case 40: result = "ئە";
	 break;
	 case 41:case 42: result = "ئو";
	 break;
	 case 43:case 44: result = "ئۇ";
	 break;
	 case 45:case 46: result = "ئۆ";
	 break;
	 case 47:case 48: result = "ئۈ";
	 break;
	 case 54:case 55: result = "ئ";
	 break;
	 case 56:case 57: result = "ا";
	 break;
	 case 58:case 59:case 60:case 61: result = "ب";
	 break;
	 case 62:case 63:case 64:case 65: result = "ت";
	 break;
	 case 66:case 67:case 68:case 69: result = "ج";
	 break;
	 case 70:case 71:case 72:case 73: result = "خ";
	 break;
	 case 74:case 75: result = "د";
	 break;
	 case 76:case 77: result = "ر";
	 break;
	 case 78:case 79: result = "ز";
	 break;
	 case 80:case 81:case 82:case 83: result = "س";
	 break;
	 case 84:case 85:case 86:case 87: result = "ش";
	 break;
	 case 88:case 89:case 90:case 91: result = "غ";
	 break;
	 case 92:case 93:case 94:case 95: result = "ف";
	 break;
	 case 96:case 97:case 98:case 99: result = "ق";
	 break;
	 case 100:case 101:case 102:case 103: result = "ك";
	 break;
	 case 104:case 105:case 106:case 107: result = "ل";
	 break;
	 case 108:case 109:case 110:case 111: result = "م";
	 break;
	 case 112:case 113:case 114:case 115: result = "ن";
	 break;
	 case 116:case 117: result = "ە";
	 break;
	 case 118:case 119: result = "و";
	 break;
	 case 122:case 123:case 124:case 125: result = "ي";
	 break;
	 case 126:case 127: result = "لا";
	 break;
	   /************************/
	 }  
	 return result;
     }else{
    	 return "";
     }
   }
 //写文件  
   public static void writeSDFile(String fileName, String write_str) throws IOException{    
           File file = new File(fileName);    
           FileOutputStream fos = new FileOutputStream(file);    
           byte [] bytes = write_str.getBytes();   
           fos.write(bytes);   
           fos.close();   
   } 
   
}





