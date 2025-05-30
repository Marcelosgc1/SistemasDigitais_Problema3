#include <stdio.h>

void escreveMatriz2(int matriz[][5], int tamanho, int matrizId){

    int linha = 0, coluna = 0, n0 = 0, n1 = 0, temp = 0;

    do{
        n0 = (linha < tamanho && coluna < tamanho) ? matriz[linha][coluna] : 0;
        n1 = (linha < tamanho && coluna < tamanho) ? matriz[linha + (coluna+1>4)][(coluna+1)%5] : 0;
        escrever(n0,n1,matrizId,linha,coluna);
        linha = linha + (coluna+1>3);
        coluna = (coluna>2) ? coluna%3 : coluna + 2;

    } while (linha < 5 && coluna < 5);
    
}

int main(){
    int matrz[5][5];
    escreveMatriz2(matrz,5,1);

    return 0;
}