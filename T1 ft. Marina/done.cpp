#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

const int tam_fixo=120; 

struct PESSOA
{
  char nome[50];
  char disciplina[50];
  char sigla[4];
  char ID[8];
  float media; 
  float frequencia;
};

struct leitura
{
  char nome[50];
  char disciplina[50];
  char sigla[4];
  char ID[8];
  char media[5]; 
  char frequencia[5];
};

int insere_pos(FILE *pt_arq, int espaco_necessario)
{
	int livre, tam, aux, pos_livre; 
	fseek(pt_arq, 0, SEEK_SET); //coloca o ponteiro no in�cio do arquivo 
	pos_livre=ftell(pt_arq); 
	while(fread(&livre, sizeof(livre), 1, pt_arq))//lendo o head
	{
		if(livre==-1)//se nenhum arquivo foi deletado 
		{	
			return(-1); 
		}
		fseek(pt_arq, livre-sizeof(int), SEEK_SET); //vai para a posi��o marcada como livre 		
		fread(&tam, sizeof(tam), 1, pt_arq); //l� o tamanho 
		aux=ftell(pt_arq); //salva a posi��o de retorno 
		if(tam >= espaco_necessario)//verifica se o tamanho da posi��o � suficiente. 
		{
			fseek(pt_arq, sizeof(char), SEEK_CUR); //pula o * 
			fread(&livre, sizeof(livre), 1, pt_arq); //l� o pr�ximo ponteiro 
			fseek(pt_arq, pos_livre, SEEK_SET); //volta na posi��o do �ltimo ponteiro 
			fwrite(&livre, sizeof(livre), 1, pt_arq); //sobrescreve o �ltimo ponteiro com o pr�ximo "livre" 
			return(aux); 
		}
		fseek(pt_arq, sizeof(char), SEEK_CUR); //pula o * 
		pos_livre=ftell(pt_arq); 
	}
	return 0;
}

void insercao(FILE *pt_arq)
{
    FILE *pt_fonte=NULL; 
    struct PESSOA campos; 
    char registro[tam_fixo+30]; 
    int tam, tam_reg; 
    int num_reg; 
    int pos_dest; 
    int pos_fonte, pos_erro; 
    int alerta_erro; 
    fseek(pt_arq, 0, SEEK_SET); //coloca o ponteiro no come�o 
    //abrindo o arquivo
    pt_fonte=fopen("insere.bin", "rb");    
    if(pt_fonte==NULL)
    {
        printf("Erro fatal."); 
        exit(1);
    }
    
    //encontrando a posi��o que ser� lida no arquivo fonte 
    fseek(pt_arq, sizeof(int), SEEK_SET); //pula o head 
    fread(&num_reg, sizeof(tam), 1, pt_arq); //conta quantos registros existem no arquivo 
	
	//verificando se a posi��o de leitura � v�lida 
    pos_fonte = num_reg*tam_fixo; 
   
    //atualiza num_reg
	fseek(pt_arq, sizeof(int), SEEK_SET); 
	num_reg++; 
	fwrite(&num_reg, sizeof(int), 1, pt_arq); 
	
    fseek(pt_fonte, 0, SEEK_END); 
    pos_erro=ftell(pt_fonte); 
	if(pos_fonte>=pos_erro)
	{
		printf("\nOperacao invalida\n"); 
		return; 
	}
    fseek(pt_fonte, pos_fonte, SEEK_SET); 
    
    //lendo o primeiro registro 
    fread(campos.ID, sizeof(campos.ID), 1, pt_fonte); //lendo o tamanho 
	fread(campos.sigla,sizeof(campos.sigla), 1, pt_fonte);
	fread(campos.nome, sizeof(campos.nome), 1, pt_fonte); 
 	fread(campos.disciplina, sizeof(campos.disciplina), 1, pt_fonte);
 	fread(&campos.media, sizeof(campos.media), 1, pt_fonte);
 	fread(&campos.frequencia, sizeof(campos.frequencia), 1, pt_fonte);
 	sprintf(registro,"%s#%s#%s#%s#%.1f#%.1f#", campos.ID, campos.sigla,campos.nome, campos.disciplina, campos.media, campos.frequencia); 
 	tam_reg=strlen(registro); 
 	
 	//encontrando a posi��o de escrita 
 	pos_dest=insere_pos(pt_arq, tam_reg); 
 	if(pos_dest==-1)//flag para indicar que o registro ser� inserido numa posi��o "nova"
 	{
 		fseek(pt_arq, 0, SEEK_END); //coloca na �ltima posi��o 
 		fwrite(&tam_reg, sizeof(int), 1, pt_arq); //insere o tamanho do novo registro
	 }
	 else 
	 {
	 	fseek(pt_arq, pos_dest, SEEK_SET);//vai para a posi��o de destino do novo registro 
		}
 	//escrevendo
 	fwrite(registro, tam_reg, 1, pt_arq); 
 	fclose(pt_fonte); 
}

void remocao(FILE *pt_arq)
{
	int HEAD=0; 
	char ID[8], IDr[8]; 
	char sigla[4], siglar[4]; 
	int tam; 
	int tam_rem=12;				 // tamanho do registro a ser removido (remove.bin)
	char aster='*'; 
	int pos_rem=0; 
	int aux_head; 
	int check_erro; 
	FILE *pt_rem=NULL; 
	pt_rem=fopen("remove.bin","rb"); 

	if(pt_rem==NULL)//verifica se o arquivo foi aberto 
	{
		printf("\nErro fatal.");
		return;
	}
	
	fseek(pt_arq, 0, SEEK_SET); 
	fread(&HEAD, sizeof(int), 1, pt_arq); 
	
	if(HEAD!=-1) //se algum arquivo j� foi deletado 
	{
		aux_head=HEAD;
		while(aux_head!=-1)//enquanto nao achou a �ltima casa da fila 
		{
			fseek(pt_arq, aux_head, SEEK_SET); //pt_arq encontra o pr�ximo item da fila 
			pos_rem++; //conta quantos registros deletados j� foram percorridos 
			fseek(pt_arq, sizeof(char), SEEK_CUR); //pula o * e o tamanho do espa�o "livre"
			//at� chegar ao ponteiro para o pr�ximo espa�o livre 
			fread(&aux_head, sizeof(aux_head), 1, pt_arq); 
		}
		pos_rem=pos_rem*tam_rem; 
	}
	
	//checando se a posi��o existe 
	fseek(pt_rem, 0, SEEK_END); 
	check_erro = ftell(pt_rem); //salva a �ltima posi��o do arquivo em check_erro
	fseek(pt_rem, pos_rem, SEEK_SET); //coloca o arquivo na posi�ao do registro que vamos ler 
	if(ftell(pt_rem)>=check_erro)//verifica se a posi��o existe 
	{
		printf("\nErro.\n");
		getchar(); 
		return; 
	}
	
	//lendo a chave de busca 
	fseek(pt_rem, pos_rem, SEEK_SET); //coloca o arquivo na posi��o que vamos ler, 
									  //que � a pr�xima posi��o no arquivo rem 
	fread(IDr, sizeof(IDr)-1, 1, pt_rem);      //le IDr 7 caracteres 
	IDr[7] = '\0';
	fseek(pt_rem, sizeof(char), SEEK_CUR);   //pula o "."
	fread(siglar, sizeof(siglar)-1, 1, pt_rem); 
	siglar[3] = '\0';
	//coloca um novo valor em check_erro para verifica��o 
		fseek(pt_arq, 0, SEEK_END);
		check_erro=ftell(pt_arq); //check_erro cont�m a �ltima posi��o v�lida 
	
	fseek(pt_arq, 2*sizeof(int), SEEK_SET);//coloca o ponteiro de arq na 1� posi��o (pula o HEAD e o L)
	
	do{			
		//pula o tamanho do registro menos o que j� foi lido (sigla, # e ID) 
		fread(&tam, sizeof(int), 1, pt_arq); 
		aux_head=ftell(pt_arq); //salva a posi��o do registro que ser� deletado
		fread(ID, sizeof(ID)-1, 1, pt_arq); 
		ID[7]= '\0';
		fseek(pt_arq, sizeof(char), SEEK_CUR);//pula o "#"
		fread(sigla,sizeof(sigla)-1 ,1, pt_arq); 
		sigla[3]= '\0';

		if(ftell(pt_arq)>check_erro)//verifica se a posi��o � v�lida
		{
			//entra aqui caso todo o arquivo j� tenha sido percorrido e a chave nao tenha sido encontrada
			printf("\nErro.Chave nao encontrada\n"); 
			exit(1);
		}
		//vai lendo e comparando at� encontrar a chave de busca 
		fseek(pt_arq, tam - 3*sizeof(char)-7*sizeof(char)-sizeof(char), SEEK_CUR); //arruma o ponteiro p/ o proximo registro 

	}while((strcmp(ID, IDr)!=0) || (strcmp(sigla, siglar)!=0));	//compara as strings 

	//encontrou a chave de busca 
	fseek(pt_arq, aux_head, SEEK_SET);
	fwrite(&aster, sizeof(aster), 1, pt_arq); //altera o primeiro caractere do registro para "*"
	fwrite(&HEAD, sizeof(int), 1, pt_arq); //guarda o ponteiro para o pr�ximo registro apagado, que antes estava em HEAD
	HEAD=aux_head; //aux_read � o novo HEAD
	fseek(pt_arq, 0, SEEK_SET); 
	fwrite(&HEAD, sizeof(int), 1, pt_arq);//coloca o novo head na 1� casa  
}

//subrotina que encontra os campos dentro de um registro 
void achar_campos(char *registro, struct leitura *campos)//recebe o ponteiro do registro que vamos separar
{
	//coloca os dados em uma struct
	//lendo o registro em partes separadas por '|'
	strcpy(campos->ID,strtok(registro, "#"));
	strcpy(campos->sigla,strtok(NULL, "#"));
	strcpy(campos->nome,strtok(NULL, "#"));
	strcpy(campos->disciplina,strtok(NULL, "#"));
	strcpy(campos->media,strtok(NULL, "#"));
	strcpy(campos->frequencia,strtok(NULL, "#"));
}

FILE * compactar(FILE *pt_arq)
{
	FILE *novo_arq=NULL; 
	int erro; 
	struct leitura campos; 
	char registro[tam_fixo]; 
	int HEAD=-1, cont, tam, tam_novo; 
	//cria um arquivo temp 
	novo_arq=fopen("tempo.bin","wb"); 
	if(novo_arq==NULL)
	{
		printf("\nErro fatal."); 
		return(NULL); 
	}
	fseek(pt_arq,sizeof(int),SEEK_SET); 
	//lendo o arquivo pt_arq
	fread(&cont, sizeof(int), 1, pt_arq); 
	fwrite(&HEAD, sizeof(int), 1, novo_arq); 
	fwrite(&cont, sizeof(int), 1, novo_arq); 
	while(fread(&tam, sizeof(tam), 1, pt_arq))
	{
		fread(registro, tam, 1, pt_arq); //l� um registro de tamanho "tam" no arquivo 	
		if(registro[0]!='*')
		{
			achar_campos(registro, &campos); //separa os campos do registro 
			//calcula o tamanho total do registro (sem o lixo e a fragmenta��o interna) 
			tam_novo=strlen(campos.ID)+(6*sizeof(char))+strlen(campos.sigla)+strlen(campos.nome)+
			strlen(campos.disciplina)+ strlen(campos.media)+strlen(campos.frequencia); 
			//escrevendo no arquivo 
			fwrite(&tam_novo, sizeof(int), 1, novo_arq); 
			fwrite(campos.ID, strlen(campos.ID), 1, novo_arq); 
			fprintf(novo_arq, "#"); 
			fwrite(campos.sigla, strlen(campos.sigla), 1, novo_arq);
			fprintf(novo_arq, "#");  
			fwrite(campos.nome, strlen(campos.nome), 1, novo_arq); 
			fprintf(novo_arq, "#"); 
			fwrite(campos.disciplina, strlen(campos.disciplina), 1, novo_arq); 
			fprintf(novo_arq, "#"); 
			fwrite(campos.media, strlen(campos.media), 1, novo_arq); 
			fprintf(novo_arq, "#"); 
			fwrite(campos.frequencia, strlen(campos.frequencia), 1, novo_arq);
			fprintf(novo_arq, "#");  
		}
	}
	fclose(novo_arq); 
	fclose(pt_arq); 
	remove("arquivo.bin"); 
	rename("tempo.bin","arquivo.bin");
	novo_arq=fopen("arquivo.bin", "r+"); 
	return(novo_arq); 
}

int main(void)
{	
	int L=0; 
	int posicao=-1; 
	int HEAD_AUX=-1; 
	int opcao=0;
	FILE *pt_arq=NULL;
	
		//abrindo o arquivo 
		pt_arq=fopen("arquivo.bin", "r+b"); //tenta ler um arquivo novo 
		if(pt_arq==NULL) //o arquivo n�o est� aberto 
		{
			pt_arq=fopen("arquivo.bin", "w+b"); //tenta ler um arquivo novo
			fwrite(&HEAD_AUX, sizeof(int), 1, pt_arq); 
			fwrite(&L, sizeof(int), 1, pt_arq); 
		}
			
   while(opcao != 4)
   {
   	   //    system("clear"); 
   	   //    getchar();
	   printf("\nMenu: \n");
	   printf("Inserir um novo registro: 1\n");
	   printf("Remover um registro: 2\n"); 
	   printf("Compactar o Arquivo: 3\n");
	   printf("Encerrar o programa: 4\n");
	   scanf("%d",&opcao); 
	   
	   switch(opcao)
		{
			case 1: //Inser��o 
			   insercao(pt_arq); 
			   break;
			case 2:                //Remo��o
		  	   remocao(pt_arq); 
		      break;
			case 3:                //Compacta��o
			  pt_arq=compactar(pt_arq); 
		      break;
			case 4:                //Sair
			   fclose(pt_arq);//fecha o arquivo e sai do programa 
		       break;
		       return(0); 
			default:                //Nenhuma das alternativas
		       printf("Tecla invalida!\n");
		       break;
		       
		}//fim do switch case 
	}//fim do while
}//fim da main 
