// v5.2: added group and serial queue approaches.
// v5.1: bug reporting sample sources. (cleaned)
// coded the sample codes (calculate pi) from the HQ slides.
// The problem: exetion time: 14 - 15 seconds. It has taken much longer than
//  Java multi-thread calls.
//
#include <jni.h>
#include <string>
#include <fcntl.h>
#include <android/log.h>
#include <dispatch/dispatch.h>
#include <time.h>
#include <sys/time.h>

#define LOG_TAG "dispatch"

void dispatch_sample(char *);
void display(double val);

extern "C" JNIEXPORT jstring JNICALL
Java_com_hms_localauth_acceleratekitdemo3_MainActivity_calculatePi(
        JNIEnv* env,
        jobject /* this */)
{
    char output[1000], temp[200];
    struct timeval start2, end2;

    dispatch_autostat_enable(env);

    gettimeofday(&start2, NULL);
    dispatch_sample(&output[0]);
    gettimeofday(&end2, NULL);

    long seconds = (end2.tv_sec - start2.tv_sec);
    long micros = ((seconds * 1000000) + end2.tv_usec) - (start2.tv_usec);
    sprintf(temp, "\n C exe time = %ld sec, %ld micros.", seconds, micros);

    strcat(output, temp);

    //__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,
    // "[Java_com_hms_localauth_acceleratekitdemo3_MainActivity_calculatePi(]: out = %s.", output);

    return env->NewStringUTF(output);
}

double calcPi1(long n, int i, int t) {
  double _pi = 0, sign = 1.0;
  long j = 1 + i;
  // pi calc formula: pi/4 = 1 - 1/3 + 1/5 - 1/7 + 1/9 ...
  while (j <= n) {
    sign = (j&1 ? 1 : (-1));
    _pi += (4*sign/(2*j -1));
    j += t;
  }

  //__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[calcPi()]: _pi = %f", _pi);
  return _pi;
}

// v1: Concurrent Queue:
// poor performance: 14 seconds
void dispatch_sample_v1_works(char * output) {
  int i;
  int t = 4;
  __block double pi1 = 0;
  long long N = 10000000000;

  //TRACE_TIME_BEGIN();
  dispatch_queue_t concurr_q = dispatch_queue_create("pi_concurr", DISPATCH_QUEUE_CONCURRENT);
  dispatch_queue_t serial_q = dispatch_queue_create("pi_concurr", DISPATCH_QUEUE_SERIAL);

  for (i = 0; i < t; i++) {
    dispatch_async(concurr_q, ^{
      double _pi = calcPi1(N, i, t);
      dispatch_sync(serial_q, ^{
        pi1 += _pi;
        //__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "dispatch_syn(): pi1 = %f", pi1);
      });
    });
  }

  dispatch_barrier_sync(concurr_q, ^{
      __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "all tasks in the concurrent queue has finished");
  });

  dispatch_release(serial_q);
  dispatch_release(concurr_q);

  sprintf(output, "C Pi = %.17g (Iteration:10^9).", pi1);
}

// v2: dispatch_apply version.
// poor performance: takes 14 seconds.
void dispatch_sample_v2_works(char * output) {
  int i;
  int t = 4;
  __block double pi1 = 0;
  long long N = 10000000000;

  //TRACE_TIME_BEGIN();
  dispatch_queue_t serial_q = dispatch_queue_create("pi_concurr", DISPATCH_QUEUE_SERIAL);

  dispatch_apply(t, DISPATCH_APPLY_AUTO, ^(size_t i){
      //TRCAE_BEGIN("sample2-calcPi1: %d-%d", t, i);
      double _pi = calcPi1(N, i, t);
      dispatch_sync(serial_q, ^{
        pi1 += _pi;
      });
      //TRACE_END();
  });

  dispatch_release(serial_q);

  sprintf(output, "C Pi = %.17g (Iteration:10^9).", pi1);
}

// v3: Group version:
// Best performance: 1 second.
void dispatch_sample(char * output) {
  int i;
  int t = 4;
  __block double pi1 = 0;
  unsigned long cost_time;

  // TRACE_TIME_BEGIN();
  dispatch_queue_t concurr_q = dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0);
  dispatch_queue_t serial_q = dispatch_queue_create("serial", DISPATCH_QUEUE_SERIAL);
  dispatch_group_t grp = dispatch_group_create();

  for (i = 0; i < t; i++) {
    dispatch_group_async(grp, concurr_q, ^{
        double _pi = calcPi1(1000000000, i, t);
        dispatch_sync(serial_q, ^{
          pi1 += _pi;
        });
    });
  }

  dispatch_group_wait(grp, DISPATCH_TIME_FOREVER);

  dispatch_release(grp);
  dispatch_release(serial_q);
  dispatch_release(concurr_q);

  sprintf(output, "C Pi = %.17g (Iteration:10^9).", pi1);
}

// v4: Sequential Queue.
// poor performance: 4 - 5 seconds
void dispatch_sample_v4_works(char * output) {
  int i;
  int t = 4;
  __block double pi1 = 0;
  unsigned long cost_time;

  // TRACE_TIME_BEGIN();
  dispatch_queue_t serial_q = dispatch_queue_create("pi_concurr", DISPATCH_QUEUE_SERIAL);

  for (i = 0; i < t; i++) {
    dispatch_async(serial_q, ^{
        double _pi = calcPi1(1000000000, i, t);
        pi1 += _pi;
    });
  }

  dispatch_sync(serial_q, ^{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Done.");
  });

  dispatch_release(serial_q);

  sprintf(output, "C Pi = %.17g (Iteration:10^9).", pi1);
}
