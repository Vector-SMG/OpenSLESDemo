package com.cornucopia.openslesdemo;

/**
 * @author Cornucopia
 * @version 1.0
 * @since 2020-02-22
 */
public class OpenSLESHelper {

  static {
    System.loadLibrary("openslesdemo");
  }

  public native void playPcmByOpenSLES(String path);

  public native void stopPlay();

  public native void recordVoiceToPcm(String path,int sampleRate,int channels,
      int bitRate);

  public native void stopRecord();
}
