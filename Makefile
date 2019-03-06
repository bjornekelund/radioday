radioday :  radioday.o sunriset.o
	gcc -o radioday radioday.o sunriset.o -lm

delta : delta.o
	gcc -o delta delta.o -lm

sunriset.o : sunriset.c sunriset.h
	gcc -c sunriset.c

radioday.o : radioday.c sunriset.h
	gcc -c radioday.c

delta.o : delta.c
	gcc -c delta.c

clean: 
	rm --force *.o
