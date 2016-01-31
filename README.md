lpcSpectrum
===========

Calculate voice's spectrum by Linear Predictive Coding (LPC).  

###install
you require gmake.
> make  

(mingw4.8 may not be able to compile, because of complex function)  

###usage
spectrum [option] [wav file]  
-l [frame length]  
-p [frame shift]  
-d [lpc order]  
-n [frame number]  




###example
> spectrum -l 320 -p 320 -d 16 -n 15 a.wav  > result.txt  
> gnuplot  
> plot "result.txt"  


License:new BSD 
