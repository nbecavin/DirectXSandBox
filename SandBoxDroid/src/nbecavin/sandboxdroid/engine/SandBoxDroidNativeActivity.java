package nbecavin.sandboxdroid.engine;

import android.os.Bundle;
import com.nvidia.devtech.*;  
import android.view.MotionEvent;
import android.view.KeyEvent;

public class SandBoxDroidNativeActivity extends NvEventQueueActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
		supportPauseResume = true;
        super.onCreate(savedInstanceState);
    }

    static
    {
        System.loadLibrary("SandBoxDroidNativeLib");
    }
}
