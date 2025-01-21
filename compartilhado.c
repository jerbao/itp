#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "compartilhado.h"

// Padrões de codificação EAN-8 para a parte esquerda
const char *padroesEan8L[10] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

// Padrões de codificação EAN-8 para a parte direita
const char *padroesEan8R[10] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

/**
 * @brief Verifica o cabeçalho de um arquivo PBM e extrai a largura e a altura.
 *
 * @param arquivo Ponteiro para o arquivo PBM.
 * @param larguraBarra Ponteiro para armazenar a largura das barras.
 * @param alturaCodigo Ponteiro para armazenar a altura do código.
 * @return true se o cabeçalho for válido, false caso contrário.
 */
bool verificarCabecalhoPbm(FILE *arquivo, int *larguraBarra, int *alturaCodigo){
    char linha[256];
    int caractere;

    // Ignora espaços em branco iniciais
    caractere = fgetc(arquivo);
    while (isspace(caractere)) {
        caractere = fgetc(arquivo);
    }

    if(caractere == EOF){
        printf("Erro! O arquivo esta vazio ou mal formatado.\n");
        return false;
    }

    // Retorna o caractere lido para o fluxo
    ungetc(caractere, arquivo);

    // Lê a primeira linha para verificar o formato
    if(!fgets(linha, sizeof(linha), arquivo)){
        printf("Erro! Nao foi possivel ler o formato.\n");
        return false;
    }

    // Remove caracteres de nova linha
    linha[strcspn(linha, "\r\n")] = '\0';

    if(strcmp(linha, "P1") != 0){
        printf("Erro! O arquivo nao esta no formato PBM esperado (P1).\n");
        return false;
    }

    // Lê as dimensões do arquivo
    if(!fgets(linha, sizeof(linha), arquivo) || sscanf(linha, "%d %d", larguraBarra, alturaCodigo) != 2){
        printf("Erro! Dimensoes invalidas no cabecalho do arquivo.\n");
        return false;
    }

    printf("%d %d", *larguraBarra, *alturaCodigo);
    return true;
}

/**
 * @brief Valida o identificador EAN-8.
 *
 * @param identificadorEan8 String contendo o identificador EAN-8.
 * @return true se o identificador for válido, false caso contrário.
 */
bool validarIdentificador(const char *identificadorEan8){
    if(strlen(identificadorEan8) != 8){
        printf("Erro! O identificador deve conter exatamente 8 digitos.\n");
        return false;
    }
    
    // Verifica se todos os caracteres são numéricos
    for(int indice = 0; indice < 8; indice++){
        if(identificadorEan8[indice] < '0' || identificadorEan8[indice] > '9'){
            printf("Erro! O identificador possui caracteres que nao sao numericos.\n");
            return false;
        }
    }
    
    // Calcula a soma ponderada para validação
    int soma = 0;
    for(int indice = 0; indice < 7; indice++){
        int digito = identificadorEan8[indice] - '0';
        if(indice % 2 == 0){
            soma += digito * 3;
        }
        else{
            soma += digito;
        }
    }
    
    // Calcula o dígito verificador
    int digitoVerificador = (10 - (soma % 10)) % 10;
    if(digitoVerificador != (identificadorEan8[7] - '0')){
        printf("Erro! Identificador nao e valido.\n");
        return false;
    }
    return true;
}

/**
 * @brief Gera o código de barras EAN-8 e salva em um arquivo PBM.
 *
 * @param dadosConfig Estrutura contendo as configurações para geração do código.
 * @return true se o código foi gerado com sucesso, false caso contrário.
 */
bool gerarCodigo(const dadosConfiguracaoBarras *dadosConfig){
    if(!validarIdentificador(dadosConfig->identificadorEan8)){
        return false;
    }

    FILE *arquivo = fopen(dadosConfig->nomeArquivoSaida, "w");
    if(!arquivo){
        printf("Erro! Nao foi possivel criar o arquivo %s.\n", dadosConfig->nomeArquivoSaida);
        return false;
    }

    const char *marcadorInicio = "101";
    const char *marcadorCentro = "01010";
    const char *marcadorFim = "101";

    int larguraCodigo = dadosConfig->espacamentoLateral * 2 + (3 + 28 + 5 + 28 + 3) * dadosConfig->larguraBarra;
    int alturaTotal = dadosConfig->alturaCodigo + 2 * dadosConfig->espacamentoLateral;

    // Escreve o cabeçalho do arquivo PBM
    fprintf(arquivo, "P1\n%d %d\n", larguraCodigo, alturaTotal);

    // Adiciona espaçamento lateral superior
    for(int linha = 0; linha < dadosConfig->espacamentoLateral; linha++){
        for(int coluna = 0; coluna < larguraCodigo; coluna++){
            fprintf(arquivo, "0 ");
        }
        fprintf(arquivo, "\n");
    }

    // Gera o código de barras linha por linha
    for(int linha = 0; linha < dadosConfig->alturaCodigo; linha++){
        // Espaçamento lateral esquerdo
        for(int coluna = 0; coluna < dadosConfig->espacamentoLateral; coluna++){
            fprintf(arquivo, "0 ");
        }

        // Marcador de início
        for(int indice = 0; indice < 3; indice++){
            for(int largura = 0; largura < dadosConfig->larguraBarra; largura++){
                fprintf(arquivo, "%d ", marcadorInicio[indice] - '0');
            }
        }

        // Padrões da parte esquerda do EAN-8
        for(int indice = 0; indice < 4; indice++){
            const char *padrao = padroesEan8L[dadosConfig->identificadorEan8[indice] - '0'];
            for(int subindice = 0; subindice < 7; subindice++){
                for(int largura = 0; largura < dadosConfig->larguraBarra; largura++){
                    fprintf(arquivo, "%d ", padrao[subindice] - '0');
                }
            }
        }

        // Marcador central
        for(int indice = 0; indice < 5; indice++){
            for(int largura = 0; largura < dadosConfig->larguraBarra; largura++){
                fprintf(arquivo, "%d ", marcadorCentro[indice] - '0');
            }
        }

        // Padrões da parte direita do EAN-8
        for(int indice = 4; indice < 8; indice++){
            const char *padrao = padroesEan8R[dadosConfig->identificadorEan8[indice] - '0'];
            for(int subindice = 0; subindice < 7; subindice++){
                for(int largura = 0; largura < dadosConfig->larguraBarra; largura++){
                    fprintf(arquivo, "%d ", padrao[subindice] - '0');
                }
            }
        }

        // Marcador de fim
        for(int indice = 0; indice < 3; indice++){
            for(int largura = 0; largura < dadosConfig->larguraBarra; largura++){
                fprintf(arquivo, "%d ", marcadorFim[indice] - '0');
            }
        }

        // Espaçamento lateral direito
        for(int coluna = 0; coluna < dadosConfig->espacamentoLateral; coluna++){
            fprintf(arquivo, "0 ");
        }

        fprintf(arquivo, "\n");
    }

    // Adiciona espaçamento lateral inferior
    for(int linha = 0; linha < dadosConfig->espacamentoLateral; linha++){
        for(int coluna = 0; coluna < larguraCodigo; coluna++){
            fprintf(arquivo, "0 ");
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    return true;
}

/**
 * @brief Extrai o identificador EAN-8 a partir de uma sequência binária.
 *
 * @param sequenciaBinaria Sequência de bits representando o código de barras.
 * @param identificadorEan8 Buffer para armazenar o identificador EAN-8 extraído.
 * @return true se a extração for bem-sucedida, false caso contrário.
 */
bool extrairIdentificador(const char *sequenciaBinaria, char *identificadorEan8){
    int posicao = 0;

    // Extrai os 4 primeiros dígitos (parte esquerda)
    for(int indice = 3; indice < 31; indice += 7){
        bool encontrado = false;
        for(int subindice = 0; subindice < 10; subindice++){
            if(strncmp(&sequenciaBinaria[indice], padroesEan8L[subindice], 7) == 0){
                identificadorEan8[posicao++] = '0' + subindice;
                encontrado = true;
                break;
            }
        }
        if(!encontrado){
            printf("Erro! Sequencia L-Code nao e valida.\n");
            return false;
        }
    }

    // Extrai os 4 últimos dígitos (parte direita)
    for(int indice = 36; indice < 64; indice += 7){
        bool encontrado = false;
        for(int subindice = 0; subindice < 10; subindice++){
            if(strncmp(&sequenciaBinaria[indice], padroesEan8R[subindice], 7) == 0){
                identificadorEan8[posicao++] = '0' + subindice;
                encontrado = true;
                break;
            }
        }
        if(!encontrado){
            printf("Erro! Sequencia R-Code nao e valida.\n");
            return false;
        }
    }

    identificadorEan8[posicao] = '\0';
    return true;
}

/**
 * @brief Verifica os marcadores de início, centro e fim no código de barras.
 *
 * @param matriz Matriz representando a imagem PBM.
 * @param larguraBarra Largura das barras no código de barras.
 * @param alturaCodigo Altura do código de barras.
 * @param larguraModulo Fator de modulação para leitura dos bits.
 * @param sequenciaBinaria Buffer para armazenar a sequência binária extraída.
 * @return true se os marcadores forem válidos, false caso contrário.
 */
bool verificarMarcadores(int **matriz, int larguraBarra, int alturaCodigo, int larguraModulo, char *sequenciaBinaria){
    int espacamento = (larguraBarra - (larguraModulo * 67)) / 2;
    int posicao = 0;

    // Converte a matriz em uma sequência binária
    for(int indice = espacamento; indice < larguraBarra - espacamento; indice += larguraModulo){
        int soma = 0;
        for(int subindice = indice; subindice < indice + larguraModulo; subindice++){
            soma += matriz[alturaCodigo / 2][subindice];
        }
        if(soma > (larguraModulo / 2)){
            sequenciaBinaria[posicao++] = '1';
        }
        else{
            sequenciaBinaria[posicao++] = '0';
        }
    }
    sequenciaBinaria[posicao] = '\0';

    // Define as posições dos marcadores
    int marcadorInicio = 0;
    int marcadorCentro = 3 + 28;
    int marcadorFim = marcadorCentro + 5 + 28;

    // Verifica o marcador de início
    if(strncmp(&sequenciaBinaria[marcadorInicio], "101", 3) != 0){
        printf("Erro! Marcador de inicio nao e valido.\n");
        return false;
    } 
    // Verifica o marcador central
    if(strncmp(&sequenciaBinaria[marcadorCentro], "01010", 5) != 0){
        printf("Erro! Marcador central nao e valido.\n");
        return false;
    } 
    // Verifica o marcador de fim
    if(strncmp(&sequenciaBinaria[marcadorFim], "101", 3) != 0){
        printf("Erro! Marcador de fim nao e valido.\n");
        return false;
    }

    return true;
}
