// Map.h
// Project2
//
//  Created by GuoYang on 1/18/16.
//  Copyright © 2016 Guo,Y. All rights reserved.

#ifndef Map_hpp
#define Map_hpp

#include <string>
#include<iostream>

//typedef-defined type
typedef std::string KeyType;
typedef double ValueType;

//class defination
//double-linked list, circular, dummy node
class Map
{
public:
    Map();  // Create an empty map
    ~Map();//Destructor
    Map(const Map&old);//Copy constructor
    Map &operator=(const Map&src);//Assign operator
    
    bool empty() const;
    
    int size() const;
    
    bool insert(const KeyType& key, const ValueType& value);
 
    bool update(const KeyType& key, const ValueType& value);
  
    bool insertOrUpdate(const KeyType& key,const ValueType& value);
  
    
    bool erase(const KeyType& key);

    bool contains(const KeyType& key)const;

    
    bool get(const KeyType& key, ValueType& value)const;
   
    
    bool get(int i, KeyType& key, ValueType& value)const;

    
    void swap(Map& other);
 
private:
    struct item
    {
        KeyType key;
        ValueType value;
        item *next;
        item *prev;//double-linked list
    };
    item *head;
    int mapSize;
};

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);


#endif /* newMap_hpp */
