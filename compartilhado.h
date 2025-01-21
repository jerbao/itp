// Aluno: Jerônimo Rafael Bezerra Filho
#ifndef COMPARTILHADO_H
#define COMPARTILHADO_H

/**
 * Define a estrutura dadosConfiguracaoBarras e declara as funções compartilhadas entre os módulos.
 */

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Estrutura para armazenar as configurações do código de barras.
 */
typedef struct {
    const char *identificadorEan8;    /**< Identificador EAN-8 a ser codificado */
    const char *nomeArquivoSaida;     /**< Nome do arquivo de saída PBM */
    int espacamentoLateral;           /**< Espaçamento lateral nas bordas do código */
    int larguraBarra;                 /**< Largura de cada barra no código */
    int alturaCodigo;                 /**< Altura do código de barras */
} dadosConfiguracaoBarras;

/**
 * @brief Gera o código de barras EAN-8 e salva em um arquivo PBM.
 *
 * @param dadosConfig Estrutura contendo as configurações para geração do código.
 * @return true se o código foi gerado com sucesso, false caso contrário.
 */
bool gerarCodigo(const dadosConfiguracaoBarras *dadosConfig);

/**
 * @brief Valida o identificador EAN-8.
 *
 * @param identificadorEan8 String contendo o identificador EAN-8.
 * @return true se o identificador for válido, false caso contrário.
 */
bool validarIdentificador(const char *identificadorEan8);

/**
 * @brief Extrai o identificador EAN-8 a partir de uma sequência binária.
 *
 * @param sequenciaBinaria Sequência de bits representando o código de barras.
 * @param identificadorEan8 Buffer para armazenar o identificador EAN-8 extraído.
 * @return true se a extração for bem-sucedida, false caso contrário.
 */
bool extrairIdentificador(const char *sequenciaBinaria, char *identificadorEan8);

/**
 * @brief Verifica o cabeçalho de um arquivo PBM e extrai a largura e a altura.
 *
 * @param arquivo Ponteiro para o arquivo PBM.
 * @param larguraBarra Ponteiro para armazenar a largura das barras.
 * @param alturaCodigo Ponteiro para armazenar a altura do código.
 * @return true se o cabeçalho for válido, false caso contrário.
 */
bool verificarCabecalhoPbm(FILE *arquivo, int *larguraBarra, int *alturaCodigo);

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
bool verificarMarcadores(int **matriz, int larguraBarra, int alturaCodigo, int larguraModulo, char *sequenciaBinaria);

#endif
