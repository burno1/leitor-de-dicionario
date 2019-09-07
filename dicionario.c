
/**
 * BIBLIOTECAS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/**
 * estruturas de dados
 */
typedef struct _dupla { // Dupla chave-valor
    unsigned long long int chave;
    char palavra[30];
} t_DuplaHash;

typedef struct _tabela { // Tabela hash de duplas chave-valor
    t_DuplaHash **duplas;
    int tamanho;
    int quantidade;
} t_TabelaHash;



/**
 * FUNÇÕES
 */
int extrairPalavras(char arquivoDestino[]) { // Extrai somente as palavras do arquivo "base_tep2.txt" para o arquivo "palavras.txt"

    // Declarar variáveis locais
    FILE *origem = fopen("base_tep2.txt", "r");
    FILE *destino = fopen(arquivoDestino, "w");
	unsigned char letra, c;
	int contador = 0;

    // Abrir arquivos
	if (origem == NULL) {
		printf("Nao foi possivel abrir o arquivo %s\n\n", "base_tep2.txt");
		return 0;
	} else if (destino == NULL) {
		printf("Nao foi possivel criar o arquivo %s\n\n", "palavras.txt");
        fclose(origem);
		return 0;
	}

    // Realizar a leitura do arquivo e capturar as palavras
    letra = fgetc(origem);
	while (!feof(origem)) {
        
        // Iniciar cópia apenas quando encontrar "{"
        if (letra == '{') {
            while ((letra = fgetc(origem)) != EOF) {
                
                // Copiar caracteres apenas quando alfabéticos (ASCII 97 a 122), acentuados (ASCII 224 a 255) ou hífen (ASCII 45)
                if ((letra >= 97 && letra <= 122) || (letra >= 224 && letra <= 255) || letra == 45) {
                    fputc(letra, destino);
                    c = letra;
                
                // Interromper loop se encontrar "}"
                } else if (letra == '}' || feof(origem)) {
                    fputc('\n', destino);
                    contador++;
                    break;
                
                // Quebrar linha no arquivo destino ao fim de cada palavra
                } else if (c != 0) {
                    fputc('\n', destino);
                    contador++;
                    c = 0;
                }
            }
        }
        letra = fgetc(origem);
	}

    // Fechar arquivos abertos
	fclose(origem);
	fclose(destino);

    // Retornar número de palavras copiadas
    return contador;
}

unsigned long long int hashCode(unsigned char palavra[]) { // Gera o código hash (chave da dupla) para a palavra passada como parâmetro

    // Declarar variáveis locais
    unsigned long long int hash = 1;
    int i, len = strlen(palavra);

    // Iterar sobre caracteres de 'palavra' e calcular o valor hash
    for (i = 0; i < len; i++) {
        hash = hash * 3 * i + palavra[i];
    }

    // Retornar valor do código hash
    return hash;
}

t_DuplaHash montarDuplaHash(char palavra[]) { // Montar a estrutura completa da dupla chave-valor

    // Declarar variável de retorno
    t_DuplaHash par;

    // Definir valor das variáveis
    strcpy(par.palavra, palavra);
    par.chave = hashCode(palavra);
    return par;
}

t_TabelaHash *criarTabelaHash(int tamanho) { // Reserva o espaço em memória para a tabela hash

    // Validar valor do parâmetro
    if (tamanho < 1)
        return NULL;

    // Alocar valor inicial da tabela
    t_TabelaHash *tabelaHash;
    tabelaHash = (t_TabelaHash *) malloc(sizeof(t_TabelaHash));

    // Validar alocação de memória para o vetor
    if (tabelaHash == NULL)
        return NULL;

    // Guarda o tamanho da tavela e quantidade de itens
    tabelaHash->tamanho = tamanho;
    tabelaHash->quantidade = 0;

    // Aloca memória para o toda a extensão da tabela
    tabelaHash->duplas = (t_DuplaHash **) malloc(tamanho * sizeof(t_DuplaHash));

    // Validar alocação de memória para a tabela
    if (tabelaHash->duplas == NULL) {
        free(tabelaHash);
        return NULL;
    }

    // Anular eventuais valores alocado às duplas hash
    int i;
    for (i = 0; i < tabelaHash->tamanho; i++) {
        tabelaHash->duplas[i] = NULL;
    }
    return tabelaHash;
}

void liberarTabelaHash(t_TabelaHash *tabelaHash) { // Libera memória alocada para a estrutura de tabela hash

    // Validar se tabela já não é nula
    if (tabelaHash != NULL) {

        // Iterar sobre as duplas da tabela, desalocando memória
        int i;
        for (i = 0; i < tabelaHash->tamanho; i++) {
            if (tabelaHash->duplas[i] != NULL) {
                free(tabelaHash->duplas[i]);
            }
        }

        // Desalocar vetores remanescentes
        free(tabelaHash->duplas);
        free(tabelaHash);
    }
}

int hashingDivisao(unsigned long long int chave, int tamanho) { // Define posição da dupla no vetor a partir da dispersão das chaves (código hash
    return chave % tamanho;
}

int sondagemLinear(int posicao, int i, int tamanho) { // Trata colisões de duplas coincidentes, enfileirando-as numa lista encadeada
    return ((posicao + i) & 0x7FFFFFFF) % tamanho; 
}

int inserirHash(t_TabelaHash *tabelaHash, t_DuplaHash par) { // Insere uma dupla chave-valor na tabela hash

    // Avaliar se a tabela hash não é nula ou está cheia    
    if (tabelaHash == NULL || tabelaHash->quantidade == tabelaHash->tamanho)
        return 0;
    
    // Declarar variáveis locais
    int i, posicao, novaPosicao;

    // Calcular posição da dupla na tabela
    posicao = hashingDivisao(par.chave, tabelaHash->tamanho);

    // Iterar lista encadeada na posição encontrada e alocar memória ao fim (quando não há memória alocada)
    for (i = 0; i < tabelaHash->tamanho; i++) {
        novaPosicao = sondagemLinear(posicao, i, tabelaHash->tamanho);
        if (tabelaHash->duplas[novaPosicao] == NULL) {
            t_DuplaHash *novoPar = (t_DuplaHash *) malloc(sizeof(t_DuplaHash));
            
            // Validar alocação de memória
            if (novoPar == NULL)
                return 0;
            
            // Inserir valores de dupla na tabela hash
            *novoPar = par;
            tabelaHash->duplas[novaPosicao] = novoPar;

            // Incrementar quantidade de duplas na tabela
            tabelaHash->quantidade++;
            return 1;
        }
        if (!strcmp(tabelaHash->duplas[novaPosicao]->palavra, par.palavra))
            return 1;
    }
    return 0;
}

int carregarTabelaHash(t_TabelaHash *tabelaHash, char arquivo[]) { // Lê arquivo passado como parâmetro, monta duplas e as carrega na tabela hash

    // Declarar variáveis locais
    FILE *fonte;
    int maxBufferSize = 30;
    char buffer[maxBufferSize];
    int progresso = 1;
    
    // Abrir arquivo com palavras
    if ((fonte = fopen(arquivo, "r")) == NULL)
        return 0;

    // Ler linha a linha o arquivo
    while (fgets(buffer, maxBufferSize, fonte) != NULL) {

        // Substituir '\n' por '\0'
        int fimPalavra = strlen(buffer) - 1;
        buffer[fimPalavra] = '\0';

        // Montar dupla hash
        t_DuplaHash par = montarDuplaHash(buffer);

        // Inserir dupla na tabela hash
        inserirHash(tabelaHash, par);
    }

    // Retornar quantidade de pares inseridos na estrutura
    return tabelaHash->quantidade;
}

int buscarHash(t_TabelaHash *tabelaHash, t_DuplaHash par) { // Busca uma dupla na tabela, refazendo caminho de inserção e avaliando lista encadeada na posição
    
    // Avaliar se a tabela hash não é nula
    if (tabelaHash == NULL)
        return 0;

    // Declarar variáveis locais
    int i, posicao, novaPosicao;

    // Calcular posição da dupla na tabela
    posicao = hashingDivisao(par.chave, tabelaHash->tamanho);

    // Iterar lista encadeada na posição e avaliar contra dupla buscada
    for (i = 0; i < tabelaHash->tamanho; i++) {
        novaPosicao = sondagemLinear(posicao, i, tabelaHash->tamanho);

        // Avaliar se chegou ao fim da posição
        if (tabelaHash->duplas[novaPosicao] == NULL)
            return 0;

        // Avaliar se encontrou a palavra
        if (strcmp(tabelaHash->duplas[novaPosicao]->palavra, par.palavra))
            continue;
        else
            return i + 1;
    }
    return 0;
}



/**
 * PROGRAMA PRINCIPAL
 */
int main(int argc, char *argv[]) {

    // Limpar tela do terminal
    system("cls");

    // Carregar variáveis locais
    char arquivo[] = "palavras.txt";
    int tamanho, posLista, posSublista;
    t_TabelaHash *estrutura;

    // Informar inicio de programa
    printf("PROGRAMA INICIADO !!!\n");
    
    // Extrair palavras para arquivo separado
    printf("Extraindo palavras... ");
    if (!(tamanho = extrairPalavras(arquivo))) {
        printf("\n\tERRO AO ABRIR ARQUIVOS\n\n");
        return -1;
    }
    printf("%d palavras extraídas \n", tamanho);

    // Alocar memória para tabela hash
    printf("Criando tabela hash... ");
    tamanho = (int) tamanho * 1.5;
    if ((estrutura = criarTabelaHash(tamanho)) == NULL) {
        printf("\n\tERRO AO CRIAR TABELA HASH\n\n");
        return -2;
    }
    printf("Tabela hash criada com %d posicoes (%d bytes) \n", tamanho, tamanho *((int) sizeof(t_TabelaHash)));

    // Carregar palavras do arquivo na estrutura
    printf("Carregando palavras na tabela hash... ");
    if (!(tamanho = carregarTabelaHash(estrutura, arquivo))) {
        printf("\n\tERRO AO CARREGAR ESTRUTURA HASH\n\n");
        return -3;
    }
    printf("%d palavras unicas encontradas \n", tamanho);

    // Coletar dados do teclado
    char pesquisa[30];
    do {
        printf("\n");
        printf("Buscar palavra ('1' para sair): ");
        scanf("%s", pesquisa);
        if (pesquisa[0] != '1') {
            t_DuplaHash par = montarDuplaHash(pesquisa);
            posLista = hashingDivisao(par.chave, estrutura->tamanho);
            if (posSublista = buscarHash(estrutura, par)) {
                printf("\tPalavra '%s' na posicao %d[%d]\n", pesquisa, posLista, posSublista - 1);
            } else {
                printf("\tPalavra '%s' nao encontrada :(\n", pesquisa);
            }
        }
    } while (pesquisa[0] != '1');
    printf("\n\tENCERRANDO...\n\n");

    // Finalizar programa
    return 0;
}
