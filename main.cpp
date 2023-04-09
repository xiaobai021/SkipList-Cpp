/*************************
File Name:main.cpp
*************************/

#include<iostream>
#include"Skiplist/skiplist.h"
#define FILE_PATH "./File/dumpFile"

int main(){
    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    Skiplist<int,std::string>skiplist(6);
    skiplist.insert_element(1,"xiaobai");
    skiplist.insert_element(3,"xiaoli");
    skiplist.insert_element(10,"xiaohei");
    skiplist.insert_element(2,"xiaoke");
    skiplist.insert_element(90,"xiaozhang");
    skiplist.insert_element(50,"xiaolin");
    skiplist.insert_element(18,"xiaomei");

    std::cout<<"skiplist size:"<<skiplist.size()<<std::endl;

    skiplist.dump_file();
    //skiplist.load_file();

    skiplist.search_element(9);
    skiplist.search_element(18);

    skiplist.display_list();

    skiplist.delete_element(100);
    skiplist.delete_element(90);

    skiplist.update_element(18,"xiaolu");
    skiplist.update_element(70,"xiaolu");

    std::cout<<"skiplist size:"<<skiplist.size()<<std::endl;

    return 0;
}