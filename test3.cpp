#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>

struct ListNode
{
    ListNode    * prev {};
    ListNode    * next {};
    ListNode    * rand {};
    std::string   data;
    ListNode(ListNode *p, ListNode *n, ListNode *r, const std::string &d):
        prev(p),
        next(n),
        rand(r),
        data(d)
    {}
};

class List
{
public:
    using node   = ListNode *;
    using lambda = std::function<void(node, int)>; 

    List();
    virtual ~List();

    void Serialize(std::ostream & stream);
    void Deserialize(std::istream & stream);
    
    void push_back(const std::string &);
    void for_each(const lambda &); 
    void clean();
    void log(); // only for testing

private:    

    inline void check_ndx(size_t ndx)   {if(ndx > count) throw std::out_of_range("out of range");}
    inline std::string get_data(node n) {return n ?  n->data : "nullptr";}     
    node    head;
    node    tail;
    size_t count;

    struct file_header
    {
        uint8_t magic = 42;  
        int32_t count;
    };
};


List::List() :
    count(0)
{}

List::~List()
{}


void List::push_back(const std::string & data)
{
    std::cout << "push: " << data << " count: " << count << std::endl;
    if (count == 0)
    {    
        head = new ListNode(nullptr,nullptr,nullptr,data);
        tail = head;
    }
    else
    {
        tail->next = new ListNode(tail, nullptr, nullptr, data);
        tail = tail->next;
    }    
    ++count; 
}

void List::clean()
{
    if(!tail) return;

    auto current = tail;
    while(current !=head && current->prev)
    {
        auto tmp = current;
        current = current->prev;
        delete tmp;
        --count;
    }    
    delete head;
}

void List::log()
{
    for_each([&](node n, int)
    {
        std::cout << 
        " data: " << get_data(n)       << 
        " prev: " << get_data(n->prev) << 
        " next: " << get_data(n->next) <<
        " rand: " << get_data(n->rand) << std::endl; 
    });
}

void List::for_each(const lambda &l)
{
    size_t index = 0;
    auto current = head; 
    while(current != tail && current->next)
    {
        l(current, index++);
        current = current->next;
    }       
} 
    
void List::Serialize(std::ostream & stream)   
{
    std::vector<node>     links;
    std::vector<int32_t> indicies;

    for_each([&](node n, int){links.push_back(n->rand);}); // 1st pass, collect random links
    for_each([&](node n, int)                              // 2nd pass, collect random indicies
    {
        auto it = std::find(links.begin(), links.end(), n);
        if (it != links.end() and *it) 
            indicies.push_back(it-links.begin());
        else 
            indicies.push_back(-1); // in case random link is nullptr      
    });
    file_header hdr;
    hdr.count = count;
    stream.write(reinterpret_cast<const char *>(&hdr),sizeof(hdr));
    stream.write(reinterpret_cast<const char *>(&indicies[0]),indicies.size() * sizeof(int32_t)); // write indicies
    for_each([&](node n, int)                                                                     // write data's
    {
        size_t size = n->data.size();   // [size][data] simple 'packet'
        stream.write(reinterpret_cast<const char *>(&size),sizeof(size_t));
        stream.write(reinterpret_cast<const char *>(n->data.c_str()),size);
    });    
}

void List::Deserialize(std::istream & stream) 
{
    clean(); // deserealize restore and does not append to current class!

    std::vector<int32_t> indicies;
    std::vector<node>     links;
   
    file_header hdr;
    stream.read(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    if(hdr.magic != 42)
        throw std::runtime_error("it's not own file ");
    indicies.resize(hdr.count);
    links.resize(hdr.count);
    stream.read(reinterpret_cast<char *>(&indicies[0]),indicies.size() * sizeof(int32_t)); // read indicies
    for(int i = 0; i < hdr.count; i++)                                                     // read data packet's
    {
        size_t       size;
        std::string  data;
        stream.read(reinterpret_cast<char *>(&size),sizeof(size_t));
        data.resize(size);
        stream.read(reinterpret_cast<char *>(&data[0]),size);
        push_back(data);
    }  
    
    for_each([&](node n, int ndx) // 1st pass, restore random links
    {
        auto it = std::find(indicies.begin(), indicies.end(), ndx);
        if (it != indicies.end() && *it !=-1)
            links[it-indicies.begin()] = n; 
    });                                                                         
  
    for_each([&](node n, int ndx) // 2st pass, final
    {
        n->rand = links[ndx];
    });    
}


int main()
{
    List test;
    try
    {
        test.push_back("node 1");
        test.push_back("node 2");
        test.push_back("node 3");
        test.push_back("node 4");
        test.push_back("node 5");
        test.push_back("node 6");

        test.log();


    }
    catch(const std::exception &e)
    {
        std::cout << "error! " << e.what() << std::endl;
        std::terminate();
    }

    return 0;
}