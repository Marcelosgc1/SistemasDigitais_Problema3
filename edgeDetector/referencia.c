#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Define as implementações do stb_image em um único arquivo .c
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

// --- DEFINES PARA CÓDIGOS DE ERRO DA COMPARAÇÃO ---
#define SSIM_CALC_SUCCESS 0
#define SSIM_ERROR_LOAD_IMG1 -1
#define SSIM_ERROR_LOAD_IMG2 -2
#define SSIM_ERROR_DIMENSIONS_MISMATCH -3
#define SSIM_ERROR_ZERO_PIXELS -4


// --- FUNÇÕES DE COMPARAÇÃO DE IMAGEM (Originais e Funcionais) ---

static double calcular_ssim_global(const char *caminho_img1, const char *caminho_img2, int *status_code) {
    int largura1, altura1, canais1;
    unsigned char *pixels_img1 = stbi_load(caminho_img1, &largura1, &altura1, &canais1, 1);
    if (!pixels_img1) {
        if (status_code) *status_code = SSIM_ERROR_LOAD_IMG1;
        fprintf(stderr, "Erro ao carregar imagem 1: %s\n", caminho_img1);
        return -2.0;
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

    long total_pixels = (long)largura1 * altura1;
    if (total_pixels == 0) {
        stbi_image_free(pixels_img1);
        stbi_image_free(pixels_img2);
        if (status_code) *status_code = SSIM_ERROR_ZERO_PIXELS;
        return -2.0;
    }

    double soma_x = 0.0, soma_y = 0.0, soma_x_quadrado = 0.0, soma_y_quadrado = 0.0, soma_xy = 0.0;
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

    const double L = 255.0, K1 = 0.01, K2 = 0.03;
    const double C1 = (K1 * L) * (K1 * L);
    const double C2 = (K2 * L) * (K2 * L);

    double numerador = (2 * mu_x * mu_y + C1) * (2 * sigma_xy + C2);
    double denominador = (mu_x * mu_x + mu_y * mu_y + C1) * (sigma_x_sq + sigma_y_sq + C2);
    double ssim_valor = (denominador == 0) ? 1.0 : (numerador / denominador);

    stbi_image_free(pixels_img1);
    stbi_image_free(pixels_img2);
    if (status_code) *status_code = SSIM_CALC_SUCCESS;
    return ssim_valor;
}

static int gerar_imagem_diferenca_log(const char *caminho_img1, const char *caminho_img2, const char *caminho_img_saida) {
    int w1, h1, c1, w2, h2, c2;
    unsigned char *img1 = stbi_load(caminho_img1, &w1, &h1, &c1, 1);
    if (!img1) { fprintf(stderr, "Erro ao carregar imagem 1: %s\n", caminho_img1); return -1; }
    unsigned char *img2 = stbi_load(caminho_img2, &w2, &h2, &c2, 1);
    if (!img2) { stbi_image_free(img1); fprintf(stderr, "Erro ao carregar imagem 2: %s\n", caminho_img2); return -1; }

    if (w1 != w2 || h1 != h2) { stbi_image_free(img1); stbi_image_free(img2); return -1; }

    unsigned char *diff_img = (unsigned char *)malloc(w1 * h1);
    if (!diff_img) { stbi_image_free(img1); stbi_image_free(img2); return -1; }

    const double log_max = log1p(255.0);
    for (int i = 0; i < w1 * h1; ++i) {
        double diff = (log1p(abs(img1[i] - img2[i])) / log_max) * 255.0;
        diff_img[i] = (unsigned char)(fmin(255.0, fmax(0.0, diff)));
    }

    int sucesso = stbi_write_png(caminho_img_saida, w1, h1, 1, diff_img, w1);
    printf("Imagem de diferença logarítmica gerada: %s\n", caminho_img_saida);
    stbi_image_free(img1); stbi_image_free(img2); free(diff_img);
    return sucesso ? 0 : -1;
}

static int comparar_imagens_diferenca_normal(const char *imagem1_path, const char *imagem2_path, const char *diferenca_path) {
    int w1, h1, c1, w2, h2, c2;
    unsigned char *img1 = stbi_load(imagem1_path, &w1, &h1, &c1, 1);
    if (!img1) { fprintf(stderr, "Erro ao carregar imagem 1: %s\n", imagem1_path); return -1; }
    unsigned char *img2 = stbi_load(imagem2_path, &w2, &h2, &c2, 1);
    if (!img2) { stbi_image_free(img1); fprintf(stderr, "Erro ao carregar imagem 2: %s\n", imagem2_path); return -1; }

    if (w1 != w2 || h1 != h2) { stbi_image_free(img1); stbi_image_free(img2); return -1; }
    
    unsigned char *diff_img = (unsigned char *)malloc(w1 * h1);
    if (!diff_img) { stbi_image_free(img1); stbi_image_free(img2); return -1; }

    double soma_modulos = 0.0;
    for (int i = 0; i < w1 * h1; ++i) {
        int diferenca = abs(img1[i] - img2[i]);
        soma_modulos += diferenca;
        diff_img[i] = (unsigned char)diferenca;
    }

    double mae = (w1 * h1 > 0) ? (soma_modulos / (double)(w1 * h1)) : 0.0;
    printf("--------------------------------------------------------------------\n");
    printf("DIFERENÇA PERCENTUAL MÉDIA: %.2f%%\n", (mae / 255.0) * 100.0);
    printf("--------------------------------------------------------------------\n");

    int sucesso = stbi_write_png(diferenca_path, w1, h1, 1, diff_img, w1);
    printf("Imagem de diferença visual absoluta salva como '%s'\n", diferenca_path);
    stbi_image_free(img1); stbi_image_free(img2); free(diff_img);
    return sucesso ? 0 : -1;
}


// --- LÓGICA DE APLICAÇÃO DE FILTROS ---

// NOVA implementação, correta e estável, para o filtro Roberts
int apply_roberts_filter(const char* input_path, const char* output_path) {
    int width, height, channels;
    unsigned char *img_data = stbi_load(input_path, &width, &height, &channels, 1);
    if (!img_data) { fprintf(stderr, "Erro ao carregar imagem '%s'\n", input_path); return -1; }

    unsigned char *output_data = (unsigned char *)calloc(width * height, sizeof(unsigned char));
    if (!output_data) { fprintf(stderr, "Erro ao alocar memória.\n"); stbi_image_free(img_data); return -1; }

    printf("Processando filtro Roberts... Dimensões: %dx%d\n", width, height);

    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            long p1 = img_data[y*width + x], p2 = img_data[y*width + x+1];
            long p3 = img_data[(y+1)*width + x], p4 = img_data[(y+1)*width + x+1];
            long sum_x = p1 - p4, sum_y = p2 - p3;
            long magnitude = (long)sqrt(sum_x * sum_x + sum_y * sum_y);
            output_data[y * width + x] = (unsigned char)(fmin(255.0, magnitude));
        }
    }

    stbi_write_png(output_path, width, height, 1, output_data, width);
    printf("Imagem salva com sucesso como '%s'\n", output_path);
    stbi_image_free(img_data); free(output_data);
    return 0;
}


// Funções da sua estrutura original, agora corrigidas e funcionais para filtros 3x3 e 5x5
int sobel(int m[3][3]) {
    const int mask_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int mask_y[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
    int sumX = 0, sumY = 0;
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) {
        sumX += mask_x[i][j] * m[i][j];
        sumY += mask_y[i][j] * m[i][j];
    }
    return (int)sqrt(sumX*sumX + sumY*sumY);
}

int preWitt(int m[3][3]) {
    const int mask_x[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    const int mask_y[3][3] = {{-1,-1,-1}, {0,0,0}, {1,1,1}};
    int sumX = 0, sumY = 0;
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) {
        sumX += mask_x[i][j] * m[i][j];
        sumY += mask_y[i][j] * m[i][j];
    }
    return (int)sqrt(sumX*sumX + sumY*sumY);
}

int sobel_expandido(int m[5][5]) {
    const int mask_x[5][5] = {{2,2,4,2,2}, {1,1,2,1,1}, {0,0,0,0,0}, {-1,-1,-2,-1,-1}, {-2,-2,-4,-2,-2}};
    const int mask_y[5][5] = {{2,1,0,-1,-2}, {2,1,0,-1,-2}, {4,2,0,-2,-4}, {2,1,0,-1,-2}, {2,1,0,-1,-2}};
    int sumX = 0, sumY = 0;
    for (int i=0; i<5; i++) for (int j=0; j<5; j++) {
        sumX += mask_x[i][j] * m[i][j];
        sumY += mask_y[i][j] * m[i][j];
    }
    return (int)sqrt(sumX*sumX + sumY*sumY);
}

int laplaciano(int m[5][5]) {
    const int mask[5][5] = {{0,0,-1,0,0}, {0,-1,-2,-1,0}, {-1,-2,16,-2,-1}, {0,-1,-2,-1,0}, {0,0,-1,0,0}};
    int sum = 0;
    for (int i=0; i<5; i++) for (int j=0; j<5; j++) {
        sum += mask[i][j] * m[i][j];
    }
    return sum;
}

int funcTeste5x5(unsigned char *dados, int i, int j, int largura, int altura, int operacao) {
    int matriz_temp[5][5];
    for(int y_off = -2; y_off <= 2; y_off++){
        for(int x_off = -2; x_off <= 2; x_off++){
            int y = i + y_off, x = j + x_off;
            if (y < 0 || y >= altura || x < 0 || x >= largura) {
                matriz_temp[y_off + 2][x_off + 2] = dados[i * largura + j];
            } else {
                matriz_temp[y_off + 2][x_off + 2] = dados[y * largura + x];
            }
        }
    }
    if(operacao == 4) return sobel_expandido(matriz_temp);
    if(operacao == 5) return laplaciano(matriz_temp);
    return 0;
}

int funcTeste3x3(unsigned char *dados, int i, int j, int largura, int altura, int operacao){
    int matriz_temp[3][3];
    for (int y_off = -1; y_off <= 1; y_off++){
        for (int x_off = -1; x_off <= 1; x_off++){
            int y = i + y_off, x = j + x_off;
            if (y < 0 || y >= altura || x < 0 || x >= largura) {
                matriz_temp[y_off + 1][x_off + 1] = dados[i * largura + j];
            } else {
                matriz_temp[y_off + 1][x_off + 1] = dados[y * largura + x];
            }
        }
    }
    if(operacao == 2) return sobel(matriz_temp);
    if(operacao == 3) return preWitt(matriz_temp);
    return 0;
}

int calcularGeratriz(unsigned char *dados, int i, int j, int largura, int altura, int operacao){
    if(operacao == 2 || operacao == 3){
        return funcTeste3x3(dados, i, j, largura, altura, operacao); 
    } else if(operacao == 4 || operacao == 5){
        return funcTeste5x5(dados, i, j, largura, altura, operacao); 
    }
    return 0;
}


// --- FUNÇÃO PRINCIPAL (main) ---

int main() {
    const char *input_filename = "data/cameraman.png";
    int operacao = 0;

    do {
        printf("\n\n=============== MENU DE OPCOES ===============\n");
        printf("FILTROS PARA GERACAO (base: %s):\n", input_filename);
        printf("  [1] Roberts (2x2)\n");
        printf("  [2] Sobel (3x3)\n");
        printf("  [3] Prewitt (3x3)\n");
        printf("  [4] Sobel Expandido (5x5)\n");
        printf("  [5] Laplaciano (5x5)\n");
        printf("----------------------------------------------\n");
        printf("COMPARACAO DE IMAGENS:\n");
        printf("  [6] Comparar resultados C vs. FPGA\n");
        printf("----------------------------------------------\n");
        printf("  [7] Sair\n");
        printf("==============================================\n");
        printf("Digite a opcao desejada: ");
        
        if (scanf("%d", &operacao) != 1) { // Limpa o buffer em caso de entrada inválida
            while(getchar() != '\n');
            operacao = -1; // Força repetição do menu
        }

        // Caso 1: Filtro Roberts (usa a implementação nova e estável)
        if (operacao == 1) {
            apply_roberts_filter(input_filename, "outputC/roberts.png");
        } 
        // Casos 2-5: Filtros 3x3 e 5x5 (usa a sua estrutura original corrigida)
        else if (operacao > 1 && operacao < 6) {
            char output_filename[128];
            switch(operacao){
                case 2: strcpy(output_filename, "outputC/sobel.png"); break;
                case 3: strcpy(output_filename, "outputC/prewitt.png"); break;
                case 4: strcpy(output_filename, "outputC/sobel_expandido.png"); break;
                case 5: strcpy(output_filename, "outputC/laplaciano.png"); break;
            }

            int width, height, channels;
            unsigned char *data = stbi_load(input_filename, &width, &height, &channels, 1);
            if (!data) { fprintf(stderr, "Erro ao carregar imagem '%s'\n", input_filename); continue; }

            unsigned char *output_data = (unsigned char*)malloc(width * height * sizeof(unsigned char));
            if (!output_data) { fprintf(stderr, "Erro ao alocar memória.\n"); stbi_image_free(data); continue; }
            
            printf("Processando filtro... Dimensões: %dx%d\n", width, height);
            
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int temp = calcularGeratriz(data, y, x, width, height, operacao);
                    output_data[y * width + x] = (unsigned char)fmin(255.0, fmax(0.0, temp));
                }
            }

            if(stbi_write_png(output_filename, width, height, 1, output_data, width)) {
                printf("Imagem salva com sucesso como '%s'\n", output_filename);
            } else {
                fprintf(stderr, "Erro ao salvar imagem '%s'\n", output_filename);
            }
            
            stbi_image_free(data);
            free(output_data);

        } 
        // Caso 6: Comparação de imagens
        else if (operacao == 6) {
            int filter_choice, compare_type;
            printf("\n--- Comparacao de Imagens C vs FPGA ---\n");
            printf("Qual filtro deseja comparar?\n[1]Roberts [2]Sobel [3]Prewitt [4]SobelExp [5]Laplaciano\n> ");
            scanf("%d", &filter_choice);
            printf("Qual tipo de diferenca? [0]Logaritmica [1]Normal\n> ");
            scanf("%d", &compare_type);

            char base_filename[64];
            int valid_choice = 1;
            switch (filter_choice) {
                case 1: strcpy(base_filename, "roberts"); break;
                case 2: strcpy(base_filename, "sobel"); break;
                case 3: strcpy(base_filename, "prewitt"); break;
                case 4: strcpy(base_filename, "sobel_expandido"); break;
                case 5: strcpy(base_filename, "laplaciano"); break;
                default: printf("Opcao de filtro invalida.\n"); valid_choice = 0; break;
            }

            if (valid_choice) {
                char path_c[128], path_fpga[128], path_diff[128];
                sprintf(path_c, "outputC/%s.png", base_filename);
                sprintf(path_fpga, "outputDFM/%s_clock.png", base_filename);

                printf("\nComparando:\n  - Imagem C: %s\n  - Imagem FPGA: %s\n", path_c, path_fpga);
                if (compare_type == 0) {
                    sprintf(path_diff, "outputDifLog/%s_diff.png", base_filename);
                    if (gerar_imagem_diferenca_log(path_c, path_fpga, path_diff) == 0) {
                        int status;
                        double ssim = calcular_ssim_global(path_c, path_fpga, &status);
                        if (status == SSIM_CALC_SUCCESS) {
                           printf("SSIM (Similaridade Estrutural): %.4f\n", ssim);
                        }
                    }
                } else if (compare_type == 1) {
                    sprintf(path_diff, "outputDifNormal/%s_diff.png", base_filename);
                    comparar_imagens_diferenca_normal(path_c, path_fpga, path_diff);
                } else {
                    printf("Tipo de diferenca invalido.\n");
                }
            }
        } 
        // Casos de saída e erro
        else if (operacao != 7) {
            printf("Opcao invalida. Tente novamente.\n");
        }

    } while (operacao != 7);

    printf("Saindo do programa.\n");
    return 0;
}
