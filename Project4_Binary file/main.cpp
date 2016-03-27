//
//  main.cpp
//  Project4
//
//  Created by GuoYang on 3/7/16.
//  Copyright © 2016 Guo,Y. All rights reserved.
//

#include <iostream>
#include <string>
#include "BinaryFile.h"
#include <cassert>

using namespace std;

class DiskList
{
public:
    DiskList(const std::string& filename);
    bool push_front(const char* data);//point to  the beginning of c string
    bool remove(const char* data);
    void printAll();
    int numberOfCallsToNew() const { return m_numberOfCallsToNew; }
    
private:
    BinaryFile bf;
    BinaryFile::Offset offsetOfFirstNode;
    BinaryFile::Offset offsetOfFirstNodeFree;
    
    struct DiskNode{
        char data[256];
        BinaryFile::Offset next;
    };
    
    //Debug
    int m_numberOfCallsToNew;

};


DiskList::DiskList(const std::string& filename){
    
    bf.createNew(filename);
    offsetOfFirstNode = 2*sizeof(BinaryFile::Offset);
    offsetOfFirstNodeFree=0;
    bf.write(offsetOfFirstNode, 0);// like a head pointer
    bf.write(offsetOfFirstNodeFree, sizeof(BinaryFile::Offset));//header of freelist
    
    m_numberOfCallsToNew=0;

}


bool DiskList::push_front(const char* data){

    if (strlen(data)>255) {
        return false;
    }
    DiskNode newNode;
    strcpy(newNode.data, data);
    bf.read(offsetOfFirstNode, 0);
    bf.read(offsetOfFirstNodeFree,sizeof(BinaryFile::Offset));
    
    if (bf.fileLength()==offsetOfFirstNode) {
        newNode.next= 0;
        bf.write(newNode, bf.fileLength());
        m_numberOfCallsToNew++;
    }
    else{
        newNode.next= offsetOfFirstNode;
        
        if (offsetOfFirstNodeFree==0) {
            
            //offsetOfFirstNode=bf.fileLength();
            bf.write(bf.fileLength(), 0);//the filelength before insert new node
            bf.write(newNode, bf.fileLength());//insert newnode at the end of bf
            m_numberOfCallsToNew++;
        }
        else{
            //if the freelist is not empty
            // write to header of freelist
            DiskNode freeHeader;
            bf.read(freeHeader,offsetOfFirstNodeFree);
            bf.write(offsetOfFirstNodeFree, 0);
            bf.write(newNode,offsetOfFirstNodeFree);
            
            //update header of freelist
           // offsetOfFirstNodeFree=freeHeader.next;
            bf.write(freeHeader.next, sizeof(BinaryFile::Offset));
            
        }
    }
    
    return true;
    
}

bool DiskList::remove(const char* data){
    
    bool removed=false;
    bf.read(offsetOfFirstNode, 0);
    if (bf.fileLength()==offsetOfFirstNode) {
        return false;
    }
    
    DiskNode preNode,curNode;
    BinaryFile::Offset preOffset,curOffset;
    bf.read(curNode,offsetOfFirstNode);
    curOffset=offsetOfFirstNode;
    preOffset=1;//invalid prevnode
    preNode.next=curOffset;
    
    while (preNode.next!=0) {//cur is not nullptr
        if (strcmp(curNode.data,data)!=0){
            
            preOffset=curOffset;
            //preNode=curNode;
            bf.read(preNode, curOffset);
            
            curOffset=preNode.next;
            bf.read(curNode, curNode.next);
        }
        else{
            // Unhook node from linked list
            
            // DiskNode tobeMoved=curNode;
            curOffset=curNode.next;
            bf.read(curNode, curNode.next);
            if (preOffset==1) {
                bf.write(curOffset,0);//header.head = cur;
            }
            else{
                bf.write(curOffset, preOffset+256);
            }
            // Add removed node to front of freeList
            
            //toBeRemoved->next = header.freeList;
            bf.read(offsetOfFirstNodeFree, sizeof(BinaryFile::Offset));
            bf.write(offsetOfFirstNodeFree, preNode.next+256);//removed node is added to freelist
            
            //header.freeList = toBeRemoved;
            //cout<<"remove:"<<offsetOfFirstNodeFree<<endl;
            bf.write(preNode.next, sizeof(BinaryFile::Offset));

            removed = true;
            
        }
    }
    return removed;
}

void DiskList::printAll(){
    //cout<<bf.fileLength()<<endl;
    //The printAll function writes, one per line, all the C strings in the list in the order they appear in the list.
    bf.read(offsetOfFirstNode, 0);
    if (bf.fileLength()==offsetOfFirstNode) {
        return;
    }
    DiskNode readNode;
    bf.read(readNode,offsetOfFirstNode);
    cout<<readNode.data;
    //<<endl;
    
    cout<<readNode.next<<endl;
    
    while (readNode.next!=0) {
        bf.read(readNode,readNode.next);
        cout<<readNode.data;
        //<<endl;
        cout<<readNode.next<<endl;
    }
    
//    //debug: printout freelist
//    bf.read(offsetOfFirstNodeFree, sizeof(BinaryFile::Offset));
//    if (offsetOfFirstNodeFree!=0) {
//        cout<<"=====Freelist====="<<endl;
//        cout<<offsetOfFirstNodeFree<<endl;
//        bf.read(readNode,offsetOfFirstNodeFree);
//        cout<<readNode.data;
//        cout<<readNode.next<<endl;
//        while (readNode.next!=0) {
//            bf.read(readNode,readNode.next);
//            cout<<readNode.data;
//            cout<<readNode.next<<endl;
//        }
//    }
    
    return;
    
    
}


int main()
{
    DiskList x("mylist.dat");
    x.push_front("Fred");
    x.push_front("Lucy");
    x.push_front("Ethel");
    x.push_front("Ethel");
    x.push_front("Lucy");
    x.push_front("Fred");
    x.push_front("Ethel");
    x.push_front("Ricky");
    x.push_front("Lucy");
    x.printAll();
    cout<<endl;
    
    x.remove("Lucy");
    x.printAll();
    
    x.push_front("Fred");
    x.push_front("Ricky");
    x.push_front("Ricky");
    x.push_front("Ricky");

    cout<<endl;
   x.printAll();  // writes, one per line
		  // Ricky  Fred  Ricky  Ethel  Fred  Ethel  Ethel  Fred
    assert(x.numberOfCallsToNew() ==10);  // not 11

}


