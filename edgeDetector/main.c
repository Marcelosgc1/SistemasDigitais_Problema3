#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "dafema.h"


void escreverMatriz2(int matriz[][5], int tamanho, int matrizId){

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

    int mask0[2][5] ={
        {1, 0},
        {0, -1} 
    };
    printf("eba");
    escreverMatriz2(mask0, 2, 1);
}

// Sobel 3x3
int sobel(){


    int mask0[3][5] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    
    escreverMatriz2(mask0, 3, 1);
}

// PreWitt 3x3 
int preWitt(){

    int mask0[3][5] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };

    escreverMatriz2(mask0, 3, 1);
}

// Sobel Expandido 5x5
int sobel_expandido() {

    int mask0[5][5] = {
        { 2, 2, 4, 2, 2 },
        { 1, 1, 2, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { -1, -1, -2, -1, -1 },
        { -2, -2, -4, -2, -2 }
    };

    int mask1[5][5] = {
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 },
        { 4, 2, 0, -2, -4 },
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 }
    };

    escreverMatriz2(mask0, 5, 1);
  
}

// Laplaciano 5x5
int laplaciano(){
    
    int mask0[5][5] = {
    { 0,  0,  -1,  0,  0 },
    { 0, -1,  -2, -1,  0 },
    {-1, -2,  16, -2, -1 },
    { 0, -1,  -2, -1,  0 },
    { 0,  0,  -1,  0,  0 }
    };

    escreverMatriz2(mask0, 5, 1);
    
}

// Geração da matriz 5x5 com tratamento de bordas (espelhamento)
int funcTeste5x5(unsigned char *dados, int i, int j, int largura, int altura, int operacao) {
    /* 
    int matriz_temp[5][5];

    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int y = i + linha - 2;
            int x = j + coluna - 2;

            // Tratamento de borda por replicação
            if (y < 0) y = 0;           if (y >= altura) y = altura - 1;
            if (x < 0) x = 0;
            if (x >= largura) x = largura - 1;

            matriz_temp[linha][coluna] = dados[y * largura + x];
        }
    }

    return sobel_expandido(matriz_temp);
    */

    int matriz_temp[5][5];
    int linha = 0;
    int coluna = 0;  

    // printf("\n %d %d", i, j);
    for(int linhaTemp = i - 2; linhaTemp < (i + 3); linhaTemp++){
        for(int colunaTemp = j - 2; colunaTemp < (j + 3); colunaTemp++){
            matriz_temp[linha][coluna] = dados[linhaTemp* largura + colunaTemp];
            coluna++;
        }
        coluna = 0;
        linha++;
    }
    escreverMatriz2(matriz_temp, 5, 0);
    int x = lerIndice(2,0,0);
    if(operacao == 4){
        convolucaoParalela();
        return sqrt(pow(x>>8,2) + pow(x&255, 2));
    }else{
        convolucao();
        return x&255;
    }
    return 0;
}

int funcTeste3x3(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    int matriz_temp[3][5];
    tamanho--;
    
    
    int linha = 0, coluna = 0;

    // Montando 3x3 parcial
    for (int w = i - 1; w < (i + 2); w++){
        for (int z = j - 1; z < (j + 2); z++){
            if ((w < 0 || w > tamanho) && (z < 0 || z > tamanho)) matriz_temp[linha][coluna] = dados[i*larg_dados + j];
            else if (w < 0) matriz_temp[linha][coluna] = dados[(w+1)*larg_dados + z];
            else if (w > tamanho) matriz_temp[linha][coluna] = dados[(w-1)*larg_dados + z];
            else if (z < 0) matriz_temp[linha][coluna] = dados[w*larg_dados + z + 1];
            else if (z > tamanho) matriz_temp[linha][coluna] = dados[w*larg_dados + z - 1];
            else matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    /*
    if(operacao == 2){
        return sobel(matriz_temp);
    } else if(operacao == 3){
        return preWitt(matriz_temp);
    }
    */
    escreverMatriz2(matriz_temp, 3, 0);
    convolucaoParalela();
    int x = ler(2,0,0);
    return sqrt(pow(x>>8,2) + pow(x&255, 2));
    //return ler(2,0,0);
}

int funcTeste2x2(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    int matriz_temp[2][5];    
    
    int linha = 0, coluna = 0;

    for (int w = i; w < (i + 2); w++){
        for (int z = j; z < (j + 2); z++){
            matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    escreverMatriz2(matriz_temp, 2, 0);
    convolucaoRoberts();
    int x = ler(2,0,0);
    return sqrt(pow(x>>8,2) + pow(x&255, 2));
}

int funcprincipal(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao){
    if(operacao == 2 || operacao == 3){
        funcTeste3x3(dados, i, j, larg_dados, tamanho);
    }else if(operacao == 4 || operacao == 5){
        funcTeste5x5(dados, i, j, larg_dados, tamanho, operacao);
    }else{
        funcTeste2x2(dados, i, j, larg_dados, tamanho);
    }
}


int main() {
    const char *input_filename = "lenna.jpeg";
    char *output_filename = "foto.png";
    iniciarDafema();
    int width, height, channels;
    unsigned char *data = stbi_load(input_filename, &width, &height, &channels, 1);
    if (!data) {
        printf("Erro ao carregar imagem '%s'\n", input_filename);
        return 1;
    }

    double *temp_data = malloc(width * height * sizeof(double));
    unsigned char *output_data = malloc(width * height * sizeof(unsigned char));

    if (!temp_data || !output_data) {
        printf("Erro ao alocar memória.\n");
        stbi_image_free(data);
        free(temp_data);
        free(output_data);
        return 1;
    }

    // Aplicar Sobel 5x5 (armazenando valores temporários)
    double max_value = 0.0;
    
    int operacao = 0;
    while (operacao < 6){
        printf("\nDIGITE O FILTRO DESEJADO ");
        printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] Sair do Programa: ");
        scanf("%d", &operacao);
        printf("teste");
        if(operacao >= 1 || operacao <6){
            switch(operacao){
                case 1:
                    roberts();
                    output_filename = "roberts.png";
                    break;
                case 2:
                    sobel();
                    output_filename = "sobel.png";
                    break;
                case 3:
                    preWitt();
                    output_filename = "prewit.png";
                    break;
                case 4:
                    sobel_expandido();
                    output_filename = "sobel_expandido.png";
                    break;
                case 5:
                    laplaciano();
                    output_filename = "laplaciano.png";
                    break;
                default:
                    break;
            }

            for (int y = 0; y < height - 1; y++) {
                for (int x = 0; x < width - 1; x++) {
                    int temporario = funcprincipal(data, y, x, width, height, operacao);
                    if (temporario>255){
                        temporario=255;
                    }else if (temporario<0){
                        temporario=0;
                    }
                    output_data[y * width + x] = temporario;
                    
                }
            }
        }
        if(operacao < 6){
             stbi_write_png(output_filename, width, height, 1, output_data, width);
             printf("Imagem salva como '%s'\n", output_filename);
        }else{
            break;
        }
    // Liberar memória
    stbi_image_free(data);
    free(temp_data);
    free(output_data);

    return 0;
}
}
