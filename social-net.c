#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define SIZE 100

typedef struct account {
	struct account* next;
	int id;
	char* name;
	char* surname;
}ACCOUNT;

typedef struct indegree {
	ACCOUNT* account;
	int indegree;
	ACCOUNT* headOfFollowerList;
}INDEGREE;

typedef struct accountInput {
	struct accountInput* next;
	int id;
	char* name;
	char* surname;
	int* ids;
	int idsLen;
}ACCOUNTINPUT;

typedef struct{
	int items[SIZE];
	int top;
}STACK;

void initStack(STACK* s){
	s->top = 0;	
}

int isFull(STACK* s){
	if(s->top == SIZE)
		return 1;
		
	return 0;
}

int isEmpty(STACK* s){
	if(s->top == 0)
		return 1;
		
	return 0;
}

int push(STACK* s, int x){
	if(isFull(s))
		return 0;
		
	s->items[(s->top)++] = x;
	
	return 1;
}

int pop(STACK* s, int* x){
	if(isEmpty(s))
		return 0;
		
	*x = s->items[--s->top];
	
	return 1;
}


int dfs(int** adjacencyMatrix, int n, int startNode){
	int i, currentPosition, counter;
	int* visited;
	STACK* stack;
	
	counter = 0;
	
	stack = (STACK*)malloc(SIZE*sizeof(STACK));
	visited = (int*)calloc(n, sizeof(int));
	initStack(stack);
	
	push(stack, startNode);
	currentPosition = startNode;
	
	visited[currentPosition] = 1;
			
	while(!isEmpty(stack)){
		i=1; 
		while(i<n && adjacencyMatrix[currentPosition][i] != 1)
			i++;	
		
		
		if(i == n){
			pop(stack, &currentPosition);
		}
		
		else{
			visited[currentPosition] = 1;
			adjacencyMatrix[currentPosition][i] = 0;
			currentPosition = i;
			push(stack, currentPosition);
		}
	}

	for(i=1; i<n; i++){
		if(visited[i] == 1 && i != startNode)
			counter++;
	}

	return counter;
}

int countLineNumberOfDoc(){
	FILE *fp;
	char c;
	int lineNumber;
	char name[] = "socialNET.txt";
	
	lineNumber = 0;
	fp = fopen(name,"r");
	
	for (c = getc(fp); c != EOF; c = getc(fp)){
    	if (c == '\n') 
        	lineNumber++;
	}
	fclose(fp);
	
	return lineNumber;
}

void printAccountInput(ACCOUNTINPUT* p) {
	printf("%d,%s,%s\n", p->id, p->name, p->surname);
}

ACCOUNTINPUT* getAccountInformationFromFile(FILE *fp, int lineNumberOfDoc){
	int i,totalCredit,numOfClasses;
	ACCOUNTINPUT* ap;
	char n[100],sn[100];
	char *token;
	char buf[255];
	char delimiter[2] = ",\0";
	
	ap  = (ACCOUNTINPUT*)malloc( sizeof(ACCOUNTINPUT) );
	
	ap->idsLen = 0;
	ap->ids = (int*)malloc((lineNumberOfDoc+3)*sizeof(int));
	
	fgets(buf,255,fp);
	
	token=strtok(buf,delimiter);
	ap->id = atoi(token);
	
	token=strtok(NULL,delimiter);
	ap->name = (char*)malloc(2*strlen(token)*sizeof(char));	
	strcpy(ap->name, token);

	token=strtok(NULL,delimiter);	
	token[strlen(token)-1] = '\0';
	ap->surname = (char*)malloc(2*strlen(token)*sizeof(char));	
	strcpy(ap->surname, token);
	
	fgets(buf,255,fp);
	
	token=strtok(buf,delimiter);	
	i = 0;
	while(token!=NULL && strcmp(token, "\n") != 0){
		ap->ids[i] = atoi(token);
		token=strtok(NULL,delimiter);
		i++;	
	}
	
	ap->idsLen = i;
	ap->next = NULL;
	
	return ap;	
}

void getAccountsFromDoc(ACCOUNTINPUT** head){	
	int n,lineNumberOfDoc;
	ACCOUNTINPUT *ptr;
	FILE *fp;
	char nameOfDoc[] = "socialNET.txt"; 

	lineNumberOfDoc = countLineNumberOfDoc(nameOfDoc)/2;
	n = lineNumberOfDoc;
	fp = fopen(nameOfDoc, "r");
	
	if(fp == NULL){
		printf("HATA '%s' isimli dosya acilamadi", nameOfDoc);
	}
	
	if(feof(fp) != 0){
		printf("Dosya bos oldugu icin okuma yapilmadi.\n");
		return;
	}
	
	while(lineNumberOfDoc > 0){	
		if(*head == NULL){
			*head = getAccountInformationFromFile(fp,n);
		}
		else{
			ptr = *head; 
			while(ptr->next != NULL){
				ptr = ptr->next;
			}
			ptr->next = getAccountInformationFromFile(fp,n);
		}
		lineNumberOfDoc--;
	}
	fclose(fp);
}

ACCOUNT* createAccountNode(int id, char* name, char* surname){
	ACCOUNT* ptr;
	
	ptr = (ACCOUNT*)malloc(sizeof(ACCOUNT));
	
	ptr->id = id;
	ptr->name = (char*)malloc(strlen(name)*sizeof(char));	
	ptr->surname = (char*)malloc(strlen(surname)*sizeof(char));
	
	strcpy(ptr->name, name);
	strcpy(ptr->surname, surname);
	
	ptr->next = NULL;
	
	return ptr;
}

void printMatrix(int** matrix, int n){
	int i,j;
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			printf("%2d,%2d: %d ", i,j,matrix[i][j]);
		}
		printf("\n");
	}
}


ACCOUNT* insertIntoLinkedList(ACCOUNT* head, ACCOUNTINPUT* accountInputPtr){
	ACCOUNT* ptr;
	
	if(head == NULL)
		head = createAccountNode(accountInputPtr->id, accountInputPtr->name, accountInputPtr->surname);
	
	else{
		ptr = head;
		while(ptr->next != NULL)
			ptr = ptr->next;
			
		ptr->next = createAccountNode(accountInputPtr->id, accountInputPtr->name, accountInputPtr->surname);
	}
	
	return head;
}

void printLinkedList(ACCOUNT* head){
	ACCOUNT* ptr;
	
	ptr = head;
	
	while(ptr != NULL){
		printf("%d, %s, %s\n", ptr->id,ptr->name, ptr->surname);
		ptr = ptr->next;
	}
}

void generateIndegreeArray(int** adjacencyMatrix, int n, INDEGREE* indegreeArray, ACCOUNTINPUT* headOfAccountInputList){
	int i,j,tempIndegree;
	ACCOUNTINPUT* ptr;
	
	ptr = headOfAccountInputList;
	while(ptr != NULL){
		indegreeArray[ptr->id].account = createAccountNode(ptr->id, ptr->name, ptr->surname);
		ptr = ptr->next;	
	}
	
	for(i=0; i<n; i++){
		indegreeArray[i].headOfFollowerList = NULL;
		indegreeArray[i].indegree = 0;
	}
	
	for(i=1; i<n; i++){
		for(j=1; j<n; j++){
			if(adjacencyMatrix[i][j] == 1){
				ptr = headOfAccountInputList;
				while(ptr != NULL && ptr->id != i){
					ptr = ptr->next;
				}
				if(ptr != NULL){
					indegreeArray[j].headOfFollowerList = insertIntoLinkedList(indegreeArray[j].headOfFollowerList, ptr);
					indegreeArray[j].indegree++;
					tempIndegree = indegreeArray[j].indegree;
				}
			}
		}
	}	
}

void generateAdjacencyMatrix(int** adjacencyMatrix, ACCOUNTINPUT* head){
	int i,n;
	ACCOUNTINPUT* ptr; 
	ptr = head;
	while(ptr!=NULL){
		n = ptr->idsLen;
		for(i=0; i<n; i++){
			adjacencyMatrix[ptr->id][ptr->ids[i]] = 1;
		}
		ptr = ptr->next;
	}
}

ACCOUNT* removeUserFromAccountList(ACCOUNT* head, int id, int* numOfDeleted){
	ACCOUNT* ptr;
	ACCOUNT* temp;
	
	ptr = head;
	
	if(head == NULL)
		return head;
	
	if(head->id == id){
		(*numOfDeleted)++;
		return head->next;
	}
	
	while(ptr->next != NULL && ptr->next->id != id)
		ptr = ptr->next;
		
	
	if(ptr->next != NULL && ptr->next->id == id){
		(*numOfDeleted)++;
		temp = ptr->next;
		ptr->next = ptr->next->next;
		free(temp);
	}

	return head;
}

void removeUserFromIndegreeArray(int n, INDEGREE* indegreeArray,  int id){
	int i=0; 
	int numOfDeleted;
	
	indegreeArray[id].indegree = INT_MAX;
	for(i=1; i<n; i++){
		numOfDeleted = 0;
		if(indegreeArray[i].indegree != INT_MAX){
			indegreeArray[i].headOfFollowerList = removeUserFromAccountList(indegreeArray[i].headOfFollowerList, id, &numOfDeleted);
			indegreeArray[i].indegree -= numOfDeleted;	
		}
	}
	
}

int isIndegreeArrayContains(int n, INDEGREE* indegreeArray, int indegreeValue, int* id){
	int i = 1;
		
	while(i<n && indegreeArray[i].indegree >= indegreeValue)
		i++;
	
	if(i==n)
		return 0;

	*id = i;
		
	return 1;
}

void clearIndegreeArray(INDEGREE* indegreeArray, int n){
	int id, m;
	
	printf("M Degerini Giriniz: ");
	scanf("%d", &m);
	
	while(isIndegreeArrayContains(n,indegreeArray, m, &id)){
		removeUserFromIndegreeArray(n,indegreeArray,id);				
	}
}

void generateAdjacencyMatrixOfFollowers(int** adjacencyMatrix, INDEGREE* indegreeArray, int n){
	int i,j;
	ACCOUNT* ptr;
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			adjacencyMatrix[i][j] = 0;
		}
	}
	
	for(i=1; i<n; i++){
		if(indegreeArray[i].indegree != INT_MAX){
			ptr = indegreeArray[i].headOfFollowerList;
			while(ptr != NULL){
				adjacencyMatrix[i][ptr->id] = 1;
				ptr = ptr->next;
			}	
		}
	}		
}

void printIndegreeArray(INDEGREE* array, int n){
	int i;
	
	printf("\n");
	printf("Indegree Degerleri\n\n");
	printf("INDEX\tINDEGREE DEGERI\n");
	for(i=1; i<n; i++)
		if(array[i].indegree != INT_MAX)
			printf("%5d\t%d\n",i, array[i].indegree);
		
	printf("\n");	
}

int menu(){
	int menuSelection;
	
	printf("MENU\n");
	printf("1- Normal Modda Calistir\n");
	printf("2- Detay Modda Calistir\n");
	printf("3- Cikis\n");
	printf("Seciminiz: ");
	scanf("%d", &menuSelection);
	
	return menuSelection;
}

void clearAdjacencyMatrix(int** matrix, int n){
	int i,j;
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			matrix[i][j] = 0; 
		}
	}
}

int main(void){
	INDEGREE* indegreeArray;
	int i,j,lineNumberOfDoc = 0 , numOfElements, x,y;
	int modeSelection = 1;
	ACCOUNTINPUT* inputList = NULL;
	int** adjacencyMatrix;
	int* indirectDegree;
	char idTitle[] = "ID";
	char nameTitle[] = "Isim";
	char surnameTitle[] = "Soyisim";
	char indegreeTitle[] = "Indegree Degerleri";
	char indirectDegreeTitle[] = "Dolayli Baglanti";
	
	lineNumberOfDoc = countLineNumberOfDoc();
	numOfElements = lineNumberOfDoc/2;
	indegreeArray = (INDEGREE*)malloc(lineNumberOfDoc*sizeof(INDEGREE));
	
	adjacencyMatrix = (int**)calloc((numOfElements+1), sizeof(int*));
	indirectDegree = (int*)calloc(numOfElements+1,sizeof(int));
	
	for(i=0; i<numOfElements+1; i++)
		adjacencyMatrix[i] = (int*)calloc((numOfElements+1), sizeof(int));
	
	
	getAccountsFromDoc(&inputList);
	generateAdjacencyMatrix(adjacencyMatrix,inputList);
	
	generateIndegreeArray(adjacencyMatrix, numOfElements+1, indegreeArray, inputList);
	
	modeSelection = menu();
	
	while(modeSelection != 3){
		if(modeSelection == 2)
			printIndegreeArray(indegreeArray, numOfElements+1);
		
		clearIndegreeArray(indegreeArray, numOfElements+1);	
		
		if(modeSelection == 2)
			printIndegreeArray(indegreeArray, numOfElements+1);
	
		generateAdjacencyMatrixOfFollowers(adjacencyMatrix, indegreeArray, numOfElements+1);
	
		for(i=1; i<numOfElements+1; i++){
			indirectDegree[i] = dfs(adjacencyMatrix, numOfElements+1, i);
			generateAdjacencyMatrixOfFollowers(adjacencyMatrix, indegreeArray, numOfElements+1);
		}
		
		printf("X Degerini Giriniz: ");
		scanf("%d", &x);
	
		printf("Y Degerini Giriniz: ");
		scanf("%d", &y);
	
		if(modeSelection == 1){
			printf("%2s\t%10s\t%10s\n", idTitle, nameTitle, surnameTitle);
			for(i=1; i<numOfElements+1; i++){
				if(indirectDegree[i] >= y && indegreeArray[i].indegree >= x)
						printf("%2d\t%10s\t%10s\n",i, indegreeArray[i].account->name, indegreeArray[i].account->surname);								
			}
			printf("\n");
		}
		
		else if(modeSelection == 2){
			printf("%2s\t%10s\t%10s\t%20s\t%20s\n", idTitle, nameTitle, surnameTitle, indegreeTitle, indirectDegreeTitle);
			for(i=1; i<numOfElements+1; i++){
				if(indirectDegree[i] >= y && indegreeArray[i].indegree >= x)
					if(indegreeArray[i].account)
						printf("%2d\t%10s\t%10s\t%20d\t%20d\n",i, indegreeArray[i].account->name, indegreeArray[i].account->surname, indegreeArray[i].indegree, indirectDegree[i]);				
			}
		}	
		
		modeSelection = menu();
		
		if(modeSelection==1 || modeSelection == 2){			
			clearAdjacencyMatrix(adjacencyMatrix, numOfElements+1);
			generateAdjacencyMatrix(adjacencyMatrix,inputList);
			generateIndegreeArray(adjacencyMatrix, numOfElements+1, indegreeArray, inputList);
		}
				
	}
		
	return 0;
}
