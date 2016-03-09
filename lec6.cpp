//
//  main.cpp
//  Virtualto
//
//  Created by 张正 on 16/3/9.
//  Copyright (c) 2016年 张正. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char memery[32 * 128];
char* pde_addr = &memery[544];

void getMemory(char filename[])
{
    FILE *fin = fopen(filename,"r");
    int a;

    for(int i = 0; i < 128; ++i) {
        for(int j = 0; j < 32; ++j) {
            fscanf(fin, "%x", &a);
            memery[i*32 + j] = a;
        }
    }
    fclose(fin);
}

int main()
{
    cout << "please input the memery textfile:";
    char filename[300];
    cin>>filename;
    cout << "Input Virtual Address: 0x";
    int Virtual_addr;
    scanf("%x", &Virtual_addr);
    int pde_index = (Virtual_addr >> 10) & 0x1f;
    
    int pte_index = (Virtual_addr >> 5) & 0x1f;
    int offset = Virtual_addr & 0x1f;
    getMemory(filename);
    int pde = pde_addr[pde_index];
    printf("    -->PDE Index:0x%x PDE content(valid: %d, pfn: 0x%x)\n", pde_index, (pde >> 7) , pde & 0x7f);
    if(((pde >> 7) & 1) == 0) {
        cout << "Fault! (page directory entry not valid)" << endl;
        return 0;
    }
    int pte = memery[((pde & 0x7f) << 5) + pte_index];
    printf("        -->PTE Index:0x%x PTE content(valid: %d, pfn: 0x%x)\n", pte_index, (pte >> 7) , pte & 0x7f);
    if(((pte >> 7) & 1) == 0) {
        cout << "Fault!(page table entry not valid)" << endl;
        return 0;
    }
    int physical_addr = ((pte & 0x7f) << 5) + offset;
    printf("            -->Translates to Physical Address:0x%x -->", physical_addr);
    printf("Value:0x%x\n", memery[physical_addr]);
    return 0;
}
