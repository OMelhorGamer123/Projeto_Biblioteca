#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LIVROS 20

typedef enum {EMPRESTAR, DEVOLVER} TipoOperacao;

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
} Livro;

Livro *p;
int quantidade = 0;
NoPilha *topo = NULL;

int grafo[MAX_LIVROS][MAX_LIVROS];

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

    fclose(arquivo);
    printf("Livros carregados com sucesso!\n");
}

void adicionarLivros() {
    if (quantidade >= MAX_LIVROS) {
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

    if (id < 1 || id > quantidade || !p[id - 1].disponivel) {
        printf("Livro indisponivel ou ID invalido!\n");
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

    if (id < 1 || id > quantidade || p[id - 1].disponivel) {
        printf("Livro nao foi emprestado ou ID invalido!\n");
        return;
    }

    int disponivel_anterior = p[id - 1].disponivel;
    p[id - 1].disponivel = 1;
    push(DEVOLVER, id, disponivel_anterior);
    printf("Livro devolvido com sucesso!\n");
}

void inicializarGrafo() {
    for (int i = 0; i < MAX_LIVROS; i++) {
        for (int j = 0; j < MAX_LIVROS; j++) {
            if (i == j)
                grafo[i][j] = 0;
            else
                grafo[i][j] = INT_MAX;
        }
    }
}

void adicionarCaminho(int de, int para, int custo) {
    if (de >= 1 && de <= quantidade && para >= 1 && para <= quantidade) {
        grafo[de - 1][para - 1] = custo;
        grafo[para - 1][de - 1] = custo;
    }
}

void menorCaminho(int origem) {
    int dist[MAX_LIVROS], visitado[MAX_LIVROS];
    for (int i = 0; i < quantidade; i++) {
        dist[i] = INT_MAX;
        visitado[i] = 0;
    }
    dist[origem - 1] = 0;

    for (int count = 0; count < quantidade - 1; count++) {
        int u = -1;
        int min = INT_MAX;
        for (int i = 0; i < quantidade; i++) {
            if (!visitado[i] && dist[i] <= min) {
                min = dist[i];
                u = i;
            }
        }

        if (u == -1) break;

        visitado[u] = 1;

        for (int v = 0; v < quantidade; v++) {
            if (!visitado[v] && grafo[u][v] != INT_MAX && dist[u] + grafo[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo[u][v];
            }
        }
    }

    printf("\nDistancias minimas a partir do livro ID %d:\n", origem);
    for (int i = 0; i < quantidade; i++) {
        if (dist[i] == INT_MAX)
            printf("Para livro ID %d (%s): Inacessivel\n", p[i].id, p[i].titulo);
        else
            printf("Para livro ID %d (%s): Custo %d\n", p[i].id, p[i].titulo, dist[i]);
    }
}

void menuCaminhos() {
    int opcao;
    printf("\n--- Menu de Caminhos ---\n");
    printf("1. Adicionar caminho entre dois livros\n");
    printf("2. Calcular caminhos minimos a partir de um livro\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);
    getchar();

    if (opcao == 1) {
        int de, para, custo;
        printf("ID do livro de origem: ");
        scanf("%d", &de);
        printf("ID do livro de destino: ");
        scanf("%d", &para);
        printf("Custo do caminho: ");
        scanf("%d", &custo);
        getchar();
        adicionarCaminho(de, para, custo);
        printf("Caminho adicionado com sucesso!\n");
    } else if (opcao == 2) {
        int origem;
        printf("Digite o ID do livro de origem: ");
        scanf("%d", &origem);
        getchar();
        menorCaminho(origem);
    } else {
        printf("Opcao invalida!\n");
    }
}

int main() {
    int opcao;

    p = (Livro *) malloc(MAX_LIVROS * sizeof(Livro));
    if (p == NULL) {
        printf("Nao e possivel alocar memoria\n");
        exit(1);
    }

    inicializarGrafo();
    carregarLivros();

    while (1) {
        printf("\n----- Biblioteca -----\n");
        printf("1. Adicionar Livro\n");
        printf("2. Listar Livros\n");
        printf("3. Emprestar Livro \n");
        printf("4. Devolver Livro \n");
        printf("5. Retirar Livro\n");
        printf("6. Desfazer\n");
        printf("7. Caminhos Minimos\n");
        printf("8. Sair\n");
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
                desfazer();
                break;
            case 7:
                menuCaminhos();
                break;
            case 8:
                printf("Saindo...\n");
                salvarLivros();
                free(p);
                exit(0);
            default:
                printf("Opcao invalida, tente novamente\n");
                break;
        }
    }

    return 0;
}
