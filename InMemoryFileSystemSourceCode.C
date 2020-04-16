//Required header files
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<dos.h>

//Variable to count the number of datablocks allocated
int mem_count=0;

//Global temporary variable
int univ;

//Datablock structure
struct datablock
	{
	  char data[1024];           //Field to store filedata
	  struct datablock *next;    //Field to store next datablock address
	}*tmpstruc=NULL,*tmpstruc1=NULL,*tmpstrucv=NULL,*tmpstruc11=NULL,*tmpstruc12,*tmpstruc13;

//Declaration for type boolean
typedef enum
	{
	  FALSE,
	  TRUE
	}boolean;

//Structure of inode
struct inode
	{
	   int node;         //Stores inode number of the file
	   int type;         //Stores file type: (0 for regular/1 for directory)
	   int size;         //Stores file size
	   struct date timestamp;
	   int time[3];            //Stores file's creation/access time
	   struct datablock *ptr;  //Links to the datablocks associated with the file
	}arr_inode[128];

//Structure for incore inode
struct incore_inode
	{
	   //All the variables stores value for the file currently in memory
	   int node;   //Stores inode number of the file
	   int type;   //Stores file type: (0 for regular/1 for directory)
	   int size;   //Stores file size
	   struct date timestamp;
	   int time[3];            //Stores file's creation/access time
	   struct datablock *ptr;  //Links to the datablocks associated with the file
	   boolean lock;           // Is set true if currently that file is being accessed else set false
	   int inode_no;
	};

//Function to open a file present in in-memory
void open_file()
	{
	  struct incore_inode obj1;
	  struct time t;
	  int i;
	  int i_num; //Variable to take inode number of the file as input which user wants to view
	  printf("File currently present in the in-memory file system: \n");
	  printf("inode no.   File type   File size   Creation time/Last accessed time\n");
	  for(i=0;i<128;i++)    //Loops through all the inodes present in the memory
	    {
	      if(arr_inode[i].node==0)  //If no inode is present, loop terminates
		{
		  break;
		}
	      else
		{
		  if(arr_inode[i].node==univ)    //Displays list of inodes present in in-memory
			{
			   printf("   %d",arr_inode[i].node);
			   printf("  \t\t%d",arr_inode[i].type);
			   printf("   \t%d  ",arr_inode[i].size);
			   printf("          \t%d:%d:%d",arr_inode[i].time[0],arr_inode[i].time[1],arr_inode[i].time[2]);
			   printf("\n");
			 }
		}
	    }
	  printf("\nEnter file's inode no. which you want to read: ");     //Asks for file's inode no. which the user wants to open
	  scanf("%d",&i_num);
	  for(i=0;i<128;i++)
		{
		   if(arr_inode[i].node==i_num)   //Displays that file's contents which corresponds to entered inode no.
		     {
			     //Loads in-core inode for this file and updates its value
			     obj1.lock=TRUE;                 //Sets a lock on this file
			     obj1.node=arr_inode[i].node;
			     obj1.type=arr_inode[i].type ;
			     obj1.size=arr_inode[i].size;
			     gettime(&t);
			     obj1.time[0]=t.ti_hour;
			     obj1.time[1]= t.ti_min;
			     obj1.time[2]= t.ti_sec;
			     tmpstrucv=arr_inode[i_num].ptr->next;
			     while(tmpstrucv!=NULL)                   //Displays the contents of the file
				 {
				   printf("%s",tmpstrucv->data);
				   tmpstrucv=tmpstrucv->next;
				 }
			     arr_inode[i].time[0]=obj1.time[0];       //Updates file access/modification time
			     arr_inode[i].time[1]= obj1.time[1] ;
			     arr_inode[i].time[2]=  obj1.time[2];
			     obj1.lock=FALSE;          //Unlocks the file
		     }
	  }
	  printf("\nPress enter to close file..");
	  close();
	  getch();
	}

//Function to close the file that is currently opened
void close()
	{
	   free(tmpstrucv);  //It deallocates the memory allocated to the datablocks of the file
	}

//Function to create a file in the in-memory file system
void create_file()
  {
    int sz=0;	//Temporary variable to store the size of the file
    int i;
    int k=0;	//Temporary variable to store current inode number
    struct time t; //Variable for timestamp
    char ch='k';
    char nch='C';
    char tmp[1024]=" ";
    int ptr=0;

    //Assigning inode number to this new file
    for(i=0;i<128;i++)    //Searches for available location in inode list
	{
	  if(arr_inode[i].node==0)
		{
		  k=i;    //Stores this available location in k
		  break;
		}
	}
    univ=(k+1)*6+2;
    arr_inode[k].node=(k+1)*6+2;   //Creates an inode number for this new file

    //Assigning file type
    arr_inode[k].type=0;

    //Assigning timestamp
    gettime(&t);                   //Gets current system time
    arr_inode[k].time[0]=t.ti_hour;     //Assigns file creation time to the inode
    arr_inode[k].time[1]= t.ti_min;
    arr_inode[k].time[2]= t.ti_sec;

    /*
      // Entering file data
      For it, first a datablock is created
      Block structure=>
	     Boot block = Block 0 (Total no. of blocks=1)
	     Super block = Block 1 to 3 (Total no. of blocks=3)
	     Inode list = Block 4 to 17 (Total no. of blocks=13)
      Now, the total number of blocks allocated = 16384 blocks
      Therefore total no. of remaining blocks = 16367 which can be allocated as data blocks
    */
    tmpstruc=(struct datablock *)malloc(sizeof(struct datablock));  //Memory for data blocks can be allocated 16367 times
    tmpstruc->next=NULL;
    mem_count++;  //Incrementing memory count after allocating memory
    arr_inode[k].ptr->next=tmpstruc;   //Storing first datablock address in the inode list
    sz=sz+sizeof(tmpstruc);       //Computing datablock size
    printf("\nYou can now enter file data. Enter 'Q' to end writing... Start typing..\n");
    ch=getchar();
    while(ch!='Q')
      {
	 if(ptr<1024)         //Accepting data for first datablock
	   {
	      tmp[ptr]=ch;
	      ptr++;
	   }
	 else                //If datablock size exceeds, a new datablock is created
	   {
	      strcpy(tmpstruc->data,tmp);
	      if(mem_count>16365)  //Checks whether memory is available or not. If no memory is available, an error message is displayed
		   {
		     printf("Memory unavailable! This much data entered in available space.. Enter 'Q' to return to main menu: ");
		   }
	      else     

//Creation of new datablock when memory is available
		   {
		     tmpstruc1=(struct datablock *)malloc(sizeof(struct datablock));
		     tmpstruc1->next=NULL;
		     mem_count++;
		     tmpstruc->next=tmpstruc1;
		     tmpstruc=tmpstruc1;
		     ptr=0;
		     sz=sz+sizeof(tmpstruc);     //Size updation
		   }
	   }
	 ch=getchar();
      }
    strcpy(tmpstruc->data,tmp);
    sz=sz+sizeof(tmpstruc);
    arr_inode[k].size=sz;
    printf("\nWarning! This file isn't written to disk file system but is still present in in-memory. If you close the program all changes will be lost (TO VIEW FILE PRESS 'V').. Press 'C' to continue..\n");
    nch=getchar();
    ter1:
    nch=getchar();
    if(nch=='C')
      {
	getch();
      }
    else if(nch=='V')
      {
	read_file();
      }
    else
      {
	printf("\nEnter correct choice: C-Continue/V-View in-memory file\nYour choice: ");
	goto ter1;
      }
  }

//Function to read a file
void read_file()
	{
	  struct incore_inode obj1;
	  struct time t;
	  int i;
	  int i_num; //Variable to take inode number of the file as input which user wants to view
	  printf("File currently present in the in-memory file system: \n");
	  printf("inode no.   File type   File size   Creation time/Last accessed time\n");
	  for(i=0;i<128;i++)    //Loops through all the inodes present in the memory
	    {
	      if(arr_inode[i].node==0)  //If no inode is present, loop terminates
		{
		  break;
		}
	      else
		{
		  if(arr_inode[i].node==univ)    //Displays list of inodes present in in-memory
			{
			   printf("   %d",arr_inode[i].node);
			   printf("  \t\t%d",arr_inode[i].type);
			   printf("   \t%d  ",arr_inode[i].size);
			   printf("          \t%d:%d:%d",arr_inode[i].time[0],arr_inode[i].time[1],arr_inode[i].time[2]);
			   printf("\n");
			 }
		}
	    }
	  printf("\nEnter file's inode no. which you want to read: ");     //Asks for file's inode no. which the user wants to open
	  scanf("%d",&i_num);
	  for(i=0;i<128;i++)
		{
		   if(arr_inode[i].node==i_num)   //Displays that file's contents which corresponds to entered inode no.
		     {
			     //Loads in-core inode for this file and updates its value
			     obj1.lock=TRUE;                 //Sets a lock on this file
			     obj1.node=arr_inode[i].node;
			     obj1.type=arr_inode[i].type ;
			     obj1.size=arr_inode[i].size;
			     gettime(&t);
			     obj1.time[0]=t.ti_hour;
			     obj1.time[1]= t.ti_min;
			     obj1.time[2]= t.ti_sec;
			     tmpstrucv=arr_inode[i_num].ptr->next;
			     while(tmpstrucv!=NULL)                   //Displays the contents of the file
				 {
				   printf("%s",tmpstrucv->data);
				   tmpstrucv=tmpstrucv->next;
				 }
			     arr_inode[i].time[0]=obj1.time[0];       //Updates file access/modification time
			     arr_inode[i].time[1]= obj1.time[1] ;
			     arr_inode[i].time[2]=  obj1.time[2];
			     obj1.lock=FALSE;          //Unlocks the file
		     }
	  }
	  printf("\nPress enter to close file..");
	  close();
	  getch();
	}

//Function containing sample files for merge operation
//This function is used only for demonstration of module 6
void spcl()    //Their are three sample files upon which merge operation can be performed
 {
   int i;
   struct datablock *mtm;
   char mar[1024]="Helloo..",mar1[1024]="You know?",mar2[1024]="This program is developed in C language..";
   tmpstruc11=(struct datablock *)malloc(sizeof(struct datablock));
   tmpstruc12=(struct datablock *)malloc(sizeof(struct datablock));
   tmpstruc13=(struct datablock *)malloc(sizeof(struct datablock));
   arr_inode[0].node=13;
   arr_inode[0].type=0;
   arr_inode[0].size=6;
   arr_inode[0].time[0]=19;
   arr_inode[0].time[1]=26;
   arr_inode[0].time[2]=43;
   for(i=0;i<1024;i++)
	   tmpstruc11->data[i]=mar[i];
   arr_inode[0].ptr->next=tmpstruc11;
   printf("\n   %d",arr_inode[0].node);
   printf("  \t\t%d",arr_inode[0].type);
   printf("   \t%d  ",arr_inode[0].size);
   printf("          \t%d:%d:%d",arr_inode[0].time[0],arr_inode[0].time[1],arr_inode[0].time[2]);
   printf("\nFile data: %s",tmpstruc11->data);
   arr_inode[1].node=29;
   arr_inode[1].type=0;
   arr_inode[1].size=6;
   arr_inode[1].time[0]=19;
   arr_inode[1].time[1]=29;
   arr_inode[1].time[2]=32;
   for(i=0;i<1024;i++)
	   tmpstruc12->data[i]=mar1[i];
   arr_inode[1].ptr->next=tmpstruc12;
   printf("\n   %d",arr_inode[1].node);
   printf("  \t\t%d",arr_inode[1].type);
   printf("   \t%d  ",arr_inode[1].size);
   printf("          \t%d:%d:%d",arr_inode[1].time[0],arr_inode[1].time[1],arr_inode[1].time[2]);
   printf("\nFile data: %s",tmpstruc12->data);
   arr_inode[2].node=34;
   arr_inode[2].type=0;
   arr_inode[2].size=6;
   arr_inode[2].time[0]=20;
   arr_inode[2].time[1]=13;
   arr_inode[2].time[2]=17;
   for(i=0;i<1024;i++)
	   tmpstruc13->data[i]=mar2[i];
   arr_inode[2].ptr->next=tmpstruc13;
   printf("\n   %d",arr_inode[2].node);
   printf("  \t\t%d",arr_inode[2].type);
   printf("   \t%d  ",arr_inode[2].size);
   printf("          \t%d:%d:%d",arr_inode[2].time[0],arr_inode[2].time[1],arr_inode[2].time[2]);
   printf("\nFile data: %s",tmpstruc13->data);
 }

//Function to merge files
void merge_file()
    {
      int i;
      int i_num1; //Variable to take inode number of the first file as input
      int i_num2; //Variable to take inode number of the second file as input
      int i_num3; //Variable containing inode number of the resulting file 3
      int sz=0;	  //Temporary variable to store the size of the file 3
      char mych='V';
      struct time t; //Variable for timestamp
      struct datablock *u1=NULL,*u2=NULL,*u11=NULL,*u12=NULL,*mjk=NULL;
      mjk=(struct datablock *)malloc(sizeof(struct datablock));
      printf("Sample files present in the in-memory file system for merge operation: \n");
      printf("inode no.   File type   File size   Creation time/Last accessed time\n");

      //On integrating this module with the remining modules, this spcl() will not be used. This function is only for demonstrating our module.
      spcl();  //Displays sample files present in in-memory

      /*
      This part of code is used in integration with other modules of the file system
      for(i=0;i<128;i++)
	    {
	      if(arr_inode[i].node==0)    //If no inode is present, loop terminates
		{
		  break;
		}
	      else      //Displays available inode details
		{
		   printf("   %d",arr_inode[i].node);
		   printf("  \t\t%d",arr_inode[i].type);
		   printf("   \t%d  ",arr_inode[i].size);
		   printf("          \t%d:%d:%d",arr_inode[i].time[0],arr_inode[i].time[1],arr_inode[i].time[2]);
		   printf("\n");
		 }
	     }
     */

      //Accepting inode no.s of files to be merged from user
      myd1:
      printf("\nEnter FIRST file's inode no. which you want to read: ");
      scanf("%d",&i_num1);
      if(i_num1!=13 && i_num1!=29 && i_num1!=34)
	 {
	    printf("\nError! File does not exist.. Enter correct file name ");
	    goto myd1;
	 }
      myd2:
      printf("\nEnter SECOND file's inode no. which you want to read: ");
      scanf("%d",&i_num2);
      if(i_num2!=13 && i_num2!=29 && i_num2!=34)
	 {
	    printf("\nError! File does not exist.. Enter correct file name ");
	    goto myd2;
	 }

      //Assigning inode number to file 3
      for(i=0;i<128;i++)    //Searches for available location in inode list
	{
	  if(arr_inode[i].node==0)
	      {
		  i_num3=i; //Stores this available location in k
		  break;
	      }
	}
      univ=(i_num3+1)*6+2;
      arr_inode[i_num3].node=(i_num3+1)*6+2;   //Creates an inode number for resultant file

      //Assigning file type to file 3
      arr_inode[i_num3].type=0;

      //Assigning timestamp to file 3
      gettime(&t);                      //Gets system time
      arr_inode[i_num3].time[0]=t.ti_hour;        //Assigns timestamp to this new file
      arr_inode[i_num3].time[1]=t.ti_min;
      arr_inode[i_num3].time[2]=t.ti_sec;

      //Allocating first datablock to file 3
      if(mem_count>16365)      //Checks whether memory is available or not
	 {
	    printf("Memory unavailable! This much data entered in available space.. Enter 'Q' to return to main menu: ");
	 }
      u1=(struct datablock *)malloc(sizeof(struct datablock));  //Memory allocation for data block
      u1->next=NULL;
      mem_count++;  //Incrementing memory count after allocating memory
      arr_inode[i_num3].ptr->next=u1;
      sz=sz+sizeof(u1);

      //Reading file 1 and adding its contents to file 3
      //This code is used for demonstration of module 6 only
      if(i_num1==13)
	mjk=tmpstruc11;
      else if(i_num1==29)
	mjk=tmpstruc12;
      else if(i_num1==34)
	mjk=tmpstruc13;
      u11=mjk;
      while(u11!=NULL)
	  {
	     strcpy(u1->data,u11->data);
	     u1->next=u11->next;
	     u11=u11->next;
	  }

      /* Code to be used in integration with other modules
	 for(i=0;i<128;i++)
	    {
		if(arr_inode[i].node==i_num1)
		    {
			u2=(struct datablock *)malloc(sizeof(struct datablock));
			u2=arr_inode[i_num1].ptr->next;
			printf("next  %s",arr_inode[i_num1].ptr->next);
			while(u2->next!=NULL)
			    {
			       printf("%s",tmpstrucv->data);
			       printf("%s",u2->data);
			       strcpy(u1->data,u2->data);
			       printf("%s",u1->data);
			       if(mem_count>16365)
				   {
				       printf("Memory unavailable! This much data entered in available space.. Enter 'Q' to return to main menu: ");
				   }
			       else
				   {
				       u11=(struct datablock *)malloc(sizeof(struct datablock));  //memory for data blocks can be allocated 16367 times
				       u11->next=NULL;
				       mem_count++;  //Incrementing memory count after allocating memory
				       u1->next=u11;
				       u1=u11;
				       u2=u2->next;
				   }
			    }
		    }
	    }
      */

      //Reading file 2 and adding contents to file 3
      //This code is used for demonstrating module 6 only
      if(i_num2==13)
	  mjk=tmpstruc11;
      else if(i_num2==29)
	  mjk=tmpstruc12;
      else if(i_num2==34)
	  mjk=tmpstruc13;
      if(mem_count>16365)
	  {
	     printf("Memory unavailable! This much data entered in available space.. Enter 'Q' to return to main menu: ");
	  }
      u12=(struct datablock *)malloc(sizeof(struct datablock));
      u12->next=NULL;
      u1->next=u12;
      u11=mjk;
      while(u11!=NULL)
	  {
		   strcpy(u12->data,u11->data);
		   u12->next=u11->next;
		   u11=u11->next;

	  }
      //Code to be used in integration with other modules

      /*for(i=0;i<128;i++)
      	 {
         		if(arr_inode[i].node==i_num2)
         		     {
         			u2=(struct datablock *)malloc(sizeof(struct datablock));
         			u2=arr_inode[i_num2].ptr->next;
         			printf("next  %s",arr_inode[i_num2].ptr->next);
      	       		while(u2->next!=NULL)
      		 		{
      					printf("%s",tmpstrucv->data);
      	   				printf("%s",u2->data);
      	   				strcpy(u1->data,u2->data);
      	   				printf("%s",u1->data);
        					if(mem_count>16365)
      	   					{
           							printf("Memory unavailable! This much data entered in available space.. Enter 'Q' to return to main menu: ");
         						}
      					else
      						{
       							 u11=(struct datablock *)malloc(sizeof(struct datablock));  //memory for data blocks can be allocated 16367 times
      							 u11->next=NULL;
      	  						 mem_count++;  //Incrementing memory count after allocating memory
      	   						 u1->next=u11;
      	   						 u1=u11;
         							 u2=u2->next;
        							 getch();
      	 					}
      				}
            		     }
      */	 }
      printf("\nFiles merged.. Press  ENTER to continue: ");
      getch();
    }

void main()
	{
	  int choice=0;	//Variable for taking user's choice as input
	  char uch='y';	//Variable to loop until user wants to continue
	  while(uch=='y')
	    {
	      clrscr();
	      printf("\nPress '1' to create a new file in the in-memory file system & write some data into it\nPress '2' to read from a file in the in-memory file system\nPress '3' to merge two files & store it as a new file in the in-memory file system\nYour choice: ");
	      scanf("%d",&choice);
	      switch(choice)
		{
		  case 1: clrscr();
			  create_file();
			  break;
		  case 2: clrscr();
			  read_file();
			  break;
		  case 3: clrscr();
			  merge_file();
			  break;
		  default: printf("Incorrect choice!");
			   getch();
		}
	      clrscr();
	      printf("Do you want to continue(y/n): ");
	      scanf("%s",&uch);
	    }
	}