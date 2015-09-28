.PHONY: clean

PYTHON_CONFIG = python3-config

CFLAGS   = `$(PYTHON_CONFIG) --cflags`
INCLUDES = `$(PYTHON_CONFIG) --includes`
LIBS     = `$(PYTHON_CONFIG) --libs`
LD_PATH  = `$(PYTHON_CONFIG) --prefix`/lib

rwatch.so: rwatch.c hook.c ceval.o
	gcc $(CFLAGS) $(INCLUDES) -L$(LD_PATH) -Wl,--export-dynamic -fPIC -shared -o $@ $^ -ldl $(LIBS)

ceval.o: ceval.c
	gcc $(CFLAGS) $(INCLUDES) -fPIC -DPy_BUILD_CORE -c -o $@ $^

clean:
	rm -f rwatch.so ceval.o
