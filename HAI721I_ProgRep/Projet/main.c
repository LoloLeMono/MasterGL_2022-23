#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<arpa/inet.h>

// constants for size of char arrays to store filename, the line from the file
#define FILENAME_SIZE 1024
#define MAX_LINE 2048
int **tabNodes;
pid_t pidMain;
int idNode;

void printTabInt(int* tab, int size)
{
    printf("[ ");

    for (int i; i<size; i++)
    {
        if (i == size-1)
        {
            printf("%d ", tab[i]);
        }
        else
        {
            printf("%d | ", tab[i]);
        }
    }

    printf("] \n");
}

void addElement(int* tab, int el)
{
    int* buff = malloc(sizeof(int) * (sizeof(tab) + 1));
    int ii;

    for(int i=0; i<(int)sizeof(tab); i++)
    {
        buff[i] = tab[i];
        ii = i;
    }

    ii ++;

    buff[ii] = el;
}

// createLink :
void createLink(int firstNode, int secondNode)
{
    bool isIn = false;
    int x = 0;

    while (isIn == false && x < (int)sizeof(tabNodes[firstNode]))
    {
        if (tabNodes[firstNode][x] == secondNode)
        {
            isIn = true;
            break;
        }

        x++;
    }

    if (isIn == true)
    {
        return;
    }
    else
    {
        int *newVoisins = malloc((int)sizeof(tabNodes[firstNode]) + sizeof(int));

        for (int i = 0; i < (int)sizeof(newVoisins); i++)
        {
            newVoisins[i] = tabNodes[firstNode][i];
        }

        newVoisins[sizeof(newVoisins)] = secondNode;

        tabNodes[firstNode] = newVoisins;

        return;
    }
}

void comunication(int argc, char *argv[], int nbNodes)
{
    if (argc != 4)
    {
    printf("utilisation : %s ip_serveur port_serveur port_client\n", argv[0]);
    exit(1);
    }

    /* Etape 1 : créer une socket */   
    int ds = socket(PF_INET, SOCK_DGRAM, 0);

    /* /!\ : Il est indispensable de tester les valeurs de retour de
    toutes les fonctions et agir en fonction des valeurs
    possibles. Voici un exemple */
    if (ds == -1)
    {
        perror("Client : pb creation socket :");
        exit(1); // je choisis ici d'arrêter le programme car le reste
        // dépendent de la réussite de la création de la socket.
    }

    /* J'ajoute des traces pour comprendre l'exécution et savoir
    localiser des éventuelles erreurs */
    printf("Client : creation de la socket réussie \n");

    struct sockaddr_in ad;
    ad.sin_family = AF_INET ;
    ad.sin_addr.s_addr = INADDR_ANY ;
    ad.sin_port = htons(atoi(argv[3]));

    int res = bind(ds, (struct sockaddr*)&ad, sizeof(ad));

    if (res < 0)
    {
        printf("Erreur de nommage");
    }

    struct sockaddr_in adServer;
    adServer.sin_family = AF_INET;
    adServer.sin_addr.s_addr = inet_addr(argv[1]);
    adServer.sin_port = htons((short)atoi(argv[2]));
    socklen_t lgAdr = sizeof(struct sockaddr_in);

    char *nbThreads = malloc(sizeof(char) * 2);
    sprintf(nbThreads, "%f", (double)nbNodes);
    printf("On envoi : %s", nbThreads);

    int snd = sendto(ds, nbThreads, strlen(nbThreads)+1, 0, (struct sockaddr*) &adServer, sizeof(struct sockaddr_in));

    if (snd < 0)
    {
        perror("Erreur envoi \n");
        exit(1);
    }

    printf("Envoi reussi \n");

    close(ds);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Utilisation : %s port_serveur lienFichier\n", argv[0]);
        exit(1);
    }

    // file pointer will be used to open/read the file
    FILE *file;

    char buffer[MAX_LINE];
    // open the the file in read mode
    file = fopen(argv[2], "r");

    // if the file failed to open, exit with an error message and status
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    int nbNodes, nbLink = -1;
    int ind = 0;

    // PARSER
    do
    {
        fgets(buffer, MAX_LINE, file);

        if (buffer[0] == 'p')
        {
            char *buff2 = strtok(buffer, " ");

            buff2 = strtok(NULL, " ");
            buff2 = strtok(NULL, " ");
            nbNodes = atoi(buff2);
            printf("nbNodes = %d \n", nbNodes);
            buff2 = strtok(NULL, " ");
            nbLink = atoi(buff2);
            tabNodes = malloc(nbNodes * sizeof(int *));

            for (int i = 0; i < nbNodes; i++)
            {
                tabNodes[i] = malloc(nbNodes * sizeof(int));
            }

            printf("nbLink = %d \n", nbLink);

            for (int i = 0; i < nbNodes; i++)
            {
                for (int y = 0; y < nbNodes; y++)
                {
                    tabNodes[i][y] = 0;
                }
            }
        }

        if (buffer[0] == 'e')
        {
            int firstNode;
            int secondNode;
            int tabNodes[nbLink][2]; // Chaques cases de tabNodes représentent un link

            char *buff2 = strtok(buffer, " ");

            buff2 = strtok(NULL, " ");
            firstNode = atoi(buff2);
            buff2 = strtok(NULL, " ");
            secondNode = atoi(buff2);
            printf("Node1 = %d, Node2 = %d \n", firstNode, secondNode);

            tabNodes[firstNode - 1][secondNode - 1] = 1;
            printf("tabNode[%d][%d] = %d \n", firstNode, secondNode, tabNodes[firstNode - 1][secondNode - 1]);
        }

        if (buffer[0] == feof(file))
        {
            printf("EOF");
        }

    } while (feof(file) == 0);

    // close our access to the file
    fclose(file);

    pidMain = getpid();
    int tabPid[nbNodes];
    idNode = -1;

    // CREATION-PROCESSUS
    for(int i=0; i<nbNodes; i++)
    {
        if (fork() == 0)
        {
            printf("Je sors cars je suis fils %d \n", getpid());
            idNode = i;
            break;
        }
        else
        {
            printf("Je suis le père %d donc je créer un fils \n", getpid());
        }
    }

    printf("%d à finis avec comme id = %d \n", getpid(), idNode);

    // CREATION TAB PID
    /*
    if (getpid() == pidMain)
    {
        int cmpt = pidMain+1;

        for(int i=0; i<nbNodes; i++)
        {
            tabPid[i] = cmpt;
            cmpt++;
        }

        printTabInt(tabPid, nbNodes);
    }
    */

    // LE MAIN ATTEND DE RECEVOIR LES ID ET ADRESSE DES PROCESSUS
    if (getpid() == pidMain)
    {
        struct sockaddr_in tabSock[nbNodes];

        /* Etape 1 : créer une socket */   
        int ds = socket(PF_INET, SOCK_DGRAM, 0);

        if (ds == -1)
        {
            perror("Serveur : pb creation socket : \n");
            exit(1);
        }

        printf("Serveur : creation de la socket réussie \n");

        /* Etape 2 : Nommer la socket du seveur */

        struct sockaddr_in adMain;
        adMain.sin_family = AF_INET;
        adMain.sin_addr.s_addr = INADDR_ANY;
        adMain.sin_port = htons(atoi(argv[1]));

        int res = bind(ds, (struct sockaddr*)&adMain, sizeof(adMain));

        if (res < 0)
        {
            perror("Erreur de nommage \n");
        }

        /* Etape 4 : recevoir l'id du client et stocker son adresse */

        int compt = 0;
        struct sockaddr_in adProc;
        socklen_t lgAdr = sizeof(struct sockaddr_in);

        while (compt < nbNodes)
        {
            int idNodeReceive;

            printf("J'attend une connexion \n");
            int rec = recvfrom(ds, &idNodeReceive, sizeof(int), 0, (struct sockaddr*) &adProc, &lgAdr);

            if (rec < 0)
            {
                perror("erreur de reception \n");
                exit(1);
            }

            printf("Message reçu : %d \n", idNodeReceive);
            tabSock[idNodeReceive] = adProc;
            compt++;
        }
    }

    // CHAQUES PROCESSUS ENVOI SON ID ET ADRESSE AU MAIN
    if (getpid() != pidMain)
    {
        sleep(3+idNode);

        /* Etape 1 : créer une socket */   
        int ds = socket(PF_INET, SOCK_DGRAM, 0);

        if (ds == -1)
        {
            printf("Processus %d :\n", getpid());
            perror("pb creation socket \n");
            exit(1);
        }

        printf("Processus %d : creation de la socket réussie \n", getpid());

        int newPort = 2300 + idNode;

        struct sockaddr_in ad;
        ad.sin_family = AF_INET ;
        ad.sin_addr.s_addr = INADDR_ANY ;
        ad.sin_port = htons(newPort);

        int res = bind(ds, (struct sockaddr*)&ad, sizeof(ad));

        if (res < 0)
        {
            printf("Processus %d :\n", getpid());
            perror("erreur de nommage \n");
            exit(1);
        }

        printf("Processus %d : nommage réussi \n", getpid());    

        struct sockaddr_in adServer;
        adServer.sin_family = AF_INET;
        adServer.sin_addr.s_addr = inet_addr("127.0.0.1");
        adServer.sin_port = htons((short)atoi(argv[1]));
        socklen_t lgAdr = sizeof(struct sockaddr_in);


        printf("Processus %d : Je vais envoyer %d au main \n", getpid(), idNode);
        int snd = sendto(ds, &idNode, sizeof(int), 0, (struct sockaddr*) &adServer, sizeof(struct sockaddr_in));

        if (snd < 0)
        {
            printf("Processus %d : \n", getpid());
            perror("erreur d'envoi \n");
            exit(1);
        }

        printf("Envoi reussi \n");

    }

    return 0;
}