#include<stdio.h>

int main() {
    FILE *fd;
    
    //////////////////////////////
    struct hist {
        char id_aluno[8];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[6] = {{"2001001", "AAA", "João da Silva", "Disc AAA", 7.5, 72.3},
                {"2001001", "BBB", "João da Silva", "Disc BBB", 8.2, 80.4},
                {"2001001", "CCC", "João da Silva", "Disc CCC", 5.4, 79.7},
                
                {"2002001", "CCC", "Paulo Souza", "Disc CCC", 6.8, 91.3},
                {"2002001", "HHH", "Paulo Souza", "Disc HHH", 7.3, 82.7},
                
                {"2002002", "HHH", "Pedro da Silva", "Disc HHH", 9.5, 92.7}};
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct remove {
        char id_aluno[8];
        char sigla_disc[4];
    } vet_r[4] = {{"2001001","CCC"},
                  {"2002001","HHH"},
                  {"2002002","HHH"},
                  {"2001001","AAA"}};
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

