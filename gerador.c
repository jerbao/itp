#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "compartilhado.h"

#define espacamentoPadrao 4           /**< Espaçamento lateral padrão */
#define larguraBarraPadrao 3          /**< Largura de barra padrão */
#define alturaPadrao 50               /**< Altura do código padrão */

/**
 * @brief Função principal que gera o código de barras EAN-8 em formato PBM.
 *
 * @param argc Número de argumentos passados na linha de comando.
 * @param argv Vetor de strings contendo os argumentos.
 * @return EXIT_SUCCESS em caso de sucesso, EXIT_FAILURE caso contrário.
 */
int main(int argc, char *argv[]){
    // Verifica se os parâmetros mínimos foram fornecidos
    if(argc < 3){
        printf("Uso: %s <Arquivo de Saida .pbm> <Identificador EAN-8> [Espacamento (Opcional)] [Largura (Opcional)] [Altura (Opcional)]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Inicializa a estrutura de configuração com os argumentos fornecidos
    dadosConfiguracaoBarras dadosConfig;
    dadosConfig.nomeArquivoSaida = argv[1];
    dadosConfig.identificadorEan8 = argv[2];

    // Define o espaçamento lateral
    if(argc > 3){
        dadosConfig.espacamentoLateral = atoi(argv[3]);
    }
    else{
        dadosConfig.espacamentoLateral = espacamentoPadrao;
    }

    // Define a largura das barras
    if(argc > 4){
        dadosConfig.larguraBarra = atoi(argv[4]);
    }
    else{
        dadosConfig.larguraBarra = larguraBarraPadrao;
    }

    // Define a altura do código
    if(argc > 5){
        dadosConfig.alturaCodigo = atoi(argv[5]);
    }
    else{
        dadosConfig.alturaCodigo = alturaPadrao;
    }

    // Verifica se a extensão do arquivo de saída é .pbm
    const char *extensao = strrchr(dadosConfig.nomeArquivoSaida, '.');
    if(!extensao || strcmp(extensao, ".pbm") != 0){
        printf("Erro! O arquivo de saida precisa ter a extensao .pbm\n");
        return EXIT_FAILURE;
    }

    // Verifica se os parâmetros de configuração são positivos
    if(dadosConfig.espacamentoLateral <= 0 || dadosConfig.larguraBarra <= 0 || dadosConfig.alturaCodigo <= 0){
        printf("Erro! Espacamento, largura e altura devem ser valores positivos.\n");
        return EXIT_FAILURE;
    }

    // Verifica se o arquivo de saída já existe
    FILE *arquivo = fopen(dadosConfig.nomeArquivoSaida, "r");
    if(arquivo){
        fclose(arquivo);
        printf("O arquivo '%s' ja existe. Deseja sobrescreve-lo? (S/N): ", dadosConfig.nomeArquivoSaida);
    
        char resposta[4];
        if(scanf("%3s", resposta) != 1){
            printf("Erro ao ler a resposta do usuario.\n");
            return EXIT_FAILURE;
        }
    
        resposta[strcspn(resposta, "\n")] = '\0';
    
        // Verifica a resposta do usuário para sobrescrever o arquivo
        if(strcmp(resposta, "S") != 0 && strcmp(resposta, "s") != 0 && strcmp(resposta, "Sim") != 0 && strcmp(resposta, "sim") != 0){
            printf("Erro! Nao foi possivel criar o novo arquivo, visto que ele ja existe com esse nome.\n");
            return EXIT_FAILURE;
        }
    }

    // Gera o código de barras
    if(!gerarCodigo(&dadosConfig)){
        return EXIT_FAILURE;
    }

    // Informa o sucesso da operação
    printf("Imagem do codigo de barras criada com sucesso! Ela pode ser encontrada no arquivo: %s\n", dadosConfig.nomeArquivoSaida);
    return EXIT_SUCCESS;
}
