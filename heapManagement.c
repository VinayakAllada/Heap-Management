//combination of buddy_block system and first fit heap management system
#include <stdio.h>
#include <stddef.h>  // these are standard libraries declared in header file
#include <math.h>
#define MAX_SIZE 8392
char memory[16416];
// The structure definition to contain metadata of each block allocated or deallocated
typedef struct meta_block
{
    size_t size;             // Carries the size of the block described by it
    int free;                //This flag is used to know whether the block described by the metadata structure is free or not -- >
                                 // if free, it is set to 0. Otherwise 1.
    struct meta_block *next; //Points to the next metadata block
}meta_block;
 
    
struct meta_block* store[13];  // contains pointers to each buddy_block
// intializing each metablock information for every buddy_block
void initialize()
{
    struct meta_block *temp=(void*)memory;
    temp->size=0;   //set initial size as zero
    temp->free=0;   //set  buddy_block as free
    temp->next=NULL;  
    store[0]=temp;   //storing buddy_block starting address int the store pointer array
    int add=2;
    //repeating the above process for every buddy_block
    for(int i=1;i<13;i++)
    {
        temp=(void *)((void *)store[i-1]+ add + sizeof(meta_block));
        store[i]=temp;
        temp->size=0;
        temp->free=0;
        temp->next=NULL;
        add=add*2;
        
    }
    
    
}
//function to find the minimum index of buddy_block  to allocate the space for user input
int findrange(int k)
{
  int count=0;
  int temp=k;
  // checking whether  size is exactly in terms of 2^n 
  if(k%2==0)
  {
      int flag=0;
     while(temp%2==0&&temp>2)
     {
         temp=temp/2;
         if(temp%2!=0)
         {
              flag=1;
          }
      
       }
     if(flag==0)
     {
      count--;      // if size is exactly 2^n then index has to be decreased
     }
  }
  while(k>1)
  {
      count++;
      k=k/2;
  }
    return count;
    
}
// My_Alloc function to allocate the give size from heap
void* My_Alloc(int noofBytes)
{
   
    if(noofBytes>MAX_SIZE)
    {
        printf("our memory range is not sufficient");
        return NULL;
        
    }
      meta_block* retval=NULL;
      int done=0;
    int index=findrange(noofBytes);  //finding the index of metablock which is the minimum size of block that can satisfy the request
  
    while(!done&&index<13)
    {
        // if initially buddy_block is free it will inserted in this block
        if(store[index]->free==0&&store[index]->next==NULL)
        {
            
                store[index]->free=1;
                store[index]->size=noofBytes;
                store[index]->next=NULL;
                retval=store[index];
                done=1;
            
        }/* if the  buddy_block is  allocated previously after then deallocated then  this space is having previous size so 
              the space here is allocated for user request */
                  
        else if(store[index]->free==0&&store[index]->size>=noofBytes)
        {
               store[index]->free=1;
                store[index]->size=noofBytes;
             
                retval=store[index];
                  done=1;
               
              
        }  // if the  buddy_block is allocated still if it has size left in the buddy then the remaining space is allocated for user request like first fit 
        else if(store[index]->size+noofBytes<pow(2,index+1))
        {
            
            meta_block* t=store[index];
            int size=t->size+sizeof(meta_block);
            // checking previous buddy_blocks are there if there it has same size as user request it will allocate this spacce to user
            while(t->next!=NULL&&!done)
            {
                if(t->free==0&&t->size>=noofBytes)
                {
                   t->size=noofBytes;
                   t->free=1;
                  done=1;     // if already block exist for that size it will be allocated 
                }
                t=t->next;
                size=size+t->size+sizeof(meta_block);
            }

            if(!done)
           {    
               // if last space is free and having sufficient space for user request this space will be allocated 
              if(size+noofBytes+sizeof(meta_block)<pow(2,index+1))
              {
                meta_block* temp=(void *)(((void*) t )+ t->size+sizeof(meta_block));
                temp->size=noofBytes;
                temp->free=1;
                temp->next=NULL;
                t->next=temp;
               
                
                done=1;
                retval=temp;
              }
               if(done==0)
              {
                index++;
              }
           }
            
        }
            
        else
        {
            index++;    // if the previous buddy_block unable to allocate check for the new buddy_block
        }
       
        
        
    }
    if(done==0)
    {
        printf("\n your memory is not sufficient  ");   //if space is not available 
    }
    else
    {
        printf("\n your memory allocation done successfully for size %d",noofBytes);
    }
    
    
   return (void *)retval;
}
// function to find the index of deleted block index
int mergeindex(void* ptr)
{
    int index=0;
    int found=0;
    int i=0;
    while(!found)
    {
        if(store[i]< ptr<store[i+1])
        {
          index=i;
          found=1;
        }
        i++;

    }

  return index;

}
// To merge the adjacent blocks in the buddy_block after deletion of block
void Merge(int index)
{  
      if(store[index]->free==0&&store[index]->next!=NULL&&store[index]->next->free==0)
      {

          store[index]->size+=store[index]->next->size;
          
          store[index]->next=store[index]->next->next;

      }
      meta_block* ptr=store[index]->next;
      while(ptr!=NULL)
      {
            if(ptr->free==0&&ptr->next!=NULL&&ptr->next->free==0)
            {

                ptr->size=ptr->size+ptr->next->size;
                ptr->next=ptr->next->next;


            }
            else
            {
                 
                 ptr=ptr->next;
            }

      }
      return ;
}



// My_free function to delete the heap elements

void My_Free(void *ptr)
{
    if (((void *)memory <= ptr) && (ptr <= (void *)(memory +16416 )))
    {
       meta_block* temp=ptr;
          temp->free=0;
          printf("\n deallocation done succesfully");
         
          int index=mergeindex(ptr);
          Merge(index);   // To merge with the adjacent blocks  after its deletion 
          
       
    }
    else
        printf("Please provide a valid pointer allocated by My_Alloc\n");
        //If a valid pointer is not provided, the above message will be printed.

}

int main()
{
    initialize();
   
      int *p = (int *)My_Alloc(150 * sizeof(int));
    char *q = (char *)My_Alloc(200 * sizeof(char));
    int *r = (int *)My_Alloc(1000* sizeof(int));
    
    char *w = (char *)My_Alloc(700);
     My_Free(w);
   
    int *k = (int *)My_Alloc(500 * sizeof(int));
    My_Free(p);
    My_Free(r);
    int *l = (int *)My_Alloc(500 * sizeof(int));
  
    int *x = (int *)My_Alloc(500 * sizeof(int));
    int *a = (int *)My_Alloc(500 * sizeof(int));
    
    int *s = (int *)My_Alloc(500 * sizeof(int));
    int *h = (int *)My_Alloc(500 * sizeof(int));
    int *ui = (int *)My_Alloc(500 * sizeof(int));
    int *hp = (int *)My_Alloc(500 * sizeof(int));
     My_Free(h);
     
     int *hl = (int *)My_Alloc(500 * sizeof(int));
    
    return 0;
}