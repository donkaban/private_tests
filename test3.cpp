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

private:    

    inline void check_ndx(size_t ndx) {if(ndx > count) throw std::out_of_range("out of range");}
         
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


void   List::push_back(const std::string & data)
{
    // ... do it for test

}

void List::for_each(const lambda &l)
{
    size_t index = 0;
    node current = head; 
    while(current != tail)
    {
        l(current, index++);
        current = current->next;
    }       
} 
    
void List::Serialize(std::ostream & stream)   
{
    std::vector<node>     links;
    std::vector<uint32_t> indicies;

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
    std::vector<uint32_t> indicies;
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
        if (it != indicies.end())
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
        // do test        





    }
    catch(const std::exception &e)
    {
        std::cout << "error! " << e.what() << std::endl;
        std::terminate();
    }

    return 0;
}