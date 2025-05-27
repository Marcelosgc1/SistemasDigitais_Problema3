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

    //printMatriz(m,3);

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

    //printMatriz(m,3);

    
    int mask0[3][5] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    
    int mask1[3][5] = {
	{-1,-1,-1},{0,0,0},{1,1,1}

    };

    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    multiplicacao();
    int x = ler(2, 0, 0);
    escreveMatriz(mask1,3,1);
    multiplicacao();
    int y = ler(2,0,0);
    return x+y;


}



int func_teste(unsigned char *dados, int i, int j, int larg_dados, int tamanho){

    int matriz_temp[3][5];
    tamanho--;
    
    
    int linha = 0, coluna = 0;

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

    
    return sobel(matriz_temp);


}














int main() {
    const char *filename = "90a8ad63-00a0-47f8-9b2a-014cdd0df533.jpeg"; // Substitua pelo caminho da sua imagem
    int width, height, channels;
    iniciarDafema();
    // Carregar a imagem (formato: RGB ou RGBA, dependendo da imagem)
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 1);

    unsigned char *new_data = malloc(sizeof(unsigned char) * 319 * 319);

    if (!data) {
        printf("Erro ao carregar a imagem!\n");
        return -1;
    }

    printf("Imagem carregada: %dx%d, %d canais\n\n", width, height, channels);

    // Definir a região a ser impressa (aqui: 10x10 pixels a partir do canto (0,0))
    const int start_x = 0;
    const int start_y = 0;
    const int print_width = 319;
    const int print_height = 319;

    printf("Valores de pixels da região %dx%d (R, G, B, ...):\n", print_width, print_height);
    for (int y = start_y; y < start_y + print_height; y++) {
        for (int x = start_x; x < start_x + print_width; x++) {
            // Calcular a posição no array 1D
            int index = (y * width + x);
            
            int temp = func_teste(data, y, x, width, print_height);
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


    stbi_write_png("sharp_output_gray.png", width, height, channels, new_data, width);

    // Liberar memória
    encerrarDafema();
    stbi_image_free(data);
    free(new_data);
    return 0;
}

