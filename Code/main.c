#include<stdio.h>
#include<stdlib.h>
#include "myheader.h"

int main(){
  int i, pno, fno, offset, recno;
  char ch;
  char str[5];

  Initialize();
  while(1)
  {
      printf("Enter Record No:- ");
      scanf("%d", &recno);
      if((recno < 0) || (recno > 94))
      {
        printf("Invalid Record Number !!! \n");
        break;
      }
      getPageDetails(recno, &pno, &offset);
      printf("PAGE NO. = %d\nPAGE OFFSET= %d\n", pno, offset);
      fno = getFrameNo(pno);
      printf("Printing Frame %d Before the update:-\n", fno);
      printFrame(fno);
      printf("Details of the Requested Record:-\n");
      printRecord(fno, offset);
      printf("Do you want to update the record (Y/N): ");
      scanf(" %c", &ch);
	  printf("\n");
	  printf("\n");
	  printf("\n");
      if((ch == 'Y') || (ch == 'y'))
      {
        printf("Enter the new name (4 characters):-");
        scanf(" %[^\n]%*c", str);
        updateRecord(fno, offset, str);
        printf("Printing Frame %d After the update:-\n", fno);
        printFrame(fno);
        printf("Details of the Record after Updaed :-\n");
        printRecord(fno, offset);
      }
	  
	  for (int i=0; i<no_of_frames; i++)
	  {
		  printf("Frame %d contents:-\n",i);
		  printFrame(i);
	  }
	  printf("\n");
	  
	  printf("The frequencies of the Frames:- \n");
	  printFreq();
  }
  Finalize();
}
