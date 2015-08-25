package com.zhaoxiaodan.hookdemo.dalvik;

import android.util.Log;

public class Demo1
{
	String TAG = "===[hookdemo]===";

	public static String staticTest(String param1)
	{
		return "staticTest";
	}

	public String test(String param1)
	{
		return "11111";
	}

	public void demo()
	{
		String param1 = "param1";
		Log.d(TAG, "===========before hook test:" + this.test(param1));
		hook(Demo1.class, "test", "(Ljava/lang/String;)Ljava/lang/String;");
		Log.d(TAG, "===========after hook test:" + this.test(param1));

		Log.d(TAG, "===========before hook staticTest:" + this.staticTest(param1));
		hook(Demo1.class, "staticTest", "(Ljava/lang/String;)Ljava/lang/String;");
		Log.d(TAG, "===========after hook staticTest:" + this.staticTest(param1));
	}

	private native void hook(Class<?> clazzToHook, String methodName, String methodSig);
}
