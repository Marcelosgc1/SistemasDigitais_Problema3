#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_write.h"

//Diferença logarítmica
static int comparar_imagens_diferenca_log(const char *imagem1_path, const char *imagem2_path, const char *diferenca_path) {
    double g_metrica_calculada;
    int w1, h1, channels1;
    unsigned char *img1_data = stbi_load(imagem1_path, &w1, &h1, &channels1, 1);
    if (!img1_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem1_path);
        return -1;
    }

    int w2, h2, channels2;
    unsigned char *img2_data = stbi_load(imagem2_path, &w2, &h2, &channels2, 1);
    if (!img2_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem2_path);
        stbi_image_free(img1_data);
        return -1;
    }

    if (w1 != w2 || h1 != h2) {
        fprintf(stderr, "As imagens têm dimensões diferentes: Imagem1 (%dx%d) vs Imagem2 (%dx%d)\n", w1, h1, w2, h2);
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    unsigned char *diff_img_data_visual = (unsigned char *)malloc(w1 * h1 * sizeof(unsigned char));
    if (!diff_img_data_visual) {
        fprintf(stderr, "Erro ao alocar memória para a imagem de diferença visual.\n");
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    const double log_de_256 = log1p(255.0);
    double soma_modulos_para_metrica = 0.0;

    for (int i = 0; i < w1 * h1; ++i) {
        int diferenca_original = (int)img1_data[i] - (int)img2_data[i];
        int diferenca_absoluta = abs(diferenca_original); 

        soma_modulos_para_metrica += diferenca_absoluta;

        double valor_pixel_visual = (log1p((double)diferenca_absoluta) / log_de_256) * 255.0;
        
        if (valor_pixel_visual < 0.0) valor_pixel_visual = 0.0;
        if (valor_pixel_visual > 255.0) valor_pixel_visual = 255.0;
        
        diff_img_data_visual[i] = (unsigned char)(valor_pixel_visual + 0.5); 
    }

    g_metrica_calculada = ((soma_modulos_para_metrica * 100.0) / 255.0)/(h1*w1);
    printf("--------------------------------------------------------------------\n");
    printf("DIFERENÇA: %.2f%%\n", g_metrica_calculada);
    printf("--------------------------------------------------------------------\n");

    if (!stbi_write_png(diferenca_path, w1, h1, 1, diff_img_data_visual, w1)) {
        fprintf(stderr, "Erro ao salvar a imagem de diferença visual '%s'\n", diferenca_path);
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        free(diff_img_data_visual);
        return -1;
    }

    printf("Imagem de diferença visual logarítmica salva como '%s'\n", diferenca_path);
    
    stbi_image_free(img1_data);
    stbi_image_free(img2_data);
    free(diff_img_data_visual);

    return 0;
}

//Diferença normal
static int comparar_imagens_diferenca_normal(const char *imagem1_path, const char *imagem2_path, const char *diferenca_path) {
    double g_metrica_calculada; 

    int w1, h1, channels1;
    unsigned char *img1_data = stbi_load(imagem1_path, &w1, &h1, &channels1, 1);
    if (!img1_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem1_path);
        return -1;
    }

    int w2, h2, channels2;
    unsigned char *img2_data = stbi_load(imagem2_path, &w2, &h2, &channels2, 1);
    if (!img2_data) {
        fprintf(stderr, "Erro ao carregar imagem '%s'\n", imagem2_path);
        stbi_image_free(img1_data);
        return -1;
    }

    if (w1 != w2 || h1 != h2) {
        fprintf(stderr, "As imagens têm dimensões diferentes: Imagem1 (%dx%d) vs Imagem2 (%dx%d)\n", w1, h1, w2, h2);
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    unsigned char *diff_img_data_visual = (unsigned char *)malloc(w1 * h1 * sizeof(unsigned char));
    if (!diff_img_data_visual) {
        fprintf(stderr, "Erro ao alocar memória para a imagem de diferença visual.\n");
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        return -1;
    }

    double soma_modulos_para_metrica = 0.0;

    for (int i = 0; i < w1 * h1; ++i) {
        int diferenca_original = (int)img1_data[i] - (int)img2_data[i];
        int diferenca_absoluta = abs(diferenca_original); 

        soma_modulos_para_metrica += diferenca_absoluta;

        diff_img_data_visual[i] = (unsigned char)diferenca_absoluta; 
    }

    long total_pixels = (long)h1 * w1;
    if (total_pixels > 0) {
        double mae = soma_modulos_para_metrica / (double)total_pixels;
        g_metrica_calculada = (mae / 255.0) * 100.0;
    } else {
        g_metrica_calculada = 0.0; 
    }
    
    printf("--------------------------------------------------------------------\n");
    printf("DIFERENÇA PERCENTUAL MÉDIA: %.2f%%\n", g_metrica_calculada);
    printf("--------------------------------------------------------------------\n");

    if (!stbi_write_png(diferenca_path, w1, h1, 1, diff_img_data_visual, w1)) {
        fprintf(stderr, "Erro ao salvar a imagem de diferença visual '%s'\n", diferenca_path);
        stbi_image_free(img1_data);
        stbi_image_free(img2_data);
        free(diff_img_data_visual);
        return -1;
    }

    printf("Imagem de diferença visual absoluta salva como '%s'\n", diferenca_path);
    
    stbi_image_free(img1_data);
    stbi_image_free(img2_data);
    free(diff_img_data_visual);

    return 0; 
}

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

int funcTeste5x5(unsigned char *dados, int i, int j, int largura, int altura, int operacao) {
    int matriz_temp[5][5];
    int linha = 0;
    int coluna = 0;  

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
    const char *input_filename = "lenna.jpeg";
    char *output_filename = "foto.png"; 

    int width, height, channels, comparar;
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

    printf("Dimensões: %d %d", width, height);
    double max_value = 0.0; 
    int operacao = 0;
    int comp = 0;
    printf("\n\nDIGITE O FILTRO/OPÇÃO DESEJADA ");
    printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] CompararC-FPGA: \n[7] Sair: ");
    scanf("%d", &operacao);

    while (operacao > 0 && operacao < 7){

        switch(operacao){
            case 1:
                output_filename = "outputC/roberts.png";
                break;
            case 2:
                output_filename = "outputC/sobel.png";
                break;
            case 3:
                output_filename = "outputC/prewitt.png";
                break;
            case 4:
                output_filename = "outputC/sobel_expandido.png";
                break;
            case 5:
                output_filename = "outputC/laplaciano.png";
                break;
            case 6: { 
                int filter_choice_for_comparison; 
                
                printf("\n--- Comparação de Imagens C vs FPGA ---\n");
                printf("Qual conjunto de resultados de filtro deseja comparar?\n");
                printf("As imagens devem estar em 'outputC/' e 'outputDafema/' respectivamente.\n");
                printf("A diferença será salva em 'outputDifC_FPGA/'.\n\n");
                printf("[1] Roberts\n");
                printf("[2] Sobel\n");
                printf("[3] Prewitt\n");
                printf("[4] Sobel Expandido\n");
                printf("[5] Laplaciano\n");
                printf("Digite o número do filtro para comparar: ");

                scanf("%d", &filter_choice_for_comparison);
                printf("Digite qual tipo de diferença deseja ver [0]Log [1]Normal: ");
                scanf("%d", &comparar);

                char base_filename[64];
                char tipoo[64];
                int valid_choice = 1;

                switch (filter_choice_for_comparison) {
                    case 1: strcpy(base_filename, "roberts"); break;
                    case 2: strcpy(base_filename, "sobel"); break;
                    case 3: strcpy(base_filename, "prewitt"); break;
                    case 4: strcpy(base_filename, "sobel_expandido"); break;
                    case 5: strcpy(base_filename, "laplaciano"); break;
                    default:
                        printf("Opção de filtro inválida (%d).\n", filter_choice_for_comparison);
                        valid_choice = 0;
                        break;
                }

                if (valid_choice) {
                    if(comparar == 0){
                        strcpy(tipoo, "Log");
                    }else{
                        strcpy(tipoo, "Normal");
                    }
                    char path_image_c[128];
                    char path_image_fpga[128];
                    char path_image_difference[128];

                    sprintf(path_image_c, "outputC/%s.png", base_filename);
                    sprintf(path_image_fpga, "outputDafema/%s.png", base_filename);
                    sprintf(path_image_difference, "outputDif%s/%s_diff.png", tipoo, base_filename); 

                    printf("\nComparando:\n  Imagem C:     %s\n  Imagem FPGA:  %s\n", path_image_c, path_image_fpga);
                    printf("  Salvando diferença em: %s\n", path_image_difference);

                    if(comparar == 0){
                        if (comparar_imagens_diferenca_log(path_image_c, path_image_fpga, path_image_difference) == 0) {
                        } else {
                            printf("Falha ao comparar imagens. Verifique os caminhos e se as imagens existem com as mesmas dimensões.\n");
                        }
                    }else{
                        if (comparar_imagens_diferenca_normal(path_image_c, path_image_fpga, path_image_difference) == 0) {
                        } else {
                            printf("Falha ao comparar imagens. Verifique os caminhos e se as imagens existem com as mesmas dimensões.\n");
                        }
                    }
                }
                break; 
            } 
            default: 
                break;
    }
        if(operacao > 0 && operacao < 6){
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
        }

        printf("\nDIGITE O FILTRO/OPÇÃO DESEJADA ");
        printf("\nFILTROS:\n[1] Roberts(2x2) \n[2] Sobel(3x3) \n[3] Prewitt(3x3) \n[4] Sobel Expandido(5x5) \n[5] Laplaciano(5x5): \n[6] CompararC-FPGA: \n[7] Sair:  ");
        scanf("%d", &operacao); 
    }

    stbi_image_free(data);
    free(temp_data); 
    free(output_data);

    return 0;
}
