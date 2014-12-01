#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <fstream>

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
    
// only test methods for list operations :

    void push_back(const std::string &);  // and link with head for testing purpose
    void for_each(const lambda &); 
    void clean();
    void log(); 

private:    
    inline std::string get_data(node n) {return n ?  n->data : "null";}     

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
    if (!head)
    {    
        head = new ListNode(nullptr,nullptr,nullptr,data);
        tail = head;
    }
    else
    {
        tail->next = new ListNode(tail, nullptr, nullptr, data);
        tail->next->rand = head;
        tail = tail->next;
    }    
    count++;
}


void List::clean()
{
    if(!tail || !head) return;
    auto current = tail;
    while(current !=head && current->prev)
    {
        auto tmp = current;
        current = current->prev;
        delete tmp;
    }    
    count = 0;
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
    if(!head || !tail) return;
    size_t index = 0;
    auto current = head; 
    do
    {
        l(current, index++);
        current = current->next;
    }
    while(current !=tail);       
    l(tail, index++);
} 
    
void List::Serialize(std::ostream & stream)   
{
    std::cout << "deserealize ... count: " << count << std::endl;
   
    std::vector<node>    links;
    std::vector<int32_t> indicies(count, -1);

    for_each([&](node n, int)  {links.push_back(n->rand);}); // 1st pass, collect random links
    for(size_t i = 0; i < links.size(); i++)                 // 2nd pass, collect random indicies
    {    
        for_each([&](node n, int ndx)  
        {
            if(n->rand == links[i]) indicies[ndx] = i;
        }); 
    }
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
    std::cout << "deserealize ... count: " << hdr.count << std::endl;
   
    indicies.resize(hdr.count);
    links.resize(hdr.count);
    stream.read(reinterpret_cast<char *>(&indicies[0]),indicies.size() * sizeof(int32_t)); // read indicies
   
    for(int i = 0; i < hdr.count; i++)  // read data packet's
    {
        size_t       size;
        std::string  data;
        stream.read(reinterpret_cast<char *>(&size),sizeof(size_t));
        data.resize(size);
        stream.read(reinterpret_cast<char *>(&data[0]),size);
        push_back(data);
    }  
    for(size_t i = 0; i < indicies.size(); i++) // 1nd pass, collect random indicies
    {    
        for_each([&](node n, int ndx)  
        {
            if(ndx == indicies[i]) links[ndx] = n;
        }); 
    }
  
    for_each([&](node n, int ndx) // 2st pass, final
    {
        n->rand = links[ndx];
    });    
}


int main()
{
    List test1;
    List test2;
    
    std::ifstream  ifile;
    std::ofstream  ofile;
    
    try
    {
       
        test1.push_back("node 1");
        test1.push_back("node 2");
        test1.push_back("node 3");
        test1.push_back("node 4");
        test1.push_back("node 5");
        test1.push_back("node 6");
        
        ofile.open("test.bin", std::ios::out | std::ios::binary);
        test1.Serialize(ofile);
        ofile.close();
        ifile.open("test.bin");
        test2.Deserialize(ifile);
        std::cout << "\nin list : \n";
        test1.log();
        std::cout << "\nout list : \n";
        test2.log();

    }
    catch(const std::exception &e)
    {
        std::cout << "error! " << e.what() << std::endl;
        std::terminate();
    }

    return 0;
}