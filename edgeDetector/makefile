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
	@gcc referencia.c -o test -std=c99 -lm
	@./test
	@rm test
