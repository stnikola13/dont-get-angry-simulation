#include <stdio.h>
#include <stdlib.h>

#define SAFE_MALLOC(pointer, size, type) \
    pointer = malloc((size) * sizeof(type));         \
    if (pointer == NULL) {                     \
        printf("MEM_GRESKA");            \
        exit(-1);\
    }

#define SAFE_REALLOC(pointer, size, type) \
    pointer = realloc(pointer, size * sizeof(type));         \
    if (pointer == NULL) {                     \
        printf("MEM_GRESKA");            \
        exit(-1);\
    }

typedef struct node {
    int node, level, type;
    struct node *next;
} Node;

typedef struct graph {
    int *vertices;
    int *branches;
} Graph;

typedef struct stack {
    int no_of_el;
    Node *top;
} Stack;

void createStack(Stack *stack) {
    stack->no_of_el = 0;
    stack->top = NULL;
}

int stackEmpty(Stack *stack) {
    if (stack->no_of_el == 0) return 1;
    else return 0;
}

void push(Stack *stack, int node, int level, int type) {
    Node *q;
    q = malloc(sizeof(Node));
    if (q == NULL) {
        printf("MEM_GRESKA");
        exit(-1);
    }
    q->node = node;
    q->level = level;
    q->type = type;
    q->next = NULL;
    if (stackEmpty(stack) == 1) {
        stack->top = q;
    }
    else {
        q->next = stack->top;
        stack->top = q;
    }
    (stack->no_of_el)++;
}

void pop(Stack *stack, int *node, int *level, int *type) {
    Node *q;
    q = stack->top;
    *node = q->node;
    *level = q->level;
    *type = q->type;
    stack->top = q->next;
    free(q);
    (stack->no_of_el)--;
}

void destroyStack(Stack *stack) {
    Node *p, *q;
    p = stack->top;
    if (p == NULL) return;
    else {
        q = p;
        p = p->next;
        for (; p != NULL; p = p->next) {
            free(q);
            q = p;
        }
    }
    free(stack);
}

void deallocateGraph (Graph *graph) {
    free(graph->vertices);
    free(graph->branches);
    free(graph);
}

void createGraph (int *vertices, int *branches, int dim) {
    for (int i = 0; i < dim; i++) {
        vertices[i] = i;
    }
    for (int i = 0; i < dim; i++) {
        branches[i] = 0;
    }
}

void addNode (int *vertices, int *branches, int *dim, int *branch_len) {
    vertices[*dim] = vertices[*dim - 1] + 1;
    branches[*branch_len] = 0;
    printf("Dodat cvor broj: %d.\n", *dim + 1);
    (*dim)++;
    (*branch_len)++;
}

void removeNode (int *vertices, int *branches, int *dim, int *branch_len, int node) {
    int i, start_ind, no_of_elems, branches_removed;
    start_ind = vertices[node - 1];

    if (node == *dim) no_of_elems = *branch_len - start_ind;
    else no_of_elems = vertices[node] - vertices[node - 1];

    // Brisanje svih njegovnih izlaznih grana i njega samog
    for (i = start_ind; i < *branch_len - no_of_elems; i++) {
        //printf("%d<->%d\n", branches[i], branches[i + no_of_elems]);
        branches[i] = branches[i + no_of_elems];
    }
    for (i = node - 1; i < *dim - 1; i++) {
        vertices[i] = vertices[i + 1];
        vertices[i] -= no_of_elems; // vracanje indeksa za onoliko grana koliko je izbrisano
    }
    branches_removed = no_of_elems;
    (*dim)--;
    *branch_len -= branches_removed;

    // Brisanje grana gde je destinacija obrisani cvor
    for (i = 0; i < *dim; i++) { // Ide po svim cvorovima
        start_ind = vertices[i];
        if (i == *dim - 1) no_of_elems = *branch_len - vertices[*dim - 1];
        else no_of_elems = vertices[i + 1] - vertices[i];
        for (int j = 0; j < no_of_elems; j++) { // Ide po granama tekuceg cvora
            if (branches[start_ind + j] == node) {
                for (int k = start_ind + j; k < *branch_len - 1; k++) {
                    branches[k] = branches[k + 1];
                }
                (*branch_len)--;
                for (int k = i + 1; k < *dim; k++) {
                    (vertices[k])--;
                }
                j--;
                no_of_elems--;
            }
        }
    }
    // Apdejt indeksa cvorova na koje grane pokazuju koje su bile vece od obrisanog
    // (posledica sto su se cvorovi siftovali u levo za jedno mesto)
    for (i = 0; i < *branch_len; i++) {
        if (branches[i] > node) (branches[i])--;
    }
}

void addBranch (int *vertices, int *branches, int *dim, int *branch_len, int start_node, int dest_node) {
    int i, start_ind, no_of_elems;
    start_ind = vertices[start_node - 1];

    if (branches[start_ind] == 0) {
        branches[start_ind] = dest_node;
    }
    else {
        if (start_node == *dim) {
            branches[*branch_len] = dest_node;
            (*branch_len)++;
        }
        else {
            no_of_elems = vertices[start_node] - vertices[start_node - 1];
            for (i = *branch_len; i > start_ind + no_of_elems; i--) {
                branches[i] = branches[i - 1];
            }
            (*branch_len)++;
            branches[start_ind + no_of_elems] = dest_node;
            for (i = start_node; i < *dim; i++) {
                (vertices[i])++;
            }
        }
    }
}

void removeBranch (int *vertices, int *branches, int *dim, int *branch_len, int start_node, int dest_node) {
    int i, start_ind, no_of_elems, t = 0;
    start_ind = vertices[start_node - 1];

    if (start_node == *dim) no_of_elems = *branch_len - start_ind;
    else no_of_elems = vertices[start_node] - vertices[start_node - 1];

    for (i = start_ind; i < start_ind + no_of_elems; i++) {
        if (branches[i] == dest_node) {
            t = 1; // t ukazuje da grana postoji
            break;
        }
    }
    if (!t) {
        printf("Data grana ne postoji u grafu!\n");
        return; // ako ne postoji grana nista se ne desava
    }

    if (no_of_elems == 1) {
        branches[start_ind] = 0;
    }
    else {
        for (i = start_ind; i < *branch_len - 1; i++) {
            if (branches[i] == dest_node) break;
        }
        for (; i < *branch_len - 1; i++) {
            branches[i] = branches[i + 1];
        }
        (*branch_len)--;
        for (i = start_node; i < *dim; i++) {
            (vertices[i])--;
        }
    }
}

void tempPrint (int *vertices, int *branches, int dim, int branch_len) {
    printf("Vertices: ");
    for (int i = 0; i < dim; i++) {
        if (i == dim - 1) printf("%d", vertices[i]);
        else printf("%d->", vertices[i]);
    }
    printf("\nBranches: ");
    for (int i = 0; i < branch_len; i++) {
        if (i == branch_len - 1) printf("%d", branches[i]);
        else printf("%d->", branches[i]);
    }
    putchar('\n');
}

void printGraph (int *vertices, int *branches, int dim, int branch_len) {
    int i, j, no_of_elems, ind_start;

    printf("Reprentacija - Susednost u grafu:\n");
    printf("----------------------------------\n");
    for (i = 0; i < dim; i++) {
        ind_start = vertices[i];
        if (i == dim - 1) no_of_elems = branch_len - vertices[dim - 1];
        else no_of_elems = vertices[i + 1] - vertices[i];

        if (no_of_elems == 1) printf("Sused cvora %d je: ", i + 1);
        else printf("Susedi cvora %d su: ", i + 1);

        for (j = 0; j < no_of_elems; j++) {
            if (j == no_of_elems - 1) printf("%d.\n", branches[ind_start + j]);
            else printf("%d, ", branches[ind_start + j]);
        }
    }
}

void printStack(Stack *stack, int count) {
    Node *p = stack->top;
    if (p == NULL) {
        printf("Nema putanje.\n");
        return;
    }

    Stack *s = NULL;
    SAFE_MALLOC(s, 1, Stack);
    createStack(s);
    for (; p != NULL; p = p->next) {
        push(s, p->node, p->level, p->type);
    }

    printf("Putanja %d: ", count);
    // for (p = s->top; p->next != NULL; p = p->next) {  // ako treba da se izbrise "kraj" sa puta + izbrisi printf kraj
    for (p = s->top; p != NULL; p = p->next) {
        printf("%d -> ", p->node);
    }
    printf("KRAJ\n");
    destroyStack(s);
}

void printP2path(Stack *stack, int count) {
    int counter = 0;
    Node *p = stack->top;
    if (p == NULL) {
        printf("Nema putanje.\n");
        return;
    }

    Stack *s = NULL;
    SAFE_MALLOC(s, 1, Stack);
    createStack(s);
    for (; p != NULL; p = p->next) {
        push(s, p->node, p->level, p->type);
    }

    printf("Putanja %d: ", count);
    // for (p = s->top; p->next != NULL; p = p->next) {  // ako treba da se izbrise "kraj" sa puta + izbrisi printf kraj
    for (p = s->top; p != NULL; p = p->next) {
        if (counter % 3 == 1 || counter == 0) printf("%d -> ", p->node);
        else if (counter % 3 == 2) printf("(%d -> ", p->node);
        else printf("%d) -> ", p->node);
        counter++;

    }
    printf("KRAJ\n");
    destroyStack(s);
}

int findPathP1 (int *vertices, int *branches, int dim, int branch_len, int startNode, int endNode, int pathLen) {
    Stack *traversal = NULL, *cur_path = NULL;
    int *visit = NULL, i, pathExists = -1, pathCount = 0;
    int start_ind, no_of_elems;

    SAFE_MALLOC(traversal, 1, Stack);
    SAFE_MALLOC(cur_path, 1, Stack);
    createStack(traversal);
    createStack(cur_path);
    SAFE_MALLOC(visit, dim + 1, int);
    for (i = 0; i < dim; i++) visit[i] = 0;

    push(traversal, startNode, 0, 0);
    while (!stackEmpty(traversal)) {
        int node, level, type;
        pop(traversal, &node, &level, &type);
        (visit[node - 1])++;
        if (visit[node - 1] > 5) {
            pathExists = -1;
            continue;
        }

        while (!stackEmpty(cur_path)) {
            int new_node, new_level, new_type;
            pop(cur_path, &new_node, &new_level, &new_type);
            if (new_level >= level) continue;
            else {
                push(cur_path, new_node, new_level, new_type);
                break;
            }
        }

        push(cur_path, node, level, type);

        if (level > pathLen) {
            int new_node, new_level, new_type;
            pop(cur_path, &new_node, &new_level, &new_type);
            continue; // Ukoliko dodjemo do razdaljine vece od trazene, preskacemo ubacivanje na stek
        }

        if (level == pathLen && node == endNode) {
            int t = 0;
            for (Node *p = cur_path->top; p != NULL; p = p->next) {
                if (p->node == endNode) t++;
            }
            if (t < 3) {
                printStack(cur_path, ++pathCount); // obezbedjuje da se do cilja dodje samo jednom
                pathExists = 1;
            }
        }

        start_ind = vertices[node - 1];
        if (node == dim) no_of_elems = branch_len - start_ind;
        else no_of_elems = vertices[node] - vertices[node - 1];
        for (i = start_ind; i < start_ind + no_of_elems; i++) {
            if (branches[i] == 0) continue;
            push(traversal, branches[i], level + 1, 0);
        }
    }
    destroyStack(traversal);
    destroyStack(cur_path);
    free(visit);
    return pathExists;
}

int findPathP2 (int *vertices, int *branches, int dim, int branch_len, int startNode, int endNode, int pathLen) {
    Stack *traversal = NULL, *cur_path = NULL;
    int *visit = NULL, i, pathExists = -1, pathCount = 0;
    int start_ind, no_of_elems;

    SAFE_MALLOC(traversal, 1, Stack);
    SAFE_MALLOC(cur_path, 1, Stack);
    createStack(traversal);
    createStack(cur_path);
    SAFE_MALLOC(visit, dim + 1, int);
    for (i = 0; i < dim; i++) visit[i] = 0;

    push(traversal, startNode, 0, 22);
    while (!stackEmpty(traversal)) {
        int node, level, type;
        pop(traversal, &node, &level, &type);
        (visit[node - 1])++;
        if (visit[node - 1] > 10) {
            pathExists = -1;
            continue;
        }

        while (!stackEmpty(cur_path)) {
            int new_node, new_level, new_type;
            pop(cur_path, &new_node, &new_level, &new_type);
            if (new_level > level) continue;
            else if (new_level == level && new_type >= type) continue;
            else {
                push(cur_path, new_node, new_level, new_type);
                break;
            }
        }

        push(cur_path, node, level, type);

        if (level > pathLen) {
            int new_node, new_level, new_type;
            pop(cur_path, &new_node, &new_level, &new_type);
            continue; // Ukoliko dodjemo do razdaljine vece od trazene, izlazimo
        }

        if (level == pathLen && node == endNode && type != 21) {
            int t = 0;
            for (Node *p = cur_path->top; p != NULL; p = p->next) {
                if (p->node == endNode) t++;
            }
            if (t < 3) { // obezbedjuje da se do cilja dodje samo jednom
                pathExists = 1;
                pathCount++;
                printP2path(cur_path, pathCount);
            }
            else continue; // obezbedjujemo da se ne stavljaju susedi ako vec ne odgovara cvor
        }

        start_ind = vertices[node - 1];
        if (node == dim) no_of_elems = branch_len - start_ind;
        else no_of_elems = vertices[node] - vertices[node - 1];
        int new_type, new_level;
        switch (type) {
            case 11: new_type = 21; new_level = level + 1; break;
            case 21: new_type = 22; new_level = level; break;
            case 22: new_type = 11; new_level = level + 1; break;
        }
        for (i = start_ind; i < start_ind + no_of_elems; i++) {
            if (branches[i] == 0) continue;
            push(traversal, branches[i], new_level, new_type);
        }
    }
    destroyStack(traversal);
    destroyStack(cur_path);
    free(visit);
    return pathExists;
}

int main() {
    int n, t = 0;

    printf("--------------------------------\n");
    printf("  Grafovi - Covece ne ljuti se \n");
    printf("--------------------------------\n");
    putchar('\n');
    printf("------------ Meni --------------\n");
    printf("     1) Formiranje grafa\n");
    printf("     2) Dodavanje grane\n");
    printf("     3) Brisanje grane\n");
    printf("     4) Dodavanje cvora\n");
    printf("     5) Brisanje cvora\n");
    printf("     6) Reprezentacija grafa\n");
    printf("     7) Simulacija igre\n");
    printf("     8) Ispis menija\n");
    printf("     9) Kraj programa\n");

    int dim, branch_len, i, doExit = 0, num;
    int *vertices = NULL, *branches = NULL;
    Graph *graph = NULL;

    while (1) {
        printf("------------------------\n");
        printf("Unesite zeljenu opciju: \n");
        scanf("%d", &n);
        if (n < 1 || n > 9) {
            printf("Neispravno unet broj opcije!\n");
            continue;
        }

        switch (n) {
            case 1: // Incijalizacija grafa
                if (t != 0) {
                    printf("Graf vec postoji, molimo restartujte program!\n");
                    break;
                }
                printf("Unesite broj cvorova: \n");
                scanf("%d", &dim);
                SAFE_MALLOC(vertices, dim, int);
                SAFE_MALLOC(branches, dim, int);
                branch_len = dim;
                createGraph(vertices, branches, dim);
                graph = malloc(1 * sizeof(Graph));
                if (graph == NULL) return 0;
                graph->branches = branches;
                graph->vertices = vertices;
                t = 1;
                break;

            case 2: // Dodavanje grane
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }

                printf("Unesite broj grana koji zelite da dodate: \n");
                scanf("%d", &num);
                for (i = 0; i < num; i++) {
                    printf("%d. grana - Unesite izvorisni i odredisni cvor: \n", i + 1);
                    int node1, node2;
                    scanf("%d %d", &node1, &node2);
                    if (node1 < 1 || node1 > dim || node2 < 1 || node2 > dim) {
                        printf("Cvorovi neispravni, pokusajte ponovo:\n");
                        i--;
                        continue;
                    }
                    SAFE_REALLOC(branches, (branch_len + 1), int);
                    addBranch(vertices, branches, &dim, &branch_len, node1, node2);
                    printf("Grana uspesno dodata!\n");
                }
                break;

            case 3: // Brisanje grane
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }
                printf("Unesite broj grana koji zelite da izbrisete: \n");
                scanf("%d", &num);
                for (i = 0; i < num; i++) {
                    printf("%d. grana - Unesite izvorisni i odredisni cvor: \n", i + 1);
                    int node1, node2;
                    scanf("%d %d", &node1, &node2);
                    if (node1 < 1 || node1 > dim || node2 < 1 || node2 > dim) {
                        printf("Cvorovi neispravni, pokusajte ponovo:\n");
                        i--;
                        continue;
                    }
                    removeBranch(vertices, branches, &dim, &branch_len, node1, node2);
                    SAFE_REALLOC(branches, (branch_len), int);
                    printf("Grana uspesno obrisana!\n");
                }
                break;

            case 4: // Dodavanje cvora
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }
                SAFE_REALLOC(vertices, (dim + 1), int);
                SAFE_REALLOC(branches, (branch_len + 1), int);
                addNode(vertices, branches, &dim, &branch_len);
                break;

            case 5: // Brisanje cvora
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }
                printf("Unesite broj cvora koji zelite da izbrisete: \n");
                scanf("%d", &num);
                SAFE_REALLOC(vertices, (dim), int);
                SAFE_REALLOC(branches, (branch_len), int);
                removeNode(vertices, branches, &dim, &branch_len, num);
                break;

            case 6: // Ispis reprezentacije
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }
                printf("Cvorovi idu redom: ");
                for (i = 0; i < dim; i++) {
                    if (i == 0) printf("X - ");
                    else if (i == dim - 1) printf("Y.\n");
                    else printf("%c - ", 'A' + i - 1);
                }
                tempPrint(vertices, branches, dim, branch_len);
                printGraph(vertices, branches, dim, branch_len);
                break;

            case 7: // Simulacija
                if (t == 0) {
                    printf("Graf ne postoji, molimo prvo inicijalizujte graf!\n");
                    break;
                }
                printf("Unesite broj pocetnog cvora i broj ciljnog cvora: \n");
                int startNode, endNode;
                scanf("%d %d", &startNode, &endNode);

                // MAJA
                printf("-- Simulacija --\n");
                printf("---------------\n");
                printf("Majini putevi: \n\n");
                int s1 = 0, e1, e2, movesP1 = 0;
                for (i = 1; i < dim + 1; i++) {
                    e1 = findPathP1(vertices, branches, dim, branch_len, startNode, endNode, i);
                    if (e1 == 1) {
                        movesP1 = i;
                        s1 = 1;
                        break;
                    }
                }
                if (s1 == 0) printf("Nijedan put nije pronadjen.\n");
                printf("---------------\n\n");

                // SANJA
                int movesP2 = 0, s2 = 0;
                printf("---------------\n");
                printf("Sanjini putevi: \n\n");
                for (i = 1; i < dim + 1; i++) {
                    e2 = findPathP2(vertices, branches, dim, branch_len, startNode, endNode, i);
                    if (e2 == 1) {
                        movesP2 = i;
                        s2 = 1;
                        break;
                    }
                }
                if (s2 == 0) printf("Nijedan put nije pronadjen.\n");
                printf("---------------\n");

                if (s1 == 1) printf("Maji je bilo potrebno %d poteza.\n", movesP1);
                if (s2 == 1) printf("Sanji je bilo potrebno %d poteza.\n", movesP2);
                printf("------------------------------------\n");
                if (s1 == 0 && s2 == 1) printf("Pobednik je Sanja, Maja nije uspela da dodje do cilja.\n");
                else if (s1 == 1 && s2 == 0) printf("Pobednik je Maja, Sanja nije uspela da dodje do cilja.\n");
                else if (movesP1 == movesP2) printf("Igra je neresena!\n");
                else if (movesP1 > movesP2) printf("Pobednik je Sanja!\n");
                else if (movesP1 < movesP2) printf("Pobednik je Maja!\n");
                printf("------------------------------------\n");
                deallocateGraph(graph);
                printf("Simulacija zavrsena. Zdravo!");
                doExit = 1;
                break;

            case 8: // Ispis menija
                printf("------------ Meni --------------\n");
                printf("     1) Formiranje grafa\n");
                printf("     2) Dodavanje grane\n");
                printf("     3) Brisanje grane\n");
                printf("     4) Dodavanje cvora\n");
                printf("     5) Brisanje cvora\n");
                printf("     6) Reprezentacija grafa\n");
                printf("     7) Simulacija igre\n");
                printf("     8) Ispis menija\n");
                printf("     9) Kraj programa\n");
                continue;

            case 9: // Kraj programa
                printf("Zdravo!");
                doExit = 1;
                break;
        }
        if (doExit) break;
    }

    return 0;
}
