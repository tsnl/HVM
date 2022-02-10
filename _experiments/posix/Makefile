.PHONY: all clean

all: readout tcc-readout preprocessed.txt

clean:
	rm -f readout

readout:
	${CC} readout.c -o readout

tcc-readout:
	tcc readout.c -o tcc-readout

preprocessed.txt:
	${CC} -E preprocessed.inl -o preprocessed.txt
