#include <stdlib.h>



struct node
{
    int key;
    int size;
    node *left;
    node *right;
    node(int k)
    {
        key = k;
        left = right = 0;
        size = 1;
    }


    ~node()
    {
        if(left) delete left;
        if(right) delete right;
    }
};


// getsize(0) any node
// return size of any node
int getsize(node *p)
{
    if(!p) return 0;

    return p->size;
}

// return the size of node
void fixsize(node *p)
{
    p->size = getsize(p->left) + getsize(p->right) + 1;
}


// find(1) the node to be found
// find(2) key
// return NULL or the pointer to the node that has been founde
node *find(node *p, int k)
{
    if(!p) return 0;

    if(k == p->key)
      return p;

    if(k < p->key)
      return find(p->left, k);
    else
      return find(p->right, k);
}

/*             5       ->       5*/
         //3(p)              2(q)
     //2(q)  c             a    3(p)
   //a   b                     b   c
node *rotateright(node *p)
{
    node *q = p->left;
    if(!q) return p;
    p->left = q->right;
    q->right = p;
    //q->size = p->size;
    fixsize(p);
    fixsize(q);
    return q;
}


/*             5           ->       5*/
         //      7(p)                    8(q)
     //        a     8(q)             7(p)   c
   //              b     c          a    b
node *rotateleft(node *p)
{
    node *q = p->right;
    if(!q) return 0;
    p->right = q->left;
    q->left = p;
    //q->size = p->size;
    fixsize(p);
    fixsize(q);
    return q;
}

// for rotate...
node *insertroot(node *p, int k)
{
    if(!p) return new node(k);

    if(k < p->key)
    {
        p->left = insertroot(p->left, k);
        return rotateright(p);
    }
    else
    {
        p->right = insertroot(p->right, k);
        return rotateleft(p);
    }
}

    /* 5(1)       ->       5(2)     ->       5(3)*/
                       /*4(1)             4(1)     8(1) return this*/
    /*                               ->        5(3)          or->   5(2) there is 2!
     *                                    4(2)                         3(2) return this
     *                                 3(1)                          4(1)*/
// insert(1) root node(relatively)
// insert(2) key
// return a pointer of new node
// NOTE: p->size will not be updated when func <insertroot> is called.
node *insert(node *p, int k)
{
    if(!p) return new node(k);

    if(rand() % (p->size + 1) == 0)
      return insertroot(p, k);

    if(p->key > k)
      p->left = insert(p->left, k);
    else
      p->right = insert(p->right, k);

    fixsize(p); // left's size + right's size + 1

    return p;
}


// joining two trees
// join(1) the left subtree of the rootnode(relative)
// join(2) the right subtree of the rootnode(relative)
// return a pointer to node that the left or the right
node* join(node* p, node* q)
{
    if(!p) return q;
    if(!q) return p;
    if(rand()%(p->size+q->size) < p->size)
    {
        p->right = join(p->right,q);
        fixsize(p);
        return p;
    }
    else
    {
        q->left = join(p,q->left);
        fixsize(q);
        return q;
    }
}

// deleting from p tree the first found node with k key
node* remove(node* p, int k)
{
    if( !p ) return p;
    if( p->key==k )
    {
        node* q = join(p->left,p->right);

        p->left = 0;  // prevent recursion...
        p->right = 0; // prevent recursion...

        delete p;

        return q;
    }
    else if( k<p->key )
        p->left = remove(p->left,k);
    else
        p->right = remove(p->right,k);
    return p;
}


int main()
{
    return 0;
}
