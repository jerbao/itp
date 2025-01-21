// Aluno: Jerônimo Rafael Bezerra Filho
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compartilhado.h"

/**
 * @brief Função principal que lê um arquivo PBM contendo um código de barras EAN-8 e extrai o identificador.
 *
 * @param argc Número de argumentos passados na linha de comando.
 * @param argv Vetor de strings contendo os argumentos.
 * @return EXIT_SUCCESS em caso de sucesso, EXIT_FAILURE caso contrário.
 */
int main(int argc, char *argv[]){
    // Verifica se o nome do arquivo foi fornecido
    if(argc < 2){
        printf("Erro! O nome do arquivo deve ser fornecido.\n");
        return EXIT_FAILURE;
    }

    const char *nomeArquivo = argv[1];
    
    // Tenta abrir o arquivo PBM para leitura
    FILE *arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL){
        printf("Erro! Nao foi possivel abrir o arquivo: %s\n", nomeArquivo);
        return EXIT_FAILURE;
    }

    int largura, altura;
    
    // Verifica o cabeçalho do arquivo PBM
    if(!verificarCabecalhoPbm(arquivo, &largura, &altura)){
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    // Aloca memória para armazenar a matriz do PBM
    int **matriz = (int **)malloc(altura * sizeof(int *));
    for(int i = 0; i < altura; i++){
        matriz[i] = (int *)malloc(largura * sizeof(int));
    }

    // Lê os dados da matriz do arquivo PBM
    for(int i = 0; i < altura; i++){
        for(int j = 0; j < largura; j++){
            if(fscanf(arquivo, "%d", &matriz[i][j]) != 1){
                printf("Erro ao ler o arquivo PBM.\n");
                fclose(arquivo);
                for(int k = 0; k <= i; k++) free(matriz[k]);
                free(matriz);
                return EXIT_FAILURE;
            }
        }
    }
    fclose(arquivo);

    char sequenciaBinaria[68];
    int larguraModulo = largura / 67;

    // Verifica os marcadores no código de barras
    if(verificarMarcadores(matriz, largura, altura, larguraModulo, sequenciaBinaria)){
        printf("\nSequencia de bits valida: %s\n", sequenciaBinaria);

        char identificador[9];
        // Extrai o identificador EAN-8 a partir da sequência binária
        if(extrairIdentificador(sequenciaBinaria, identificador)){
            printf("Identificador lido: %s\n", identificador);
        }
        else{
            printf("Erro ao ler os digitos.\n");
        }
    }
    else{
        printf("Erro: Codigo de barras nao e valido.\n");
    }

    // Libera a memória alocada para a matriz
    for(int i = 0; i < altura; i++) {
        if (matriz[i] != NULL) {
            free(matriz[i]);
        }
    }
    free(matriz);
    return EXIT_SUCCESS;
}
