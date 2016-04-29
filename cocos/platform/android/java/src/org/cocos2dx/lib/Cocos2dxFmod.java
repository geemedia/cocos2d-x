package org.cocos2dx.lib;

import android.content.Context;
import android.util.Log;

import org.fmod.FMODAudioDevice;

public class Cocos2dxFmod {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = "Cocos2dxFmod";

    // ===========================================================
    // Fields
    // ===========================================================
    private final Context mContext;
    private FMODAudioDevice mFmodAudioDevice = null;

    static 
    {
        System.loadLibrary("fmodex");
    }

    // ===========================================================
    // Constructors
    // ===========================================================

    public Cocos2dxFmod(final Context context) {
        this.mContext = context;
        mFmodAudioDevice = new FMODAudioDevice();
    }

    public void start() {   
        mFmodAudioDevice.start();
    }

    public void stop() {
        mFmodAudioDevice.stop();
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================
}
