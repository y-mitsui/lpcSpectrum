lpcSpectrum
===========

Calculate voice's spectrum by Linear Predictive Coding (LPC).




##installation
you require gmake.
> make  
(mingw4.8 may not be able to compile, because of complex function)  

##usage
spectrum [option] [wav file]<br>
-l [frame length]<br>
-p [frame shift]<br>
-d [lpc order]<br>
-n [frame number]<br>




examle:<br>
 spectrum -l 320 -p 320 -d 16 -n 15 a.wav  > result.txt<br>
 gnuplot<br>
> plot "result.txt"
