.PHONY: all clean

all: readout preprocessed.txt

clean:
	rm -f readout

readout:
	${CC} readout.c -o readout

preprocessed.txt:
	${CC} -E preprocessed.inl -o preprocessed.txt