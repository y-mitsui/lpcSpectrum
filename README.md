lpcSpectrum
===========

Calculate voice's spectrum by Linear Predictive Coding (LPC).




installation:
make


(mingw4.8 may not be able to compile, because of complex function)




usage:<br>
spectrum [option] [wav file]<br>
-l [frame length]<br>
-p [frame shift]<br>
-d [lpc order]<br>
-n [frame number]<br>




examle:
 spectrum -l 320 -p 320 -d 16 -n 15 a.wav  > result.txt
 gnuplot
> plot "result.txt"
