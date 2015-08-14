package com.zhaoxiaodan.hookdemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity
{
	static {
		System.loadLibrary("hookdemo");
	}

	public static final String TAG = "===[hookdemo]===";
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		String param1 = "param1";
		Log.d(TAG, "===========before hook test:" + this.test(param1));
		hook(MainActivity.class, "test", "(Ljava/lang/String;)Ljava/lang/String;");
		Log.d(TAG, "===========after hook test:" + this.test(param1));

		Log.d(TAG, "===========before hook staticTest:" + this.staticTest(param1));
		hook(MainActivity.class, "staticTest", "(Ljava/lang/String;)Ljava/lang/String;");
		Log.d(TAG, "===========after hook staticTest:" + this.staticTest(param1));

	}

	public static String staticTest(String param1)
	{
		return "staticTest";
	}

	public String test(String param1)
	{
		return "11111";
	}

	public native void hook(Class<?> clazzToHook, String methodName, String methodSig);
}
