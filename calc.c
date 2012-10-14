//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

//#define DEBUG

#define CPL 'p'
#define CMN 'm'
#define CAD '+'
#define CSB '-'
#define CDV '/'
#define CML 'x'
#define OPN '('
#define CLS ')'

int LIST_ID=0;
int deep=0;
char* exp;
int u=0;



void ExpErrorHandle(char* pos){
	int p=pos-exp;
	printf("Error occurred!\n%s\n",exp);
	int i;
	for(i=0;i<p;i++)
		printf(" ");
	printf("^\n");
}

enum TYPES{
	OP_NUM,
	OP_PLUS,
	OP_MINUS,
	OP_ADD,
	OP_SUB,
	OP_DIV,
	OP_MUL,
	OP_OPEN,
	OP_CLOSE
};

typedef struct TOperation {
	int type;
	union {
		float num;
		struct TOperation *uNode;
		struct TOperation *bNode[2];
	};
} TOPERATION;

typedef struct TListNode {
	TOPERATION *node;
	struct TListNode *pNode;
    struct TListNode *nNode;

} TLISTNODE;

typedef struct List {
	int id;
	TLISTNODE *start;
	TLISTNODE *end;
} TLIST;

void printSep(void) {
	printf("  ");
}

int isNodeOpBin(TLISTNODE* node){
	int type=node->node->type;
	if(type==OP_ADD||type==OP_SUB||type==OP_MUL||type==OP_DIV) {
		return 1;
	}
	return 0;
}

void freeList(TLIST* list){
	int id = list->id;
	TLISTNODE* tmp0;
	TLISTNODE* tmp = list->start;
	if(tmp!=NULL) {
		while(tmp->nNode!=NULL) {
			free(tmp->node);
			tmp0=tmp;
			tmp=tmp->nNode;
			free(tmp0);
		}
	}
	free(list);
	#ifdef DEBUG
		printf("\nList ID=%d deleted\n",id);
	#endif
	
}

void printTOperation(TOPERATION* op) {
	deep+=3;
	int i;
	printf("\n");
	for(i=0;i<deep;i++){
		printf(" ");	
	}
	switch(op->type){
	case OP_NUM:
		printf("%4.2f",op->num); break;
	case OP_PLUS:
		printf("+u"); 
		printTOperation(op->uNode);
		break;
	case OP_MINUS:
		printf("-u"); 
		printTOperation(op->uNode);
		break;
	case OP_ADD:
		printf("+b"); 
		printTOperation(op->bNode[0]);
		printTOperation(op->bNode[1]);
		break;
	case OP_SUB:
		printf("-b"); 
		printTOperation(op->bNode[0]);
		printTOperation(op->bNode[1]);
		break;
	case OP_MUL:
		printf("*b"); 
		printTOperation(op->bNode[0]);
		printTOperation(op->bNode[1]);
		break;
	case OP_DIV:
		printf("/b"); 
		printTOperation(op->bNode[0]);
		printTOperation(op->bNode[1]);
		break;
	case OP_OPEN:
		printf("("); break;
	case OP_CLOSE:
		printf(")"); break;
	}
	deep-=3;
}

void printListTree(TLIST* list) {
	TOPERATION* root = list->end->node;
	#ifdef DEBUG
		printf("\nTree from list ID=%d:\n",list->id);
	#endif
	printTOperation(root);
}	

int isNodeOpUn(TLISTNODE* node){
	int type=node->node->type;
	if(type==OP_MINUS||type==OP_PLUS) {
		return 1;
	}
	return 0;
}

TLISTNODE* AddTreeNode(TLISTNODE* node) {
	if(node==NULL) 
		return NULL;
	
	TLISTNODE* tmp=node;
	if (isNodeOpBin(node))
	{	
		node->node->bNode[1]=node->pNode->node;
		if(tmp->pNode->node->type!=0) {
			tmp=AddTreeNode(node->pNode);
		} else 
			tmp=tmp->pNode;
		node->node->bNode[0]=tmp->pNode->node;
		if(tmp->pNode->node->type!=0) {
			tmp=AddTreeNode(tmp->pNode);
		} else
			return tmp->pNode;
		return tmp;
	}

	if (isNodeOpUn(node))
	{
		node->node->uNode=tmp->pNode->node;
		if(tmp->pNode->node->type!=0) {
			tmp=AddTreeNode(tmp->pNode);
		} else
			return tmp->pNode;
		return tmp;
	}
	
	
	tmp=AddTreeNode(node->pNode);
	
	return tmp;
}

TOPERATION* buildTreeFromRPN(TLIST* list) {
	#ifdef DEBUG
		printf("\nMaking Tree...\n");
	#endif
	TOPERATION* root=list->end->node;
	AddTreeNode(list->end);
	#ifdef DEBUG
		printf("\nTree done\n");
	#endif
	
	return root;
}


void printListNode(TLISTNODE* lp) {
	
	switch (lp->node->type) {
	case OP_NUM:
		printf("%3.1f",lp->node->num); break;
	case OP_PLUS:
		printf("+u"); break;
	case OP_MINUS:
		printf("-u"); break;
	case OP_ADD:
		printf("+"); break;
	case OP_SUB:
		printf("-"); break;
	case OP_MUL:
		printf("*"); break;
	case OP_DIV:
		printf("/"); break;
	case OP_OPEN:
		printf("("); break;
	case OP_CLOSE:
		printf(")"); break;
	}
}

int getOpPriority(int type) {
	switch (type) {
	case OP_PLUS:
		return 4;
	case OP_MINUS:
		return 4;
	case OP_ADD:
		return 2;
	case OP_SUB:
		return 2;	
	case OP_MUL:
		return 3;
	case OP_DIV:
		return 3;
	case OP_OPEN:
		return 1;
	case OP_CLOSE:
		return 1;
	default:
		return 0;
	}
}

void printList (TLIST* lp) {
	#ifdef DEBUG
		printf("\nList ID=%d: \n",lp->id);
	#endif
	TLISTNODE* node = lp->start;
	while(1) {
		printListNode(node);
		if (node->nNode==NULL)
			return;
		printSep();
		node = node->nNode;
	}
	printf("\n");
}


//---------------------------------------------------------------------------
TLIST* CreateNullList(void) {
	TLIST* ret =(TLIST*) malloc(sizeof(TLIST));
	ret->start = NULL;
	ret->end = NULL;
	ret->id = LIST_ID++;
	#ifdef DEBUG
		printf("\nList ID=%d created\n",ret->id);
	#endif
	return ret;
}	
int isListEmpty(TLIST* list) {
	if(list->start==NULL) {
		return 1; 
	} else {
		return 0;
	}	
}
void ListAddNode(TLIST *list,TLISTNODE *node) {
	#ifdef DEBUG
		printf("Add node: ");
		printListNode(node);
		printf(" at list ID=%d \n",list->id);
	#endif
	if(list->start==NULL) {
		list->start = node;
		list->start->pNode = NULL;
	} else {
		list->end->nNode = node;
		node->pNode = list->end;
	}
	list->end = node;
	list->end->nNode = NULL;
}

TLISTNODE* ListRemoveLast(TLIST *list,int fr) {
	
	TLISTNODE *ret = list->end;
	#ifdef DEBUG
		printf("Removed node: ");
		printListNode(ret);
		printf(" at list ID=%d \n",list->id);
	#endif
	list->end = list->end->pNode;
	if (ret->pNode==NULL) {
		list->start = NULL;
	}
	if (fr) {
		free(ret->node);
		free(ret);
		return NULL;
	} else {
		return ret;
	}
}
int isUnariChar(char *chr){
	if (chr==exp)
		return 1;
	char *prev; prev = chr; prev--;
	if(*prev==OPN || *prev==CSB || *prev==CAD || *prev==CML || *prev==CDV)
		return 1;
	return 0;
}

int getPrev(char *chr) {
	if(chr!=exp) {
		char *prev; prev = chr; prev--;
		int t=isSpecChar(prev);
		return t;
	}
	return -1;
}

int isSpecChar(char* chr) {
	int p = getPrev(chr);
	p = (p !=OP_NUM) && (p!=OP_CLOSE);

	switch (*chr) {
		case CAD:
			if(isUnariChar(chr))
				if(u)
					return -1;
				else
					return OP_PLUS;
			return OP_ADD;
		case CSB:
			if(isUnariChar(chr))
				if(u)
					return -1;
				else
					return OP_MINUS;
			return OP_SUB;
		case CDV:
			if(p)
				return -1;
			return OP_DIV;
		case CML:
			if(p)
				return -1;
			return OP_MUL;
		case OPN:
			return OP_OPEN;
		case CLS:
			return OP_CLOSE;
		default: return 0;
	}
}

TLIST* makeRPNListFromExpression(char* str) {
	#ifdef DEBUG
		printf("\nConverting to RPN...\n ");
	#endif
	TLIST *ret = CreateNullList();
	TLIST *store = CreateNullList();
	TLISTNODE *CurrNode = NULL;
	TOPERATION *TOp;
	int t;
	int t_stor;
	int c=0;
	int co=0;
	while (*(str)!=0) {
		while (t=isSpecChar(str),t!=0) {
			co=0;
			TOp = (TOPERATION*) malloc(sizeof(TOPERATION));
			TOp->type = t;
			if(t==OP_PLUS || t==OP_MINUS){
				u=1;
			} else u=0;
			if(t==-1) {
					ExpErrorHandle(str);
					freeList(store);
					freeList(ret);
					return NULL;
				}
			
			if (t==OP_CLOSE) {
				co=1;
				if(c==0) {
					ExpErrorHandle(str);
					freeList(store);
					freeList(ret);
					return NULL;
				}
				c--;
				while (t_stor=store->end->node->type,t_stor!=OP_OPEN) {
							CurrNode = ListRemoveLast(store,0);
							ListAddNode(ret,CurrNode);
						}
				CurrNode = ListRemoveLast(store,1);
				free(TOp);
			}
			 else {
				 
			CurrNode = (TLISTNODE*) malloc(sizeof(TLISTNODE));
			CurrNode->node = TOp;
			
			if (t==OP_OPEN) {
					ListAddNode(store,CurrNode);
					c++;
			} else {
				
					if (isListEmpty(store)) {
						ListAddNode(store,CurrNode);
					} else {
					if(t_stor=store->end->node->type,getOpPriority(t_stor)<getOpPriority(t)) {
						
						ListAddNode(store,CurrNode);
						
					} else {
						while (t_stor=store->end->node->type,getOpPriority(t_stor)>=getOpPriority(t)) {
							ListAddNode(ret,ListRemoveLast(store,0));
							if (isListEmpty(store))
								break;
						}
						ListAddNode(store,CurrNode);
					}
					}					
				}
			 }
			
		
			str++;
			
		}
        TOp = (TOPERATION*) malloc(sizeof(TOPERATION));
		TOp->type = t;
		float tmp = 0;
		char* stop;
        tmp = strtof(str,&stop);
		if (stop!=str) {
			str=stop;	
			TOp->num = tmp;
			CurrNode = (TLISTNODE*) malloc(sizeof(TLISTNODE));
			CurrNode->node = TOp;
			ListAddNode(ret,CurrNode);
		} else {
				if(!co){

				ExpErrorHandle(str);
				freeList(store);
				freeList(ret);
				return NULL;
			}
		}
	}
	if(c!=0) {
		ExpErrorHandle(str);
		freeList(store);
		freeList(ret);
		return NULL;
	}
	while (!isListEmpty(store)) {
		CurrNode = ListRemoveLast(store,0);
		ListAddNode(ret,CurrNode);
	}
	freeList(store);
	
	#ifdef DEBUG
		printf("\nConverting done\n ");
	#endif
	return ret;
	
}


float CalcNode(TOPERATION* op) {
	switch(op->type) {
	case OP_NUM:
		return op->num;
	case OP_MINUS:
		return -1*CalcNode(op->uNode);
	case OP_PLUS:
		return CalcNode(op->uNode);
	case OP_ADD:
		return CalcNode(op->bNode[0])+CalcNode(op->bNode[1]);
	case OP_SUB:
		return CalcNode(op->bNode[0])-CalcNode(op->bNode[1]);
	case OP_MUL:
		return CalcNode(op->bNode[0])*CalcNode(op->bNode[1]);
	case OP_DIV:
		return CalcNode(op->bNode[0])/CalcNode(op->bNode[1]);
	default:
		return 0;	
	}
}
float CalcTree(TLIST* list,int p) {
	float ret = CalcNode(list->end->node);
	if(p)
		printf("\nResult:%f\n",ret);
	return ret;
}

int main(int argc, char** argv)
{
	exp = "(-1x2)/-+1";
	printf("Initial expression:\n%s\n",exp);
	TLIST* list = makeRPNListFromExpression(exp);
	if (list){
		printList(list);
		buildTreeFromRPN(list); 
		printListTree(list);
		CalcTree(list,1);
		freeList(list);
	}
	return 0;

}