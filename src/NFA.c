#include "NFA.h"
#include "debug.h"
#include "utils.h"

#include <ctype.h>

NFA *newNFA(unsigned int size) {
	NFA *nfa=(NFA*)malloc(sizeof(NFA));
	nfa->size=size;
	
	nfa->is_recv=(bool*)calloc(sizeof(bool),size);
	nfa->is_start=(bool*)calloc(sizeof(bool),size);
	
	nfa->trans=(NFAtransNode*)calloc(sizeof(NFAtransNode),size);
	return nfa;
}

int addNFATrans(NFA *nfa,state_t from,state_t to,char_t ch) {
	if (to>=nfa->size) {
		puts("Out of Size!");
		return -1;
	}

	debug("from=%d, to=%d, ch=%d\n",from,to,ch);

	if (nfa->trans[from][ch]==NULL) {
		nfa->trans[from][ch]=newStateList();
	}
	insertState(nfa->trans[from][ch],to);
	return 0;
}

int addNFARecv(NFA *nfa,state_t recv) {
	if (recv>=nfa->size) {
		puts("Out of Size!");
		return -1;
	}

	nfa->is_recv[recv]=true;
	return 0;
}

StateList* getNFASucc(NFA *nfa,state_t state,char_t ch) {
	StateList *list=nfa->trans[state][ch];
	if (list==NULL||list->size==0) {
		// puts("No Such Transfer!");
		return NULL;
	}
	return list;
}

void printNFA(NFA *nfa) {
	FILE *fp = Fopen("NFA.gv", "w");
	fprintf(fp,"//Number of Nodes: %d\n",nfa->size);
	fprintf(fp,"digraph G {\n\tnodesep=1;\n\tranksep=0.6;\n\trankdir=LR;\n");
	for (int state=0;state<nfa->size;++state) {
		for (char_t ch=0;ch<CHAR_SIZE;++ch) {
			if (nfa->trans[state][ch]!=NULL&&nfa->trans[state][ch]->size!=0) {
				printGraphvizList(fp, state, ch, nfa->trans[state][ch]);
			}
		}
	}
	fprintf(fp,"\t0:body[shape=doublecircle, width=0.4];\n");
	for (int state=1;state+1<nfa->size;++state) {
		fprintf(fp,"\t%d:body[shape=circle, width=0.4];\n",state);
	}
	fprintf(fp,"\t%d:body[shape=doublecircle, width=0.4];\n",nfa->size-1);
	fprintf(fp,"}\n");
	fclose(fp);
}