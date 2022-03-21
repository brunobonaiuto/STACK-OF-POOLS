/*
this program is a stack of pools, base on LIFO (first in last out) in which we have multiples stacks
storage in a vector, by the standar library std::vector
the vector created by adding elements on a stacks, and free_nodes wich are the avalible positions to add more elements
on the vector(stack)
the program contains the implementations of the functions push and pop
*/
#include <iostream>
#include <vector>
#include <exception> 
#include <algorithm>
#include <utility>

/*
the class iterator is develop below to work on the stack_pool class
*/
template <typename SP, typename N, typename T>
class _iterator {
    // Iterator for one stack from the pool
    SP* stackpool;
    N currentvalue; //stack_type

public:

    using value_type = T;
    using stack_type = N;
    using stack_pool = SP;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    // constructor of the iterator
    _iterator(stack_pool* _stackpool, stack_type _currentvalue) : stackpool{ _stackpool }, currentvalue{ _currentvalue } {}

    //DEREFERENCE OPERATOR
    reference operator*() const { return stackpool->value(currentvalue); }

    //PRE-INCREMENT
    _iterator& operator++() {
        currentvalue = stackpool->next(currentvalue);
        return *this;
    }

    //POST-INCREMENT
    _iterator operator++(int) {
        auto temp = *this;
        ++(*this); //xx
        return temp;
    }

    //EQUALITY
    friend bool operator==(const _iterator& x, const _iterator& y) {
        return x.currentvalue == y.currentvalue;
    }

    //NOT EQUALITY
    friend bool operator!=(const _iterator& x, const _iterator& y) {
        return !(x == y);
    }
};

/*
EmptyStack, exception for popping a stack which is empty
*/
struct EmptyStack : public std::exception {
    std::string message;
    EmptyStack(std::string message) : message{ message } {}
};

/*
NoHeadStack, exception when the user is popping a stack by no passing the head(last element added)
*/
struct NoHeadStack : public std::exception {
    std::string message;
    NoHeadStack(std::string message) : message{ message } {}
};

/*
the class for stack_pool is develop below
*/
template <typename T, typename N = std::size_t>
class stack_pool {

    struct node_t {
        T value;
        N next;
        bool last;

        template <typename O>
        node_t(O&& value, N next, bool last) : value{ std::forward<O>(value) }, next{ next }, last{ last } {}

    };

    using stack_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;
    std::vector<node_t> pool; //xx

    stack_type free_nodes{ end() }; // at the beginning, it is empty

    node_t& node(stack_type x) noexcept { return pool[x - 1]; }
    const node_t& node(stack_type x) const noexcept { return pool[x - 1]; }



public:
    /*
    stack_pool() is an object empty
    */
    stack_pool() = default;

    /*
    An explicit stac_pool by passing the capacity as n
    */
    explicit stack_pool(size_type n) { reserve(n); }

    using iterator = _iterator<stack_pool, N, const T>;
    using const_iterator = _iterator<stack_pool, N, const T>;

    iterator begin(stack_type x) noexcept { return iterator(this, x); }
    iterator end(stack_type) noexcept { return const_iterator(this, end()); }

    const_iterator begin(stack_type x) const noexcept { return const_iterator(this, x); }
    const_iterator end(stack_type) const noexcept { return const_iterator(this, end()); }

    const_iterator cbegin(stack_type x) const noexcept { return const_iterator(this, x); }
    const_iterator cend(stack_type) const noexcept { return const_iterator(this, end()); }

    /*
    Creating a new stack for the stack_pool
    this functions does not throw any exception
    */
    stack_type new_stack()const noexcept { return end(); }

    /*
    Increasing the capacity of the std::vector to a value grater or equal than n,
    otherwise it will storage a new capacity
    */
    void reserve(size_type n) { pool.reserve(n); }

    /*
    to obtain the capacity of std::vector
    */
    size_type capacity() const noexcept { return pool.capacity(); }

    /*
    this method will check if the stack is empty
    returns a boolean if the stack is empty or no
    */
    bool empty(stack_type x) const noexcept { return x == end(); }

    /*
    this method will return the end of the stack
    1-past-theLast element
    */
    stack_type end() const noexcept { return stack_type(0); }

    /*
    this method returns the value of the node
    */
    T& value(stack_type x) { return node(x).value; }
    const T& value(stack_type x) const { return node(x).value; }

    /*
    to obtain the index of the next node
    */
    stack_type& next(stack_type x) { return node(x).next; }
    const stack_type& next(stack_type x) const { return node(x).next; }

    /*
    to obtain the head of the stack
    */
    bool& last(stack_type x) { return node(x).last; }
    const bool& last(stack_type x) const { return node(x).last; }

    /*
    this method insert a value in a specific stack
    it may throw an exception if we try to insert an invalid type
    */
    stack_type push(const T& val, stack_type head) { return _push(val, head); }
    stack_type push(T&& val, stack_type head) { return _push(std::move(val), head); }

    /*
    this method insert an element in a specific stack,
    becoming the value inserted the head of the specific stack
    */
    template<typename X>
    stack_type _push(X&& val, const stack_type head) {
       
        if (empty(free_nodes)) {
            pool.emplace_back(std::forward<X>(val), head, true);
            last(head) = false;
            return static_cast<stack_type>(pool.size());
        }
        else {
            auto tmp = free_nodes;
            value(free_nodes) = val;
            free_nodes = next(free_nodes);
            next(tmp) = head;
            last(head) = false;
            last(tmp) = true;
            return tmp;
        }
    }

    /*
    this method remove the frist element in a specific stack,
    changing the head by the previous element inserted in that specific stack
    may throw an exception is a empty stack is called to pop, or by passing no-head
    */
    stack_type pop(stack_type x) {// delete first node
        if (!last(x)) {
            throw NoHeadStack("Trying to pop an stack by passing no-head.");
        }
        if (empty(x)) {
            throw EmptyStack("Trying to pop an Empty Stack.");
        }

        auto tmp = next(x);
        next(x) = free_nodes;
        free_nodes = x;
        value(x) = -1;
        last(tmp) = true;
        last(x) = false;
        return tmp;
    }
    /*
    this method is able to remove all the elements on a given stack
    */
    stack_type free_stack(stack_type x) {
        while (x != end())
            x = pop(x);
        return x;
    }
    /*
    this function is able to print a full stack
    */
    void print_stack(stack_type x) {
        std::cout << "the stacks is:" << std::endl;
        for (auto start = begin(x), finish = end(x); start != finish; ++start)
        {
            std::cout << "[" << *start << "]";
        }
        std::cout << "\n";
    }
    /*
    this function is able to print the full stack_pool
    */
    void print_pool() noexcept {
        std::cout << "the pool of stacks is: " << std::endl;
        for (auto i = 0; i < pool.size(); i++) {
            std::cout << "[" << pool[i].value << "]";
        }
        std::cout << "\n-------------------" << std::endl;
    }
};
/*
int main() {
    //adding s1
    stack_pool<int> pool{ 8 };
    auto s1 = pool.new_stack();
    s1 = pool.push(10, s1);
    s1 = pool.push(20, s1);
    s1 = pool.push(30, s1);
    //adding s2
    auto s2 = pool.new_stack();
    s2 = pool.push(90, s2);
    s2 = pool.push(80, s2);
    s2 = pool.push(70, s2);
    //popping and adding
    s1 = pool.pop(s1);
    s2 = pool.push(50, s2); //added where the 30 was
    //printing the full stack
    pool.print_pool();
    pool.print_stack(s1);
    pool.print_stack(s2);
    try {
        //s2 = pool.pop(s2);
        //s2 = pool.pop(s2);
        //s2 = pool.pop(s2);
        //s2 = pool.pop(s2);
        //s2 = pool.pop(s2); //multiples pop and the stack is empty
        //s2 = pool.pop(5); //by passing no head
    }
    catch (const NoHeadStack& s) {
        std::cerr << s.message << std::endl;
        return 1;
    }
    catch (const EmptyStack& s) {
        std::cerr << s.message << std::endl;
        return 2;
    }
    return 0;
}
*/
