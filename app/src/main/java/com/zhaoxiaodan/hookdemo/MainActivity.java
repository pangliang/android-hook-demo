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

		Log.d(TAG, "===========before hook:" + this.test());
		hook(MainActivity.class, "test", "()Ljava/lang/String;");
		Log.d(TAG, "===========after hook:" + this.test());

		Log.d(TAG, "===========before hook:" + this.test2222());
		hook2(MainActivity.class, "test2222", NewClass.class, "test2222","()Ljava/lang/String;");
		Log.d(TAG, "===========after hook:" + this.test2222());

	}

	public String test()
	{
		return "111111";
	}

	public String test2222()
	{
		return "111111";
	}

	public native void hook(Class<?> clazzToHook, String methodName, String methodSig);

	public native void hook2(Class<?> clazzToHook, String methodName, Class<?> proxyClazz, String proxyMethod, String methodSig);
}
