// =----------------------------------------------------------=
// =                           AVISO:                         =
// =                   MODIFIQUE ESTE ARQUIVO!                =
// = Aqui, você deve implementar, obrigatoriamente, essas     =
// = duas funções:                                            =
// =     - `a_menor_que_b`: deve retornar quando uma string   =
// =        é menor que a outra (ambos com o mesmo tamanho);  =
// =     - `external_sort`: aqui, você deverá implementar o   =
// =        seu trabalho. Ordene os registros em `input_file` =
// =        e escreva-os em `output_file`, usando somente     =
// =        `memory` KB de memória.                           =
// =                                                          =
// = Instruções:                                              =
// =     - use a função `mathias_malloc` ao invés de          =
// =       `malloc`;                                          =
// =     - use a função `mathias_free` ao invés de `free`;    =
// =     - lembre-se que você tem somente `memory` KB de      =
// =       memória;                                           =
// =     - note que a sua implementação de `a_menor_que_b`    =
// =       afetará o resultado da verificação da resposta.    =
// =                                                          =
// = Dicas:                                                   =
// =     - você pode usar a sua função `a_menor_que_b` em     =
// =       sua implementação da função `external_sort`;       =
// =                                                          =
// =                      BOM TRABALHO!                       =
// =----------------------------------------------------------=

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"

// Parâmetros:
//     - a: uma cadeia de caracteres de tamanho `len`;
//     - b: uma cadeia de caracteres de tamanho `len`;
//     - len: o tamanho (quantidade de bytes) de `a` e `b`.
// Essa função deve retornar 1, se `a` é menor que `b` (`a` deve vir antes de `b` no
// arquivo), e 0 caso contrário.
// Ela também é usada para verificação da ordenação! Então, se você implementá-la
// erroneamente, a verificação também estará errada! 
int a_menor_que_b(char* a, char* b, int len) {
    if (strcmp(a, b) < 0) {
        return 1;
    }
    return 0;
}

// Parâmetros:
//     - a: um índice do buffer guardado pelo heap;
//     - b: um índice do buffer guardado pelo heap;
// Essa função deve retornar 1, se `a` é do mesmo tipo que `b` (`a` e `b` devem
// ter o mesmo sinal), e 0 caso contrário.
int is_equal_type(int a, int b) {
    return (
        (a < 0 && b < 0) || // Ambos negativos
        (a > 0 && b > 0) // Ambos positivos
    );
}

// Parâmetros:
//     - a_index: um índice do heap;
//     - b_index: um índice do heap;
//     - heap: vetor que armazena índices do buffer;
//     - buffer: contém linhas lidas da memória externa;
//     - line_size: tamanho das linhas contidas no buffer;
//     - curr_block_type: tipo do bloco que estão sendo inseridas novas linhas;
// Essa função deve retornar 1, se `a` é menor `b` levando em consideração um
// tipo e os valores presentes no buffer. Caso os tipos sejam diferentes, o
// elemento que é do tipo especificado é considerado o menor.
int a_is_lower_than_b_on_heap(
    int a_index,
    int b_index,
    int *heap,
    char **buffer,
    int line_size,
    int curr_block_type
) {
    return (
        // a e b são do mesmo bloco
        is_equal_type(heap[a_index], heap[b_index]) &&
        // a é menor que b
        a_menor_que_b(
            buffer[abs(heap[a_index])],
            buffer[abs(heap[b_index])],
            line_size
        )
    ) || (
        // a e b não são do mesmo bloco
        !is_equal_type(heap[a_index], heap[b_index]) &&
        // a é do bloco atual
        is_equal_type(curr_block_type, heap[a_index])
    );
}

// Parâmetros:
//     - heap: vetor que armazena índices do buffer;
//     - heap_size: tamanho do vetor heap;
//     - node_index: índice do nó que deve ser rebaixado;
//     - buffer: contém linhas lidas da memória externa;
//     - line_size: tamanho das linhas contidas no buffer;
//     - curr_block_type: tipo do bloco que estão sendo inseridas novas linhas;
// Essa função rebaixa no heap o nó representado pelo índice fornecido de
// acordo com o critério do heap e com o tipo do bloco. No caso, o heap é um
// min-heap.
// A operação de rebaixamento de um nó no heap tem o pior caso como o nó que
// sendo a raíz, e seu lugar correto é ser uma das folhas do heap (maior
// elemento do heap). Assim, o loop principal é executado log(M) vezes, sendo M
// o tamanho máximo do heap dado pela quantidade de linhas que cabem na memória
// e log(M) sendo a  altura da árvore. Logo, a complexidade desta função é
// O(log(M)).
void demote_heap_node(
    int *heap,
    int heap_size,
    int node_index,
    char **buffer,
    int line_size,
    int curr_block_type
) {
    int temp;
    int son_index = 2 * node_index + 1;
    while (
        // Nó não é uma folha
        son_index < heap_size
    ) {
        // Pega o menor nó filho
        if (
            // O nó pai tem 2 nós filhos
            son_index < heap_size - 1 &&
            // O outro nó filho é menor
            a_is_lower_than_b_on_heap(
                son_index + 1, son_index,
                heap, buffer, line_size, curr_block_type
            )
        ) {
            son_index = son_index + 1;
        }

        if (a_is_lower_than_b_on_heap(
            node_index, son_index,
            heap, buffer, line_size, curr_block_type
        )) {
            // Nó está no lugar certo
            break;
        }

        // Troca nó pai com nó filho
        temp = heap[node_index];
        heap[node_index] = heap[son_index];
        heap[son_index] = temp;

        // Novo nó filho é recuperado
        node_index = son_index;
        son_index = 2 * node_index + 1;
    }
}

// Parâmetros:
//     - heap: vetor que armazena índices do buffer;
//     - heap_size: tamanho do vetor heap;
//     - node_index: índice do nó que deve ser promovido;
//     - buffer: contém linhas lidas da memória externa;
//     - line_size: tamanho das linhas contidas no buffer;
//     - curr_block_type: tipo do bloco que estão sendo inseridas novas linhas;
// Essa função promove no heap o nó representado pelo índice fornecido de
// acordo com o critério do heap e com o tipo do bloco. No caso, o heap é um
// min-heap.
// A operação de promoção de um nó no heap tem o pior caso como o nó que sendo
// uma folha, e seu lugar correto é ser a raíz do heap (menor elemento do heap).
// Assim, o loop principal é executado log(M) vezes, sendo M o tamanho máximo do
// heap dado pela quantidade de linhas que cabem na memória e log(M) sendo a
// altura da árvore. Logo, a complexidade desta função é O(log(M)).
void promote_heap_node(
    int *heap,
    int heap_size,
    int node_index,
    char **buffer,
    int line_size,
    int curr_block_type
) {
    int temp;
    int parent_index = (node_index - 1) / 2;
    while (
        // Nó não é a raiz do heap
        node_index > 0 &&
        // Nó não está no lugar certo
        a_is_lower_than_b_on_heap(
            node_index, parent_index,
            heap, buffer, line_size, curr_block_type
        )
    ) {
        // Troca nó pai com nó filho
        temp = heap[node_index];
        heap[node_index] = heap[parent_index];
        heap[parent_index] = temp;

        // Novo nó pai é recuperado
        node_index = parent_index;
        parent_index = (node_index - 1) / 2;
    }
}

// Parâmetros:
//     - ptr_input_file: ponteiro para o arquivo de entrada;
//     - ptr_output_file: ponteiro para o arquivo de saída;
//     - memory_max_lines: número máximo de linhas na memória interna;
//     - line_size: tamanho das linhas contidas no buffer;
//     - heap: vetor que armazena índices do buffer;
//     - buffer: contém linhas lidas da memória externa;
//     - aux_line: linha auxiliar;
// Essa função processa o arquivo de entrada dividindo-o em blocos ordenados que
// são registrados no arquivo de saída. Estes são os primeiros blocos que dão
// início ao processo de ordenação do arquivo.
// A função lê todas as N linhas do arquivo de entrada, fazendo operações de
// inserção no Heap com complexidade O(log(M)) no heap, sendo M o tamanho do
// heap (no máximo memory_max_lines). Logo, a complexidade final desta função é
// O(N*log(M)).
int create_initial_blocks(
    FILE *ptr_input_file,
    FILE *ptr_output_file,
    int memory_max_lines,
    int line_size,
    int *heap,
    char **buffer,
    char *aux_line
) {
    int curr_block_type = 1; // Tipo do bloco que está sendo gerado. (1 | -1)
    int heap_size = 0; // Tamanho atual do heap
    int block_count = 1; // Quantidade de blocos no arquivo de saída
    int index = 0; // Guarda o índices de linhas no buffer
    int i;

    fprintf(ptr_output_file, "%d\n", line_size);

    // Construção inicial do heap
    for (
        i = 1;
        (
            // Há espaço na memória
            i <= memory_max_lines &&
            // O arquivo de entrada não está vazio
            fscanf(ptr_input_file, "%[^\n]%*c", aux_line) != EOF
        );
        i++
    ) {
        // Adiciona a linha lida no buffer
        strcpy(buffer[i], aux_line);

        // Adiciona o índice da linha no buffer no final do heap
        heap[heap_size] = i;
        heap_size++;

        // Promove a linha lida no heap
        promote_heap_node(heap, heap_size, heap_size - 1,
            buffer, line_size, curr_block_type);
    }


    // Divisão em blocos ordenados escritos no arquivo de saída
    while (heap_size > 0) {
        index = heap[0]; // Recupera linha que está no topo do heap

        // Começa um novo bloco caso o tipo da linha seja diferente
        if (!is_equal_type(index, curr_block_type)) {
            curr_block_type = curr_block_type * (-1);
            fprintf(ptr_output_file, "\n"); // Separa os blocos
            block_count++;
        }

        // Coloca a linha retirada no arquivo de saída
        index = abs(index);
        fprintf(ptr_output_file, "%s\n", buffer[index]);
        strcpy(aux_line, buffer[index]);

        // Processa a próxima linha do arquivo de entrada
        if (
            fscanf(ptr_input_file, "%[^\n]%*c", buffer[index]) != EOF &&
            !feof(ptr_input_file)
        ) {
            // Coloca a linha lida no topo do heap
            if (a_menor_que_b(
                buffer[index],
                aux_line,
                line_size
            )) {
                // Marca com o tipo do próximo bloco
                heap[0] = index * curr_block_type * (-1);
            } else {
                // Marca com o tipo do bloco atual
                heap[0] = index * curr_block_type;
            }
        }
        // Se não houver mais linhas
        else {
            heap[0] = heap[heap_size - 1]; // Coloca uma folha do heap no topo
            heap[heap_size - 1] = 0;
            heap_size--;
        }

        // Rebaixa o topo do heap
        demote_heap_node(heap, heap_size, 0, buffer, line_size,
            curr_block_type);
    }

    return block_count;
}

// Parâmetros:
//     - ptr_output_file: ponteiro para o arquivo de saída;
//     - aux_files: vetor de arquivos auxiliares;
//     - aux_line: linha auxiliar;
//     - line_size: tamanho das linhas contidas no buffer;
//     - block_count: quantidade de blocos no arquivo de saída;
//     - memory_max_lines: número máximo de linhas na memória interna;
// Essa função distribui os blocos do arquivo de saída entre os arquivos
// auxiliares disponíveis. Ela não ultrapassa o limite de arquivos auxiliares
// definindo um número máximo de blocos em cada arquivo auxiliar.
// Para distrubuir entre os arquivos auxiliares, esta função passa por todas
// as N linhas que estavam presentes no arquivo de saída. Para cada linha, a
// função realiza operações constantes para copiá-la para o arquivo auxiliar.
// Assim, a complexidade da função é da ordem de O(N).
int spread_blocks_into_aux_files(
    FILE *ptr_output_file,
    FILE **aux_files,
    char *aux_line,
    int line_size,
    int block_count,
    int memory_max_lines
) {
    int aux_file_max_block_count = (block_count / memory_max_lines) + 1;
    int aux_file_curr_block_count = 1;
    int aux_files_count = 1;
    aux_files[aux_files_count] = tmpfile();
    FILE *ptr_aux_file = aux_files[aux_files_count];

    // Volta para o início do arquivo de saída
    rewind(ptr_output_file);
    // Pula a primeira linha
    fgets(aux_line, line_size + 1, ptr_output_file);

    // Lê linhas do arquivo de saída enquanto não chegar ao fim
    while (
        fgets(aux_line, line_size + 1, ptr_output_file) != NULL &&
        !feof(ptr_output_file)
    ) {
        // Caso seja uma divisão entre blocos
        if (strcmp(aux_line, "\n") == 0) {
            // Caso o arquivo auxiliar esteja com a quantidade de blocos
            // máxima
            if (aux_file_curr_block_count == aux_file_max_block_count) {
                // Volta o arquivo auxiliar para o início
                rewind(ptr_aux_file);
                // Abre outro arquivo auxiliar
                aux_files_count++;
                aux_files[aux_files_count] = tmpfile();
                ptr_aux_file = aux_files[aux_files_count];
                // Reinicia a contagem de blocos
                aux_file_curr_block_count = 1;
            }
            // Caso ainda caiba mais blocos no arquivo auxiliar
            else {
                fprintf(ptr_aux_file, "\n"); // Separa os blocos
                aux_file_curr_block_count++; // Conta mais 1 bloco
            }
        }
        // Caso seja uma linha normal
        else {
            // Pula a quebra de linha
            fscanf(ptr_output_file, "%*c");
            // Adiciona a linha no arquivo de saída
            fprintf(ptr_aux_file, "%s\n", aux_line);
        }
    }
    // Volta o arquivo auxiliar para o início
    rewind(ptr_aux_file);

    return aux_files_count;
}

// Parâmetros:
//     - ptr_output_file: ponteiro para o arquivo de saída;
//     - output_file: nome do arquivo de saída;
//     - aux_files: vetor de arquivos auxiliares;
//     - aux_files_locked: flags que indicam se o arquivo está bloqueado;
//     - aux_files_count: quantidade de arquivos auxiliares atual;
//     - buffer: contém linhas lidas da memória externa;
//     - aux_line: linha auxiliar;
//     - block_count: quantidade de blocos no arquivo de saída;
//     - heap: vetor que armazena índices do buffer;
// Essa função intercala os blocos que estão armazenados nos arquivos auxiliares
// registrando os novos blocos no arquivo de saída. Em todos os arquivos
// auxiliares estão contidas as N linhas do arquivo de entrada inicial. Cada
// linha é lida e inserida no Heap com uma complexidade de O(log(M)), sendo
// M o tamanho máximo do heap. Assim, o custo assintótico desta função é da
// ordem de O(N*log(M))
int merge_aux_files_blocks(
    FILE *ptr_output_file,
    const char *output_file,
    FILE **aux_files,
    int *aux_files_locked,
    int aux_files_count,
    char **buffer,
    char *aux_line,
    int line_size,
    int *heap
) {
    // Limpa o arquivo de saída
    ptr_output_file = freopen(output_file, "w+", ptr_output_file);
    fprintf(ptr_output_file, "%d", line_size);
    int heap_size;
    int block_count = 0;
    int index;
    int i;
    int aux_files_empty_count = 0; // Quantidade de arquivos já totalmente lidos

    // Enquanto houver arquivos auxiliares com linhas para serem lidas
    while(aux_files_empty_count < aux_files_count) {
        // Construção inicial do heap
        heap_size = 0; // Limpa o heap
        for (i = 1; i <= aux_files_count; i++) {
            // Caso haja linhas no arquivo auxiliar
            if (
                fscanf(aux_files[i], "%[^\n]%*c", aux_line) != EOF &&
                !feof(aux_files[i])
            ) {
                // Destrava o arquivo para continuar a leitura do bloco
                aux_files_locked[i] = 0;

                // Adiciona a linha no buffer
                strcpy(buffer[i], aux_line);

                // Adiciona o índice da linha no buffer no final do heap
                heap[heap_size] = i;
                heap_size++;

                // Promove a linha lida no heap
                promote_heap_node(heap, heap_size, heap_size - 1,
                    buffer, line_size, 1);
            }
            // Caso não haja linhas no arquivo auxiliar e ele não está
            // travado
            else if (aux_files_locked[i] == 0) {
                // Trava o arquivo
                aux_files_locked[i] = 1;
                // Incrementa o contador de arquivos vazios
                aux_files_empty_count++;
            }
        }

        // Caso haja mais blocos
        if (heap_size > 0) {
            fprintf(ptr_output_file, "\n"); // Divide os blocos
            block_count++;
        }

        // Enquanto o heap não estiver vazio
        while (heap_size > 0) {
            // Recupera linha que está no topo do heap
            index = heap[0];

            // Coloca a linha do topo no arquivo de saída
            fprintf(ptr_output_file, "%s\n", buffer[index]);

            // Se o arquivo não está vazio ou travado, lê uma nova linha
            // e a coloca como o novo topo
            if (
                // O arquivo auxiliar não está travado
                !aux_files_locked[index] &&
                // O arquivo auxiliar não está no final
                fgets(aux_line, line_size + 1, aux_files[index]) != NULL &&
                !feof(aux_files[index])
            ) {
                // Caso a linha lida seja uma separação de blocos
                if (strcmp(aux_line, "\n") == 0) {
                    // Trava o arquivo
                    aux_files_locked[index] = 1;
                    // Coloca uma folha do heap no topo
                    heap[0] = heap[heap_size - 1];
                    heap[heap_size - 1] = 0;
                    heap_size--;
                }
                // Caso seja uma linha normal
                else {
                    // Pula a quebra de linha
                    fscanf(aux_files[index], "%*c");
                    // Adiciona a linha no topo do heap
                    strcpy(buffer[index], aux_line);
                }
            }
            // Se o arquivo está travado ou vazio, coloca uma folha do heap
            // no topo
            else {
                // Coloca uma folha do heap no topo
                heap[0] = heap[heap_size - 1];
                heap[heap_size - 1] = 0;
                heap_size--;
            }

            // Rebaixa o topo do heap até seu lugar
            demote_heap_node(heap, heap_size, 0, buffer, line_size, 1);
        }
    }

    for (i = 1; i <= aux_files_count; i++) {
        aux_files_locked[i] = 0;
        if (aux_files[i] != NULL) {
            fclose(aux_files[i]);
            aux_files[i] = NULL;
        }
    }

    return block_count;
}

// Parâmetros:
//     - ptr_input_file: ponteiro para o arquivo de entrada;
//     - ptr_output_file: ponteiro para o arquivo de saída;
//     - output_file: nome do arquivo de saída;
//     - aux_files: vetor de arquivos auxiliares;
//     - aux_files_locked: flags que indicam se o arquivo está bloqueado;
//     - block_count: quantidade inicial de blocos no arquivo de saída;
//     - memory_max_lines: número máximo de linhas na memória interna;
//     - line_size: tamanho das linhas contidas no buffer;
//     - heap: vetor que armazena índices do buffer;
//     - buffer: contém linhas lidas da memória externa;
//     - aux_line: linha auxiliar;
// Essa função intercala os blocos que estão armazenados no arquivo de saída
// após a primeira passada sobre o arquivo de entrada. Ela faz a intercalação
// até que reste somente 1 bloco.
// Os blocos gerados através do Heap são em média 2 vezes maiores que o próprio
// tamanho do Heap. Assim, a quantidade de blocos B diminui em média 2 vezes
// após uma iteração do loop principal, levando à um total de log(B) iterações.
// Logo, a complexidade desta função é O(log(B)*(N*log(M)))
void merge_output_file_blocks(
    FILE *ptr_input_file,
    FILE *ptr_output_file,
    const char *output_file,
    FILE **aux_files,
    int *aux_files_locked,
    int block_count,
    int memory_max_lines,
    int line_size,
    int *heap,
    char **buffer,
    char *aux_line
) {
    int aux_files_count;
    // Enquanto estiver mais de 1 bloco no arquivo de saída
    while (block_count > 1) {
        // Distribui os blocos em arquivos auxiliares
        // O(N)
        aux_files_count =  spread_blocks_into_aux_files(ptr_output_file,
            aux_files, aux_line, line_size, block_count, memory_max_lines);

        // Intercala os blocos dos arquivos auxiliares no arquivo de saída
        // O(N*log(M))
        block_count = merge_aux_files_blocks(ptr_output_file, output_file,
            aux_files, aux_files_locked, aux_files_count, buffer, aux_line,
            line_size, heap);
    }
}

// Parâmetros:
//     - num: número de elementos no vetor;
//     - size: tamanho de um elemento;
// Essa função é inspirada na função calloc que aloca a memória zerando todas as
// posições. Ela utiliza o mathias_malloc para alocação e zera o conteúdo logo
// após.
void * mathias_calloc(int num, int size) {
    void *ptr = mathias_malloc(num * size);
    ptr = memset(ptr, 0, num * size);
    return ptr;
}

// Parâmetros:
//     - input_file: o nome do arquivo com os registros de entrada;
//     - output_file: o nome do arquivo com os registros de entrada ordenados;
//     - memory: o limite de memória a ser usado.
// Essa função deverá ler todos registros do arquivo `input_file`, ordená-los e
// escrevê-los em `output_file`, usando memória menor ou igual a `memory`. Lembre-se
// de fechar todos os arquivos que você usar por aqui!!
// A complexidade final de tempo para se ordenar o arquivo de entrada se dá por
// O((log(B))*(N*log(M))), sendo N o tamanho do arquivo de entrada e M a
// quantidade máxima de linhas que podem armazenadas em memória principal e B
// a quantidade de blocos gerados na primeira passada.
// O número de passadas necessário para ordenar o arquivo de entrada se dá por
// log_F(teto(N/2M)), sendo F no máximo M.
void external_sort(
    const char* input_file,
    const char* output_file,
    unsigned int memory
) {
    int i;

    // Ponteiros para os arquivos de I/O
    FILE *ptr_input_file = fopen(input_file, "r");
    FILE *ptr_output_file = fopen(output_file, "w+");

    // Tamanho das linhas do arquivo de entrada
    unsigned int line_size;
    fscanf(ptr_input_file, "%u%*c", &line_size);

    // Máximo de linhas na memória em bytes dada
    unsigned int memory_max_lines;
    memory_max_lines = (
        (
            (memory * 1024) - // Memória em bytes
            (line_size + 1) * sizeof(char) // aux_line
        ) / (
            (line_size + 1) * sizeof(char) + // Tamanho da linha em bytes
            sizeof(char *) + // buffer
            sizeof(int) + // heap
            sizeof(FILE *) + // aux_files
            sizeof(int) // aux_files_locked
        )
    );

    // Vetor que guarda as linhas trazidas da memória secundária
    char **buffer = (char **) mathias_malloc(memory_max_lines * sizeof(char *));
    for (i = 0; i < memory_max_lines; i++) {
        buffer[i] = (char *) mathias_malloc((line_size + 1) * sizeof(char));
    }
    buffer = &buffer[-1]; // Transforma o vetor para "1-indexed"

    // Heap que guardará os indices das strings no buffer
    int *heap = (int *) mathias_calloc(memory_max_lines, sizeof(int));

    // String auxiliar utilizada ao longo do programa
    char *aux_line = (char *) mathias_malloc((line_size + 1) * sizeof(char));

    // Ponteiros para os arquivos auxiliares utilizados
    FILE **aux_files = (FILE **) mathias_calloc(memory_max_lines, sizeof(FILE *));
    aux_files = &aux_files[-1]; // Transforma o vetor para "1-indexed"

    // Vetor que indica que um arquivo auxiliar ou está vazio ou está esperando
    // o término da leitura de outros blocos.
    int *aux_files_locked = (int *) mathias_calloc(memory_max_lines, sizeof(int));
    aux_files_locked = &aux_files_locked[-1]; // Transforma para "1-indexed"


    // Divisão do arquivo de entrada em blocos ordenados (O(N*log(M)))
    int block_count = create_initial_blocks(ptr_input_file, ptr_output_file,
        memory_max_lines, line_size, heap, buffer, aux_line);


    // Intercalação dos blocos do arquivo de saída (O(log(B)*(N*log(M))))
    merge_output_file_blocks(ptr_input_file, ptr_output_file, output_file,
        aux_files, aux_files_locked, block_count, memory_max_lines, line_size,
        heap, buffer, aux_line);


    // Libera memória alocada
    buffer = &buffer[1]; // Transforma vetor em "0-indexed"
    for (i = 0; i < memory_max_lines; i++) {
        mathias_free(buffer[i]);
    }
    mathias_free(buffer);
    mathias_free(heap);
    mathias_free(aux_line);
    aux_files = &aux_files[1]; // Transforma vetor em "0-indexed"
    mathias_free(aux_files);
    aux_files_locked = &aux_files_locked[1]; // Transforma vetor em "0-indexed"
    mathias_free(aux_files_locked);

    // Fecha arquivos de I/O
    fclose(ptr_input_file);
    fclose(ptr_output_file);
}