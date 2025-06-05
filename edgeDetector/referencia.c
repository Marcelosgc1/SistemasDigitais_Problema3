#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define SSIM_CALC_SUCCESS 0
#define SSIM_ERROR_LOAD_IMG1 -1
#define SSIM_ERROR_LOAD_IMG2 -2
#define SSIM_ERROR_DIMENSIONS_MISMATCH -3
#define SSIM_ERROR_ZERO_PIXELS -4

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stb_image.h"
#include "stb_image_write.h"

static double calcular_ssim_global(const char *caminho_img1, const char *caminho_img2, int *status_code) {
    int largura1, altura1, canais1;
    unsigned char *pixels_img1 = stbi_load(caminho_img1, &largura1, &altura1, &canais1, 1); 
    if (!pixels_img1) {
        if (status_code) *status_code = SSIM_ERROR_LOAD_IMG1;
        fprintf(stderr, "Erro ao carregar imagem 1: %s\n", caminho_img1);
        return -2.0; // Valor de erro distinto do range normal do SSIM (-1 a 1)
    }

    int largura2, altura2, canais2;
    unsigned char *pixels_img2 = stbi_load(caminho_img2, &largura2, &altura2, &canais2, 1); 
    if (!pixels_img2) {
        stbi_image_free(pixels_img1);
        if (status_code) *status_code = SSIM_ERROR_LOAD_IMG2;
        fprintf(stderr, "Erro ao carregar imagem 2: %s\n", caminho_img2);
        return -2.0;
    }

    if (largura1 != largura2 || altura1 != altura2) {
        stbi_image_free(pixels_img1);
        stbi_image_free(pixels_img2);
        if (status_code) *status_code = SSIM_ERROR_DIMENSIONS_MISMATCH;
        fprintf(stderr, "Erro: As dimensões das imagens não coincidem (%dx%d vs %dx%d).\n", largura1, altura1, largura2, altura2);
        return -2.0;
    }

    int largura = largura1;
    int altura = altura1;
    long total_pixels = (long)largura * altura;

    if (total_pixels == 0) {
        stbi_image_free(pixels_img1);
        stbi_image_free(pixels_img2);
        if (status_code) *status_code = SSIM_ERROR_ZERO_PIXELS;
        fprintf(stderr, "Erro: Imagem com zero pixels.\n");
        return -2.0;
    }

    double soma_x = 0.0, soma_y = 0.0;
    double soma_x_quadrado = 0.0, soma_y_quadrado = 0.0;
    double soma_xy = 0.0;

    for (int i = 0; i < total_pixels; ++i) {
        double pixel_x = (double)pixels_img1[i];
        double pixel_y = (double)pixels_img2[i];

        soma_x += pixel_x;
        soma_y += pixel_y;
        soma_x_quadrado += pixel_x * pixel_x;
        soma_y_quadrado += pixel_y * pixel_y;
        soma_xy += pixel_x * pixel_y;
    }

    double mu_x = soma_x / total_pixels;
    double mu_y = soma_y / total_pixels;

    double sigma_x_sq = (soma_x_quadrado / total_pixels) - (mu_x * mu_x);
    double sigma_y_sq = (soma_y_quadrado / total_pixels) - (mu_y * mu_y);

    double sigma_xy = (soma_xy / total_pixels) - (mu_x * mu_y);

    const double L_range_dinamico = 255.0; // Para imagens de 8 bits
    const double K1 = 0.01;
    const double K2 = 0.03;
    const double C1 = (K1 * L_range_dinamico) * (K1 * L_range_dinamico);
    const double C2 = (K2 * L_range_dinamico) * (K2 * L_range_dinamico);

    // Fórmula do SSIM (Equação 13 do artigo)
    // SSIM(x,y) = ((2*μx*μy + C1) * (2*σxy + C2)) / ((μx^2 + μy^2 + C1) * (σx^2 + σy^2 + C2))
    double numerador   = (2 * mu_x * mu_y + C1) * (2 * sigma_xy + C2);
    double denominador = (mu_x * mu_x + mu_y * mu_y + C1) * (sigma_x_sq + sigma_y_sq + C2);

    double ssim_valor;
    if (denominador == 0) {
       
        if (numerador == 0 && denominador == 0) { // Acontece se mu_x,mu_y,sigma_x_sq,sigma_y_sq,sigma_xy forem todos zero (imagem toda preta)
             ssim_valor = 1.0; // Duas imagens totalmente pretas são idênticas
        } else if (denominador == 0 && numerador != 0) { // Situação instável
             ssim_valor = 0.0; // Ou algum outro valor indicando instabilidade
        } else { // Denominador muito próximo de zero mas não exatamente zero, numerador também.
             ssim_valor = 1.0; // Caso de imagens constantes idênticas pode levar aqui.
        }
         
         if (denominador < 1e-10 && numerador < 1e-10) { // Evitar divisão por "quase zero"
            ssim_valor = 1.0; // Provavelmente imagens constantes idênticas
         } else if (denominador < 1e-10) {
            ssim_valor = 0.0; // Instável, sem similaridade clara
         } else {
             ssim_valor = numerador / denominador;
         }

    } else {
        ssim_valor = numerador / denominador;
    }
    
    stbi_image_free(pixels_img1);
    stbi_image_free(pixels_img2);

    if (status_code) *status_code = SSIM_CALC_SUCCESS;
    return ssim_valor;
}

//Diferença Log
static int gerar_imagem_diferenca_log(const char *caminho_img1, const char *caminho_img2, const char *caminho_img_saida) {
    int largura1, altura1, canais1;
    unsigned char *pixels_img1 = stbi_load(caminho_img1, &largura1, &altura1, &canais1, 1);
    if (!pixels_img1) {
        return -1; 
    }

    int largura2, altura2, canais2;
    unsigned char *pixels_img2 = stbi_load(caminho_img2, &largura2, &altura2, &canais2, 1);
    if (!pixels_img2) {
        stbi_image_free(pixels_img1);
        return -1;
    }

    if (largura1 != largura2 || altura1 != altura2) {
        stbi_image_free(pixels_img1);
        stbi_image_free(pixels_img2);
        return -1; 
    }

    int largura = largura1;
    int altura = altura1;
    long total_pixels = (long)largura * altura;

    unsigned char *pixels_img_diferenca = (unsigned char *)malloc(total_pixels * sizeof(unsigned char));
    if (!pixels_img_diferenca) {
        stbi_image_free(pixels_img1);
        stbi_image_free(pixels_img2);
        return -1; 
    }

    // Constante para a escala logarítmica: log(1 + valor_max_abs_diff_possivel)
    // O valor máximo da diferença absoluta entre pixels de 8 bits (0-255) é 255.
    const double log_do_max_abs_diff_mais_1 = log1p(255.0); // log(1 + 255) = log(256)

    for (int i = 0; i < total_pixels; ++i) {
        int diff_original = (int)pixels_img1[i] - (int)pixels_img2[i];
        int diff_abs = abs(diff_original); // Módulo da diferença, valor entre 0 e 255

        // Aplica a escala logarítmica para o pixel da imagem de saída
        // A fórmula é: (log(1 + diff_abs) / log(1 + max_abs_diff_possivel)) * 255
        // Se diff_abs é 0, log1p(0) é 0, então o pixel_final será 0.
        double pixel_final_log = (log1p((double)diff_abs) / log_do_max_abs_diff_mais_1) * 255.0;
        
        if (pixel_final_log < 0.0) pixel_final_log = 0.0;
        if (pixel_final_log > 255.0) pixel_final_log = 255.0;
        
        pixels_img_diferenca[i] = (unsigned char)(pixel_final_log + 0.5); // Adiciona 0.5 para arredondamento
    }

    int sucesso_ao_salvar = stbi_write_png(caminho_img_saida, largura, altura, 1, pixels_img_diferenca, largura);
    printf("Imagem de diferença logarítmica gerada: %s\n", caminho_img_saida);

    stbi_image_free(pixels_img1);
    stbi_image_free(pixels_img2);
    free(pixels_img_diferenca);

    return sucesso_ao_salvar ? 0 : -1;
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

    int width, height, channels, comparar, status;
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
                        if (gerar_imagem_diferenca_log(path_image_c, path_image_fpga, path_image_difference) == 0) {
                            double ssim = calcular_ssim_global(path_image_c, path_image_fpga, &status);
                            double dssim_percent = ((1.0 - ssim) / 2.0) * 100.0;
                            printf("DSSIM Percentual (Dissimilaridade Estrutural): %.2f%%\n", dssim_percent);
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
