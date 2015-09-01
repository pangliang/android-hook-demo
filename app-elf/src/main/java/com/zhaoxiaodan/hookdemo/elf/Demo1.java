package com.zhaoxiaodan.hookdemo.elf;

import android.util.Log;

public class Demo1
{

	static {
//		System.loadLibrary("call-test-function");
	}

	String TAG = "===[hookdemo]===";

	public void demo()
	{
		hook("libcall-test-function.so","_Z12testFunctionPKci");
//		callTestFunction("beforehook",1111);
	}

	private native void callTestFunction(String p1,int p2);

	private native void hook(String soname,String functionName);
}
