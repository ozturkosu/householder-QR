
CC = g++

#Note: Change -O to -g when using debugger.
#CFLAGS = -g -Wall
CFLAGS = -O -Wall

all:	householderQR

householderQR.o: householderQR.cpp
	$(CC) $(CFLAGS) -c $<

householderQR: householderQR.o
	$(CC) $< -o $@ -lm

plot: plotresults.m results.m
	octave -q -f --eval plotresults
	epstopdf errors.eps
	epstopdf times.eps

clean:
	rm -f householderQR.o householderQR
