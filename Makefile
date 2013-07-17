CXXFLAGS	+=-O3 -w -c -fpic -I./sdk/include/
LDFLAGS		=-shared -lAnalyzer64 -L./sdk/lib/

all: libSbwAnalyzer.so

libSbwAnalyzer.so: SbwAnalyzer.o SbwAnalyzerResults.o SbwAnalyzerSettings.o SbwSimulationDataGenerator.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o

veryclean: clean
	rm -f libSbwAnalyzer.so
