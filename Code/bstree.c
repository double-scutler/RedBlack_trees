#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current);

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_insert_case0(ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_insert_case1(ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_insert_case2_left(ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_insert_case2_right(ptrBinarySearchTree x);

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case1_left(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case1_right(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case2_left(ptrBinarySearchTree p, ptrBinarySearchTree x);
ptrBinarySearchTree fixredblack_remove_case2_right(ptrBinarySearchTree p, ptrBinarySearchTree x);

/*------------------------  BSTreeType  -----------------------------*/

typedef enum {red, black} NodeColor;

struct _bstree {
    BinarySearchTree *parent;
    BinarySearchTree *left;
    BinarySearchTree *right;
    int root;
    NodeColor color;
};

/*------------------------  BaseBSTree  -----------------------------*/

BinarySearchTree *bstree_create() {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */

BinarySearchTree *bstree_cons(BinarySearchTree *left, BinarySearchTree *right, int root, BinarySearchTree *parent) {
    BinarySearchTree *t = malloc(sizeof(struct _bstree));
    t->parent = parent;
    t->left = left;
    t->right = right;
    if (t->left != NULL)
        t->left->parent = t;
    if (t->right != NULL)
        t->right->parent = t;
    t->root = root;
    t->color=red;
    return t;
}

void bstree_delete(ptrBinarySearchTree *t) {
    while (!bstree_empty(*t))
        bstree_remove_node(t, *t);
}

bool bstree_empty(const BinarySearchTree *t) {
    return t == NULL;
}

int bstree_root(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->root;
}

BinarySearchTree *bstree_left(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree *bstree_right(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree *bstree_parent(const BinarySearchTree *t) {
    assert(!bstree_empty(t));
    return t->parent;
}

/*------------------------  BSTreeDictionary  -----------------------------*/

/* Obligation de passer l'arbre par référence pour pouvoir le modifier */

/**void bstree_add_rec(ptrBinarySearchTree *t, int v) {
    if (*t == NULL)
        bstree_cons(NULL, NULL, v);
    else {
        if ( v < (*t)->root) {
            bstree_add_rec(&((*t)->left), v);
            (*t)->left->parent = *t;
        }
        else if ( v > (*t)->root) {
            bstree_add_rec(&((*t)->right), v);
            (*t)->right->parent = *t;
        }
    }
}**/

/**void bstree_add(ptrBinarySearchTree *t, int v) {
    ptrBinarySearchTree *cur = t;
    BinarySearchTree *parcours = NULL;
    while(*cur) {
        parcours = *cur;
        if((*cur)->root != v)
            cur=(((*cur)->root) > v) ? &((*cur)->left) : &((*cur)->right);
    }
    *cur = bstree_cons(NULL,NULL,v);
    (*cur)->parent = parcours;
}**/

void bstree_add(ptrBinarySearchTree *t, int v) {
	ptrBinarySearchTree *current = t;
	ptrBinarySearchTree parent = NULL;
	while (*current != NULL) {
		if ((*current)->root == v)
			return;
	parent = *current;
	current = ( ((*current)->root > v) ? &((*current)->left) : &((*current)->right));
	}
	*current = bstree_cons(NULL, NULL, v, parent);
	parent = fixredblack_insert(*current);
	if (parent->parent == NULL)
		*t = parent;
}

bool bstree_search(const BinarySearchTree *t, int v) {
    if(bstree_empty(t))
        return false;
    else if (bstree_root(t) == v)
        return true;
    else{
        while(!bstree_empty(t)){
		    if (bstree_root(t) == v)
			    return true;
		    if (v < bstree_root(t))
			    t = bstree_left(t);
		    else
			    t = bstree_right(t);
		    }
		return false;
    }
}

BinarySearchTree *bstree_successor(const BinarySearchTree *x) {
    assert(!bstree_empty(x));
    BinarySearchTree *y = bstree_right(x);
    if(y) while(bstree_left(y)) y = bstree_left(y);
    else{
    	y = bstree_parent(x);
    	while(y && (x == bstree_right(y))){
    		x = y;
    		y = bstree_parent(y);
    	}
    }
    return y;
}

BinarySearchTree *bstree_predecessor(const BinarySearchTree *x) {
    assert(!bstree_empty(x));
    BinarySearchTree *y = bstree_left(x);
    if(y){
        while(bstree_right(y))
            y = bstree_right(y);
    }
    else{
    	y=bstree_parent(x);
    	while(y && (x == bstree_left(y))){
    		x = y;
    		y = bstree_parent(y);
    	}
    }
    return y;
}

void bstree_swap_nodes(ptrBinarySearchTree *tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(*tree) && !bstree_empty(from) && !bstree_empty(to));
    ptrBinarySearchTree *cur=tree;

    (*cur)->root=(from)->root;

    if((from)->parent)
		(*cur)->parent=(from)->parent;
    else{
        (*cur)->parent=NULL;
        ((*tree))->root=(to)->root;
    }

    if((from)->left)
		(*cur)->left=(from)->left;
    else
        (*cur)->left=NULL;

    if((from)->right)
		(*cur)->right=(from)->right;
	else
        (*cur)->right=NULL;

    (from)->root=(to)->root;

	if((to)->parent)
		(from)->parent=(to)->parent;
	else{
		(from)->parent=NULL;
		((*tree))->root=(*cur)->root;
    }

	if((to)->right)
	    (from)->right=(to)->right;
	else
		(from)->right=NULL;

	if((to)->left)
	    (from)->left=(to)->left;
	else
		(from)->left=NULL;

	(to)->parent=(*cur)->parent;
	(to)->left=(*cur)->left;
	(to)->right=(*cur)->right;
	(to)->root=(*cur)->root;
}

// t -> the tree to remove from, current -> the node to remove
/**void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current){
	ptrBinarySearchTree substitute;
	if (current->left == current->right) {
		substitute = NULL;
	} else if (! current->left) {
		substitute = current->right;
	} else if (! current->right) {
	substitute = current->left;
	} else {
	ptrBinarySearchTree leaf = bstree_successor(current);
	current->root = leaf->root;
	current = leaf;
	substitute = current->right;
	}
	if (substitute != NULL)
		substitute->parent = current->parent;
	if ( ! current->parent )
	*t = substitute;
	else if (current->parent->left == current)
		current->parent->left = substitute;
	else
		current->parent->right = substitute;
	if (current->color == black) {
		if ( (substitute == NULL) || (substitute->color == black) ) {
			ptrBinarySearchTree subtreeroot = fixredblack_remove(current->parent,substitute);
		if (subtreeroot->parent == NULL)
			*t = subtreeroot;
		} else {
		substitute->color = black;
		}
	}
	free(current);
}**/

void bstree_remove_node(ptrBinarySearchTree *t, ptrBinarySearchTree current) {
    ptrBinarySearchTree *m;
    BinarySearchTree *leaf;
    if(!(current)->parent) m = t;

	else if(current->parent->left == current) m = &(current->parent->left);

	else m = &(current->parent->right);

	if(current->left == current->right){
		*m = bstree_create();
		free(current);
	}
	else if(!current->left){
		*m = current->right;
		current->right->parent = current->parent;
		free(current);
	}else if (!current->right){
		*m = current->left;
		current->left->parent = current->parent;
		free(current);
	}
	else {
		leaf = bstree_successor(current);
		if(leaf){
			(current)->root=leaf->root;
			bstree_remove_node(t, leaf);
		}
	}
}

void bstree_remove(ptrBinarySearchTree *t, int v) {
    BinarySearchTree *current = *t;
	while(!bstree_empty(current)) {
		if(bstree_root(current)==v)
			break;
		current=(bstree_root(current)>v?bstree_left(current):bstree_right(current));
	}
	if(!bstree_empty(current)) {
		bstree_remove_node(t,current);
	}
}

/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    f(t,userData);
    if(!bstree_empty(t->left))
        bstree_depth_prefix(t->left,f,userData);
    if(!bstree_empty(t->right))
        bstree_depth_prefix(t->right,f,userData);
}

void bstree_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if(!bstree_empty(t->left))
        bstree_depth_infix(t->left,f,userData);
    f(t,userData);
    if(!bstree_empty(t->right))
        bstree_depth_infix(t->right,f,userData);
}

void bstree_depth_postfix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    if(!bstree_empty(t->left))
        bstree_depth_postfix(t->left,f,userData);
    if(!bstree_empty(t->right))
        bstree_depth_postfix(t->right,f,userData);
    f(t,userData);
}

void bstree_iterative_depth_infix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
	BinarySearchTree *current=(BinarySearchTree *)t;
    if(!bstree_empty(current)){
	    while(!bstree_empty(bstree_left(current)))
		    current = bstree_left(current);
	    while(!bstree_empty(current)) {
		    f(current, userData);
		    current = bstree_successor(current);
	    }
    }
}

void bstree_iterative_breadth_prefix(const BinarySearchTree *t, OperateFunctor f, void *userData) {
    Queue *q = createQueue();
    q = queuePush(q,t);
    while (!queueEmpty(q)){
		t = queueTop(q);
		q = queuePop(q);
		f(t,userData);
		if (!bstree_empty(bstree_left(t)))
			q = queuePush(q,bstree_left(t));
		if( !bstree_empty(bstree_right(t)))
			q = queuePush(q,bstree_right(t));
    }
}

/*------------------------  BSTreeIterator  -----------------------------*/

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree *collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree *(*begin)(const BinarySearchTree *);
    /* the current element pointed by the iterator */
    const BinarySearchTree *current;
    /* function that goes to the next element according to the iterator direction */
    BinarySearchTree *(*next)(const BinarySearchTree *);
};

/* minimum element of the collection */
const BinarySearchTree *goto_min(const BinarySearchTree *e) {
	while(bstree_left(e))
		e = bstree_left(e);
	return e;
}

/* maximum element of the collection */
const BinarySearchTree *goto_max(const BinarySearchTree *e) {
	while(bstree_right(e))
		e = bstree_right(e);
	return e;
}

/* constructor */
BSTreeIterator *bstree_iterator_create(const BinarySearchTree *collection, IteratorDirection direction) {
	BSTreeIterator *it = malloc(sizeof(struct _BSTreeIterator));
    it->collection = collection;
	if(direction == forward){
		it->begin = goto_min;
		it->next = bstree_successor;
	}
	else{
		it->begin = goto_max;
		it->next = bstree_predecessor;
	}
	it->current = it->begin(it->collection);
	return it;
}

/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator *i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator *bstree_iterator_begin(BSTreeIterator *i) {
    i->current = i->begin(i->collection);
    return i;
}

bool bstree_iterator_end(const BSTreeIterator *i) {
    return i->current == NULL;
}

BSTreeIterator *bstree_iterator_next(BSTreeIterator *i) {
    i->current = i->next(i->current);
    return i;
}

const BinarySearchTree *bstree_iterator_value(const BSTreeIterator *i) {
    return i->current;
}

void printNode(const BinarySearchTree *n, void *out){
	FILE *file = (FILE *) out;

	if (n->color == red)
    	fprintf(file, "\tn%d [style=filled, fillcolor=red,label=\"{{<parent>}|%d|{<left>|<right>}}\"];\n", bstree_root(n), bstree_root(n));
	else
		fprintf(file, "\tn%d [style=filled, fillcolor=grey,label=\"{{<parent>}|%d|{<left>|<right>}}\"];\n", bstree_root(n), bstree_root(n));

    if (bstree_left(n)) {
        fprintf(file, "\tn%d:left:c -> n%d:parent:c [headclip=false, tailclip=false]\n",bstree_root(n), bstree_root(bstree_left(n)));
    } else {
        fprintf(file, "\tlnil%d [style=filled, fillcolor=black, label=\"NIL\"];\n", bstree_root(n));
        fprintf(file, "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(n));
    }
    if (bstree_right(n)) {
        fprintf(file, "\tn%d:right:c -> n%d:parent:c [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(bstree_right(n)));
    } else {
        fprintf(file, "\trnil%d [style=filled, fillcolor=black, label=\"NIL\"];\n", bstree_root(n));
        fprintf(file, "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n", bstree_root(n), bstree_root(n));
    }
}

void rbtree_export_dot(const BinarySearchTree *t, FILE * file){
	fprintf(file, "digraph RedBlackTree {\n\tgraph [ranksep=0.5];\n\tnode [shape = record];\n\n" ) ;
	bstree_iterative_depth_infix(t, printNode, file );
	fprintf(file, "\n}\n");
}

void leftrotate(BinarySearchTree *x){
	BinarySearchTree *y=malloc(sizeof(BinarySearchTree));
	y=x->right;
	x->right=y->left;
	if(y->left!=NULL)
		y->left->parent=x;
	y->parent=x->parent;
	if(x->parent!=NULL){
		if(x==x->parent->right)
			x->parent->right=y;
		else
			x->parent->left=y;
	}
	y->left=x;
	x->parent=y;
}

void rightrotate(BinarySearchTree *y){
	BinarySearchTree *x=malloc(sizeof(BinarySearchTree));
	x=y->left;
	y->left=x->right;
	if(x->right!=NULL)
		x->right->parent=y;
	x->parent=y->parent;
	if(y->parent!=NULL){
		if(y==y->parent->left)
			y->parent->left=x;
		else
			y->parent->right=x;
	}
	x->right=y;
	y->parent=x;
}

void testrotateleft(BinarySearchTree *t){
	leftrotate(t);
}

void testrotateright(BinarySearchTree *t){
	rightrotate(t);
}

ptrBinarySearchTree grandparent(ptrBinarySearchTree n){
	return n->parent->parent;
}

ptrBinarySearchTree uncle(ptrBinarySearchTree n){
	if(n->parent==grandparent(n)->left)
		return grandparent(n)->right;
	return grandparent(n)->left;
}

ptrBinarySearchTree fixredblack_insert_case2_left(ptrBinarySearchTree x){
	ptrBinarySearchTree p = bstree_parent(x);
	ptrBinarySearchTree gp = grandparent(x);
	if (x == p->left) {
		rightrotate(gp);
		p->color = black;
		gp->color = red;
		return p;
	} else {
		leftrotate(p);
		rightrotate(gp);
		x->color = black;
		gp->color = red;
		return x;
	}
}

ptrBinarySearchTree fixredblack_insert_case2_right(ptrBinarySearchTree x){
	ptrBinarySearchTree p = bstree_parent(x);
	ptrBinarySearchTree gp = grandparent(x);
	if (x == p->right) {
		leftrotate(gp);
		p->color = black;
		gp->color = red;
		return p;
	} else {
		rightrotate(p);
		leftrotate(gp);
		x->color = black;
		gp->color = red;
		return x;
	}
}

ptrBinarySearchTree fixredblack_insert_case2(ptrBinarySearchTree x){
	ptrBinarySearchTree p=bstree_parent(x);
	ptrBinarySearchTree gp=grandparent(x);
	if(gp->left == p)
		return fixredblack_insert_case2_left(x);
	else
		return fixredblack_insert_case2_right(x);
}

ptrBinarySearchTree fixredblack_insert_case1(ptrBinarySearchTree x){
	ptrBinarySearchTree f=uncle(x);
	if((f!=NULL) && (f->color==red)){
		ptrBinarySearchTree gp=grandparent(x);
		x->parent->color=black;
		f->color=black;
		gp->color=red;
		return fixredblack_insert(gp);
	}
	else return fixredblack_insert_case2(x);
}

ptrBinarySearchTree fixredblack_insert_case0(ptrBinarySearchTree x){
	ptrBinarySearchTree p=x->parent;
	if(p->parent==NULL)
		p->color=black;
	else
		x=fixredblack_insert_case1(x);
	return x;
}

ptrBinarySearchTree fixredblack_insert(ptrBinarySearchTree x){
	if(x->parent==NULL)
		x->color=black;
	else if(x->parent->color==red)
		x=fixredblack_insert_case0(x);
	return x;
}

ptrBinarySearchTree fixredblack_remove(ptrBinarySearchTree p, ptrBinarySearchTree x){
	if (p == NULL) {
		return x;
	} else {
		ptrBinarySearchTree b = (p->left == x) ? (p->right) : (p->left);
		if ( b->color==black)
			return fixredblack_remove_case1(p, x);
		else
			return fixredblack_remove_case2(p, x);
	}
}

ptrBinarySearchTree fixredblack_remove_case1_left(ptrBinarySearchTree p, ptrBinarySearchTree x) {
	(void)x;
	ptrBinarySearchTree b = p->right;
	if ( ( (b->left == NULL) || (b->left->color == black) ) && ( (b->right == NULL) || (b->right->color == black) ) ) {
		b->color = red;
		if ( p->color == red) {
			p->color = black;
			return p;
		} else {
			return fixredblack_remove(p->parent, p);
		}
	} else if ((b->right != NULL) && (b->right->color == red)) {
		leftrotate(p);
		b->color = p->color;
		p->color = black;
		b->right->color = black;
		return b;
	} else {
		b->left->color = black;
		b->color = red;
		rightrotate(b);
		leftrotate(p);
		b->color = p->color;
		return b->parent;
	}
}

ptrBinarySearchTree fixredblack_remove_case1_right(ptrBinarySearchTree p, ptrBinarySearchTree x) {
	ptrBinarySearchTree b = p->left;
	(void)x;
	if ( ( (b->right == NULL) || (b->right->color == black) ) && ( (b->left == NULL) || (b->left->color == black) ) ) {
		printf("if\n");
		b->color = red;
		if ( p->color == red) {
			p->color = black;
				return p;
		} else {
			return fixredblack_remove(p->parent, p);
		}
	} else if ((b->left != NULL) && (b->left->color == red)) {
		printf("else if\n");
		rightrotate(p);
		b->color = p->color;
		p->color = black;
		b->right->color = black;
		return b;
	} else {
		printf("else\n");
		b->right->color = black;
		b->color = red;
		leftrotate(b);
		rightrotate(p);
		b->color = p->color;
		return b->parent;
	}
}

ptrBinarySearchTree fixredblack_remove_case1(ptrBinarySearchTree p, ptrBinarySearchTree x) {
	if (x == p->left)
		return fixredblack_remove_case1_left(p, x);
	else
		return fixredblack_remove_case1_right(p, x);
}

ptrBinarySearchTree fixredblack_remove_case2_left(ptrBinarySearchTree p, ptrBinarySearchTree x){
	ptrBinarySearchTree f = p->right;
	leftrotate(p);
	p->color = red;
	f->color = black;
	p = fixredblack_remove_case1_left(p, x);
	return f;
}

ptrBinarySearchTree fixredblack_remove_case2_right(ptrBinarySearchTree p, ptrBinarySearchTree x){
	ptrBinarySearchTree f = p->left;
	rightrotate(p);
	p->color = red;
	f->color = black;
	p = fixredblack_remove_case1_right(p, x);
	return f;
}

ptrBinarySearchTree fixredblack_remove_case2(ptrBinarySearchTree p, ptrBinarySearchTree x) {
	if (x == p->left)
		return fixredblack_remove_case2_left(p, x);
	else
		return fixredblack_remove_case2_right(p, x);
}
