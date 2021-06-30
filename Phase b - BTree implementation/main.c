
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Darwin.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */


/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize (void)
{
	Species_root = NULL;
	for(int i = 0; i < 5; ++i ) {
		continents[i] = malloc(sizeof(struct ContinentTree));
		continents[i]->sentinel = malloc(sizeof(struct ContinentPopulation));
		continents[i]->sentinel->gid = -1; 
		continents[i]->sentinel->lc = NULL; 
		continents[i]->sentinel->rc = NULL;
		continents[i]->population_root = continents[i]->sentinel;
	}
	Homo_Sapiens_root = NULL;
	return 1;
}

/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */

struct Species *insert(struct Species *root,int sid){
	if (root == NULL){
		struct Species *newSp = malloc(sizeof(struct Species)); 
		newSp->sid = sid;
		newSp->population_root = NULL;
		newSp->lc = NULL;
		newSp->rc = NULL;
		root = newSp;
		return root;
	}
	if(root->sid < sid){
		struct Species *newSp = malloc(sizeof(struct Species)); 
		newSp->sid = sid;
		newSp->population_root = NULL;
		newSp->lc = root;
		newSp->rc = NULL;
		return newSp;
	}
	else{
		root->lc = insert(root->lc, sid);
		return root;
	}
}

void speciesPrint (struct Species *root){
	if (root == NULL)return;
	speciesPrint (root->lc);
	speciesPrint (root->rc);
	printf("< %d > ",root->sid);

}


int insert_species (int sid)
{
	Species_root = insert(Species_root , sid);

	/*Print*/
	speciesPrint(Species_root);
	printf("\n");
	
	return 1;
}



/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */

struct Species * searchSpecie(struct Species *root,int sid) {
	if(root == NULL) 
		return NULL;

	if (root->sid == sid)
		return root;
	
	if(sid > root->sid) 
		return NULL;
	
	if(root->lc != NULL && sid <= root->lc->sid)
		return searchSpecie(root->lc,sid);
	
	if(root->rc != NULL && sid <= root->rc->sid)
		return searchSpecie(root->rc,sid);
	
	return NULL;
}

struct Population *instertPopulation(struct Population *root,struct Population *parent, int gid, int sid, int cid) {
	if(root == NULL) {
		struct Population * newP = malloc(sizeof(struct Population));
		newP->gid = gid; 
		newP->sid = sid; 
		newP->cid = cid; 
		newP->lc = NULL;
		newP->rc = NULL;
		newP->parent = parent;
		if(parent == NULL)  
			return newP;
		else if(gid < parent->gid)
			parent->lc = newP;
		else if(gid > parent->gid)
			parent->rc = newP;
	}
	else {
		if(gid < root->gid) 
			instertPopulation(root->lc, root, gid, sid, cid);
		else 
			instertPopulation(root->rc, root, gid, sid, cid);
	}
	return root;
}


void populationPrint (struct Population *root){
	if (root == NULL) return;
	populationPrint (root->lc);
	printf("< %d , %d > ",root->gid, root->cid);
	populationPrint (root->rc);
}


void speciesPopulationPrint (struct Species *root){
	if (root == NULL)return;
	speciesPopulationPrint (root->lc);
	speciesPopulationPrint (root->rc);
	printf("< %d > \n\t",root->sid);
	populationPrint(root->population_root);
	printf("\n");
}

int insert_population(int gid, int sid, int cid){
	struct Species *tmp = searchSpecie(Species_root,sid);
	tmp->population_root = instertPopulation(tmp->population_root, NULL, gid, sid, cid);
	/*Print*/
	speciesPopulationPrint(Species_root);
	printf("\n");
	return 1;
}

/**
 * @brief find the lowest (earliest) common ancestor of populations with ID <gid1> and <gid2> of species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */



struct Population * searchPopulation(struct Population *root, int gid, int *counter){
	if(root == NULL) 
		return NULL;

	++(*counter);
	
	if (root->gid == gid){
		return root;
	}

	if(gid < root->gid) 
		return searchPopulation(root->lc, gid, counter);

	if(gid > root->gid) 
		return searchPopulation(root->rc, gid, counter);
	
	//--(*counter);
	return NULL;
}

int lowest_common_ancestor(int sid, int gid1, int gid2){
	struct Species *tmp = searchSpecie(Species_root,sid);

	int counter1 = 0;
	int counter2 = 0;
	struct Population *pop1 = searchPopulation(tmp->population_root, gid1, &counter1);
	struct Population *pop2 = searchPopulation(tmp->population_root, gid2, &counter2);

//	if(pop1 != NULL || pop2 != NULL) return 1;

	int difference = counter1 - counter2;

	struct Population *p1, *p2; 
	p1 = pop1; 
	p2 = pop2;
	for(int i = 0; i < abs(difference); ++i){
		if(counter1 < counter2) {
			p2 = p2->parent;
		}else {
			p1 = p1->parent;
		}
	}

	while(p1 != NULL && p2 != NULL){
		if(p1!=p2 ){
		p1 = p1->parent;
		p2 = p2->parent;
		}
		else 
			break;
	}

	/* Print */
	printf("Lowest Common Ancestor: < %d > \n", p1->gid);
	return 1;
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
void treeCounter(struct Population *root,int *count){
	if (root == NULL)return;
	(*count)++;
	treeCounter(root->lc, count);	
	treeCounter(root->rc, count);
}

void treeTraverse(struct Population *root,int *count,struct Population *array[]){
	if (root == NULL)return;
	treeTraverse(root->lc, count, array);
	array[*count] = root;	
	(*count)++;
	treeTraverse(root->rc, count, array);
}


void mergeArrays(struct Population *arr1[], struct Population *arr2[], int n1, int n2, struct Population *arr3[]) 
{ 
    int i = 0, j = 0, k = 0; 
      while (i<n1 && j <n2) 
    { 
        if (arr1[i]->gid < arr2[j]->gid) 
            arr3[k++] = arr1[i++]; 
        else
            arr3[k++] = arr2[j++]; 
    } 
    while (i < n1) 
        arr3[k++] = arr1[i++]; 
    while (j < n2) 
        arr3[k++] = arr2[j++]; 
} 

struct Population *arrayToTree(struct Population *arr[],struct Population *parent, int start, int end)  
{  
    if (start > end)  
    	return NULL;  
    int mid = (start + end)/2;  

    struct Population * newP = malloc(sizeof(struct Population));
	newP->gid = arr[mid]->gid; 
	newP->sid = arr[mid]->sid; 
	newP->cid = arr[mid]->cid; 
	newP->parent = parent;
	/*if(arr[mid]->gid < parent->gid)
		parent->lc = newP;
	else if(arr[mid]->gid > parent->gid)
		parent->rc = newP;  
*/
    newP->lc = arrayToTree(arr, newP, start, mid - 1);  
    newP->rc = arrayToTree(arr, newP, mid + 1, end);  
	return newP;  
}  


// struct Species *insert2(struct Species *root,int sid, ){
// 	if (root == NULL){
// 		struct Species *newSp = malloc(sizeof(struct Species)); 
// 		newSp->sid = sid;
// 		newSp->population_root = NULL;
// 		newSp->lc = NULL;
// 		newSp->rc = NULL;
// 		root = newSp;
// 		return root;
// 	}
// 	if(root->sid < sid){
// 		struct Species *newSp = malloc(sizeof(struct Species)); 
// 		newSp->sid = sid;
// 		newSp->population_root = NULL;
// 		newSp->lc = root;
// 		newSp->rc = NULL;
// 		return newSp;
// 	}
// 	else{
// 		root->lc = insert(root->lc, sid);
// 		return root;
// 	}


struct Species *insert2(struct Species *root,struct Species **s){
	if (root == NULL){
		root = *s;
		return root;
	}
	if(root->sid < (*s)->sid){
		(*s)->lc = root;
		return *s;
	}
	else{
		root->lc = insert2(root->lc, s);
		return root;
	}
}

struct Species * searchSpecieWithParent(struct Species *root, struct Species **parent, int sid) {
	if(root == NULL) 
		return NULL;

	if (root->sid == sid)
		return root;
	
	if(sid > root->sid) 
		return NULL;
	
	if(root->lc != NULL && sid <= root->lc->sid){
		*parent = root; 
		return searchSpecieWithParent(root->lc,parent,sid);
	}
	
	if(root->rc != NULL && sid <= root->rc->sid){
		*parent = root;
		return searchSpecieWithParent(root->rc,parent,sid);
	}
	
	return NULL;
}


struct Species *deleteSpecie(struct Species *root,int sid){
	struct Species *parent = NULL;
	struct Species *s = searchSpecieWithParent(root, &parent, sid); 
	assert(s); 
	if(s->lc == NULL && s->rc == NULL) {
		if(parent != NULL) {
			if((parent)->lc == s) 
				(parent)->lc = NULL;
			else
				(parent)->rc = NULL;	
		} else {
			Species_root = NULL;
		}
		free(s);
	} else if(s->lc != NULL && s->rc == NULL){
		if(parent != NULL) {
			if((parent)->lc == s) 
				(parent)->lc = s->lc;
			else
				(parent)->rc = s->lc;	
		} else {
			Species_root = s->lc;;
		}
		free(s);
	} else if(s->lc == NULL && s->rc != NULL){
		if(parent != NULL) {
			if((parent)->lc == s) 
				(parent)->lc = s->rc;
			else
				(parent)->rc = s->rc;	
		} else {
			Species_root = s->rc;
		}
		free(s);
	}else{
		struct Species *tmp = s;
		struct Species *prev = NULL;
		while(tmp->rc!=NULL){
			tmp->sid = tmp->rc->sid; 
			tmp->population_root = tmp->rc->population_root; 
			prev = tmp;
			tmp = s->rc;
		}
		if(prev!=NULL) prev->rc = NULL;
		free(tmp);
	}
}



int merge_species(int sid1, int sid2, int sid3){
	struct Species *s1 = searchSpecie(Species_root, sid1);
	struct Species *s2 = searchSpecie(Species_root, sid2);
	int counter1 = 0;
	int counter2 = 0;
	treeCounter(s1->population_root, &counter1);
	treeCounter(s2->population_root, &counter2);
	struct Population *tree1[counter1];				
	struct Population *tree2[counter2];				

	counter1 = 0;
	counter2 = 0;
	treeTraverse(s1->population_root, &counter1, tree1);
	treeTraverse(s2->population_root, &counter2, tree2);

	struct Population *mergedTrees[counter1 + counter2];
	mergeArrays(tree1, tree2, counter1, counter2, mergedTrees);
	struct Species *s3 = malloc (sizeof(struct Species));
	s3->sid = sid3;
	s3->lc = NULL;
	s3->rc = NULL;
	s3->population_root = arrayToTree(mergedTrees, NULL, 0, counter1+counter2-1);
	

	deleteSpecie(Species_root,sid1);
	deleteSpecie(Species_root,sid2);
	Species_root = insert2(Species_root, &s3);

	/*Print*/
	speciesPopulationPrint(Species_root);
	printf("\n");
	return 1;
}

/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
struct ContinentPopulation* insertContinentTree(struct ContinentPopulation* root, struct ContinentPopulation* sentinel, int gid){
	struct ContinentPopulation* prev = sentinel; 
	struct ContinentPopulation* tmp = root;

	while(tmp != sentinel) {
		if(tmp->gid == gid) {
			return root;
		}
		prev = tmp; 
		if(gid < tmp->gid) {
			tmp = tmp->lc;
		}else {
			tmp = tmp->rc;
		}
	}
	struct ContinentPopulation* new = malloc(sizeof(struct ContinentPopulation));
	new->gid = gid;	
	new->lc = sentinel;
	new->rc = sentinel;
	if(prev == sentinel) return new;
	if(gid < prev->gid) prev->lc = new;
	else prev->rc = new;
	return root;
}


void traversePop(struct Population* root) {
	if(root == NULL) {
		return;
	}
	continents[root->cid]->population_root = insertContinentTree(continents[root->cid]->population_root, continents[root->cid]->sentinel, root->gid);
	traversePop(root->lc); 
	traversePop(root->rc); 
}


void traverseSpecies(struct Species* root) {
	if(root == NULL) {
		return;
	}
	traversePop(root->population_root);
	traverseSpecies(root->lc); 
	traverseSpecies(root->rc); 
}

void printContinets(struct ContinentPopulation* root, struct ContinentPopulation* sentinel) {
	if(root == sentinel) {
		return;
	}
	printContinets(root->lc, sentinel); 
	printf("< %d >, ", root->gid);
	printContinets(root->rc, sentinel); 
}

int distribute(){
	traverseSpecies(Species_root);
	for(int i = 0; i < 5; ++i ) {
		printf("Continent %d : ", i);
		printContinets(continents[i]->population_root, continents[i]->sentinel); 
		printf("\n");
	}
	return 1;
}

/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */



struct  Population * inorderNext(struct Population* p) { 
  struct Population* tmp = p; 

  while (tmp->lc != NULL) { 
    tmp = tmp->lc; 
  } 
  return tmp; 
} 

struct  Population * deletePopulationNode(struct Population* root, int gid,int *cid) {
	
	int tmp_count;											// unused variable (reuse searchPopulation())
	struct Population* p = searchPopulation(root, gid, &tmp_count);
	assert(p);
	struct Population* y, *x;

	*cid = p->cid;

	if(p->lc == NULL || p->rc == NULL) {
		y = p;
	}else{
		//return root;
		y = inorderNext(p);
	}

	if(y->lc != NULL) {
		x = y->lc;
	}else{
		x = y->rc;
	}

	if(x!=NULL){
		x->parent = y->parent;
	}
	if(y->parent == NULL){
		return x; 
	}else if(y == y->parent->lc){
		y->parent->lc = x;
	}else{
		y->parent->rc = x;
	}
	if(y != p) {
		p->gid = y->gid;
		p->cid = y->cid;
		p->sid = y->sid;
	}
	free(y);
	return root;

}

struct ContinentPopulation * searchPopulationParent(struct ContinentPopulation *root,struct ContinentPopulation* sentinel, int gid){
	if(root == sentinel) 
		return sentinel;
	if (root->gid == gid){
		return sentinel;
	}
	if (root->lc != sentinel){
		if (root->lc->gid == gid){
			return root;
		}
	}
	if (root->rc != sentinel){
		if (root->rc->gid == gid){
			return root;
		}
	}
	if(gid < root->gid) 
		return searchPopulationParent(root->lc, sentinel, gid);

	if(gid > root->gid) 
		return searchPopulationParent(root->rc, sentinel, gid);
}


struct  ContinentPopulation * inorderContinentNext(struct ContinentPopulation* p, struct ContinentPopulation* sentinel) { 
  struct ContinentPopulation* tmp = p; 

  while (tmp->lc != sentinel) { 
    tmp = tmp->lc; 
  } 
  return tmp; 
} 


struct  ContinentPopulation * deleteContinentNode(struct ContinentPopulation* root,struct ContinentPopulation* sentinel, int gid) { 
	struct ContinentPopulation* parent = searchPopulationParent(root, sentinel, gid);
	struct ContinentPopulation* y, *x, *p , *yParent;
	if (root == sentinel){
		return root;
	}
	if (parent == sentinel){
		assert(root->gid == gid);
		p = root;
	}
	else{
		if (parent->lc != sentinel && parent->lc->gid == gid){
			p = parent->lc;
		}
		else{
			p = parent->rc;
		}
	}
	
	if(p->lc == sentinel || p->rc == sentinel) {
		y = p;
		yParent = parent;
	}else{
		y = inorderContinentNext(p, sentinel);
		yParent = searchPopulationParent(root, sentinel, y->gid);
	}

	if(y->lc != sentinel) {
		x = y->lc;
	}else{
		x = y->rc;
	}

	if(yParent == sentinel){
		return x; 
	}else if(y == yParent->lc){
		yParent->lc = x;
	}else{
		yParent->rc = x;
	}
	if(y != p) {
		p->gid = y->gid;
	}
	free(y);
	//printContinets(continents[i]->population_root, continents[i]->sentinel); 
	return root;

}

int delete_population(int gid, int sid){
	struct Species * s = searchSpecie(Species_root, sid);
	int cid = 0 ;
	s->population_root = deletePopulationNode(s->population_root, gid,&cid); 
	continents[cid]->population_root = deleteContinentNode(continents[cid]->population_root, continents[cid]->sentinel, gid);
	printf("SPECIES \n");
	speciesPopulationPrint(Species_root);

	printf("\nCONTINENTS\n");

	for(int i = 0; i < 5; ++i ) {
		printf("Continent %d : ", i);
		printContinets(continents[i]->population_root, continents[i]->sentinel); 
		printf("\n");
	}

	return 1;
}

/**
 * @brief delete the species with lowest id and its populations
 *
 * @return 1 on success
 *         0 on failure
 */

void traversePop2(struct Population *root){
	if (root == NULL)return;
	traversePop2(root->lc);

	deleteContinentNode(continents[root->cid]->population_root, continents[root->cid]->sentinel, root->gid);
	traversePop2(root->rc);
}


int delete_species(){

	struct Species *prev = NULL;
	struct Species *s = Species_root;
	while(s!=NULL && s->lc!=NULL){
		prev = s; 
		s = s->lc;
	} 

	traversePop2(s->population_root);

	assert(s->lc == NULL && s->rc == NULL);
	if(prev != NULL) {
		prev->lc = NULL;
	} else {
		Species_root = NULL;
	}
	free(s);

	printf("SPECIES \n");
	speciesPopulationPrint(Species_root);

	printf("\nCONTINENTS\n");

	for(int i = 0; i < 5; ++i ) {
		printf("Continent %d : ", i);
		printContinets(continents[i]->population_root, continents[i]->sentinel); 
		printf("\n");
	}
	return 1;
}

/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
void insertToHomo(struct Species *s){
	int sid = s->sid;
	struct HomoSapiens *prev = NULL;
	struct HomoSapiens *prev2prev = NULL;
	struct HomoSapiens *p = Homo_Sapiens_root;
	while ( p != NULL){
		prev2prev = prev;
		prev = p;
		if (sid <= p->sid){
			p = p->lc;
		}
		else{
			p = p->rc;
		}
	}
	struct HomoSapiens *new = malloc(sizeof(struct HomoSapiens));
	new->next = NULL;
	new->population_root = NULL;
	new->lc = NULL;
	new->rc = NULL;
	if (prev == NULL){
		new->sid = sid;
		new->population_root = s->population_root;
		Homo_Sapiens_root = new;
		return ;
	}
	struct HomoSapiens *parent = malloc(sizeof(struct HomoSapiens));
	parent->next = NULL;
	parent->population_root = NULL;
	parent->lc = prev;
	parent->rc = new;

	new->next = prev->next;
	prev->next = new;

	if(prev->sid > sid){
		parent->sid = sid;
		new->sid = prev->sid;
		new->population_root = prev->population_root;
		prev->sid = sid;
		prev->population_root = s->population_root;
	}
	else{
		parent->sid = prev->sid;
		new->sid = sid;
		new->population_root = s->population_root;
	}
	if(prev2prev != NULL){
		if(prev2prev->lc == prev){
			prev2prev->lc = parent;
		}
		else{
			prev2prev->rc = parent;
		}

		return;
	}
	else{
		Homo_Sapiens_root = parent;
		return ;
	}


}

void traverseSpeciesTree(struct Species* root) {
	if(root == NULL) {
		return;
	}
	insertToHomo(root);
	traverseSpeciesTree(root->lc); 
	traverseSpeciesTree(root->rc); 
}


void populationPrint_Species (struct Population *root){
	if (root == NULL)return;
	populationPrint_Species (root->lc);
	printf("< %d , %d > ",root->gid, root->sid);
	populationPrint_Species (root->rc);
}


int evolution(){
	traverseSpeciesTree(Species_root);
	struct HomoSapiens *prev = NULL;
	struct HomoSapiens *p = Homo_Sapiens_root;
	assert(Homo_Sapiens_root);
	while (p != NULL){
		prev = p;
		p = p->lc;
	}
	printf("Homo Sapiens: \n" );
	while(prev!= NULL){
		printf("[");
		populationPrint_Species(prev->population_root);
		printf("]\n");
		prev = prev->next;
	}

	// Na svisw ton pinaka me species!
	Species_root = NULL;
	return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
	struct HomoSapiens *prev = NULL;
	struct HomoSapiens *p = Homo_Sapiens_root;
	while (p != NULL){
		prev = p;
		p = p->lc;
	}

	int counter = 0; 
	printf("Homo Sapiens: ");
	while(prev!= NULL){
		counter++;
		printf("< %d >, ", prev->sid);
		prev = prev->next;
	}

	printf("\nHomo Sapiens species: %d\n", counter);
	return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
	struct HomoSapiens *prev = NULL;
	struct HomoSapiens *p = Homo_Sapiens_root;
	while (p != NULL){
		prev = p;
		if (sid <= p->sid){
			p = p->lc;
		}
		else{
			p = p->rc;
		}
	}
	int counter = 0; 
	treeCounter(prev->population_root, &counter);
	printf("Homo Sapiens populations: %d\n", counter);
	return 1;
}

/**
 * @brief Print species' leaves list
 *
 * @return 1 on success
 *         0 on failure
 */

int print_species(){
	speciesPrint(Species_root);
	printf("\n");
	return 1;
}

/**
 * @brief Print species' tree with population trees
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
	speciesPopulationPrint(Species_root);
	printf("\n");
	return 1;
}

/**
 * @brief Print continents array with each continent's population tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
	for(int i = 0; i < 5; ++i ) {
		printf("Continent %d : ", i);
		printContinets(continents[i]->population_root, continents[i]->sentinel); 
		printf("\n");
	}
	return 1;
}

/**
 * @brief Print homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
	struct HomoSapiens *prev = NULL;
	struct HomoSapiens *p = Homo_Sapiens_root;
	assert(Homo_Sapiens_root);
	while (p != NULL){
		prev = p;
		p = p->lc;
	}
	printf("Homo Sapiens: \n" );
	while(prev!= NULL){
		printf("[");
		populationPrint_Species(prev->population_root);
		printf("]\n");
		prev = prev->next;
	}
	return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void)
{
	return 1;
}


/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event;

	/* Check command buff arguments */
	/*if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
		return EXIT_FAILURE;
	}*/

	/* Open input file */
	if (( fin = fopen("C:\\Users\\user\\Desktop\\testfiles\\s50_p500.txt", "r") ) == NULL ) {
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin)) {

		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

			/* Comment */
			case '#':
				break;

				/* Insert Species
				 * S <sid> */
			case 'S':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (insert_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Insert population
				 * G <gid> <sid> <cid> */
			case 'G':
				{
					int gid, sid, cid;

					sscanf(buff, "%c %d %d %d", &event, &gid, &sid, &cid);
					DPRINT("%c %d %d %d\n", event, gid, sid, cid);

					if (insert_population(gid, sid, cid)) {
						DPRINT("%c %d %d %d succeeded\n", event, gid, sid, cid);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, gid, sid, cid);
					}

					break;
				}

				/* Lowest Common Ancestor
				 * L <sid> <gid1> <gid2> */
			 case 'L':
				{
					int sid, gid1, gid2;

					sscanf(buff, "%c %d %d %d", &event, &sid, &gid1, &gid2);
					DPRINT("%c %d %d %d\n", event, sid, gid1, gid2);

					if (lowest_common_ancestor(sid, gid1, gid2)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid, gid1, gid2);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid, gid1, gid2);
					}

					break;
				}

				/* Merge species
				 * M <sid1> <sid2> <sid3> */
			case 'M':
				{
					int sid1, sid2, sid3;

					sscanf(buff, "%c %d %d %d", &event, &sid1, &sid2, &sid3);
					DPRINT("%c %d %d %d\n", event, sid1, sid2, sid3);

					if (merge_species(sid1, sid2, sid3)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid1, sid2, sid3);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid1, sid2, sid3);
					}

					break;
				}

				/* Distribute species
				 * D */
			case 'D':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (distribute()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Delete population
				 * K <gid> <sid> */
			case 'K':
				{
					int gid, sid;

					sscanf(buff, "%c %d %d", &event, &gid, &sid);
					DPRINT("%c %d %d\n", event, gid, sid);

					if (delete_population(gid, sid)) {
						DPRINT("%c %d %d succeeded\n", event, gid, sid);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, gid, sid);
					}

					break;
				}

				/* Delete species
				 * F */
			case 'F':
				{
					DPRINT("%c \n", event);

					if (delete_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Evolution to homo sapiens
				 * E */
			case 'E':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (evolution()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Species' statistics
				 * N */
			case 'N':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (species_statistics()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Population statistics
				 * J <sid> */
			case 'J':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (population_statistics(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Print species
				 * P */
			case 'P':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print populations
				 * X */
			case 'X':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_populations()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print continents
				 * C */
			case 'C':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_continents()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print homo sapiens
				 * W */
			case 'W':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_homo_sapiens()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Empty line */
			case '\n':
				break;

				/* Ignore everything else */
			default:
				DPRINT("Ignoring buff: %s \n", buff);

				break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
