
//  Map.cpp
//  Project 2
//
//  Created by GuoYang on 1/18/16.
//  Copyright © 2016 Guo,Y. All rights reserved.
//

#include "Map.h"
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

//Constructor: An empty,circular list with a dummy node
Map::Map(){
    mapSize=0;
    item* newItem=new item;
    newItem->prev=newItem;
    newItem->next=newItem;
    head=newItem;
}

//Destructor:delete all nodes
Map::~Map(){
    item* temp;
    item* dummy=head;
    head=head->next;
    while (head!=dummy) {
        temp=head;
        head=head->next;
        delete temp;
    }
    delete  dummy;
}

//Copy constructor
Map::Map(const Map &old){
    //Create a new linklist
    item* newItem=new item;
    newItem->prev=newItem;
    newItem->next=newItem;
    head=newItem;
    mapSize=0;
    //Copy old list
    //Copy backwards from old list to keep the same order, since the insert function always insert new node right after the head node.
    item* temp=old.head;
    temp=temp->prev;
    while (temp!=old.head) {
       insert(temp->key, temp->value);
        temp=temp->prev;
    }
  
}

//Assign operator:
Map& Map::operator=(const Map&src){
    if (&src==this)
        return (*this);
    //Delete original data in linklist
    this->~Map();
    item* temp;
    //Creat a new linklist
    item* newItem=new item;
    newItem->prev=newItem;
    newItem->next=newItem;
    head=newItem;
    mapSize=0;
    //Copy old linklist(backwards)
    temp=src.head;
    temp=temp->prev;
    while (temp!=src.head) {
        insert(temp->key, temp->value);
        temp=temp->prev;
    }
    return (*this);
}

// Return true if the map is empty, otherwise false.
bool Map:: empty() const{
    if(mapSize==0)
        return true;
    return false;
}

// Return the number of key/value pairs in the map.
int Map::size() const{
    return mapSize;
}

//Insert new node
bool Map::insert(const KeyType& key, const ValueType& value){
    //Check of the new key is already in the list
    item* dummy=head;
    head=head->next;
    while (head!=dummy) {
        if (head->key==key){
            head=dummy;//head pointer return to dummy node
            return false;
        }
        head=head->next;
    }
    //If new key is not in the list, insert new node right after head node, update mapSize
    item *newItem=new item;
    newItem->key=key;
    newItem->value=value;
    newItem->prev=head;
    newItem->next=head->next;
    head->next->prev=newItem;
    head->next=newItem;
    mapSize++;
    return true;
}



// Update value: If key is equal to a key currently in the list, map to
// the value of the second parameter
bool Map::update(const KeyType&key, const ValueType& value){
    item* dummy=head;
    head=head->next;
    while (head!=dummy) {
        if (head->key==key){
            head->value=value;
            head=dummy;
            return true;
        }
        head=head->next;
    }
    return false;
}




//Insert or update a node
bool Map::insertOrUpdate(const KeyType& key, const ValueType& value){
    item* dummy=head;
    head=head->next;
    while (head!=dummy) {
        // If key is equal to a key currently in the list, update value;
        if (head->key==key){
            head->value=value;
            head=dummy;
            return true;
        }
        head=head->next;
    }
    // If key is not equal to any key currently in the list, insert a new node.
    item *newItem=new item;
    newItem->key=key;
    newItem->value=value;
    newItem->prev=head;
    newItem->next=head->next;
    head->next->prev=newItem;
    head->next=newItem;
    mapSize++;
    return true;
}


// Remove a  node: If key is equal to a key currently in the map, remove the key/value
///pair and return true.
bool Map::erase(const KeyType& key){
    item* dummy=head;
    head=head->next;
    while (head!=dummy) {
        //update
        if (head->key==key){
            mapSize--;
            head->prev->next=head->next;
            head->next->prev=head->prev;
            delete head;
            head=dummy;
            return true;
        }
        head=head->next;
    }
    return false;
}

// Check if key is contained in the list
bool Map::contains(const KeyType& key)const{
    item* temp=head;
    temp=temp->next;
    while (temp!=head) {
        if (temp->key==key){
            return true;
        }
        temp=temp->next;
    }
    return false;
}

//Get value of a key
// If key is equal to a key currently in the map, get mapped value with value.
bool Map::get(const KeyType& key, ValueType& value)const{
    item* temp=head;
    temp=temp->next;
    while (temp!=head) {
        if (temp->key==key){
            value=temp->value;
            return true;
        }
        temp=temp->next;
    }
    return false;
}

//Get function for iteration
bool Map::get(int i, KeyType& key, ValueType& value)const{
    if ((i<0)||(i>=mapSize))
        return false;
    item* temp=head;
    //Find the (i+1)th node and get its value
    for(int j=0;j<=i;j++){
        temp=temp->next;
    }
    key=temp->key;
    value=temp->value;
    return true;
}


// Exchange the contents of this list with the other one.
void Map::swap(Map& other){
    // Swap the head pointers to dynamic arrays.
    item* temphead = head;
    head= other.head;
    other.head = temphead;
    // Swap sizes
    int t = mapSize;
    mapSize = other.mapSize;
    other.mapSize = t;
    
}

//Combine two lists
bool combine(const Map& m1, const Map& m2, Map& result){
    KeyType k1,k2;
    ValueType v1,v2;
    int i,j;
    bool res=true;
    Map temp=m1;
    for(i=0;i<m2.size();i++){
        // Get one node in m2
        m2.get(i,k2,v2);
        // Compare every node in m1 with this node
        for(j=0;j<m1.size();j++){
            m1.get(j, k1, v1);
            //If a key appears in both m1 and m2, but with different corresponding values, returns false;
            if (k1==k2) {
                if (v1!=v2){
                    temp.erase(k1);
                    res=false;
                    break;
                }
            }
        }
        //If a key appears in exactly one of m1 and m2, OR appears in both m1 and m2, with the same corresponding value, do insert; the insert function won't insert a repeated key in list.
        if (j==m1.size()){
            temp.insert(k2, v2);
        }
    }
    result=temp;
    return res;
}

// Substract lists
void subtract(const Map& m1, const Map& m2, Map& result){
    KeyType k1,k2;
    ValueType v1,v2;
    int i,j;
    //copy m1 list as result
    Map temp=m1;
    for(i=0;i<m2.size();i++){
        // Get one node in m2
        m2.get(i,k2,v2);
        // Compare every node in m1 with this node
        for(j=0;j<m1.size();j++){
            m1.get(j, k1, v1);
            // if this node apears both in m1 and m2, erase it from result.
            if (k1==k2) {
                temp.erase(k1);
            }
        }
    }
    result=temp;
}
