#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include <stdio.h>
#include <math.h>
#include "dafema.h"


void printMatriz(unsigned char matriz[][3], int tamanho){
    printf("Imprimindo matriz %dx%d:\n",tamanho,tamanho);
    for (int i = 0; i < tamanho; i++){
        for (int j = 0; j < tamanho; j++){
            printf("%d\t", matriz[i][j]);
        }
        printf("\n");
    }
}

int sharpen(unsigned char m[3][3]){

    int mask0[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    int sumX = 0;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            sumX = sumX + mask0[i][j] * m[i][j];
        }
    }

    //printf("%f\n", x);

    
    return sumX;


}

void escreveMatriz(int matriz[][5], int tamanho, int matrizId){
    for (int i = 0; i < tamanho; i++){
        for (int j = 0; j < tamanho; j++){
            escreverIndice(matriz[i][j], matrizId, i, j);
        }
    }
}


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

    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    multiplicacao();
    int x = ler(2, 0, 0);
    escreveMatriz(mask1,3,1);
    multiplicacao();
    int y = ler(2,0,0);

    int resultado = sqrt(x*x + y*y);
    return resultado;

}

int sobel_expandido(int m[5][5]){
    // Kernel Sobel 5x5 para eixo X
    int sobel5x5_X[5][5] = {
        { -1, -2, 0, 2, 1 },
        { -4, -8, 0, 8, 4 },
        { -6, -12, 0, 12, 6 },
        { -4, -8, 0, 8, 4 },
        { -1, -2, 0, 2, 1 }
    };

    // Kernel Sobel 5x5 para eixo Y (transposto de X)
    int sobel5x5_Y[5][5] = {
        { -1, -4, -6, -4, -1 },
        { -2, -8, -12, -8, -2 },
        {  0,  0,   0,  0,  0 },
        {  2,  8,  12,  8,  2 },
        {  1,  4,   6,  4,  1 }
    };

     int sumX = 0;
     int sumY = 0;

     for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            sumX = sumX + sobel5x5_X[i][j] * m[i][j];
            sumY = sumY + sobel5x5_Y[i][j] * m[i][j]; 
        }
    }

    double x = sqrt(sumX*sumX + sumY*sumY);
    //printf("%f\n", x);
    return x;

}

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

    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    multiplicacao();
    int x = ler(2, 0, 0);
    escreveMatriz(mask1,3,1);
    multiplicacao();
    int y = ler(2,0,0);

    int resultado = sqrt(x*x + y*y);
    return resultado;

}

int roberts(unsigned char m[2][2]){

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

    double x = sqrt(sumX*sumX + sumY*sumY);
    //printf("%f\n", x);
    return x;
}

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



int funcTeste2x2(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    unsigned char matriz_temp[2][2];
    tamanho--;

    int linha = 0;
    int coluna = 0;

    for(int w = i; w < (i + 1); w++){
        for(int z = j; z < (j + 1); z++){
            matriz_temp[linha][coluna] = dados[w*larg_dados + z];
            coluna++;
        }
        coluna = 0;
        linha++;
    }

    return roberts(matriz_temp);

}

int funcTeste3x3(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int tipo){

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

    if(tipo == 1){
        return sobel(matriz_temp);
    } else if(tipo == 3){
        return preWitt(matriz_temp);
    }

}

int funcTeste5x5(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int tipo){
    /*int matriz_temp[5][5];
    int linha = 0;
    int coluna = 0;  

    for(int linhaTemp = i - 2; linhaTemp < (i + 4); linhaTemp++){
        for(int colunaTemp = j - 2; colunaTemp < (j + 4); colunaTemp++){
            matriz_temp[linha][coluna] = dados[linhaTemp*larg_dados + colunaTemp];
            coluna++;
        }
        coluna = 0;
        linha++;
    }
    
    if(tipo == 2){
        return sobel_expandido(matriz_temp);
    } else if(tipo == 5){
        return laplaciano(matriz_temp);
    }

    return 0;*/

    int matriz_temp[5][5];
    tamanho--; // tamanho - 1 para ajustar índices válidos

    int linha = 0, coluna = 0;

    for (int w = i - 2; w <= i + 2; w++) {
        for (int z = j - 2; z <= j + 2; z++) {

            int w_corrigido = w;
            int z_corrigido = z;

            // Correção para bordas (espelhamento simples)
            if (w < 0) w_corrigido = 0;
            else if (w > tamanho) w_corrigido = tamanho;

            if (z < 0) z_corrigido = 0;
            else if (z > tamanho) z_corrigido = tamanho;

            matriz_temp[linha][coluna] = dados[w_corrigido * larg_dados + z_corrigido];

            coluna++;
        }
        coluna = 0;
        linha++;
    }

    if(tipo == 2){
        return sobel_expandido(matriz_temp);
    } else if(tipo == 5){
        return laplaciano(matriz_temp);
    }

    return 0; 
}



int function_principal(unsigned char *dados, int i, int j, int larg_dados, int tamanho, int tipo){
    if(tipo == 1 || tipo == 3){
        return funcTeste3x3(dados, i, j, larg_dados, tamanho, tipo);
    }else if(tipo == 2 || tipo == 5){
        return funcTeste5x5(dados, i, j, larg_dados, tamanho, tipo);
    }else{
        return funcTeste2x2(dados, i, j, larg_dados, tamanho);
    }
}

void preencherMatriz(unsigned char *data, unsigned char *new_data, int larg_dados){
    for(int i = 0; i < 1; i++){
        for(int j = 0; j < 1; j++){
            new_data[i*larg_dados + j] = data[i*larg_dados + j];
        }
    }

    for(int i = 317; i < 318; i++){
        for(int j = 317; j < 318; j++){
            new_data[i*larg_dados + j] = data[i*larg_dados + j];
        }
    }

}

int main() {
    const char *filename = "lenna.jpeg"; // Substitua pelo caminho da sua imagem
    int width, height, channels, tipo;
    iniciarDafema();    

    // Carregar a imagem (formato: RGB ou RGBA, dependendo da imagem)
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 1);

    unsigned char *new_data = malloc(sizeof(unsigned char) * 319 * 319);

    if (!data) {
        printf("Erro ao carregar a imagem!\n");
        return -1;
    }

    printf("Imagem carregada: %dx%d, %d canais\n\n", width, height, channels);

    printf("FILTROS:\n Sobel(3x3) [1]\n Sobel Expandido(5x5) [2]\n Prewitt(3x3) [3]\n Roberts(2x2) [4]\n Laplaciano(5x5) [5]\n Digite qual filtro quer usar: ");
    scanf("%d", &tipo);

    // Definir a região a ser impressa (aqui: 10x10 pixels a partir do canto (0,0))
    const int start_x = 0;
    const int start_y = 0;
    const int print_width = 319;
    const int print_height = 319;
    preencherMatriz(data, new_data, width);
    printf("Valores de pixels da região %dx%d (R, G, B, ...):\n", print_width, print_height);
    for (int y = start_y; y < start_y + print_height; y++) {
        for (int x = start_x; x < start_x + print_width; x++) {
            // Calcular a posição no array 1D
            int index = (y * width + x);
            
            int temp = function_principal(data, y, x, width, print_height, tipo);
            if (temp>255){
                temp = 255;
            }else if (temp<0){
                temp = 0;
            }
            
            new_data[index] = temp;
            //printf("%d\n", new_data[index]);
            //printf("Pixel (%3d, %3d): ", x, y);
            //printf("%3u ", data[index]); // Valores de 0 a 255
            
            //printf("\n");
        }
        //printf("\n");
    }

    char nomeFoto[20];
    if(tipo == 1){
        strcpy(nomeFoto, "LennaSobel.png");
    }else if(tipo == 2){
        strcpy(nomeFoto, "LennaSobelExpand.png");
    }else if(tipo == 3){
        strcpy(nomeFoto, "LennaPreWit.png");
    }else if(tipo == 4){
        strcpy(nomeFoto, "LennaRoberts.png");
    }else{
        strcpy(nomeFoto, "LennaLaplaciano.png");
    }

    stbi_write_png(nomeFoto, width, height, channels, new_data, width);

    // Liberar memória
    encerrarDafema();
    stbi_image_free(data);
    free(new_data);
    return 0;
}


