//
//  main.cpp
//  mallocandfree
//
//  Created by 张正 on 16/3/7.
//  Copyright (c) 2016年 张正. All rights reserved.
//
#include <iostream>

#define MAX 1024	//内存的大小
#define MIN 4		//对齐字节

using namespace std;

struct zz_block
{
    int startAddr;          //块的其实地址
    int size;       //块的大小
    bool used;          //是否用过
    struct zz_block *last;      //指向前面块的指针
    struct zz_block *next;      //指向后面快的指针
    struct zz_block *lastFree ;      //指向上个空闲的内存块
    struct zz_block *nextFree ;      //指向下个空闲的内存块
};

zz_block *zz_base;          //用来保存所有内存块
zz_block *zz_freeBase;      //用来保存所有空闲的内存块

void zz_block_Init(){           //对开始的数据库进行初始化
    zz_base = new zz_block();
    zz_base->startAddr = 0;
    zz_base->size = MAX;
    zz_base->last = NULL;
    zz_base->next = NULL;
    zz_base->lastFree = NULL;
    zz_base->nextFree = NULL;
    zz_base->used = false;
    
    zz_freeBase = new zz_block();
    zz_freeBase->startAddr = -1;
    zz_freeBase->size = -1;
    zz_freeBase->last = NULL;
    zz_freeBase->next = NULL;
    zz_freeBase->lastFree = NULL;
    zz_freeBase->nextFree = zz_base;
    zz_base->lastFree = zz_freeBase;
}

void zz_block_Split(zz_block *head, int num){
    int a = num % MIN;
    if(a != 0){
        num = (num/MIN+1) * MIN;
    }
    if(num == head->size){
        head->used = true;
    }
    else{
        zz_block *myBlock = new zz_block();
        myBlock->startAddr = head->startAddr + num;
        myBlock->size = head->size - num;
        head->size = num;
        myBlock->next = head->next;
        if(head->next!=NULL)
        {
            head->next->last = myBlock;
        }
        head->next = myBlock;
        myBlock->last = head;
        head->used = true;
        myBlock->used = false;
        myBlock->lastFree = head->lastFree;
        if(myBlock->lastFree != NULL){
            myBlock->lastFree->nextFree = myBlock;
        }
        myBlock->nextFree = head->nextFree;
        if(myBlock->nextFree != NULL)
            myBlock->nextFree->lastFree = myBlock;
        head->lastFree = head->nextFree = NULL;
    }
}

void *zz_Malloc(int num){
    zz_block *head;
    if(zz_freeBase->nextFree != NULL){
        head = zz_freeBase->nextFree;
    }
    else{
        return NULL;        //内存满
    }
    while(true){
        if(head->size >= num){
            zz_block_Split(head, num);
            return head;
        }
        else{
            if(head->nextFree != NULL){
                head = head->nextFree;
            }
            else{
                return NULL;
            }
        }
    }
}
void blockPrint(){
    zz_block *head = zz_base;
    int i = 1;
    while(head){
        cout << "No." << i << "block, " << "size: " << head->size << ", ";
        if(head->used == false){
            cout << "not used." << endl;
        }
        else{
            cout << "used." << endl;
        }
        head = head->next;
        i++;
    }
    cout << "-------------------------------" <<endl;
}
void zz_Free(void *p){
    zz_block *freeHead = zz_freeBase->nextFree;
    zz_block *head = (zz_block *)p;
    head->used = false;
    while(freeHead){
        if(freeHead->startAddr < head->startAddr){
            if(freeHead->nextFree != NULL){
                if(freeHead->nextFree->startAddr < head->startAddr){
                    freeHead = freeHead->nextFree;
                    continue;
                }
                else
                    break;
            }
            else
                break;
        }
        else{
            freeHead = freeHead->lastFree;
            break;
        }
    }
    //找到释放块的上一块空闲块
    
    if(!freeHead){
        zz_freeBase->nextFree = head;
        head->lastFree = zz_freeBase;
        head->nextFree = NULL;
    }
    else{
        if(freeHead->startAddr+freeHead->size == head->startAddr){
            cout<<"here!"<<endl;
            head = head->last;
            cout<<head->size<<endl;
            head->size = head->size + head->next->size;
            head->next = head->next->next;
            //blockPrint();
            if(head->next != NULL)
                head->next->last = head;
        }
        else{
            head->nextFree = freeHead->nextFree;
            head->lastFree = freeHead;
            freeHead->nextFree = head;
        }
        if(head->nextFree != NULL && head->startAddr + head->size == head->nextFree->startAddr){
            head->nextFree = head->nextFree->nextFree;
            if(head->nextFree != NULL)
            head->nextFree->nextFree->lastFree = head;
            head->size = head->size + head->next->size;
            head->next = head->next->next;
            if(head->next != NULL)
                head->next->last = head;
        }
    }
    
}

int main(){
    zz_block_Init();
    blockPrint();
    int *a = (int *)zz_Malloc(41);
    blockPrint();
    char *b = (char *)zz_Malloc(111);
    blockPrint();
    zz_Free(a);
    blockPrint();
    char *c = (char *)zz_Malloc(27);
    blockPrint();
    zz_Free(b);
    blockPrint();
    zz_Free(c);
    blockPrint();
}
