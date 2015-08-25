package com.zhaoxiaodan.hookdemo.dalvik;

import android.util.Log;


public class Demo2
{
	String TAG = "===[hookdemo]===";

	public interface Handler
	{
		/**
		 * 在原方法执行之前执行
		 * @param params 原方法参数
		 */
		public void before(Object instance, Object[] params);
	}

	public String test(String param1, long param2, int[] param3)
	{
		return param1;
	}

	public static String staticTest(String param1, Long param2, int[] param3)
	{
		return param1;
	}
	public void demo()
	{
		String param1 = "param1";

		/**
		 * 设置hook
		 * hook 住Demo2 这个类的 test 方法
		 * 实现一个Handler类来实现 回调函数 before
		 */
		hook(Demo2.class, "test", "(Ljava/lang/String;J[I)Ljava/lang/String;", new Handler()
		{
			@Override
			public void before(Object instance, Object[] params)
			{
				Log.d(TAG, "===========Handler before,param len: "+params.length+",instance:"+instance);
				for (int i = 0; i < params.length; i++)
				{
					Log.d(TAG, "p" + i + "=" + params[i]);
				}
			}
		});
		Log.d(TAG, "===========call test" + this.test(param1,999L,new int[]{1,2,3}));


		/**
		 * 测试静态函数的hook, 则回调函数参数中 instance 为 null
		 */
		hook(Demo2.class, "staticTest", "(Ljava/lang/String;Ljava/lang/Long;[I)Ljava/lang/String;", new Handler()
		{
			@Override
			public void before(Object instance, Object[] params)
			{
				Log.d(TAG, "===========Handler before,param len: "+params.length+",instance:"+instance);
				for (int i = 0; i < params.length; i++)
				{
					Log.d(TAG, "p" + i + "=" + params[i]);
				}
			}
		});
		Log.d(TAG, "===========call test" + this.staticTest(param1, 999L, new int[]{1, 2, 3}));
	}

	/**
	 * hook 函数, 是个jni 函数
	 * @param clazzToHook
	 * @param methodName
	 * @param methodSig
	 * @param handler
	 */
	private native void hook(Class<?> clazzToHook, String methodName, String methodSig, Handler handler);
}
