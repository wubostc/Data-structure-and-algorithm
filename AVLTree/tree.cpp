

/*h height is in the range from log2(n+1) to 1.44log2(n+2)-0.328*/
//(log(n + 1) / log(2)) to (1.44 * log(n + 2) / log(2) - 0.328)
//when n=10^9 the tree height will not exceed h=44,
//2^40=10.9GB more than 10GB of memort allocated for nodes storage

#include <functional>
#include <iostream>
#define DEBUG
#include "../../comm.h"
using namespace std;

class viewer;

struct node
{
    friend class viewer;

    int key;
    unsigned char height;
    node *left;
    node *right;

    node(int k) :
        key(k),
        height(1),
        left(nullptr),
        right(nullptr) {}

    ~node()
    {
        if(left)
            delete left;
        if(right)
            delete right;
        LOGI(~node r=%d, key);
    }

    void release()
    {
        left = nullptr;
        right = nullptr;
        delete this;
    }

    //node(const node &) = delete;
    const node &operator=(const node &) = delete;

    inline unsigned char geth(node *);

    inline int balance_factor(node *p);

    inline void fixheight(node *p);

    inline node *rotateright(node *p);
    inline node *rotateleft(node *p);

    node *balance(node *p);

    node *remove(node *p, int k);

    node *remove_min(node *p);

    node *insert(node *,int k);

    inline node *find_min(node *p);
};

inline unsigned char node::geth(node *p)
{
    return p ? p->height : 0;
}

// the height difference between the left subtree and right subtree
// return right - left
inline int node::balance_factor(node *p)
{
    return geth(p->right) - geth(p->left);
}

// fixheight(1) the height of a pointer to node will assigned as max + 1
// return maxinum + 1 of both (p->left->height  p->right->height)
inline void node::fixheight(node *p)
{
    unsigned char hl = geth(p->left);
    unsigned char hr = geth(p->right);

    p->height = (hl > hr ? hl : hr) + 1;
}

/*             5       ->         5
 *           /                   /
 *          3(p)               2(q)
 *        /    \               /   \
 *      2(q)    c             a     3(p)
 *     /  \                        /    \
 *    a   b                       b      c
 *-------------------------------------------
 *           5        ->          5
 *            \                    \
 *             8                    7
 *            / \                  /  \
 *           7   c                a    8
 *          / \                       / \
 *         a   b                     b   c
 *                                             */
inline node *node::rotateright(node *p)
{
    node *q = p->left;
    p->left = q->right;
    q->right = p;
    fixheight(p);
    fixheight(q);
    return q;
}


/*        5           ->           5
 *          \                       \
 *          7(p)                   8(q)
 *         /   \                  /   \
 *        a     8(q)             7(p)  c
 *             /  \             /  \
 *            b    c           a    b
 *---------------------------------------------
 *        5         ->          5
 *       /                     /
 *     3                      4
 *    /  \                   / \
 *   a    4                 3   c
 *       / \               / \
 *      b   c              a  b                 */
inline node *node::rotateleft(node *p)
{
    node *q = p->right;
    p->right = q->left;
    q->left = p;
    fixheight(p);
    fixheight(q);
    return q;
}

// balance(1) the root node
// return the root node after balance
node *node::balance(node *p)
{
    fixheight(p);

    //   p->right->height - p->left->height
    if(balance_factor(p) == 2)
    {
        LOG(INFO, r-l=2 k=%d,p->key);

        //    15(4)(p)     ->     15(4)(p)
        //   /     \             /       \
        //  10(1)   20(3)        10(1)   18(2)
        //          /                   /    \
        //         18(2)               17(1)  20(1)
        //        /
        //       17(1)
        //-------------------------------------------
        //    15(4)(p)     ->       15(4)(p)
        //   /      \              /       \
        //  10(1)    20(3)        10(1)     18(3)
        //          /                        \
        //          18(2)                    20(2)
        //             \                    /
        //              19(1)             19(1)
        //-------------------------------------------
        //     15(4)(p)              15(4)(p)
        //     /      \             /       \
        //   10(1)    20(3)        10(1)    18(3)
        //            /   \                /   \
        //           18(2) 25(1)         17(1)  20(2)
        //           /                           \
        //          17(1)                         25(1)
        //-------------------------------------------
        //     15(4)(p)              15(4)(p)
        //     /      \             /        \
        //   10(1)    20(3)        10(1)     18(3)
        //            /   \                 /   \
        //           18(2) 25(1)          17(1)  20(2)
        //            \                         /   \
        //            19(1)                  19(1)  25(1)
        if(balance_factor(p->right) < 0)
        {
            p->right = rotateright(p->right);
            LOG(INFO, r-l<0 k=%d, p->right->key);
        }

        //   15(4)(p)         ->        15(3)(p)
        //  /       \                  /       \
        //  10(1)   18(2)            10(1)   18(2)
        //         /    \                   /    \
        //        17(1)  20(1)            17(1)  20(1)
        if(balance_factor(p) == 1)
        {
            fixheight(p);
            return p;
        }

        //     15(4)(p)      ->           18(3)(q)
        //    /       \                 /      \
        //   10(1)     18(3)          15(2)     20(2)
        //              \            /         /
        //              20(2)       10(1)      19(1)
        //             /
        //           19(1)
        //-------------------------------------------
        //      15(4)(p)      ->           18(3)(q)
        //     /       \                  /      \
        //    10(1)    18(3)             15(2)    20(2)
        //            /   \            /    \      \
        //          17(1)  20(2)      10(1) 17(1)   25(1)
        //                  \
        //                   25(1)
        //-------------------------------------------
        //     15(4)(p)       ->       18(3)(q)
        //     /      \              /        \
        //   10(1)    18(3)        15(2)      20(2)
        //            /   \        /    \         \
        //           17(1) 20(2)  10(1) 17(1)      25(2)
        //                  \
        //                   25(1)
        return rotateleft(p);
    }

    //   p->right->height - p->left->height
    if(balance_factor(p) == -2)
    {
        LOG(INFO, r-l=-2 k=%d,p->key);

        //     15(4)(p)     ->     15(4)(p)
        //     /     \             /       \
        //   10(3)   20(1)        12(2)   20(1)
        //    \                   /  \
        //     12(2)            10(1) 13(1)
        //     \
        //      13(1)
        //-------------------------------------------
        //    15(4)(p)     ->       15(4)(p)
        //   /      \               /      \
        //  10(3)    20(1)        13(3)    20(1)
        //    \                    /
        //    13(2)              10(2)
        //     /                  \
        //    11(1)               11(1)
        //-------------------------------------------
        //     15(4)(p)      ->      15(4)(p)
        //     /      \             /       \
        //   10(3)    20(1)        13(1)    20(1)
        //   /   \                 /    \
        //  8(1)  13(2)           10(2)  14(1)
        //         \             /
        //          14(1)        8(1)
        //-------------------------------------------
        //     15(4)(p)    ->         15(4)(p)
        //     /      \             /        \
        //   10(3)    20(1)        14(1)     20(1)
        //  /    \                /
        // 12(1)  14(2)          10(2)
        //       /              /   \
        //      13(1)          12(1) 13(1)
        if(balance_factor(p->left) > 0)
        {
            p->left = rotateleft(p->left);
            LOGI(r-l>0 k=%d, p->left->key);
        }

        // omit...
        if(balance_factor(p) == 1)
        {
            fixheight(p);
            return p;
        }

        // omit...
        return rotateright(p);
    }

    return p; // nothing
}

// insert(1) the root node
// insert(2) the key of globally unique
// return the root node
node *node::insert(node *p, int k)
{
    if(!p) return new node(k);

    if(k < p->key)
      p->left = insert(p->left, k);
    else if(k > p->key)
      p->right = insert(p->right, k);
    else
      return p;

    return balance(p);
}



/*
 *
 *                    26(p)
 *                 /          \
 *            20(q)             32(r)
 *          /   \              /     \
 *       17     22         30(min)     40
 *     /    \
 *  16     19
 *---------------------------------------
 *                    26(p)
 *                 /          \
 *            20(q)             32(r)
 *          /   \              /     \
 *       17     22          30(min)    40
 *     /    \                  \
 *  16     19                   31
 *                                          */
inline node *node::find_min(node *p)
{
    return p->left ? find_min(p->left) : p;
}

/*
 *
 *             26(p)                        26(p)
 *            /      \                    /       \
 *         20(q)      32(r)             20(q)     32(r)
 *       /   \       /     \           /   \      /     \
 *     17     22   30(min)  40       17    22   null    40
 *    /  \                          /  \
 *  16    19                       16   19
 *------------------------------------------------------------
 *             26(p)                     26(p)
 *            /      \                 /       \
 *         20(q)      32(r)         20(q)      32(r)
 *       /   \        /    \       /   \       /     \
 *     17     22   30(min)  40    17    22    31     40
 *    /   \          \           /  \
 *  16     19         31        16  19
 *                                                            */
node *node::remove_min(node *p)
{
    if(p->left == nullptr)
      return p->right;
    p->left = remove_min(p->left);
    return balance(p);
}

node *node::remove(node *p, int k)
{
    if(!p) return nullptr;

    // search... until the key is found
    if(k < p->key)
      p->left = remove(p->left, k);
    else if(k > p->key)
      p->right = remove(p->right, k);
    else // the key is found
    {
        node *l = p->left;
        node *r = p->right;
        p->release(); // safe-release

/*        10(3)
 *      /        \
 *    5(1)        22(2)(del)
 *                /        \
 *              20(1)(q)    null(0)(r)
 *                                              */
        if(!r) return l;
        if(!l) return r;

/*
 *        50(4)(del)    ->            60(2)
 *      /         \                 /     \
 *    40(3)(q)     70(2)(r)       40(3)    70(2)
 *   /  \          /              /  \
 *  20  45        60(1)(min)      20  45
 *      /                             /
 *     44                            44
 *                                                          */
        node *min = find_min(r);   // search the minimal key on the right subtree
        min->right = remove_min(r);// balance in remove_min...
        min->left = l;


/*        60(ret)   ->     60(ret)       ->      45        -> AVLTREE
 *      /     \           /      \            /      \
 *    40       70        45(3)    70(1)     40         60(ret)
 *   /  \              /                   /  \         \
 *  20   45           40                 20   44        70
 *       /           /  \
 *      44          20  44
 *                                                                 */
        return balance(min);
    }


    return balance(p);
}


class viewer
{
typedef node           map_t;
typedef const map_t    const_map_t;
typedef map_t*         ptr_map_t;
typedef const map_t*   const_ptr_map_t;
public:

    ostream &operator<<(ptr_map_t m)
    {
        //中序遍历...
        function<ostream &(ostream &, ptr_map_t)> func =
            [&](ostream &os, ptr_map_t m)->ostream &{

                if(m->left) func(os, m->left);
                os << m->key << "[" << (int)m->height << "]" << ',';
                if(m->right)func(os, m->right);
                return os;
            };

        return func(cout, m);
    }

};


#include <memory>
int main()
{
    srand(time(0));

    node *root = new node(50), *p;
    viewer v;
    for(int i = 0; i < 15; ++i)
    {
        cout << "insert\n";
        int k = rand()%100+1;
        LOG(INFO, i=%d k=%d, i, k);
        root = root->insert(root,k);
        v << root << endl;
        if(rand()%3 == 0)
        {
            if(p = root->remove(root, k)) //!= nullptr
            {
                //\cout << "rm\n";
                root = p;
                v << root << endl;
            }
        }
    }


    delete root;

}


//references
//https://kukuruku.co/post/avl-trees/
