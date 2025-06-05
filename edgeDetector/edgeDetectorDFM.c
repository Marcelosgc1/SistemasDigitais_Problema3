#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"
#include "lib/dafema.h"


void escreverMatriz(int matriz[][5], int tamanho, int matrizId){
    int linha = 0, coluna = 0, n0 = 0, n1 = 0, temp = 0;

    do{
        n0 = (linha < tamanho && coluna < tamanho) ? matriz[linha][coluna] : 0;
        n1 = ((linha + (coluna+1>4)) < tamanho && ((coluna+1)%5) < tamanho) ? matriz[linha + (coluna+1>4)][(coluna+1)%5] : 0;
        escrever(n0,n1,matrizId,linha,coluna);
        linha = linha + (coluna+1>3);
        coluna = (coluna>2) ? coluna%3 : coluna + 2;
    } while (linha < 5 && coluna < 5);
    
}



// Roberts 2x2
void roberts(){
    int mascara[2][5] ={
        {1,  0},
        {0, -1} 
    };

    escreverMatriz(mascara, 2, 1);
}

// Sobel 3x3
void sobel(){
    int mascara[3][5] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
        
    escreverMatriz(mascara, 3, 1);
}

// PreWitt 3x3 
void prewitt(){
    int mascara[3][5] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };

    escreverMatriz(mascara, 3, 1);
}

// Sobel Expandido 5x5
void sobel_expandido() {
    int mascara[5][5] = {
        {  2,  2,  4,  2,  2 },
        {  1,  1,  2,  1,  1 },
        {  0,  0,  0,  0,  0 },
        { -1, -1, -2, -1, -1 },
        { -2, -2, -4, -2, -2 }
    };

    escreverMatriz(mascara, 5, 1);
}

// Laplaciano 5x5
void laplaciano(){
    
    int mascara[5][5] = {
    { 0,  0,  -1,  0,  0 },
    { 0, -1,  -2, -1,  0 },
    {-1, -2,  16, -2, -1 },
    { 0, -1,  -2, -1,  0 },
    { 0,  0,  -1,  0,  0 }
    };

    escreverMatriz(mascara, 5, 1);
    
}


int geratriz5x5(unsigned char *dados, int i, int j, int largura, int tamanho, int operacao, int somaFpga) {

    int matriz_temp[5][5];
    int linha = 0, coluna = 0, resultado;
    tamanho--;

    for(int w = i - 2; w < (i + 3); w++){
        for(int z = j - 2; z < (j + 3); z++){
            if ((w < 0 || w > tamanho) && (z < 0 || z > tamanho)) matriz_temp[linha][coluna] = dados[i*largura + j];
            else if (w < 0 || w > tamanho) matriz_temp[linha][coluna] = dados[i*largura + z];
            else if (z < 0 || z > tamanho) matriz_temp[linha][coluna] = dados[w*largura + j];
            else matriz_temp[linha][coluna] = dados[w*largura + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    escreverMatriz(matriz_temp, 5, 0);
    if(operacao == 4){
        convolucaoParalela();
        resultado = ler(2,0,0);
	    resultado = somaFpga ? ler(2,0,2) : sqrt(pow(resultado>>8, 2) + pow(resultado&255, 2));
	    return resultado;

    }else{
        convolucao(i, j);
        resultado = ler(2,0,0);
        return resultado&255;
    }
    return 0;
}

int geratriz3x3(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int somaFpga){

    int matriz_temp[3][5];
    int linha = 0, coluna = 0, resultado;
    tamanho--;

    for (int w = i - 1; w < (i + 2); w++){
        for (int z = j - 1; z < (j + 2); z++){
            if ((w < 0 || w > tamanho) && (z < 0 || z > tamanho)) matriz_temp[linha][coluna] = dados[i*larg_dados + j];
            else if (w < 0 || w > tamanho) matriz_temp[linha][coluna] = dados[i*larg_dados + z];
            else if (z < 0 || z > tamanho) matriz_temp[linha][coluna] = dados[w*larg_dados + j];
            else matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    escreverMatriz(matriz_temp, 3, 0);
    convolucaoParalela();
    resultado = ler(2,0,0);
    resultado = somaFpga ? ler(2,0,2) : sqrt(pow(resultado>>8,2) + pow(resultado&255, 2));
    return resultado;
}

int geratriz2x2(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int somaFpga){

    int matriz_temp[2][5];    
    int linha = 0, coluna = 0, resultado;
    tamanho--;

    for (int w = i; w < (i + 2); w++){
        for (int z = j; z < (j + 2); z++){
            if      (w>tamanho && z>tamanho) matriz_temp[linha][coluna] = dados[i*larg_dados + j];
            else if (w>tamanho) matriz_temp[linha][coluna] = dados[i*larg_dados + z];
            else if (z>tamanho) matriz_temp[linha][coluna] = dados[w*larg_dados + j];
            else                matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    escreverMatriz(matriz_temp, 2, 0);
    convolucaoRoberts();
    resultado = ler(2,0,0);
    resultado = somaFpga ? ler(2,0,2) : sqrt(pow(resultado>>8, 2) + pow(resultado&255, 2));
    return resultado;
}

int calcularGeratriz(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao, int somaFpga){
    int resultado;
    if(operacao == 2 || operacao == 3){
        resultado = geratriz3x3(dados, i, j, larg_dados, tamanho, somaFpga);
    }else if(operacao == 4 || operacao == 5){
        resultado = geratriz5x5(dados, i, j, larg_dados, tamanho, operacao, somaFpga);
    }else{
        resultado = geratriz2x2(dados, i, j, larg_dados, tamanho, somaFpga);
    }
    return resultado;
}


int main() {
    const char *inputImagem = "data/clock.png";
    char *outputImagem = "foto.png";
    int larguraImg, alturaImg, channels, operacao, somaFpga = 0;
    unsigned char *dadosImagem = stbi_load(inputImagem, &larguraImg, &alturaImg, &channels, 1);
    unsigned char *novaImagem = malloc(larguraImg * alturaImg * sizeof(char));



    if (!dadosImagem) {
        printf("Erro ao carregar imagem '%s'\n", inputImagem);
        return 1;
    }
    if (!novaImagem) {
        printf("Erro ao alocar memória.\n");
        stbi_image_free(dadosImagem);
        free(novaImagem);
        return 1;
    }


    iniciarDafema();

    printf("\nDIGITE O FILTRO DESEJADO ");
    printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] Sair do Programa: ");
    scanf("%d", &operacao);

    while (operacao > 0 && operacao < 6){
        if (operacao!=5){
            printf("\nVocê prefere realizar a soma pitagórica [0], ou a soma do módulo das geratrizes [1]?");
            scanf("%d", &somaFpga);
        }

        clock_t comecoProcedimento = clock();
        
        switch(operacao){
            case 1:
                roberts();
                outputImagem = "outputDFM/roberts_";
                break;
            case 2:
                sobel();
                outputImagem = "outputDFM/sobel_";
                break;
            case 3:
                prewitt();
                outputImagem = "outputDFM/prewit_";
                break;
            case 4:
                sobel_expandido();
                outputImagem = "outputDFM/sobel_expandido_";
                break;
            case 5:
                laplaciano();
                outputImagem = "outputDFM/laplaciano_";
                break;
            default:
                break;
        }

        for (int y = 0; y < alturaImg - 1; y++) {
            for (int x = 0; x < larguraImg - 1; x++) {
                int geratriz = calcularGeratriz(dadosImagem, y, x, larguraImg, alturaImg, operacao, somaFpga);
                if (geratriz > 255){
                    geratriz = 255;
                }else if (geratriz < 0){
                    geratriz = 0;
                }
                novaImagem[y * larguraImg + x] = geratriz;
                
            }
        }

        clock_t fimProcedimento = clock();
        double tempo = (fimProcedimento - comecoProcedimento) / CLOCKS_PER_SEC;
        printf("\nEste procedimento durou: %.6f segundos\n", tempo);
    
    
        char *nomeImagem = inputImagem + 5;
        strcat(outputImagem, nomeImagem);
        stbi_write_png(outputImagem, larguraImg, alturaImg, 1, novaImagem, larguraImg);
        printf("Imagem salva como '%s'\n", outputImagem);

        printf("\nDIGITE O FILTRO DESEJADO ");
        printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] Sair do Programa: ");
        scanf("%d", &operacao);
    }

    // Liberar memória
    encerrarDafema();
    stbi_image_free(dadosImagem);
    free(novaImagem);

    return 0;
}

