#include "State.h"

NFAState* newNFAState(state_t state) {
	NFAState *tmpState=(NFAState*)malloc(sizeof(NFAState));
	tmpState->next=tmpState->prev=NULL;
	tmpState->state=state;
	return tmpState;
}

StateList* newStateList() {
	StateList *tmpList=(StateList*)malloc(sizeof(StateList));
	tmpList->head=tmpList->tail=NULL;
	tmpList->size=0;
	return tmpList;
}

void insElem(NFAState *prev,NFAState *next, NFAState *node) {
	node->prev=prev; node->next=next;
	if (prev!=NULL) prev->next=node;
	if (next!=NULL) next->prev=node;
}

void insertState(StateList *list,state_t state) {
	NFAState *node=newNFAState(state);
	list->size++;
	if (list->size==1) {
		list->head=list->tail=node;
		return ;
	}
	NFAState *it=list->head;
	if (it->state>state) {
		// insElem head
		insElem(NULL,list->head,node);
		list->head=node;
		return ;
	} else if (it->state==state) {
		list->size--;
		return ;
	}

	NFAState *prev=it;
	for (it=it->next;it!=NULL;it=it->next,prev=prev->next) {
		if (it->state>state) {
			insElem(prev,it,node);
			return ;
		} else if (it->state==state) {
			list->size--;
			return ;
		}
	}

	// insElem tail
	insElem(list->tail,NULL,node);
	list->tail=node;
}

void mergeList(StateList *a,StateList *b) {
	if (isEmpty(b)) return ;
	NFAState *it=b->head;
	for (;it!=NULL;it=it->next) {
		insertState(a,it->state);
	}
}

bool isIn(StateList *list,state_t state) {
	NFAState *it=list->head;
	for (;it!=NULL;it=it->next) {
		if (it->state>state) return false;
		else if (it->state==state) return true;
	}
	return false;
}

bool isEqual(StateList *a,StateList *b) {
	if (a->size!=b->size) return false;
	NFAState *ia=a->head,*ib=b->head;
	for (;ia!=NULL&&ib!=NULL;ia=ia->next,ib=ib->next) {
		if (ia->state!=ib->state) return false;
	}
	return true;
}

bool isEmpty(StateList *list) {
	return list==NULL||list->size==0;
}

void printGraphvizList(FILE *file,state_t state,char_t ch,StateList *list) {
	for (NFAState *it=list->head;it!=NULL;it=it->next) {
		if (ch) fprintf(file,"\t%d -> %d [styel=bold,label=\" %c\"];\n",state,it->state,ch);
		else fprintf(file,"\t%d -> %d [styel=bold,label=\" ε\"];\n",state,it->state);
	}
}

void printList(StateList *list) {
	for (NFAState *it=list->head;it!=NULL;it=it->next) {
		printf("%d ",it->state);
	} puts("");
}