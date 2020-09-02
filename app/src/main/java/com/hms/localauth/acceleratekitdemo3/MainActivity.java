/*
* v1.1: cleaned.
*/
package com.hms.localauth.acceleratekitdemo3;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private volatile double mPi;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        StringBuffer buffer = new StringBuffer();

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);

        // Accelerate Kit
        long begin = System.nanoTime();
        buffer.append(calculatePi());
        double durationInsecondC = (System.nanoTime() - begin)/1000000000;

        buffer.append("\n  JNI exe time: " + String.valueOf(durationInsecondC) + " seconds.");
        buffer.append("\n\n");

        // Java Multi-threading
        begin = System.nanoTime();
        calcPiMT();
        double durationInsecondJava = (System.nanoTime() - begin)/1000000000;
        buffer.append("  Java(MT) Pi: " + mPi + "\n  (Iteration: 10^9)\n  time: " + durationInsecondJava + " seconds.");

        // Java Single thread brute-force
        begin = System.nanoTime();
        calcPiST();
        durationInsecondJava = (System.nanoTime() - begin)/1000000000;
        buffer.append("\n\n  Java(ST) Pi: " + mPi + "\n  (Iteration: 10^9)\n  time: " + durationInsecondJava + " seconds.");

        tv.setText(buffer.toString());
    }

    private double calcPi1(long n, int i, int t) {
        double _pi = 0, sign = 1.0;
        long j = 1 + i;
        // pi calc formula: pi/4 = 1 - 1/3 + 1/5 - 1/7 + 1/9 ...
        while (j <= n) {
          sign = ((j & 1) == 0L ? (-1) : 1);
          _pi += (4*sign/(2*j -1));
          j += t;
        }

        return _pi;
    }

    /**
    * Multi-threading calculation of Pi.
    */
    private void calcPiMT() {
        final long N = 1000000000;
        int i = 0;
        int m = 10;
        List<Thread> threads = new ArrayList<Thread>();
        Thread t;

        // Start threads ...
        for (; i < m; i++) {
            final int ii = i;
            final int mm = m;
            t = new Thread() {
              @Override
              public void run() {
                  double _pi = calcPi1(N, ii, mm);
                  mPi += _pi;
                  Log.d("dispatch", "Inside thread.  calcPi = " + _pi + ", mPi = " + mPi);
              }
            };

            threads.add(t);
            t.start();
        }

        // Wait until all threads finish.
        for (Thread tt : threads) {
            try {
                tt.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    /**
    * Single threading brute-force calculation of Pi.
    */
    private void calcPiST() {
        final long N = 1000000000;
        mPi = calcPi1(N, 0, 1);
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String calculatePi();
}
