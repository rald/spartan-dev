#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "ezxml.h"


int main() {

  ezxml_t bible = ezxml_parse_file("kjv.xml"),book,chap,vers;

  char *bname=NULL;
  size_t bnum=0;
  size_t cnum=0,vnum=0;
  char *text=NULL;

  size_t pbnum=0,pcnum=0;

  char folder[PATH_MAX];
  char url[PATH_MAX];


  FILE *fp;

  for (book = ezxml_child(bible, "BIBLEBOOK"); book; book = book->next) {
    for (chap = ezxml_child(book, "CHAPTER"); chap; chap = chap->next) {
      for (vers = ezxml_child(chap, "VERS"); vers; vers = vers->next) {

        bname =strdup(ezxml_attr(book,"bname"));
        bnum = atoi(ezxml_attr(book,"bnumber"));
        cnum = atoi(ezxml_attr(chap,"cnumber"));
        vnum = atoi(ezxml_attr(vers,"vnumber"));
        text = strdup(ezxml_txt(vers));

        strcpy(url,bname);
        char *p=NULL;
        while((p=strchr(url,' '))) {
          *p='_';
        }

        if(bnum!=pbnum) {

          if(bnum==1) {
            fp=fopen("bible/index.gmi","a");

            fprintf(fp,"#King James Version\n\n\n");
            fprintf(fp,"##Old Testament\n\n");

            fclose(fp);

          } else if(bnum==40) {

            fp=fopen("bible/index.gmi","a");

            fprintf(fp,"\n\n\n##New Testament\n\n");
            fclose(fp);
          }

          sprintf(folder,"bible/%s",url);

          if (mkdir(folder, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            printf("Error: %s\n", strerror(errno));
          }

          fp=fopen("bible/index.gmi","a");

          fprintf(fp,"=> %s/index.gmi %s\n",url,bname);

          fclose(fp);

        }



        if(bnum!=pbnum || cnum!=pcnum) {

          if(cnum==1) {
            sprintf(folder,"bible/%s/index.gmi",url);
            fp=fopen(folder,"a");
            fprintf(fp,"#%s\n",bname);
            fclose(fp);
          }

          sprintf(folder,"bible/%s/index.gmi",url);
          fp=fopen(folder,"a");
          fprintf(fp,"=> Chapter_%zu.gmi Chapter %zu\n",cnum,cnum);
          fclose(fp);
        }


        sprintf(folder,"bible/%s/Chapter_%zu.gmi",url,cnum);

        fp=fopen(folder,"a");

        if(vnum==1) {
          fprintf(fp,"#%s\n##Chapter %zu\n\n",bname,cnum);
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

  return 0;
}

