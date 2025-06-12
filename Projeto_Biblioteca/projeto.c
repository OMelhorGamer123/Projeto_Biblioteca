#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {EMPRESTAR, DEVOLVER} TipoOperacao;


typedef struct sNoFila {
    char nomePessoa[50];
    struct sNoFila *prox;
} NoFila;

typedef struct sFila {
    NoFila *inicio;
    NoFila *fim;
} Fila;


typedef struct sOperacao {
    TipoOperacao tipo;
    int id;
    int disponivel_anterior;
} Operacao;

typedef struct sNoPilha {
    Operacao operacao;
    struct sNoPilha *prox;
} NoPilha;

typedef struct sLivro {
    int id;
    int ano;
    char titulo[50];
    char autor[50];
    int disponivel;
    Fila filaDeEspera; 
} Livro;

Livro *p;
int quantidade = 0;
NoPilha *topo = NULL;


void enfileirar(Fila *f, const char *nome) {
    NoFila *novo = (NoFila *) malloc(sizeof(NoFila));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para a fila\n");
        return;
    }
    strcpy(novo->nomePessoa, nome);
    novo->prox = NULL;

    if (f->fim != NULL) {
        f->fim->prox = novo;
    } else {
        f->inicio = novo;
    }
    f->fim = novo;
    printf("Voce foi adicionado a fila de espera.\n");
}

char* desenfileirar(Fila *f) {
    if (f->inicio == NULL) {
        return NULL;
    }
    NoFila *temp = f->inicio;
    static char nome[50];
    strcpy(nome, temp->nomePessoa);

    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    free(temp);
    return nome;
}

void liberarFila(Fila *f) {
    NoFila *atual = f->inicio;
    while (atual != NULL) {
        NoFila *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    f->inicio = NULL;
    f->fim = NULL;
}

void exibirFila(Fila *f, int id) {
    if (f->inicio == NULL) {
        printf("Nao ha ninguem na fila de espera para o livro de ID %d.\n", id);
        return;
    }
    printf("Fila de espera para o livro de ID %d:\n", id);
    NoFila *atual = f->inicio;
    int pos = 1;
    while (atual != NULL) {
        printf("%d. %s\n", pos++, atual->nomePessoa);
        atual = atual->prox;
    }
}


void push(TipoOperacao tipo, int id, int disponivel_anterior) {
    NoPilha *novo = (NoPilha *) malloc(sizeof(NoPilha));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para a pilha\n");
        return;
    }
    novo->operacao.tipo = tipo;
    novo->operacao.id = id;
    novo->operacao.disponivel_anterior = disponivel_anterior;
    novo->prox = topo;
    topo = novo;
}

int pop(Operacao *op) {
    if (topo == NULL) {
        return 0;
    }
    NoPilha *temp = topo;
    *op = topo->operacao;
    topo = topo->prox;
    free(temp);
    return 1;
}

void desfazer() {
    Operacao op;
    if (pop(&op)) {
        if (op.id < 1 || op.id > quantidade) {
            printf("Livro com ID %d nao existe mais. Nao foi possivel desfazer.\n", op.id);
            return;
        }
        p[op.id - 1].disponivel = op.disponivel_anterior;
        printf("Operacao desfeita com sucesso.\n");
    } else {
        printf("Nenhuma operacao para desfazer.\n");
    }
}

void salvarLivros() {
    FILE *arquivo = fopen("livros.dat", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }
    fwrite(&quantidade, sizeof(int), 1, arquivo);
    fwrite(p, sizeof(Livro), quantidade, arquivo);
    
    fclose(arquivo);
    printf("Livros salvos com sucesso!\n");
}

void carregarLivros() {
    FILE *arquivo = fopen("livros.dat", "rb");
    if (arquivo == NULL) {
        printf("Nenhum arquivo de livros encontrado. Iniciando com biblioteca vazia.\n");
        return;
    }
    
    fread(&quantidade, sizeof(int), 1, arquivo);
    fread(p, sizeof(Livro), quantidade, arquivo);
    
    for(int i = 0; i < quantidade; i++) {
        p[i].filaDeEspera.inicio = NULL;
        p[i].filaDeEspera.fim = NULL;
    }

    fclose(arquivo);
    printf("Livros carregados com sucesso!\n");
}

void adicionarLivros() {
    if (quantidade >= 20) {
        printf("Biblioteca Cheia\n");
        return;
    }
    
    p[quantidade].id = quantidade + 1;
    
    printf("Digite o Titulo do Livro: ");
    fgets(p[quantidade].titulo, sizeof(p[quantidade].titulo), stdin);
    strtok(p[quantidade].titulo, "\n");
    
    printf("Digite o Autor do Livro: ");
    fgets(p[quantidade].autor, sizeof(p[quantidade].autor), stdin);
    strtok(p[quantidade].autor, "\n");
    
    printf("Digite o Ano de Publicacao do Livro: ");
    scanf("%d", &p[quantidade].ano);
    getchar();
    
    p[quantidade].disponivel = 1;
    p[quantidade].filaDeEspera.inicio = NULL;
    p[quantidade].filaDeEspera.fim = NULL;
    
    quantidade++;
    
    printf("Livro Cadastrado com Sucesso\n");
    
    char opcao;
    printf("Deseja adicionar outro livro? (s/n) ");
    scanf(" %c", &opcao);
    getchar();
    
    if (opcao == 's' || opcao == 'S') {
        adicionarLivros();
    }
}

void listarRecursivo(int index) {
    if (index >= quantidade) {
        return;
    }
    printf("----\nID: %d\nTitulo: %s\nAutor: %s\nAno de Publicacao: %d\nDisponivel: %s\n----\n", 
           p[index].id, p[index].titulo, p[index].autor, p[index].ano, p[index].disponivel ? "Sim" : "Nao");
    listarRecursivo(index + 1);
}

void listarLivros() {
    if (quantidade == 0) {
        printf("Nenhum livro cadastrado\n");
        return;
    }
    
    listarRecursivo(0);
}

void retirarLivro() {
    if (quantidade == 0) {
        printf("Nenhum livro cadastrado para retirar.\n");
        return;
    }
    
    int id;
    printf("Digite o ID do livro a ser retirado: ");
    scanf("%d", &id);
    getchar();
    
    if (id < 1 || id > quantidade) {
        printf("ID invalido!\n");
        return;
    }

    liberarFila(&p[id - 1].filaDeEspera);
    
    for (int i = id - 1; i < quantidade - 1; i++) {
        p[i] = p[i + 1];
        p[i].id = i + 1;
    }
    quantidade--;
    
    printf("Livro retirado com sucesso!\n");
}

void emprestarLivro() {
    int id;
    printf("Digite o ID do livro que deseja emprestar: ");
    scanf("%d", &id);
    getchar();
    
    if (id < 1 || id > quantidade) {
        printf("ID invalido!\n");
        return;
    }
    
    if (!p[id - 1].disponivel) {
        printf("Livro indisponivel no momento.\n");
        char opcao;
        printf("Deseja entrar na fila de espera? (s/n) ");
        scanf(" %c", &opcao);
        getchar();

        if (opcao == 's' || opcao == 'S') {
            char nome[50];
            printf("Digite seu nome: ");
            fgets(nome, sizeof(nome), stdin);
            strtok(nome, "\n");
            enfileirar(&p[id - 1].filaDeEspera, nome);
        }
        return;
    }
    
    int disponivel_anterior = p[id - 1].disponivel;
    p[id - 1].disponivel = 0;
    push(EMPRESTAR, id, disponivel_anterior);
    printf("Livro emprestado com sucesso!\n");
}

void devolverLivro() {
    int id;
    printf("Digite o ID do livro que deseja devolver: ");
    scanf("%d", &id);
    getchar();
    
    if (id < 1 || id > quantidade) {
        printf("ID invalido!\n");
        return;
    }

    if (p[id - 1].disponivel) {
        printf("Este livro ja esta disponivel!\n");
        return;
    }
    
    int disponivel_anterior = p[id - 1].disponivel;
    p[id - 1].disponivel = 1; // Marca como disponível
    push(DEVOLVER, id, disponivel_anterior);
    printf("Livro devolvido com sucesso!\n");

    char* proximoDaFila = desenfileirar(&p[id-1].filaDeEspera);
    if(proximoDaFila != NULL) {
        printf("Notificacao: O livro '%s' agora esta disponivel para %s, que era o proximo na fila.\n", p[id-1].titulo, proximoDaFila);
        p[id-1].disponivel = 0; // Torna indisponível novamente, pois foi "reservado"
        printf("O livro foi reservado para %s.\n", proximoDaFila);
    }
}

void verFilaDeEspera() {
    if (quantidade == 0) {
        printf("Nenhum livro cadastrado.\n");
        return;
    }
    
    int id;
    printf("Digite o ID do livro para ver a fila de espera: ");
    scanf("%d", &id);
    getchar();

    if (id < 1 || id > quantidade) {
        printf("ID invalido!\n");
        return;
    }

    exibirFila(&p[id - 1].filaDeEspera, id);
}

int main() {
    int opcao;
    
    p = (Livro *) malloc(20 * sizeof(Livro));
    if (p == NULL) {
        printf("Nao e possivel alocar memoria\n");
        exit(1);
    }
    carregarLivros();
    
    while (1) {
        printf("\n----- Biblioteca -----\n");
        printf("1. Adicionar Livro\n");
        printf("2. Listar Livros\n");
        printf("3. Emprestar Livro\n");
        printf("4. Devolver Livro\n");
        printf("5. Retirar Livro\n");
        printf("6. Ver Fila de Espera\n"); // NOVO
        printf("7. Desfazer ultima operacao\n"); // Movido
        printf("8. Sair\n"); // Movido
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                adicionarLivros();
                break;
            case 2:
                listarLivros();
                break;
            case 3:
                emprestarLivro();
                break;
            case 4:
                devolverLivro();
                break;
            case 5:
                retirarLivro();
                break;
            case 6: 
                verFilaDeEspera();
                break;
            case 7: 
                desfazer();
                break;
            case 8:
                printf("Saindo...\n");
                salvarLivros();
                for(int i = 0; i < quantidade; i++) {
                    liberarFila(&p[i].filaDeEspera);
                }
                free(p);
                exit(0);
            default:
                printf("Opcao invalida, tente novamente\n");
                break;
        }
    }
    return 0;
}
