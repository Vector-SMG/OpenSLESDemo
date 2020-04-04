package com.cornucopia.openslesdemo;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;
import java.io.File;

public class MainActivity extends AppCompatActivity {

  private static final String TAG = "MainActivity";
  private OpenSLESHelper openSLESHelper=new OpenSLESHelper();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    Button btn1=findViewById(R.id.btn1);
    Button btn2=findViewById(R.id.btn2);
    String storeDir=FileUtils.queryStoreDir(this);
    final String pcmPath=storeDir+"/1.pcm";
    final String recordPcmPath=storeDir+"/2.pcm";
    Log.e(TAG,recordPcmPath);
    btn1.setOnClickListener(new View.OnClickListener() {
      @Override public void onClick(View v) {
        if(new File(pcmPath).exists()){
          openSLESHelper.playPcmByOpenSLES(recordPcmPath);
          openSLESHelper.recordVoiceToPcm(recordPcmPath,
              44100,2,16);
        }
      }
    });

    btn2.setOnClickListener(new View.OnClickListener() {
      @Override public void onClick(View v) {
        openSLESHelper.stopPlay();
        //openSLESHelper.stopRecord();
      }
    });

  }


}
