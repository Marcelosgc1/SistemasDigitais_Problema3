#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"

// Roberts 2x2
int roberts(int m[2][5]){

    int mask0[2][2] ={
        {1, 0},
        {0, -1} 
    };

    int mask1[2][2] ={
        {0, 1},
        {-1, 0} 
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int x = sqrt(sumX*sumX + sumY*sumY);
    //printf("%f\n", x);
    return x;
}

// Sobel 3x3
int sobel(int m[3][5]){


    int mask0[3][5] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    int mask1[3][5] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int resultado = sqrt(sumX*sumX + sumY*sumY);

    return resultado;

/*
    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    multiplicacao();
    int x = ler(2, 0, 0);
    escreveMatriz(mask1,3,1);
    multiplicacao();
    int y = ler(2,0,0);
*/

    

}

// PreWitt 3x3 
int preWitt(int m[3][5]){

    int mask0[3][5] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    
    int mask1[3][5] = {
        {-1,-1,-1},
        {0,0,0},
        {1,1,1}
    };

    int sumX = 0, sumY = 0;
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
            sumY = sumY + mask1[i][j] * m[i][j]; 
        }
    }

    int resultado = sqrt(sumX*sumX + sumY*sumY);

    return resultado;
/*
    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    multiplicacao();
    int x = ler(2, 0, 0);
    escreveMatriz(mask1,3,1);
    multiplicacao();
    int y = ler(2,0,0);

    int resultado = sqrt(x*x + y*y);
*/
    

}

// Sobel Expandido 5x5
int sobel_expandido(int m[5][5]) {

    int sobel5x5_X[5][5] = {
        { 2, 2, 4, 2, 2 },
        { 1, 1, 2, 1, 1 },
        { 0, 0, 0, 0, 0 },
        { -1, -1, -2, -1, -1 },
        { -2, -2, -4, -2, -2 }
    };

    int sobel5x5_Y[5][5] = {
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 },
        { 4, 2, 0, -2, -4 },
        { 2, 1, 0, -1, -2 },
        { 2, 1, 0, -1, -2 }
    };

    int sumX = 0, sumY = 0;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            sumX += sobel5x5_X[i][j] * m[i][j];
            sumY += sobel5x5_Y[i][j] * m[i][j];
        }
    }

    int result = sqrt(sumX * sumX + sumY * sumY);

    //printf("\n %d", result);
    return result;
  
}

// Laplaciano 5x5
int laplaciano(int m[5][5]){
    
    int mask0[5][5] = {
    { 0,  0,  -1,  0,  0 },
    { 0, -1,  -2, -1,  0 },
    {-1, -2,  16, -2, -1 },
    { 0, -1,  -2, -1,  0 },
    { 0,  0,  -1,  0,  0 }
    };

    int sumG = 0;

    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            sumG = sumG + mask0[i][j] * m[i][j];
        }
    }

    return sumG;
    
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
            if (y < 0) y = 0;
            if (y >= altura) y = altura - 1;
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

    if(operacao == 4){
        return sobel_expandido(matriz_temp);
    } else if(operacao == 5){
        return laplaciano(matriz_temp);
    }

    return 0;
}

int funcTeste3x3(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao){

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
            //printf("%d/%d\n", w, z);
        }
        coluna = 0;
        linha++;
    }

    if(operacao == 2){
        return sobel(matriz_temp);
    } else if(operacao == 3){
        return preWitt(matriz_temp);
    }

    return 0;
}

int funcTeste2x2(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    int matriz_temp[2][5];    
    
    int linha = 0, coluna = 0;

    for (int w = i; w < (i + 2); w++){
        for (int z = j; z < (j + 2); z++){
            matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
            //printf("%d/%d\n", w, z);
        }
        coluna = 0;
        linha++;
    }

    return roberts(matriz_temp);
}

int calcularGeratriz(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int operacao){
    if(operacao == 2 || operacao == 3){
        funcTeste3x3(dados, i, j, larg_dados, tamanho, operacao);
    }else if(operacao == 4 || operacao == 5){
        funcTeste5x5(dados, i, j, larg_dados, tamanho, operacao);
    }else{
        funcTeste2x2(dados, i, j, larg_dados, tamanho);
    }
}

int main() {
    const char *input_filename = "data/Lena.jpeg";
    char *output_filename = "foto.png";

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

    printf("\n%d %d", width, height);
    // Aplicar Sobel 5x5 (armazenando valores temporários)
    double max_value = 0.0;
    int operacao = 0;
    printf("\nDIGITE O FILTRO DESEJADO: ");
    printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): ");
    scanf("%d", &operacao);
    while (operacao > 0 && operacao < 6){
        switch(operacao){
            case 1:
                output_filename = "outputC/roberts.png";
                break;
            case 2:
                output_filename = "outputC/sobel.png";
                break;
            case 3:
                output_filename = "outputC/prewit.png";
                break;
            case 4:
                output_filename = "outputC/sobel_expandido.png";
                break;
            case 5:
                output_filename = "outputC/laplaciano.png";
                break;
            default:
                break;
        }

        for (int y = 0; y < height - 1; y++) {
            for (int x = 0; x < width - 1; x++) {
                int temporario = calcularGeratriz(data, y, x, width, height, operacao);
                if (temporario>255){
                    temporario=255;
                }else if (temporario<0){
                    temporario=0;
                }
                output_data[y * width + x] = temporario;
                
            }
        }
        
        stbi_write_png(output_filename, width, height, 1, output_data, width);
        printf("Imagem salva como '%s'\n", output_filename);

        printf("\nDIGITE O FILTRO DESEJADO ");
        printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] Sair do Programa: ");
        scanf("%d", &operacao);
    }

    // Liberar memória
    stbi_image_free(data);
    free(temp_data);
    free(output_data);

    return 0;
}
