This project demonstrates how to use Accelerate Kit in the real world example. This project includes the calculation of number Pi using some algorithm with 1000000000 iterations. The C source codes, which uses the multi-thread library in Accelerate Kit,  are shown in the source file native-lib.app (under project_root/app/src/main/cpp).

For comparison, Java multi-threading codes, using the same algorithm as the Accelerate Kit with the same number of iterations, are shown in the source file MainActivity.java (under project_root/app/src/main/java/com/hms/localauth/acceleratekitdemo3).

The calculation execution time, in both C and Java codes, are shown the front screen of the app:
C Pi = 3.14... (Iteration: 10^9)
C exe time: 16 sec, 15384497 micros  # execution time calculated in C codes.
JNI exe time: 15.0 seconds  # the time calculated from Java codes which calls the
                            # C codes via JNI.

Java Pi: 3.14...

For any questions or suggestions, please contact Alan Wu at ywu3@futurewei.com. 
