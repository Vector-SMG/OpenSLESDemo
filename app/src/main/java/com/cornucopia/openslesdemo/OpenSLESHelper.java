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
}
