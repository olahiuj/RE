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

void delStateList(StateList *list) {
	if (list==NULL) return ;
	switch (list->size) {
		default: for (NFAState *it=list->tail->prev;it!=NULL;it=it->prev) free(it->next);
		case 1: free(list->head);
		case 0:
			list->head=list->tail=NULL;
			list->size=0;
	}
}

void insElem(NFAState *prev,NFAState *next,state_t state) {
	NFAState *node=newNFAState(state);
	node->prev=prev; node->next=next;
	if (prev!=NULL) prev->next=node;
	if (next!=NULL) next->prev=node;
}

void insertState(StateList *list,state_t state) {
	list->size++;
	if (list->size==1) {
		list->head=list->tail=newNFAState(state);
		return ;
	}
	NFAState *it=list->head;
	if (it->state>state) {
		// insElem head
		insElem(NULL,list->head,state);
		list->head=list->head->prev;
		return ;
	} else if (it->state==state) {
		list->size--;
		return ;
	}

	NFAState *prev=it;
	for (it=it->next;it!=NULL;it=it->next,prev=prev->next) {
		if (it->state>state) {
			insElem(prev,it,state);
			return ;
		} else if (it->state==state) {
			list->size--;
			return ;
		}
	}

	// insElem tail
	insElem(list->tail,NULL,state);
	list->tail=list->tail->next;
}

void mergeList(StateList *a,StateList *b) {
	if (isEmpty(b)) return ;
	StateList *new_list=newStateList();
	NFAState *ia=a->head,*ib=b->head;
	for (;ia!=NULL&&ib!=NULL;) {
		if (ia->state<ib->state) {
			insertState(new_list,ia->state);
			ia=ia->next;
		} else if (ia->state>ib->state) {
			insertState(new_list,ia->state);
			ia=ia->next;
		} else {
			insertState(new_list,ia->state);
			ia=ia->next;
			ib=ib->next;
		}
	}
	NFAState *it=(ia==NULL)?ib:ia;
	for (;it!=NULL;it=it->next) {
		insertState(new_list,it->state);
	}
	delStateList(a);
	a->head=new_list->head;
	a->tail=new_list->tail;
	a->size=new_list->size;
}

bool isIn(StateList *list,state_t state) {
	NFAState *it=list->head;
	for (;it!=NULL;it=it->next) {
		if (it->state>state) return false;
		else if (it->state==state) return true;
	}
	return false;
}

bool isEqual(StateList a,StateList b) {
	if (a.size!=b.size) return false;
	NFAState *ia=a.head,*ib=b.head;
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
		else fprintf(file,"\t%d -> %d [styel=bold,label=\" ??\"];\n",state,it->state);
	}
}

void printList(StateList *list) {
	for (NFAState *it=list->head;it!=NULL;it=it->next) {
		printf("%d ",it->state);
	} puts("");
}