package com.zhaoxiaodan.hookdemo.art;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity
{
	static {
		System.loadLibrary("hookdemo-art");
	}
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		((Button)findViewById(R.id.button)).setOnClickListener(new View.OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				new Demo1().demo();
			}
		});
	}

}
