#ifndef DAFEMA_H
#define DAFEMA_H
#include <stdlib.h>

/*
 * Stuct que indica um indice de uma matriz
 *
 * matrizId: unsig char
 * 0 => matriz A
 * 1 => matriz B
 * 2 => matriz C
 * 
 * linha: unsig char
 * coluna: unsig char
 */
typedef struct dafemaEndereco{
    unsigned char matrizId;
    unsigned char linha;
    unsigned char coluna;
} endereco;

/* Mapeia a memória para o endereço base do AXI Bridge*/
void iniciarDafema();

/* Libera memória do endereço para o AXI Bridge*/
void encerrarDafema();

/* Escreve dois números no endereço indicado
 * n0: Valor escrito no endereço indicado
 * n1: Valor escrito no próximo espaço da matriz
 * endereco: ponteiro p/ struct cujo dados serão escritos
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido 
 */
void escreverASM(signed char n0, signed char n1, endereco *endereco);

/* Escreve dois números no endereço indicado
 * n0: Valor escrito no endereço indicado
 * n1: Valor escrito no próximo espaço da matriz
 * matrizId: Indica qual a matriz que está sendo escrita
 * 0 => matriz A
 * 1 => matriz B
 * 2 => matriz C
 * linha: de 0 a 4
 * coluna: de 0 a 4
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido 
 */
void escrever(signed char n0, signed char n1, unsigned char matrizId, unsigned char linha, unsigned char coluna){
    endereco *__DEATHCONSCIOUSNESS__  = malloc(3 * sizeof(unsigned char));
    __DEATHCONSCIOUSNESS__->matrizId  = matrizId;
    __DEATHCONSCIOUSNESS__->linha     = linha;
    __DEATHCONSCIOUSNESS__->coluna    = coluna;
    escreverASM(n0, n1, __DEATHCONSCIOUSNESS__);
    free(__DEATHCONSCIOUSNESS__);
}

/* Escreve dois números no endereço indicado
 * num: Valor escrito no endereço indicado
 * endereco: ponteiro p/ struct cujo dados serão escritos
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido 
 */
void escreverIndiceASM(signed char num, endereco *endereco);

/* Escreve dois números no endereço indicado
 * num: Valor escrito no endereço indicado
 * matrizId: Indica qual a matriz que está sendo escrita
 * 0 => matriz A
 * 1 => matriz B
 * 2 => matriz C
 * linha: de 0 a 4
 * coluna: de 0 a 4
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido 
 */
void escreverIndice(signed char num, unsigned char matrizId, unsigned char linha, unsigned char coluna){
    endereco *__MASSOFTHEFERMENTINGDREGS__  = malloc(3 * sizeof(unsigned char));
    __MASSOFTHEFERMENTINGDREGS__->matrizId  = matrizId;
    __MASSOFTHEFERMENTINGDREGS__->linha     = linha;
    __MASSOFTHEFERMENTINGDREGS__->coluna    = coluna;
    escreverIndiceASM(num, __MASSOFTHEFERMENTINGDREGS__);
    free(__MASSOFTHEFERMENTINGDREGS__);
}

/* Lê dois números no endereço indicado
 * endereco: ponteiro p/ struct cujo dados serão lido
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido
 * 
 * retorno: pacote de 16 bits contendo n0 nos 8 bits menos
 * significativos e n1 nos 8 bits mais significativos
 */
unsigned int lerASM(endereco *endereco);

/* Lê dois números no endereço indicado
 * matrizId: Indica qual a matriz que está sendo escrita
 * 0 => matriz A
 * 1 => matriz B
 * 2 => matriz C
 * linha: de 0 a 4
 * coluna: de 0 a 4
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido
 * 
 * retorno: pacote de 16 bits contendo n0 nos 8 bits menos
 * significativos e n1 nos 8 bits mais significativos
 */
unsigned int ler(unsigned char matrizId, unsigned char linha, unsigned char coluna){
    endereco *__NUMBERGIRL__  = malloc(3 * sizeof(unsigned char));
    __NUMBERGIRL__->matrizId  = matrizId;
    __NUMBERGIRL__->linha     = linha;
    __NUMBERGIRL__->coluna    = coluna;
    unsigned int SAPPUKEI = lerASM(__NUMBERGIRL__);
    free(__NUMBERGIRL__);
    return SAPPUKEI;
}

/* Lê um valor da matriz 
 * endereco: ponteiro p/ struct cujo dados serão lido
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido
 * 
 * retorno: valor de 8 bits lido do endereco indicado
 */
signed char lerIndiceASM(endereco *endereco);

/* Lê um valor da matriz 
 * matrizId: Indica qual a matriz que está sendo escrita
 * 0 => matriz A
 * 1 => matriz B
 * 2 => matriz C
 * linha: de 0 a 4
 * coluna: de 0 a 4
 * 
 * exceção: caso endereço não esteja parametrizado corretamente,
 * o programa é terminado e o código de erro 255 é emitido
 * 
 * retorno: valor de 8 bits lido do endereco indicado
 */
signed char lerIndice(unsigned char matrizId, unsigned char linha, unsigned char coluna){
    endereco *__RADIOHEAD__  = malloc(3 * sizeof(unsigned char));
    __RADIOHEAD__->matrizId  = matrizId;
    __RADIOHEAD__->linha     = linha;
    __RADIOHEAD__->coluna    = coluna;
    signed char OKCOMPUTER = lerIndiceASM(__RADIOHEAD__);
    free(__RADIOHEAD__);
    return OKCOMPUTER;
}

/* Multiplica matriz A (0) por char 
 * 
 * O resultado é escrito na matriz C (2)
 */
void multiEscalar(char multi);

/* Realiza convolução da matriz A, usando matriz B como kernel 
 *
 * O resultado é escrito na matriz C (2)
 * Na posição [0][0]
 */
void convolucao();

/* Soma matrizes A e B  
 * 
 * O resultado é escrito na matriz C (2)
 */
void soma();

/* Subtrai matriz A por matriz B  
 * 
 * O resultado é escrito na matriz C (2)
 */
void subtracao();

/* Realiza convolução da matriz A, usando matriz B como kernel 
 * É usada a matriz B rotacionada 45 graus (2x2) como segundo kernel 
 *
 * O resultado é escrito na matriz C (2)
 * Resultado da 1ª convolução:      [0][0]
 * Resultado da 2ª convolução:      [0][1]
 * Resultado da soma de resultados: [0][2]
 */
void convolucaoRoberts();

/* Realiza convolução da matriz A, usando matriz B como kernel 
 * É usada a matriz B transposta como segundo kernel 
 *
 * O resultado é escrito na matriz C (2)
 * Resultado da 1ª convolução:      [0][0]
 * Resultado da 2ª convolução:      [0][1]
 * Resultado da soma de resultados: [0][2]
 */
void convolucaoParalela();

/* Calcula de determinante da matriz A 2x2  
 * 
 * O resultado é escrito na matriz C (2), linha 0 e coluna 0
 */
void determinante2x2();

/* Calcula de determinante da matriz A 3x3   
 * 
 * O resultado é escrito na matriz C (2), linha 0 e coluna 0
 */
void determinante3x3();

/* Calcula de determinante da matriz A 4x4   
 * 
 * O resultado é escrito na matriz C (2), linha 0 e coluna 0
 */
void determinante4x4();

/* Calcula de determinante da matriz A 5x5   
 * 
 * O resultado é escrito na matriz C (2), linha 0 e coluna 0
 */
void determinante5x5();


#endif
