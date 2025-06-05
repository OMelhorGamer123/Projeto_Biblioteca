#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        printf("3. Emprestar Livro \n");
        printf("4. Devolver Livro \n");
        printf("5. Retirar Livro\n");
        printf("6. Desfazer\n");
        printf("7. Sair\n");
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
