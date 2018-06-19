

typedef struct node node;
struct node {
	node* leftChild;
	node* rightChild;
	node* parent;
	int value;
};
