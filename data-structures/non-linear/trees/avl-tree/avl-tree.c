#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct Data {
    int height, num;
    struct Data *left, *right;
}Node;

int max(int a, int b);
int getHeight(Node *node);
void updateHeight(Node *node);
int balanceFactor(Node *node);
Node *createNode(int num);
Node *insert(Node *node, int num);
Node *rebalance(Node *node);
Node *leftRotation(Node *x);
Node *rightRotation(Node *y);
Node *pop(Node *node, int num);
Node *findMin(Node *node);
Node *search(Node *node, int num);
int countNode(Node *node);
void preOrder(Node *node);
void inOrder(Node *node);
void postOrder(Node *node);

int max(int a, int b) {return (a > b) ? a : b;}

int getHeight(Node *node){
    if(!node) return 0;
    return node->height;
}

void updateHeight(Node *node){node->height = 1 + max(getHeight(node->left), getHeight(node->right));}

int balanceFactor(Node *node){
    if(!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

Node *search(Node *node, int num){
    if(!node || node->num == num) return node;
    if(num < node->num) return search(node->left, num);
    return search(node->right, num);
}

int countNode(Node *node){
    if(!node) return 0;
    return 1 + countNode(node->left) + countNode(node->right);
}

Node *createNode(int num){
    Node *newnode = (Node *)malloc(sizeof(Node));
    newnode->num = num;
    newnode->height = 1;
    newnode->left = newnode->right = NULL;
    return newnode;
}

Node *insert(Node *node, int num){
    if(!node) return createNode(num);
    if(num < node->num) node->left = insert(node->left, num);
    else if(num > node->num) node->right = insert(node->right, num);
    else return node;
    return rebalance(node);
}

Node *rebalance(Node *node){
    updateHeight(node);
    int bf = balanceFactor(node);

    if(bf > 1){ // LL atau LR
        if(balanceFactor(node->left) < 0) node->left = leftRotation(node->left);
        return rightRotation(node);
    }
    if(bf < -1){ // RR atau RL
        if(balanceFactor(node->right) > 0) node->right = rightRotation(node->right);
        return leftRotation(node);
    }
    return node;
}

Node *leftRotation(Node *x){
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);
    
    return y;
}

Node *rightRotation(Node *y){
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);
    
    return x;
}

Node *pop(Node *node, int num){
    if(!node) return node;
    if(num < node->num) node->left = pop(node->left, num);
    else if(num > node->num) node->right = pop(node->right, num);
    else{
        if(!node->left && !node->right){
            free(node);
            return NULL;
        }
        if(!node->left){
            Node *temp = node->right;
            free(node);
            return temp;
        }
        if(!node->right){
            Node *temp = node->left;
            free(node);
            return temp;
        }
        Node *temp = findMin(node->right);
        node->num = temp->num; // ganti angka yg didelete dengan cucu kanan
        node->right = pop(node->right, temp->num); // delete posisi awal cucu kanan karena udah pindah tempat
    }
    return rebalance(node);
}

Node *findMin(Node *node){
    if(!node) return node;
    while(node->left) node = node->left;
    return node;
}

void preOrder(Node *node){ // preorder (sebelum)
    if(!node) return;
    printf("%d ", node->num);
    preOrder(node->left);
    preOrder(node->right);
}

void inOrder(Node *node){ // inorder (tengah)
    if(!node) return;
    inOrder(node->left);
    printf("%d ", node->num);
    inOrder(node->right);
}

void postOrder(Node *node){ // postorder (sesudah)
    if(!node) return;
    postOrder(node->left);
    postOrder(node->right);
    printf("%d ", node->num);
}

int main(){
    int N;
    Node *root = NULL;

    printf("=== AVL Tree ===\n");
    printf("Enter the number of commands: ");
    scanf("%d",&N);    
    printf("Available commands:\n");
    printf("  ADD <num>   - add a number to the tree\n");
    printf("  DEL <num>   - delete a number from the tree\n");
    printf("  FIND <num>  - check if a number exists\n");
    printf("  SHOW-ALL    - show all numbers (in-order)\n");
    printf("---------------------------------------------\n");
    for(int i=0; i<N; i++){
        char op[10]; int num;
        scanf("%s", op);
        if(strcmp(op, "ADD") == 0){
            scanf("%d",&num);
            if(search(root, num)) {
                printf("Case %d:\nNumber already exists\n",i+1);
                continue;
            }
            printf("Case %d:\nSuccessfully Added\n",i+1);
            root = insert(root, num);
        }
        if(strcmp(op, "DEL") == 0){
            scanf("%d",&num);
            if(!search(root, num)) {
                printf("Case %d:\nNumber not found\n",i+1);
                continue;
            }
            printf("Case %d:\nSuccessfully Deleted\n",i+1);
            root = pop(root, num);
        }
        if(strcmp(op, "FIND") == 0){
            scanf("%d",&num);
            if(search(root, num)) printf("Case %d:\nFound\n",i+1);
            else printf("Case %d:\nNot Found\n",i+1);
        }
        if(strcmp(op, "SHOW-ALL") == 0){
            printf("Case %d:\nTotal Numbers (%d):\n",i+1, countNode(root));
            inOrder(root);
        }
    }
    return 0;
}
