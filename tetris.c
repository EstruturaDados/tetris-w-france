
/*
 * Sistema de Peças - Fila Circular e Pilha de Reserva
 *
 * Requisitos atendidos:
 * - Fila circular de peças com capacidade fixa (5), sempre inicializada cheia
 * - Pilha de reserva com capacidade limitada (3)
 * - Ações: jogar peça (dequeue), reservar peça (dequeue + push), usar peça reservada (pop)
 * - Estado exibido após cada ação
 * - Peças removidas não retornam ao jogo
 * - Peças geradas automaticamente (gerarPeca) e adicionadas ao fim da fila após cada ação
 *
 * Bibliotecas utilizadas: stdio.h, stdlib.h, string.h, time.h (para aleatoriedade)
 * Comentários detalham a lógica, filas/pilhas e fluxo de execução.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----------------------- Configurações do Sistema ------------------------ */
#define QUEUE_CAP 5   // Capacidade fixa da fila de peças
#define STACK_CAP 3   // Capacidade máxima da pilha de reserva

/* ----------------------------- Definições -------------------------------- */
typedef struct {
    char nome;   // Tipo da peça: 'I', 'O', 'T', 'L'
    int id;      // ID único de criação (ordem crescente)
} Peca;

/* Fila circular: armazena as próximas peças do jogo */
typedef struct {
    Peca dados[QUEUE_CAP];
    int frente;     // índice do primeiro elemento
    int tamanho;    // quantidade atual na fila
} Fila;

/* Pilha linear (LIFO): reserva de peças */
typedef struct {
    Peca dados[STACK_CAP];
    int topo;       // quantidade atual (topo está em topo-1); - vazio se topo == 0
} Pilha;

/* ------------------------ Estado/Serviços Auxiliares --------------------- */
static int proximoId = 0;  // Contador global para IDs únicos

void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta lixo de entrada */ }
}

/* Gera uma nova peça com nome aleatório e ID sequencial */
Peca gerarPeca(void) {
    const char tipos[] = { 'I', 'O', 'T', 'L' };
    int idx = rand() % (int)(sizeof(tipos) / sizeof(tipos[0]));
    Peca p;
    p.nome = tipos[idx];
    p.id = proximoId++;
    return p;
}

/* ----------------------------- Fila Circular ----------------------------- */
void initFila(Fila *f) {
    f->frente = 0;
    f->tamanho = 0;
}

/* Retorna 1 se cheia, 0 caso contrário */
int filaCheia(const Fila *f) {
    return f->tamanho == QUEUE_CAP;
}

/* Retorna 1 se vazia, 0 caso contrário */
int filaVazia(const Fila *f) {
    return f->tamanho == 0;
}

/* Enfileira elemento no fim (retorna 1 se OK, 0 se cheia) */
int enfileirar(Fila *f, Peca p) {
    if (filaCheia(f)) return 0;
    int pos = (f->frente + f->tamanho) % QUEUE_CAP; // posição do fim
    f->dados[pos] = p;
    f->tamanho++;
    return 1;
}

/* Desenfileira elemento da frente (retorna 1 se OK, 0 se vazia) */
int desenfileirar(Fila *f, Peca *out) {
    if (filaVazia(f)) return 0;
    *out = f->dados[f->frente];
    f->frente = (f->frente + 1) % QUEUE_CAP;
    f->tamanho--;
    return 1;
}

/* ------------------------------- Pilha ----------------------------------- */
void initPilha(Pilha *p) {
    p->topo = 0;
}

int pilhaCheia(const Pilha *p) {
    return p->topo == STACK_CAP;
}

int pilhaVazia(const Pilha *p) {
    return p->topo == 0;
}

/* Empilha no topo (retorna 1 se OK, 0 se cheia) */
int empilhar(Pilha *p, Peca x) {
    if (pilhaCheia(p)) return 0;
    p->dados[p->topo] = x;
    p->topo++;
    return 1;
}

/* Desempilha do topo (retorna 1 se OK, 0 se vazia) */
int desempilhar(Pilha *p, Peca *out) {
    if (pilhaVazia(p)) return 0;
    p->topo--;
    *out = p->dados[p->topo];
    return 1;
}

/* ----------------------------- Visualização ------------------------------ */
void mostrarEstado(const Fila *f, const Pilha *p) {
    printf("\nEstado atual:\n\n");

    /* Fila: ordem de jogo (frente -> fim) */
    printf("Fila de peças\t");
    if (filaVazia(f)) {
        printf("(vazia)");
    } else {
        for (int i = 0; i < f->tamanho; i++) {
            int pos = (f->frente + i) % QUEUE_CAP;
            printf("[%c %d] ", f->dados[pos].nome, f->dados[pos].id);
        }
    }
    printf("\n");

    /* Pilha: topo -> base */
    printf("Pilha de reserva \t(Topo -> Base): ");
    if (pilhaVazia(p)) {
        printf("(vazia)");
    } else {
        for (int i = p->topo - 1; i >= 0; i--) {
            printf("[%c %d] ", p->dados[i].nome, p->dados[i].id);
        }
    }
    printf("\n");
}

/* ------------------------------ Ações ------------------------------------ */
void jogarPeca(Fila *fila) {
    Peca jogada;
    if (desenfileirar(fila, &jogada)) {
        printf("\n[Ação] Jogou peça: [%c %d]\n", jogada.nome, jogada.id);
    } else {
        printf("\n[Info] Não há peças na fila para jogar.\n");
    }
}

void reservarPeca(Fila *fila, Pilha *pilha) {
    if (pilhaCheia(pilha)) {
        printf("\n[Info] Pilha de reserva está cheia. Não é possível reservar.\n");
        return;
    }
    Peca frente;
    if (desenfileirar(fila, &frente)) {
        if (empilhar(pilha, frente)) {
            printf("\n[Ação] Reservou peça da fila para pilha: [%c %d]\n", frente.nome, frente.id);
        } else {
            /* Caso raro: pilha ficou cheia entre checagem e empilhar */
            printf("\n[Info] Pilha está cheia; peça [%c %d] não foi reservada e foi descartada.\n",
                   frente.nome, frente.id);
        }
    } else {
        printf("\n[Info] Fila está vazia; não há peça para reservar.\n");
    }
}

void usarPecaReservada(Pilha *pilha) {
    Peca usada;
    if (desempilhar(pilha, &usada)) {
        printf("\n[Ação] Usou peça reservada: [%c %d]\n", usada.nome, usada.id);
    } else {
        printf("\n[Info] Não há peças reservadas na pilha para usar.\n");
    }
}

/* Após cada ação, gera uma nova peça e tenta enfileirar para manter a fila cheia. */
void reporFilaComNovaPeca(Fila *fila) {
    Peca nova = gerarPeca();
    if (enfileirar(fila, nova)) {
        printf("[Sistema] Nova peça gerada e adicionada ao fim da fila: [%c %d]\n", nova.nome, nova.id);
    } else {
        printf("[Sistema] Fila já está cheia; nova peça [%c %d] foi descartada.\n", nova.nome, nova.id);
    }
}

/* ------------------------------ Execução --------------------------------- */
int main(void) {
    /* Inicialização da aleatoriedade para gerar peças variadas */
    srand((unsigned int)time(NULL));

    Fila fila;
    Pilha pilha;
    initFila(&fila);
    initPilha(&pilha);

    /* Inicializa a fila com QUEUE_CAP peças, mantendo-a cheia desde o início */
    for (int i = 0; i < QUEUE_CAP; i++) {
        enfileirar(&fila, gerarPeca());
    }

    printf("=== Sistema de Peças - Fila & Pilha de Reserva ===\n");
    printf("Curadoria de TI.\n\n");

    int opcao;
    do {
        mostrarEstado(&fila, &pilha);

        printf("\nOpções de Ação:\n");
        printf("Código\tAção\n");
        printf("1\tJogar peça\n");
        printf("2\tReservar peça\n");
        printf("3\tUsar peça reservada\n");
        printf("0\tSair\n");

        printf("\nOpção: ");
        if (scanf("%d", &opcao) != 1) {
            printf("[Erro] Entrada inválida. Tente novamente.\n");
            limparEntrada();
            continue;
        }
        limparEntrada();

        switch (opcao) {
            case 1:
                /* Jogar peça da fila */
                jogarPeca(&fila);
                reporFilaComNovaPeca(&fila);
                break;

            case 2:
                /* Reservar a peça da frente da fila para o topo da pilha (se houver espaço) */
                reservarPeca(&fila, &pilha);
                reporFilaComNovaPeca(&fila);
                break;

            case 3:
                /* Usar (remover) a peça do topo da pilha */
                usarPecaReservada(&pilha);
                reporFilaComNovaPeca(&fila);
                break;

            case 0:
                printf("\nEncerrando... As peças removidas não retornam ao jogo. Até mais!\n");
                break;

            default:
                printf("\n[Aviso] Opção inválida. Escolha entre 0, 1, 2, ou 3.\n");
                break;
        }

        /* Exibe estado após a ação e reposição */
        mostrarEstado(&fila, &pilha);

    } while (opcao != 0);

    return 0;
}