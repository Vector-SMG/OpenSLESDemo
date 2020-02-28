package com.cornucopia.openslesdemo;

import android.content.Context;
import android.os.Environment;
import java.io.File;

/**
 * @author Cornucopia
 * @version 1.0
 * @since 2020-02-03
 */
public class FileUtils {

  public static String queryStoreDir(Context context) {
    File file = context.getExternalFilesDir(Environment.DIRECTORY_MOVIES);
    if (file != null && file.exists()) {
      return file.getAbsolutePath();
    }
    return null;
  }
}
