help:
	@echo "Comandos:"
	@echo "run: roda código de teste"
	@echo "send: envia projeto p/ de1-soc, use send ip=x, sendo x o ip da placa"


run:
	@as coprocessor_lib.s -o t.o
	@gcc main.c t.o -z noexecstack -o exe -std=c99 -lm
	@rm t.o
	@./exe
	@rm exe

send:
	@scp -r ../edgeDetector user@172.65.213.$(ip):/home/user/TEC499/TP01/G03/

