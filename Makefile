radioday :  radioday.o sunriset.o
	gcc -o radioday radioday.o sunriset.o -lm

sunriset.o : sunriset.c sunriset.h
	gcc -c sunriset.c

radioday.o : radioday.c sunriset.h
	gcc -c radioday.c

clean:
	rm -f *.o
