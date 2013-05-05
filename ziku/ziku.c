#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
void main()
{
    FILE *fd ;
    size_t       length;
    char filename[]="//Users//macbookpro//d/d//soft//hand//ziku//zikuGB.txt";
    fd = fopen(filename,"r");
    char buf[700000];
    
    char hz[30000][100];
    char hzhz[30000][4];
    memset(hz, 0,30000*100);
    memset(hzhz, 0,30000*4);
     int i=0;
     int m=0;
  
    if(fd !=  0)
    {
        fseek(fd,0L,SEEK_END);
        length = ftell(fd);
        int imaxcnt=0;
        int ir=0;
        fseek(fd,0L,SEEK_SET);
        ir = fread(buf, length, 1, fd);
       
        if(ir == 1)
        {
            buf[length]=0;
            char *q=buf;
            
            int istep=0;
            while(i<length)
            {
  			    
                if(q[0]==0x0D )
                {
                   i = i +2;
                   q = q+2;
                }
         
                else if(q[0]>=0X31 && q[0]<=0X35)
                {
                   i++;
                   q++;
                   continue;
                }
                else 
                {
                  strncpy(hzhz[m++],q,2);
                  q = q+2;
                  i =i+2;
                }
   
            }
          
         }
        }
        fclose(fd);
        int j=0,k=0,icnt=0;;
        int maxlen=0,imax=0;
        /*
        for(j=0;j<i;++j)
        {
             for( k=j+1;k<i;++k)
             {
                 if(strcmp(hz[j],hz[k])==0)
                 {
                 	  
                      printf("[%d] hz[%d]=%s－－－",icnt++,j+1,hz[j]);
                      printf("hz[%d]=%s\n",k+1,hz[k]);
                      
                 }
             }
        }
        
        
        for(j=0;j<i;++j)
        {
             if(strlen(hz[j])>maxlen)
             {
             	maxlen =strlen(hz[j]);
             	imax = j;
             }
        }
        printf("笔画最多的汉字是：hz[%d]=%s\n",imax+1,hz[imax]);
        */
        
        for(j=0;j<m;j++)
       	{
       		wprintf("［%s］\n",hzhz[j]);
       	}

   return;
}