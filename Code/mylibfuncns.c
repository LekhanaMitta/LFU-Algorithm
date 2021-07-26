#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<errno.h>
#include "myheader.h"

typedef struct {
  int frm_no;
  int valid_bit;
  int modify_bit;
  int reference_bit;
}pageItem;



int pagesize = rec_per_page * RECORDSIZE;
char * Frames[no_of_frames];
int fd;
int FrameTable[no_of_frames];
int page_fault_count = 0;
pageItem PageTable[PTABLESIZE];
int counter; 
int time[100];
									/*array that is implemented to know at what particular instant a particular frame is being modified (or) which is used when 										     frequency of frames are same but we need to follow FIFO principle */
int frequency[no_of_frames];  						/*array which is implemented to know how many times a particular frame is modified*/
int pos;								/*pos determines the frame to which a new entry has to be read i.e., which satisfies low frequency , oldest entered*/


/*  **********************    FUNCTIONS WRITTEN BY STUDENT    ************************************ */

/* REWRITE THIS FUNCTION PROPERLY   */
int findLFU(int time[],int n, int freq[])
{
	int min_freq = freq[0];						/*assigning a random value for min_freq i.e., minimum frequency */
	int min_time = 1000;						/*assigning a random value for min_time i.e., minimum time or oldest entry*/
	int pos = 0;							/*pos determines the frame to which a new entry has to be read i.e., which satisfies low frequency , oldest entered*/
	
	for (int i=0; i<n; i++)						/*finding the frame which is less frequently used*/ 
	{
		if (freq[i]<min_freq)
		{
			min_freq = freq[i];
		}
	}
	
	for (int i=0; i<n; i++)						/*finding the frame which is first used when frequencies of two frames are same*/
	{
		if (time[i] < min_time && min_freq == freq[i])
		{
			pos = i;
			min_time = time[i];
		}

	}
	freq[pos] = 1;		
	return FrameTable[pos];						/*returning victim page no */
}

/******Function used to print the frequency of a particular frame ******/
void printFreq()
{
	for (int i=0;i<no_of_frames;i++)
	{
		printf(">%d > frame %d \n",frequency[i],i);
	}
	printf("\n");
	printf("The Page Fault Count = %d\n\n",page_fault_count);
}


int getFrameNo(int pno)
{
  int fno, var1=0, var2=0, pos;

  /*  Scan through the page table to check if the page is present in memory */
  
  if (PageTable[pno].valid_bit == 1)
  {
	  fno = PageTable[pno].frm_no;		  		/* If found, simply return the corresponding frame no.  */
	  counter++;						/* as soon as a entry we increment the counter which represents the time at which that frame is used*/
	  time[fno] = counter;	
	  var1 = 1;	
	  var2 = 1;
	  frequency[fno]++;					/*frequency of that frame is increased since it is accessed for more time*/
	  return fno;
  }

  /* If not found, find a free frame-no. by searching the frame table */
  /* If found, read the requested page into the free frame, and then return the corresponding frame no. */
  
  if (var1 == 0)
  {
	  for(int i=0; i<no_of_frames; i++)
	  {
		  if (FrameTable[i] == -1)
		  {
			counter++;	 			/* as soon as a entry we increment the counter which represents the time at which that frame is used*/
			time[i] = counter;
			fno = i;				/*the empty frame no. is to be returned*/
			var2 = 1;				/*since already there is an empty frame we need not go to a loop where we go when there is no free frame*/
			PageTable[pno].valid_bit = 1;		/*as that page no. is present in memory we update valid_bit to 1 */
			PageTable[pno].frm_no = fno;		/*updating the page table where a particular page is present in frame table*/
			readPage(pno,fno);			/*reading that particular page into memory */
			frequency[i]++;				/*frequency of that particular frame is being increased */
			break;		
		  }
	  }
  }
  if (var2 == 0)
  {
	  int victim_pno;
	  victim_pno = findLFU(time,no_of_frames,frequency);	 /* If no free frame found, invoke your Page-Replacement-Algorithm to find victim page-no */
	  fno = PageTable[victim_pno].frm_no;	                 /*Scan through the page table to get the frame-no. corresponding to the victim page-no */
	  PageTable[victim_pno].valid_bit = 0;	                 /*since the victim page is removed from frame i.e., from memory so we have to update valid_bit =0 for that page */
	  PageTable[victim_pno].frm_no = -1;	                 /*since the page is removed from the frame it becomes empty which is represented by frm_no=-1*/
	  PageTable[pno].valid_bit = 1;			         /*then the new page will be entered into the recently emptied frame (which is before occupied by victiom page*/
	  PageTable[pno].frm_no = fno;				 /*valid_bit =1 updated for new entry and page table for frame no is also updated */
	  counter++;						 /* as soon as a entry we increment the counter which represents the time at which that frame is used*/
	  time[fno] = counter;
	  if (PageTable[victim_pno].modify_bit == 1)		 /*Write the victim page back into the disk-file, IF modify_bit is set */
		  writeFrame(fno, victim_pno);
	  readPage(pno, fno);					 /*the new entry page is read into memory*/
  }
  return fno;							 /* Read the requested page into the freed frame, and then return the corresponding frame no. */
}

/*  ***************************************  FUNCTION DEFINITIONS GO HERE  *******************************      */
/******************************************************************************************************************/
/******************************************************************************************************************/
/******************************************************************************************************************/

/* Initialization Function: Initializes all aspects */
void Initialize()
{
  int i;

  /*  Allocating Memory to Frames, and initializing the FrateTable */

  for(i = 0; i < no_of_frames; i++)
  {
    Frames[i] = (char *) calloc(pagesize, sizeof(char));
    FrameTable[i] = -1; // -1 indicates frame is empty
  }

  /* Initializing the PageTable */
  
  for(i = 0; i < PTABLESIZE; i++)
  {
    PageTable[i].frm_no = -1;
    PageTable[i].valid_bit = 0;
    PageTable[i].modify_bit = 0;
    PageTable[i].reference_bit = 0;
  }
  
  /*Initializing the Frequency table for frames*/
  
  for (int i=0; i<no_of_frames; i++)
  {
	frequency[i] = 0;
  }

  /*  Opening the Data File  */
  
  fd = open("./student-data.csv", O_RDWR );
  if(fd<0)
  {
	printf("Error Number % d\n", errno);
     	perror("Program");
	exit(1);
  }
}

/* Finalization Function: Finalizes all aspects */

void Finalize()
{
  int i;

  /* Write back all non-empty frames to file, and freeing the memory allocated to all frames */

  for(i = 0; i < no_of_frames; i++)
  {
    if(FrameTable[i] != -1)
    {
      writeFrame(i, FrameTable[i]);
    }
    free(Frames[i]);
  }
  
  /* Closing the Data File */
  
  close(fd);
  if(fd<0)
	  printf("Error! Could not close the data file...\n");
}

/* Reads specified page from disk-file to the specified Frame in memory */
void readPage(int pno, int frm_no)
{
  lseek(fd, (pno*pagesize), SEEK_SET);
  read(fd, Frames[frm_no], pagesize);
  FrameTable[frm_no] = pno;
  page_fault_count++;
}

/* Writes the contents of the specified frame in memory to the specified page of the disk-file */
void writeFrame(int frm_no, int pno)
{
  int res;
  lseek(fd, (pno*pagesize), SEEK_SET);
  res = write(fd, Frames[frm_no], pagesize);
  if(res<0)
  {
    perror("Write Error.");
    exit(1);
  }
  PageTable[pno].modify_bit = 0;
}

/* Displays the contents of the specified memory-frame */
void printFrame(int frm_no)
{
  int i;
  for(i=0; i < pagesize; i++)
  {
    printf("%c",Frames[frm_no][i]);
  }
  printf("\n");
}

/* Translates the specified slno (record-number) to its corresponding page no. and page offset values */
void getPageDetails(int slno, int *pno, int *offset)
{
  int q, r;
  q = (slno-1)/rec_per_page;
  r = (slno-1) - (rec_per_page*q);
  *pno = q;
  *offset = r;
}

/* Displays the RECORD contained at the specified offset of the specified memory-frame */
void printRecord(int frm_no, int offset)
{
  int i, startindx, endindx;
  startindx = offset*RECORDSIZE;
  endindx = startindx + RECORDSIZE;
  for(i = startindx; i < endindx; i++)
  {
    printf("%c",Frames[frm_no][i]);
  }
  printf("\n");
}


/* Updates the Name Field of the RECORD contained at the specified offset of the specified memory-frame, with the given string */
void updateRecord(int frm_no, int offset, char *string)
{
  int i, startindx, endindx;
  if(strlen(string) != 4)
  {
    printf("Length of the input string must be equal to 4.\n");
    exit(0);
  }
  else
  {
	PageTable[FrameTable[frm_no]].modify_bit = 1;
        startindx = (offset*RECORDSIZE) + 14;
        for(i = 0; i < 4; i++) 
		Frames[frm_no][startindx++] = string[i];
  }
}
