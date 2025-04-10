#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "ezxml.h"


int main() {

  ezxml_t bible = ezxml_parse_file("adb.xml"),book,chap,vers;

  char *bname=NULL;
  size_t bnum=0;
  size_t cnum=0,vnum=0;
  char *text=NULL;

  size_t pbnum=0,pcnum=0;

  char folder[PATH_MAX];
  char url[PATH_MAX];

  FILE *fp;

  char *books[66];
  char line[256];
  size_t bctr=0;

  fp=fopen("books","r");

  while(fgets(line,256,fp)) {
    char *p=strchr(line,'\n');
    if(p) *p='\0';
    books[bctr++]=strdup(line);
  }

  fclose(fp);


  for (book = ezxml_child(bible, "BIBLEBOOK"); book; book = book->next) {
    for (chap = ezxml_child(book, "CHAPTER"); chap; chap = chap->next) {
      for (vers = ezxml_child(chap, "VERS"); vers; vers = vers->next) {

        bname =strdup(ezxml_attr(book,"bname"));
        bnum = atoi(ezxml_attr(book,"bnumber"));
        cnum = atoi(ezxml_attr(chap,"cnumber"));
        vnum = atoi(ezxml_attr(vers,"vnumber"));
        text = strdup(ezxml_txt(vers));

        strcpy(url,books[bnum-1]);
        char *p=NULL;
        while((p=strchr(url,' '))) {
          *p='_';
        }

        if(bnum!=pbnum) {

          if(bnum==1) {
            fp=fopen("bible/index.gmi","a");

            fprintf(fp,"#Ang Dating Biblia\n\n\n");
            fprintf(fp,"##Ang Lumang Tipan\n\n");

            fclose(fp);

          } else if(bnum==40) {

            fp=fopen("bible/index.gmi","a");

            fprintf(fp,"\n\n\n##Ang Bagong Tipan\n\n");
            fclose(fp);
          }

          sprintf(folder,"bible/%s",url);

          if (mkdir(folder, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            printf("Error: %s\n", strerror(errno));
          }

          fp=fopen("bible/index.gmi","a");

          fprintf(fp,"=> %s/index.gmi %s\n",url,books[bnum-1]);

          fclose(fp);

        }



        if(bnum!=pbnum || cnum!=pcnum) {

          if(cnum==1) {
            sprintf(folder,"bible/%s/index.gmi",url);
            fp=fopen(folder,"a");
            fprintf(fp,"#%s\n",books[bnum-1]);
            fclose(fp);
          }

          sprintf(folder,"bible/%s/index.gmi",url);
          fp=fopen(folder,"a");
          fprintf(fp,"=> Kabanata_%zu.gmi Kabanata %zu\n",cnum,cnum);
          fclose(fp);
        }


        sprintf(folder,"bible/%s/Kabanata_%zu.gmi",url,cnum);

        fp=fopen(folder,"a");

        if(vnum==1) {
          fprintf(fp,"#%s\n##Kabanata %zu\n\n",books[bnum-1],cnum);
        }

        fprintf(fp,"%zu %s\n\n",vnum,text);

        fclose(fp);

        free(bname);
        bname=NULL;

        free(text);
        text=NULL;

        pbnum=bnum;
        pcnum=cnum;

      }
    }
  }

  ezxml_free(book);

  for(int i=0;i<bctr;i++) {
    free(books[i]);
  }

  return 0;
}

