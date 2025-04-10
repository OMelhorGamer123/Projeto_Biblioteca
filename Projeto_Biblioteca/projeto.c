#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sLivro {
    int id;
    int ano;
    char titulo[50];
    char autor[50];
    int disponivel;
} Livro;

Livro *p;
int quantidade = 0;

void salvarLivros() {
    FILE *arquivo = fopen("livros.dat", "wb"); // Abre o arquivo para escrita binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }
    
// Gravação da quantidade de livros e os livros
    fwrite(&quantidade, sizeof(int), 1, arquivo);
    fwrite(p, sizeof(Livro), quantidade, arquivo);
    
    fclose(arquivo);
    printf("Livros salvos com sucesso!\n");
}

void carregarLivros() {
    FILE *arquivo = fopen("livros.dat", "rb"); // Abre o arquivo para leitura binária
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
}

void listarLivros() {
    if (quantidade == 0) {
        printf("Nenhum livro cadastrado\n");
        return;
    }
    
    for (int i = 0; i < quantidade; i++) {
        printf("----\nID: %d\nTitulo: %s\nAutor: %s\nAno de Publicacao: %d\nDisponivel: %s\n----\n", 
            p[i].id, p[i].titulo, p[i].autor, p[i].ano, p[i].disponivel ? "Sim" : "Nao");
    }
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
    
// Remove o livro deslocando os elementos do vetor
    for (int i = id - 1; i < quantidade - 1; i++) {
        p[i] = p[i + 1];
        p[i].id = i + 1;  // Atualiza o ID para manter a sequência
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
    
    p[id - 1].disponivel = 0;
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
    
    p[id - 1].disponivel = 1;
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
        printf("6. Sair\n");
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
