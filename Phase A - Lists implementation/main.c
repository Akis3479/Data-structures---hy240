
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int i = 0;
	while(i < 5){
		continents[i] = malloc(sizeof(struct ContinentList));
		continents[i]->sentinel = malloc(sizeof(struct ContinentPopulation));
		continents[i]->sentinel->gid = -1;
		continents[i]->sentinel->next = NULL;
		continents[i]->populationList = continents[i]->sentinel;
		i++;
	}

	return 1;
}

/**
 * @brief Print species' list with population lists
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
	struct Species *curr = Species_head;
	while (curr != NULL ){
		DPRINT("< %d >\n 	",curr->sid);
		struct Population *currP;
		currP = curr->Population_head;
		while (currP != NULL){
			DPRINT("< %d , %d> ",currP->gid,currP->cid);
			currP = currP->next;
		}
		DPRINT("\n");
		curr = curr->next;
	}
	DPRINT("\nDONE\n");
	return 1;
}

/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_species (int sid)
{
	struct Species *newSp;
	newSp = malloc(sizeof(struct Species));
	newSp->sid = sid;
	newSp->next = NULL;
	newSp->prev = NULL;
	newSp->Population_head = NULL;
	newSp->Population_tail = NULL;
	if ( Species_head == NULL ){
		Species_tail = newSp;
		Species_head = newSp;
		Species_head->prev = NULL;
	}
	else if (Species_head->sid > sid){
		newSp->prev = NULL;
		Species_head->prev = newSp;
		newSp->next = Species_head;
		Species_head = newSp;
	}
	else if(Species_tail->sid < sid){
		newSp->prev = Species_tail;
		Species_tail->next = newSp;
		Species_tail = newSp;
	}
	else{
		struct Species *curr = Species_head;
		while ( curr->sid < sid){
			curr = curr->next;
		}
		curr->prev->next = newSp;
		newSp->prev = curr->prev;
		curr->prev = newSp;
		newSp->next = curr;
	}
	/*Print*/
	struct Species *curr = Species_head;
	while (curr != NULL ){
		DPRINT("< %d >\n 	",curr->sid);
		struct Population *currP;
		currP = curr->Population_head;
		while (currP != NULL){
			DPRINT("< %d > ",currP->gid);
			currP = currP->next;
		}
		DPRINT("\n");
		curr = curr->next;
	}
	DPRINT("\nDONE\n");
	return 1;
}

/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_population(int gid, int sid, int cid){
	struct Population *newGr;
	newGr = malloc(sizeof(struct Population));
	newGr->gid = gid;
	newGr->sid = sid; 
	newGr->cid = cid;
	newGr->next = NULL;
	newGr->next_species = NULL;
	struct Species *curr = Species_head;
	while (curr != NULL && curr->sid != sid){
		curr = curr->next;
	}
	/* case empty*/
	if (curr->Population_head == NULL){
		curr->Population_head = newGr;
		curr->Population_tail = newGr;
	}
	else{
		struct Population *currGr = curr->Population_head;
		struct Population *prevGr = NULL;
		while (currGr != NULL && currGr->gid < gid){
			prevGr = currGr;
			currGr = currGr->next;
		}
		newGr->next = currGr;
		if (prevGr == curr->Population_tail){
			curr->Population_tail = newGr ;
		}
		if (prevGr == NULL){
			curr->Population_head = newGr;
		}
		else{
			prevGr->next = newGr;
		}
	}
	/*Print*/
	curr = Species_head;
	while (curr != NULL ){
		DPRINT("< %d >\n 	",curr->sid);
		struct Population *currP;
		currP = curr->Population_head;
		while (currP != NULL){
			DPRINT("< %d , %d> ",currP->gid,currP->cid);
			currP = currP->next;
		}
		DPRINT("\n");
		curr = curr->next;
	}
	DPRINT("\nDONE\n");

	return 1;
}

/**
 * @brief Print continents array with each continent's population list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
	int i ;
	struct ContinentPopulation *curr;
	for (i = 0 ; i < 5; i++){
		DPRINT("Continent %d : ", i );
		curr = continents[i]->populationList;
		while(curr->gid != -1){
			DPRINT("< %d >", curr->gid);
			curr = curr->next;
		}
		DPRINT("\n");
	}
	return 1;
}

/**
 * @brief delete species with ID <sid> and its populations
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_species(int sid){
	struct Species *tmp = Species_tail;
	struct ContinentPopulation *c1;
	struct ContinentPopulation *prevC = NULL;
	while(tmp != NULL && tmp->sid != sid){
		tmp = tmp->prev;
	}
	if (tmp == NULL){
		DPRINT("Sid doesnt exist")
		return 0;
	}
	struct Population *p1 = tmp->Population_head;
	while (p1 != NULL){
		int cont = p1->cid;
		c1 = continents[cont]->populationList;
		while(c1->gid !=  -1 && c1->gid != p1->gid){
			prevC = c1;
			c1 = c1->next;
		}

		if (prevC == NULL){
			continents[cont]->populationList = c1->next;
		}
		else{
			prevC->next = c1->next;
		}
		tmp->Population_head = p1->next;
		free(p1);
		p1 = tmp->Population_head;
	}
	if(tmp->prev == NULL){
		Species_head = tmp->next;
	}
	else if(tmp->next == NULL){
		Species_tail = tmp->prev;
		tmp->prev->next = NULL;
	}else{
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;
	}
	free(tmp);
	print_populations();
	print_continents();

	return 1;
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
int merge_species(int sid1, int sid2, int sid3){
	/*struct Species *tmp1 ;
	struct Species *tmp2 ;	
	struct Species *tmp3 ;
	insert_species (sid3);
	tmp1 = Species_head;
	tmp2 = Species_head;
	tmp3 = Species_head;
	while (tmp1 != NULL && tmp1->sid != sid1 ){
		tmp1 =  tmp1->next;
	}
	if (tmp1 == NULL){
		DPRINT("Sid doesnt exists\n");
		return 0;
	}
	while (tmp2 != NULL && tmp2->sid != sid1 ){
		tmp2 =  tmp2->next;
	}
	if (tmp2 == NULL){
		DPRINT("Sid doesnt exists\n");
		return 0;
	}

	while (tmp3 != NULL && tmp3->sid != sid1 ){
		tmp3 =  tmp3->next;
	}
	if (tmp3 == NULL){
		DPRINT("Sid doesnt exists\n");
		return 0;
	}
	
	struct Population *p1 ; 
	struct Population *p2 ;
	struct Population *ptr ;
	p1 = tmp1->Population_head; 
	p2 = tmp2->Population_head;

	if (p1 == NULL){
		tmp3->Population_head = p2;
	} 

	if (p2 == NULL){
		tmp3->Population_head = p1;
	} 

	if (p1 != NULL && p2 != NULL){
		if(p1->gid > p2->gid){
			ptr = p2;
			p2 = p2->next;
		}
		else{
			ptr = p1;
			p1 = p1->next;
		}
	}
	tmp3->Population_head = ptr;

	while (p1 != NULL && p2 != NULL){
		if (p1->gid > p2->gid){
			ptr->next = p1;
			ptr = p1;
			p1 = p1->next;
		}
		else{
			ptr->next = p2;
			ptr = p2;
			p2 = p2->next;
		}
	}
	if (p1 == NULL){
		ptr->next = p2;
	}
	if (p2 == NULL){
		ptr->next = p1;
	}
	delete_species(sid1);
	delete_species(sid2);
	print_populations();*/
	return 1;
}




/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
int distribute(){
	struct Species *s1;
	struct Population *p1;
	s1 = Species_head;
	while(s1 != NULL){
		p1 = s1->Population_head;
		while(p1 != NULL){
			struct ContinentPopulation *newC = malloc(sizeof(struct ContinentPopulation));
			newC->gid = p1->gid;
			newC->next = NULL;
			newC->next = continents[p1->cid]->populationList;
			continents[p1->cid]->populationList = newC;
			p1 = p1->next;
		}
		s1 = s1->next;
	}
	print_continents();

	return 1;
}

/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_population(int gid, int sid){
	struct Species *s1;
	s1 = Species_head;
	while (s1 != NULL && s1->sid != sid ){
		s1 =  s1->next;
	}
	if (s1 == NULL){
		DPRINT("Sid doesnt exists\n");
		return 0;
	}
	struct Population *p1;
	struct Population *prev = NULL;
	p1 = s1->Population_head;
	while(p1 != NULL && p1->gid != gid){
		prev = p1;
		p1 = p1->next;
	}
	if (p1 == NULL){
		DPRINT("Sid doesnt exists\n");
		return 0;
	}
	int cont = p1->cid;
	if (prev == NULL){
		s1->Population_head = p1->next;
	}
	else{
		prev->next = p1->next;
	}
	
	struct ContinentPopulation *c1;
	struct ContinentPopulation *prevC = NULL;
	c1 = continents[cont]->populationList;
	while(c1->gid !=  -1 && c1->gid != gid){
		prevC = c1;
		c1 = c1->next;
	}

	if (prevC == NULL){
		continents[cont]->populationList = c1->next;
	}
	else{
		prevC->next = c1->next;
	}
	


	DPRINT("SPECIES\n");
	DPRINT("< %d >\n 	",sid);
	struct Population *tmp1;
	tmp1 = s1->Population_head;
	while (tmp1 !=NULL){
		DPRINT("< %d >, ", tmp1->gid);
		tmp1 = tmp1->next;
	}
	DPRINT("\nCONTINENTS\n");
	DPRINT("	Continent %d : ",cont);
	struct ContinentPopulation *tmp2;
	tmp2 = continents[cont]->populationList;
	while(tmp2->gid != -1){
		DPRINT("<%d> ", tmp2->gid);
		tmp2 = tmp2->next;
	}
	DPRINT("\n");

	return 1;
}


/**
 * @brief Print homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
	DPRINT("Homo Sapiens: ");
	struct Population *tmp1;
	struct Population *tmp2;
	tmp1 = hs_L;
	while(tmp1 != NULL){
		tmp2 = tmp1;
		DPRINT("[ ");
		while (tmp2 != NULL){
			DPRINT(" < %d , %d > ,",tmp2->gid , tmp2->sid);
			tmp2 = tmp2->next;
		}
		DPRINT("] ");
		tmp1 = tmp1->next_species;
	}
	return 1;
}



/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
int evolution(){
	struct Species *s1;
	struct Species *s2;
	s1 = Species_head;

	while (s1 != Species_tail){
		if(s1->Population_head != NULL){
			if(hs_L == NULL){
				hs_L = s1->Population_head;
				hs_L->next_species = s1->next->Population_head;
				s1 = s1->next;
			}
			else{
				if(s1->next->Population_head == NULL){
					s2 = s1->next;
					while(s2 != NULL && s2->Population_head != NULL){
						s2 = s2->next;
					}
					if (s2 != NULL){
						s1->Population_head->next_species = s2->Population_head;
						s1 = s2;
					}
					if (s2 == NULL){
						break;
					}
				}
				else{
					s1->Population_head->next_species = s1->next->Population_head;
					s1 = s1->next;
				}
			}
		}
		else{
			s1 = s1->next;
		}

	}
	print_homo_sapiens();

	return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
	struct Population *tmp;
	int count = 0;
	tmp = hs_L;
	while(tmp != NULL){
		count++;
		tmp = tmp->next_species;
	}
	DPRINT("Homo Sapiens species: < %d >\n", count);
	return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
	struct Population *tmp;
	int count = 0;
	tmp = hs_L;
	while(tmp != NULL && tmp->sid != sid){
		tmp = tmp->next_species;
	}
	if (tmp == NULL){
		DPRINT("The sid doesnt exists\n");
		return 0;
	}
	while (tmp != NULL){
		tmp = tmp->next;
		count++;
	}

	DPRINT("Homo Sapiens population: < %d >\n", count);
	return 1;
}

/**
 * @brief Print species' list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_species(){
	struct Species *curr = Species_head;
	while (curr != NULL ){
		DPRINT("< %d >\n 	",curr->sid);
		struct Population *currP;
		currP = curr->Population_head;
		while (currP != NULL){
			DPRINT("< %d > ",currP->gid);
			currP = currP->next;
		}
		DPRINT("\n");
		curr = curr->next;
	}
	DPRINT("\nDONE\n");
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
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if (( fin = fopen(argv[1], "r") ) == NULL ) {
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
				 * F <sid> */
			case 'F':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (delete_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
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
