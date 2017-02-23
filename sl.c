//Garrett Beatty, Greg Heitman, Mike Packi

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define SKIPLIST_MAX_LEVEL 6

typedef struct snode {
    int key;
    int value;
    int count;
    struct snode **forward;
} snode;

typedef struct skiplist {
    int level;
    struct snode *header;
} skiplist;

skiplist *skiplist_init(skiplist *list) {
    int i;
    snode *header = (snode *) malloc(sizeof(struct snode));
    list->header = header;
    header->key = INT_MAX;
    header->forward = (snode **) malloc(
            sizeof(snode*) * (SKIPLIST_MAX_LEVEL + 1));
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;
    }

    list->level = 1;

    return list;
}

static int rand_level() {
    int level = 1;
    while (rand() < RAND_MAX / 2 && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}

int skiplist_insert(skiplist *list, int key, int value) {
    snode *update[SKIPLIST_MAX_LEVEL + 1];
    snode *x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];

    if (key == x->key) {
        x->value = value;
        x->count = x->count + 1;
        return key;
    } else {
        level = rand_level();
        if (level > list->level) {
            for (i = list->level + 1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        x = (snode *) malloc(sizeof(snode));
        x->key = key;
        x->value = value;
        x->count = 1;
        x->forward = (snode **) malloc(sizeof(snode*) * (level + 1));
        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
    }
    return key;
}

int skiplist_search(skiplist *list, int key) {
    snode *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    if (x->forward[1]->key == key) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}

int pre(skiplist *list, int key){

    if(!skiplist_search(list, key)) return -1;

    snode *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    
    while(x->forward[1]->key < key){
        x = x->forward[1];
    }

    if(x->key == INT_MAX)return -1;
    return x->key;
}

int suc(skiplist *list, int key){

    if(!skiplist_search(list, key)) return -1;
    snode *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key <= key)
            x = x->forward[i];
    }
    x = x->forward[1];
    if(x->key == INT_MAX)return -1;
    return x->key;
}

int min(skiplist *list){
    snode *x = list->header;
    x = x->forward[1];
    if(x->key == INT_MAX) return -1;
    return x->key;
}

int max(skiplist *list){
    snode *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key != INT_MAX)
            x = x->forward[i];
    }
    if(x->key == INT_MAX) return -1;
    return x->key;
}

static void skiplist_node_free(snode *x) {
    if (x) {
        free(x->forward);
        free(x);
    }
}

int skiplist_delete(skiplist *list, int key) {
    int i;
    snode *update[SKIPLIST_MAX_LEVEL + 1];
    snode *x = list->header;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }

    x = x->forward[1];
    if (x->key == key) {
        for (i = 1; i <= list->level; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[i] = x->forward[i];
        }
        int c = x->count;
        skiplist_node_free(x);

        while (list->level > 1 && list->header->forward[list->level]
                == list->header)
            list->level--;
        return c;
    }
    return 0;
}

int height(skiplist *list){
    if (list->header->forward[1]->key == INT_MAX) return -1;
    return list->level - 1;
}

void skiplist_free(skiplist *list)
{
    snode *current_node = list->header->forward[1];
    while(current_node != list->header) {
        snode *next_node = current_node->forward[1];
        free(current_node->forward);
        free(current_node);
        current_node = next_node;
    }
    free(current_node->forward);
    free(current_node);
    free(list);
}

int main(void){

	skiplist* list = (skiplist *)malloc(sizeof(skiplist));
	skiplist_init(list);

	char* operation = "";
	int key = 0;

	char *line = NULL;
	size_t size;
	while(getline(&line, &size, stdin) != -1) {

		if(strlen(line) > 3){
			char *p;
			p = strtok(line, " ");
			if(p){
				operation = p;
			}
			p = strtok(NULL, " ");

			if(p){
				 key = atoi(p);
			 }

		}else{
			operation = line;
		}
		
		if(strcmp(operation, "INS") == 0){
			printf("%d\n", skiplist_insert(list, key, 0));
		}else if(strcmp(operation, "DEL") == 0){
			
			if(skiplist_search(list, key) == 1){
				printf("%d\n", skiplist_delete(list, key));
			}else{
				printf("%s\n", "0");
			}

		}else if(strcmp(operation, "SEA") == 0){
			printf("%d\n", 	skiplist_search(list, key));
		
		}else if(strcmp(operation, "MIN\n") == 0 || strcmp(operation, "MIN") == 0){
			printf("%d\n", 	min(list));
		}else if(strcmp(operation, "MAX\n") == 0 || strcmp(operation, "MAX") == 0){
			printf("%d\n", 	max(list));
		}else if(strcmp(operation, "PRE") == 0){
            printf("%d\n", pre(list, key) );
			
		}else if(strcmp(operation, "SUC") == 0){
            printf("%d\n", suc(list, key));
		
		}else if(strcmp(operation, "HEI\n") == 0 || strcmp(operation, "HEI") == 0){
			printf("%d\n", height(list));
		}

	}

	 skiplist_free(list);

	return 0;
}

