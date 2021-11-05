#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>

typedef int64_t ll;

class splayTree {
private:
    struct node;
public:
    void print();

    splayTree();

    explicit splayTree(node *oth);

    explicit splayTree(ll val);

    explicit splayTree(std::vector<ll> &);

    std::array<splayTree, 2> split_kth(ll count);

    std::array<splayTree, 3> cut_middle(ll l, ll r);

    void merge(splayTree other);

    void merge(ll val);

    void add(ll val);

    void set(ll val);

    void next();

    void prev();

    static node *lastInverse(node *);

    static node *lastRevInverse(node *);

    static node *rightest(node *);

    ll getSize();

    ll getSumm();

private:

    struct node {
        node *l = nullptr;
        node *r = nullptr;
        node *p = nullptr;
        ll val; //
        ll sz = 1;
        ll mx; //
        ll mn; //
        ll summ; // TODO add values
        ll settedValue = 0; //
        ll addedValue = 0; //
        bool added = false;
        bool setted = false;
        bool reversed = false;
        bool sorted = true;
        bool revSorted = true;
        ll rValue;
        ll lValue;

        node(ll value) : rValue(value), lValue(value), val(value), summ(value), mn(value), mx(value) {};
    };

    void reverse();

    static ll getSize(node *);

    static void reverse(node *);

    static ll getSumm(node *);

    static void setParent(node *, node *);

    static void keepParent(node *);

    static void rotate(node *, node *);

    static void update(node *);

    static void push(node *);

    static ll getMax(node *);

    static ll getMin(node *);

    static bool getSorted(node *);

    static bool getRevSorted(node *);

    static ll getRValue(node *);

    static ll getLValue(node *);

    static node *splay(node *);

    static node *findKth(node *root, ll cnt);

    static node *findLastMore(node *root, ll val);

    static node *findLastLess(node *, ll val);

    static void print(node *);

    node *root;
};


ll splayTree::getSumm(splayTree::node *root) {
    if (root == nullptr)
        return 0;
    if (root->setted) {
        return root->sz * root->settedValue;
    }
    if (root->added) {
        return root->summ + root->sz * root->addedValue;
    }
    return root->summ;
}

ll splayTree::getSize(splayTree::node *root) {
    if (root == nullptr)
        return 0;
    return root->sz;
}

splayTree::splayTree() {
    root = nullptr;
}

splayTree::splayTree(ll val) {
    root = new node(val);
}

splayTree::splayTree(std::vector<ll> &values) {
    root = nullptr;
    for (ll i:values) {
        merge(i);
    }
}

void splayTree::setParent(splayTree::node *c, splayTree::node *p) {
    if (c != nullptr)
        c->p = p;
}

void splayTree::keepParent(splayTree::node *root) {
    setParent(root->l, root);
    setParent(root->r, root);
}

void splayTree::rotate(splayTree::node *p, splayTree::node *c) {
    push(p);
    push(c);
    node *gp = p->p;
    if (gp != nullptr) {
        if (gp->l == p) {
            gp->l = c;
            update(gp);
        } else {
            gp->r = c;
            update(gp);
        }
    }
    if (p->l == c) {
        p->l = c->r;
        update(p);
        c->r = p;
        update(c);
    } else {
        p->r = c->l;
        update(p);
        c->l = p;
        update(c);
    }

    keepParent(c);
    keepParent(p);
    c->p = gp;
}

splayTree::node *splayTree::splay(node *root) {
    if (root->p == nullptr)
        return root;
    node *p = root->p;
    node *gp = p->p;
    if (gp == nullptr) {
        rotate(p, root);
        return root;
    } else {
        bool zigzig = (gp->l == p) == (p->l == root);
        if (zigzig) {
            rotate(gp, p);
            rotate(p, root);
        } else {
            rotate(p, root);
            rotate(gp, root);
        }
    }
    return splay(root);
}

splayTree::node *splayTree::findKth(splayTree::node *root, ll cnt) {
    push(root);
    if (getSize(root->l) == cnt) {
        return splay(root);
    }
    if (getSize(root->l) > cnt)
        return findKth(root->l, cnt);
    else
        return findKth(root->r, cnt - getSize(root->l) - 1);
}

std::array<splayTree, 2> splayTree::split_kth(ll count) {
    if (count <= 0)
        return {splayTree(nullptr), *this};
    if (count >= getSize())
        return {*this, splayTree(nullptr)};
    auto nr = splay(findKth(root, count));
    push(nr);
    auto l = nr->l;
    l->p = nullptr;
    nr->l = nullptr;
    update(nr);
    return {splayTree(l), splayTree(nr)};
}

splayTree::splayTree(splayTree::node *oth) {
    root = oth;
}

std::array<splayTree, 3> splayTree::cut_middle(ll l, ll r) {
    auto l_mr = split_kth(l);
    auto m_r = l_mr[1].split_kth(r - l + 1);
    return {l_mr[0], m_r[0], m_r[1]};
}

ll splayTree::getSize() {
    return getSize(root);
}

ll splayTree::getSumm() {
    return getSumm(root);
}

void splayTree::merge(splayTree other) {
    push(root);
    push(other.root);
    if (root == nullptr) {
        root = other.root;
        return;
    }
    if (other.root == nullptr) {
        return;
    }
    root = splay(findKth(root, getSize() - 1));
    assert(root->r == nullptr);
    root->r = other.root;
    other.root->p = root;
    update(root);
}

void splayTree::push(node *root) {
    if (root == nullptr)
        return;
    if (root->setted) {
        root->setted = false;
        root->val = root->settedValue;
        root->mx = root->settedValue;
        root->mn = root->settedValue;
        root->summ = root->settedValue * root->sz;
        root->sorted = true;
        root->revSorted = true;
        root->lValue = root->settedValue;
        root->rValue = root->settedValue;
        if (root->l != nullptr) {
            root->l->setted = true;
            root->l->added = false;
            root->l->settedValue = root->settedValue;
        }
        if (root->r != nullptr) {
            root->r->setted = true;
            root->r->added = false;
            root->r->settedValue = root->settedValue;
        }
    }
    if (root->added) {
        root->added = false;
        root->val += root->addedValue;
        root->mx += root->addedValue;
        root->mn += root->addedValue;
        root->summ += root->addedValue * root->sz;
        root->lValue += root->addedValue;
        root->rValue += root->addedValue;
        if (root->l != nullptr) {
            if (root->l->added)
                root->l->addedValue += root->addedValue;
            if (root->l->setted)
                root->l->settedValue += root->addedValue;
            if (!root->l->setted && !root->l->added) {
                root->l->added = true;
                root->l->addedValue = root->addedValue;
            }
        }
        if (root->r != nullptr) {
            if (root->r->added)
                root->r->addedValue += root->addedValue;
            if (root->r->setted)
                root->r->settedValue += root->addedValue;
            if (!root->r->setted && !root->r->added) {
                root->r->added = true;
                root->r->addedValue = root->addedValue;
            }
        }
    }
    if (root->reversed) {
        root->reversed = false;
        std::swap(root->l, root->r);
        std::swap(root->sorted, root->revSorted);
        std::swap(root->lValue, root->rValue);
        if (root->l != nullptr)
            root->l->reversed ^= true;
        if (root->r != nullptr)
            root->r->reversed ^= true;
    }
}

void splayTree::update(splayTree::node *root) {
    if (root == nullptr)
        return;
    root->sz = getSize(root->l) + 1 + getSize(root->r);
    root->mx = std::max(std::max(getMax(root->l), root->val), getMax(root->r));
    root->mn = std::min(std::min(getMin(root->l), root->val), getMin(root->r));
    root->summ = root->val + getSumm(root->l) + getSumm(root->r);
    if (root->r == nullptr)
        root->rValue = root->val;
    else
        root->rValue = getRValue(root->r);
    if (root->l == nullptr)
        root->lValue = root->val;
    else
        root->lValue = getLValue(root->l);

        root->sorted = getSorted(root->l) && getMax(root->l) <= root->val && root->val <= getMin(root->r) &&
                           getSorted(root->r);

        root->revSorted = getRevSorted(root->l) && getMin(root->l) >= root->val && root->val >= getMax(root->r) &&
                              getRevSorted(root->r);

}

void splayTree::merge(ll val) {
    merge(splayTree(val));
}

void splayTree::add(ll val) {
    push(root);
    root->added = true;
    root->addedValue = val;
}

void splayTree::set(ll val) {
    push(root);
    root->setted = true;
    root->settedValue = val;
}

const ll inf = 1e9;

ll splayTree::getMax(splayTree::node *root) {
    if (root == nullptr)
        return -inf;
    if (root->added)
        return root->mx + root->addedValue;
    if (root->setted)
        return root->settedValue;
    return root->mx;
}

ll splayTree::getMin(splayTree::node *root) {
    if (root == nullptr)
        return inf;
    if (root->added)
        return root->mn + root->addedValue;
    if (root->setted)
        return root->settedValue;
    return root->mn;
}

bool splayTree::getSorted(splayTree::node *root) {
    if (root == nullptr)
        return true;

    return root->setted || !root->reversed && root->sorted || root->reversed && root->revSorted;
}

bool splayTree::getRevSorted(splayTree::node *root) {
    if (root == nullptr)
        return true;
    return root->setted ||  !root->reversed && root->revSorted || root->reversed && root->sorted;
}

void splayTree::next() {
    push(root);
    if (getSize() <= 1)
        return;
    if (getRevSorted(root)) {
        reverse();
        return;
    }
    node *ml = lastRevInverse(root);
    splay(ml);
    node *r = ml->r;
    if (ml->r != nullptr) {
        ml->r->p = nullptr;
        ml->r = nullptr;
        update(ml);
    }

    node *mr = findLastMore(r, ml->val);
    splay(mr);
    node *mr_l = mr->l;
    node *mr_r = mr->r;
    {
        if (mr_l != nullptr)
            mr_l->p = nullptr;
        if (mr_r != nullptr)
            mr_r->p = nullptr;
        mr->l = mr->r = nullptr;
        update(mr);
    }

    splayTree L(ml);
    auto ml_l_r = L.split_kth(L.getSize() - 1);

    splayTree LL(ml_l_r[0]);
    LL.merge(splayTree(mr));
    splayTree R(mr_l);
    R.merge(ml_l_r[1]);
    R.merge(splayTree(mr_r));
    R.reverse();
    LL.merge(R);
    root = LL.root;
}

void splayTree::prev() {
    push(root);
    if (getSize() <= 1)
        return;
    if (getSorted(root)) {
        reverse();
        return;
    }
    node *ml = lastInverse(root);
    splay(ml);
    node *r = ml->r;
    if (ml->r != nullptr) {
        ml->r->p = nullptr;
        ml->r = nullptr;
        update(ml);
    }

    node *mr = findLastLess(r, ml->val);
    splay(mr);
    node *mr_l = mr->l;
    node *mr_r = mr->r;
    {
        if (mr_l != nullptr)
            mr_l->p = nullptr;
        if (mr_r != nullptr)
            mr_r->p = nullptr;
        mr->l = mr->r = nullptr;
        update(mr);
    }

    splayTree L(ml);
    auto ml_l_r = L.split_kth(L.getSize() - 1);

    splayTree LL(ml_l_r[0]);
    LL.merge(splayTree(mr));
    splayTree R(mr_l);
    R.merge(ml_l_r[1]);
    R.merge(splayTree(mr_r));
    R.reverse();
    LL.merge(R);
    root = LL.root;
}

void splayTree::reverse() {
    if (root == nullptr)
        return;
    push(root);
    root->reversed = true;
}

splayTree::node *splayTree::lastInverse(splayTree::node *root) {
    push(root);
    if (!getSorted(root->r))
        return lastInverse(root->r);
    if (root->r != nullptr && getSorted(root->r) && root->val > getMin(root->r))
        return root;
    if (root->val < getRValue(root->l))
        return rightest(root->l);
    return lastInverse(root->l);
    // TODO
}

splayTree::node *splayTree::lastRevInverse(splayTree::node *root) {
    push(root);
    if (!getRevSorted(root->r))
        return lastRevInverse(root->r);
    if (root->r != nullptr && getRevSorted(root->r) && root->val < getMax(root->r))
        return root;
    if (root->val > getRValue(root->l))
        return rightest(root->l);
    return lastRevInverse(root->l);
    // TODO
}

void splayTree::reverse(splayTree::node *root) {
    if (root == nullptr)
        return;
    push(root);
    root->reversed = true;
}

splayTree::node *splayTree::findLastMore(splayTree::node *root, ll val) {
    push(root);
    if (getMax(root->r) > val)
        return findLastMore(root->r, val);
    if (getMax(root->r) <= val && root->val > val)
        return root;
    return findLastMore(root->l, val);
}

splayTree::node *splayTree::findLastLess(splayTree::node *root, ll val) {
    push(root);
    if (getMin(root->r) < val)
        return findLastLess(root->r, val);
    if (getMin(root->r) >= val && root->val < val)
        return root;
    return findLastLess(root->l, val);
}

void splayTree::print() {
    print(root);
    std::cout << '\n';
}

void splayTree::print(splayTree::node *root) {
    if (root == nullptr)
        return;
    push(root);
    print(root->l);
    std::cout << root->val << ' ';
    print(root->r);
}

ll splayTree::getRValue(splayTree::node *root) {
    if (root->setted)
        return root->settedValue;
    if (root->reversed) {
        if (root->added)
            return root->lValue + root->addedValue;
        else
            return root->lValue;
    }
    if (root->added)
        return root->rValue + root->addedValue;
    else
        return root->rValue;
}

ll splayTree::getLValue(splayTree::node *root) {
    if (root->setted)
        return root->settedValue;
    if (root->reversed) {
        if (root->added)
            return root->rValue + root->addedValue;
        else
            return root->rValue;
    }
    if (root->added)
        return root->lValue + root->addedValue;
    else
        return root->lValue;
}

splayTree::node *splayTree::rightest(node *root) {
    push(root);
    if (root->r != nullptr) // TODO while
        return rightest(root->r);
    return root;
}

class permutation {
public:
    explicit permutation(std::vector<ll> &values);

    ll getSumm(ll l, ll r);

    void insert(ll pos, ll value);

    void erase(ll pos);

    void set(ll l, ll r, ll val);

    void add(ll l, ll r, ll val);

    void next(ll l, ll r);

    void prev(ll l, ll r);

    void print();

private:
    splayTree tree;
};

ll permutation::getSumm(ll l, ll r) {
    auto res = tree.cut_middle(l, r);
    ll ans = res[1].getSumm();
    res[0].merge(res[1]);
    res[0].merge(res[2]);
    tree = res[0];
    return ans;
}

void permutation::insert(ll pos, ll value) {
    auto res = tree.split_kth(pos);
    res[0].merge(splayTree(value));
    res[0].merge(res[1]);
    tree = res[0];
}

void permutation::erase(ll pos) {
    auto l_mr = tree.split_kth(pos);
    auto m_r = l_mr[1].split_kth(1);
    l_mr[0].merge(m_r[1]);
    tree = l_mr[0];
}

void permutation::set(ll l, ll r, ll val) {
    auto res = tree.cut_middle(l, r);
    res[1].set(val);
    res[0].merge(res[1]);
    res[0].merge(res[2]);
    tree = res[0];
}

void permutation::add(ll l, ll r, ll val) {
    auto res = tree.cut_middle(l, r);
    res[1].add(val);
    res[0].merge(res[1]);
    res[0].merge(res[2]);
    tree = res[0];
}

void permutation::next(ll l, ll r) {
    auto res = tree.cut_middle(l, r);
    res[1].next();
    res[0].merge(res[1]);
    res[0].merge(res[2]);
    tree = res[0];
}

void permutation::prev(ll l, ll r) {
    auto res = tree.cut_middle(l, r);
    res[1].prev();
    res[0].merge(res[1]);
    res[0].merge(res[2]);
    tree = res[0];
}

permutation::permutation(std::vector<ll> &values):tree(values) {}

void permutation::print() {
    tree.print();
}


int32_t main() {
    ll n;
    std::cin >> n;
    std::vector<ll> data(n);
    for (ll &i:data)
        std::cin >> i;

    ll q;
    std::cin >> q;
    permutation current(data);
    ll Q = q;
    while (q--) {
        ll type;
        std::cin >> type;
        if (type == 1) {
            ll l, r;
            std::cin >> l >> r;
            std::cout << current.getSumm(l, r) << '\n';
        }
        if (type == 2) {
            ll x, pos;
            std::cin >> x >> pos;
            current.insert(pos, x);
        }
        if (type == 3) {
            ll pos;
            std::cin >> pos;
            current.erase(pos);
        }
        if (type == 4) {
            ll l, r, x;
            std::cin >> x >> l >> r;
            current.set(l, r, x);
        }
        if (type == 5) {
            ll l, r, x;
            std::cin >> x >> l >> r;
            current.add(l, r, x);
        }
        if (type == 6) {
            int l, r;
            std::cin >> l >> r;
            current.next(l, r);
        }
        if (type == 7) {
            int l, r;
            std::cin >> l >> r;
            current.prev(l, r);
        }
    }

    current.print();
    return 0;
}
