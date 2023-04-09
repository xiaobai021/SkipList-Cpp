/****************************************
 File Name:skiplist.h
 ***************************************/

#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include<iostream>
#include<mutex>
#include<fstream>
#include<cstring>
#include<cstdlib>
#define STORE_FILE "File/dumpFile"

std::mutex mtx;
std::string delimiter=":";

//Class template to implement node
template<typename K,typename V>
class Node{
public:

    Node(){};
    Node(K k,V v,int);
    ~Node();

    K get_key()const;
    V get_value()const;
    void set_value(V);
    Node<K,V>**forward;

    int node_level;

private:
    K key;
    V value;
};

template<typename K,typename V>
Node<K,V>::Node(const K k,const V v,int level){

    this->key=k;
    this->value=v;
    this->node_level=level;

    this->forward=new Node<K,V>*[level+1];
    memset(this->forward,0,sizeof(Node<K,V>*)*(level+1));
}

template<typename K,typename V>
Node<K,V>::~Node(){
    delete []forward;
}

template<typename K,typename V>
K Node<K,V>::get_key()const{
    return key;
}

template<typename K,typename V>
V Node<K,V>::get_value()const{
    return value;
}

template<typename K,typename V>
void Node<K,V>::set_value(V value){
    this->value=value;
}

//Class template for Skip list
template <typename K, typename V> 
class Skiplist{
public:

    Skiplist(int);
    ~Skiplist();
    
    Node<K,V>*create_node(K,V,int);
    int insert_element(K,V);
    void delete_element(K);
    void update_element(K,V);
    bool search_element(K);
    void display_list();
    int get_random_level();
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const std::string&str,std::string*key,std::string*value);
    bool is_valid_string(const std::string&str);

    int _max_level;
    int _skip_list_level;
    int _element_count;
    Node<K,V>*_header;

    std::ofstream _file_writer;
    std::ifstream _file_reader;
};

template <typename K, typename V> 
Skiplist<K,V>::Skiplist(int max_level){

    this->_max_level=max_level;
    this->_skip_list_level=0;
    this->_element_count=0;

    K k;
    V v;
    this->_header=new Node<K,V>(k,v,_max_level);
}

template <typename K, typename V> 
Skiplist<K,V>::~Skiplist(){

    if(_file_writer.is_open()){
        _file_writer.close();
    }
    if(_file_reader.is_open()){
        _file_reader.close();
    }
    delete _header;
}

template <typename K, typename V> 
Node<K,V>*Skiplist<K,V>::create_node(const K k,const V v,int level){
    Node<K,V>*node=new Node<K,V>(k,v,level);
    return node;
}

template <typename K, typename V> 
int Skiplist<K,V>::insert_element(const K key,const V value){

    mtx.lock();
    Node<K,V>*current=this->_header;

    Node<K,V>*update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;i--){
        while(current->forward[i]!=nullptr&&current->forward[i]->get_key()<key){
            current=current->forward[i];
        }
        update[i]=current;
    }

    current=current->forward[0];

    if(current!=nullptr&&current->get_key()==key){
        std::cout << "key: " << key << ", exists" << std::endl;
        mtx.unlock();
        return 1;
    }

    if(current==nullptr||current->get_key()!=key){
        int random_level=get_random_level();

        if(random_level>_skip_list_level){
            for(int i=_skip_list_level;i<random_level+1;i++){
                update[i]=_header;
            }
            _skip_list_level=random_level;
        }

        Node<K,V>*inserted_node=create_node(key,value,random_level);

        for(int i=0;i<=random_level;i++){
            inserted_node->forward[i]=update[i]->forward[i];
            update[i]->forward[i]=inserted_node;
        }
        std::cout<<"Successfully inserted key:"<<key<<",value:"<<value<<std::endl;
        _element_count++;
    }
    mtx.unlock();
    return 0;
}

template <typename K, typename V>
void Skiplist<K,V>::delete_element(K key){

    mtx.lock();
    Node<K,V>*current=this->_header;

    Node<K,V>*update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;i--){
        while(current->forward[i]!=nullptr&&current->forward[i]->get_key()<key){
            current=current->forward[i];
        }
        update[i]=current;
    }

    current=current->forward[0];

    if(current!=nullptr&&current->get_key()==key){
        for(int i=0;i<=_skip_list_level;i++){
            if(update[i]->forward[i]!=current){
                break;
            }
            update[i]->forward[i]=current->forward[i];
        }

        while(_skip_list_level>0&&_header->forward[_skip_list_level]==0){
            _skip_list_level--;
        }
        std::cout << "Successfully deleted key "<< key << std::endl;
        _element_count --;
    }else{
        std::cout << key << " is not exist, please check your input !\n";
    }
    mtx.unlock();
    return;
}

template <typename K, typename V>
void Skiplist<K,V>::display_list(){

    std::cout<<"\n*****Skip List*****"<<"\n";
    for(int i=0;i<=_skip_list_level;i++){
        Node<K,V>*node=this->_header->forward[i];
        std::cout<<"Level "<<i<<": ";

        while(node!=nullptr){
            std::cout<<node->get_key()<<":"<<node->get_value()<<";";
            node=node->forward[i];
        }
        std::cout<<std::endl;
    }
}

template<typename K,typename V>
void Skiplist<K,V>::update_element(const K key,V value){
    std::cout << "update_element-----------------" << std::endl;
    mtx.lock();
    Node<K,V>*current=this->_header;
    Node<K,V>*update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i = _skip_list_level; i >= 0; i--) {
        while(current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];  
        }
        update[i] = current; 
    }

    current=current->forward[0];

    if(current!=nullptr&&current->get_key()==key){
        std::cout<<"key: "<<key<<",exist"<<std::endl;
        std::cout<<"old value: "<<current->get_value()<<std::endl;
        current->set_value(value);
        std::cout<<"new value: "<<current->get_value()<<std::endl;
        mtx.unlock();
        return;
    }

    std::cout<<key<<" is not exist,update failed"<<std::endl;
    mtx.unlock();
    return;
}

template <typename K, typename V>
bool Skiplist<K,V>::search_element(K key){

    std::cout << "search_element-----------------" << std::endl;
    Node<K,V>*current=_header;

    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    current=current->forward[0];

    if(current and current->get_key()==key){
        std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }
    std::cout<<"Not Found Key:"<<key<<std::endl;
    return false;
}

template <typename K, typename V>
int Skiplist<K,V>::get_random_level(){

    int k=1;
    while(rand()%2){
        k++;
    }
    k=(k<_max_level)?k:_max_level;
    return k;
}

template <typename K, typename V>
int Skiplist<K,V>::size(){
    return _element_count;
}

template <typename K, typename V>
void Skiplist<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value){

    if(!is_valid_string(str)){
        return;
    }
    *key=str.substr(0,str.find(delimiter));
    *value=str.substr(str.find(delimiter)+1,str.length());
}

template<typename K, typename V>
bool Skiplist<K,V>::is_valid_string(const std::string&str){

    if(str.empty()){
        return false;
    }
    if(str.find(delimiter)==std::string::npos){
        return false;
    }
    return true;
}

template<typename K, typename V>
void Skiplist<K,V>::dump_file(){

    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K,V>*node=this->_header->forward[0];

    while(node!=nullptr){
        _file_writer<<node->get_key()<<":"<<node->get_value()<<"\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    return;
}

template<typename K, typename V>
void Skiplist<K,V>::load_file(){

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();

    while(getline(_file_reader,line)){
        get_key_value_from_string(line,key,value);
        if(key->empty()||value->empty()){
            continue;
        }
        // insert_element(stoi(*key),*value);
        insert_element(*key,*value);//待改进，加载进来为字符串类型，这个只能插键为int类型
        std::cout << "key:" << *key << ",value:" << *value << std::endl;
    }
    _file_reader.close();
}

#endif