help:
	@echo "Comandos:"
	@echo "run: roda código de teste"


run:
	@as lib/coprocessor_lib.s -o t.o
	@gcc edgeDetectorDFM.c t.o -z noexecstack -o DFM -std=c99 -lm
	@rm t.o
	@./DFM
	@rm DFM

runTest:
	@as lib/coprocessor_lib.s -o t2.o
	@gcc referencia.c t2.o -z noexecstack -o test -std=c99 -lm
	@rm t2.o
	@./test
	@rm test
