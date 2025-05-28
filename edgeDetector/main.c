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


void escreveMatriz(int matriz[][5], int tamanho, int matrizId){
    for (int i = 0; i < tamanho; i++){
        for (int j = 0; j < tamanho; j++){
            escreverIndice(matriz[i][j], matrizId, i, j);
        }
    }
}

int prewitt(int m[3][5]){

    int mask0[3][5] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };

    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    convolucao();
    int x = ler(2, 0, 2);
    return x;

}

int sobel(int m[3][5]){

    int mask0[3][5] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    escreveMatriz(mask0,3,1);
    escreveMatriz(m,3,0);
    convolucao();
    int x = ler(2, 0, 2);
    return x;

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
    char *filename = "data/medicine.png"; // Substitua pelo caminho da sua imagem
    int width, height, channels;
    iniciarDafema();
    
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 1);

    unsigned char *new_data = malloc(sizeof(unsigned char) * 319 * 319);

    if (!data) {
        printf("Erro ao carregar a imagem!\n");
        return -1;
    }

    printf("Imagem carregada: %dx%d, %d canais\n\n", width, height, channels);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            int index = (y * width + x);
            
            int temp = func_teste(data, y, x, width, height);
            if (temp>255){
                temp = 255;
            }
            
            new_data[index] = temp;
        }
    }

    char new_file[100] = "output";
    char *file = filename + 4;
    strcat(new_file, file);
    stbi_write_png(new_file, width, height, 1, data, width);

    // Liberar memória
    encerrarDafema();
    stbi_image_free(data);
    free(new_data);
    return 0;
}

