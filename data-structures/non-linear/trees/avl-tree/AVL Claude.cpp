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

    // LL atau LR
    if(bf > 1){
        if(balanceFactor(node->left) < 0) node->left = leftRotation(node->left);
        return rightRotation(node);
    }
    // RR atau RL
    if(bf < -1){
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
    scanf("%d",&N);
    Node *root = NULL;
    for(int i=0; i<N; i++){
        char op[100];
        int num;
        scanf("%s %d",op,&num);
        if(strcmp(op, "ADD")==0) root = insert(root, num);
        if(strcmp(op, "POP")==0) root = pop(root, num);
    }
    printf("In-order: ");
    inOrder(root);
    printf("\nRoot: %d",root->num);
    printf("\nHeight: %d",root->height);
}
