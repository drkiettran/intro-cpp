CPP_APPS = file_read_0

all: ${CPP_APPS} ${CU_APPS}

%: %.cu
	nvcc -g -G -I../tclap-1.4.0-rc1/include -o $@ $<
%: %.cpp
	g++ -g -I../tclap-1.4.0-rc1/include -o $@ $<

clean:
	rm -f ${CU_APPS} ${C_APPS}