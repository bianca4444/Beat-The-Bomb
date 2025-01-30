#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES_GRAPH 100
#define MAX_VERTICES_SUBGRAPH 100
#define MAX_LINE_LENGTH 1000


struct node_secund {
    int dest;
    char answer[3];
    char continut_nod_graf_secundar[MAX_LINE_LENGTH];
    struct node_secund* next;
};

struct Graph_secund {
    struct node_secund* head[MAX_VERTICES_SUBGRAPH];
};

struct node {
    int dest;
    char continut_nod_graf_principal[MAX_LINE_LENGTH];
    struct node* next;
    struct node_secund* next_secundar; // Adăugăm o legătură către nodurile secundare
};

struct Graph {
    struct node* head[MAX_VERTICES_GRAPH];
};

struct Edge {
    int src, dest;
};

// Funcția pentru crearea grafului secundar
struct Graph_secund* createGraphSecund(struct Edge edges[], int num_edges, int num_vertices, struct node* node1,
                                       struct Graph* graph_principal, int nr_intrebari_utilizate)
{
    struct Graph_secund* graph_secund = (struct Graph_secund*)malloc(sizeof(struct Graph_secund));
    if (graph_secund == NULL) {
        fprintf(stderr, "Memory allocation failed for secondary graph.\n");
        exit(EXIT_FAILURE);
    }

    FILE *fisier_continut_intrebari = fopen("quiz_intrebari.txt", "r");
    if (fisier_continut_intrebari == NULL) {
        fprintf(stderr, "Failed to open quiz_intrebari.txt.\n");
        exit(EXIT_FAILURE);
    }

    FILE *fisier_raspunsuri_intrebari = fopen("input_answer.txt", "r");
    if (fisier_raspunsuri_intrebari == NULL) {
        fprintf(stderr, "Failed to open input_answer.txt.\n");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    char line_answer[3];
    while (nr_intrebari_utilizate > 0 && fgets(line, sizeof(line), fisier_continut_intrebari) != NULL) {
        nr_intrebari_utilizate--;
        fgets(line_answer, sizeof(line_answer), fisier_raspunsuri_intrebari);
    }

    // Reset head pointers
    for (int i = 0; i < num_vertices; i++) {
        graph_secund->head[i] = NULL;
    }

    // Create the first link: primary node --> secondary node
    struct node_secund* newnode = (struct node_secund*)malloc(sizeof(struct node_secund));
    if (newnode == NULL) {
        fprintf(stderr, "Memory allocation failed for secondary node.\n");
        exit(EXIT_FAILURE);
    }
    newnode->dest = 1;
    newnode->next = NULL;
    node1->next_secundar = newnode;

    // Read the question of the first secondary node
    if (fgets(line, sizeof(line), fisier_continut_intrebari) != NULL) {

        line[strcspn(line, "\n")] = '\0'; // Remove the newline character at the end of the line
        strncpy(newnode->continut_nod_graf_secundar, line, sizeof(newnode->continut_nod_graf_secundar));

        //se adauga raspunsul corect pentru fiecare intrebare
        fgets(line_answer, sizeof(line_answer), fisier_raspunsuri_intrebari);
        line_answer[strcspn(line_answer, "\n")] = '\0';
        strncpy(newnode->answer, line_answer, sizeof(newnode->answer));

    }

    int cnt_edge = 1;
    // Create links between secondary nodes
    for (int i = 0; i < num_edges; i++) {
        int src = cnt_edge;
        int dest = src + 1;

        struct node_secund* newnode_secund = (struct node_secund*)malloc(sizeof(struct node_secund));
        if (newnode_secund == NULL) {
            fprintf(stderr, "Memory allocation failed for secondary node.\n");
            exit(EXIT_FAILURE);
        }
        newnode_secund->dest = dest;
        newnode_secund->next = NULL;

        // Read the question of the node
        if (fgets(line, sizeof(line), fisier_continut_intrebari) != NULL) {

            line[strcspn(line, "\n")] = '\0'; // Remove the newline character at the end of the line
            strncpy(newnode_secund->continut_nod_graf_secundar, line, sizeof(newnode_secund->continut_nod_graf_secundar));

            //se adauga raspunsul corect pentru fiecare intrebare
            fgets(line_answer, sizeof(line_answer), fisier_raspunsuri_intrebari);
            line_answer[strcspn(line_answer, "\n")] = '\0';
            strncpy(newnode_secund->answer, line_answer, sizeof(newnode_secund->answer));
        }

        newnode->next = newnode_secund;
        newnode = newnode_secund; // Ne  mutam la next node
        cnt_edge++;
    }

    fclose(fisier_continut_intrebari);
    fclose(fisier_raspunsuri_intrebari);
    return graph_secund;
}


// Funcția pentru crearea grafului principal
struct Graph* createGraph(struct Edge edges[], int num_edges, int num_vertices) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));

    FILE *fisier_continut_quiz = fopen("quiz_categorii.txt", "r");
    if (fisier_continut_quiz == NULL) {
        fprintf(stderr, "Nu s-a putut deschide fisierul.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_vertices; i++) {
        graph->head[i] = NULL;
    }

    char line[MAX_LINE_LENGTH];
    for (int i = 0; i < num_edges; i++) {
        int src = edges[i].src;
        int dest = edges[i].dest;

        struct node* newnode = (struct node*)malloc(sizeof(struct node));
        newnode->dest = dest;

        // Citim categoria nodului
        if (fgets(line, sizeof(line), fisier_continut_quiz) != NULL) {
            line[strcspn(line, "\n")] = '\0'; // Eliminăm caracterul newline de la sfârșitul liniei
            strncpy(newnode->continut_nod_graf_principal, line, sizeof(newnode->continut_nod_graf_principal));
        }

        newnode->next = graph->head[src];
        graph->head[src] = newnode;

        // O sa facem graful secund pt fiecare nod pricipal
        // fiecare nod principal o sa contina 4 intrebari (alte 4 noduri formand un subgraf)
        struct Edge edges_secund[4]; // muchii pentru graf secundar

        int nr_intrebari_utilizate, ed, ver;

        if( i == 0 )
        {
            nr_intrebari_utilizate = src * 3;
            ed = 2;
            ver = 3;
        }
        else{
            nr_intrebari_utilizate = src * 4 - 1;
            ed = 3;
            ver = 4;

        }
        struct Graph_secund* graph_secund = createGraphSecund(edges_secund, ed, ver, newnode, graph, nr_intrebari_utilizate);
    }

    fclose(fisier_continut_quiz);
    return graph;
}

// Funcția pentru afișarea grafului
void printGraph(struct Graph* graph, int num_vertices) {
    for (int i = 0; i < num_vertices; i++) {
        struct node* ptr = graph->head[i];
        if (ptr != NULL) {
            printf("Nod principal %d: %s\n", i, ptr->continut_nod_graf_principal);
            struct node_secund* ptrs = ptr->next_secundar;
            while (ptrs != NULL) {
                printf("  - Nod secundar %d: %s\n answer:%s\n", ptrs->dest, ptrs->continut_nod_graf_secundar, ptrs->answer);
                ptrs = ptrs->next;
            }
            ptr = ptr->next; // Trecem la urmatorul nod principal
            printf("\n");
        }
    }
}
