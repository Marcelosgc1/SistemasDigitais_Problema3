help:
	@echo "Comandos:"
	@echo "run: roda código de teste"


run:
	@as coprocessor_lib.s -o t.o
	@gcc main.c t.o -z noexecstack -o exe -std=c99 -lm
	@rm t.o
	@./exe
	@rm exe
