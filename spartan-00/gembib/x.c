#define _GNU_SOURCE

#include "ezxml/ezxml.h"



#define PASSAGE_IMPLEMENTATION
#include "passage.h"

#define INFO_IMPLEMENTATION
#include "info.h"



typedef struct Book Book;

struct Book {
  char *bname;
  size_t bnum;
};



size_t countBooks(Passage **passages,size_t npassages);

size_t countChapters(Passage **passages,size_t npassages,size_t bnum);

size_t countVerses(Passage **passages,size_t npassages,size_t bnum,size_t cnum);



void getBooks(Book ***books,size_t *nbooks,Passage **passages,size_t npassages);



Book *Book_New(char *bname,size_t bnum);
void Book_Free(Book **book);
void Books_Free(Book ***books,size_t *nbooks);
void Book_Append(Book ***books,size_t *nbooks,Book *book);


Book *Book_New(char *bname,size_t bnum) {
  Book *book=malloc(sizeof(*book));
  if(book) {
    book->bname=strdup(bname);
    book->bnum=bnum;
  }
  return book;
}

void Book_Free(Book **book) {
  free((*book)->bname);
  (*book)->bname=NULL;
  free(*book);
  *book=NULL;
}

void Books_Free(Book ***books,size_t *nbooks) {
  size_t i;
  for(i=0;i<*nbooks;i++) {
    Book_Free(&(*books)[i]);
  }
  free(*books);
  *books=NULL;
  *nbooks=0;
}

void Book_Append(Book ***books,size_t *nbooks,Book *book) {
  (*books)=realloc(*books,sizeof(**books)*((*nbooks)+1));
  (*books)[(*nbooks)++]=book;
}



size_t countBooks(Passage **passages,size_t npassages) {
  size_t i;
  size_t nbook=0;
  size_t bnum=0;
  size_t pbnum=0;
  for(i=0;i<npassages;i++) {
    bnum=passages[i]->bnum;
    if(bnum!=pbnum) {
      nbook++;
    }
    pbnum=bnum;
  }
  return nbook;
}



size_t countChapters(Passage **passages,size_t npassages,size_t bnum) {
  size_t i;
  size_t nchap=0;
  size_t cnum=0;
  size_t pcnum=0;
  for(i=0;i<npassages;i++) {
    cnum=passages[i]->cnum;
    if(bnum==passages[i]->bnum && cnum!=pcnum) nchap++;
    pcnum=cnum;
  }
  return nchap;
}



size_t countVerses(Passage **passages,size_t npassages,size_t bnum,size_t cnum) {
  size_t i;
  size_t nvers=0;
  size_t vnum=0;
  size_t pvnum=0;
  for(i=0;i<npassages;i++) {
    vnum=passages[i]->vnum;
    if( bnum==passages[i]->bnum &&
        cnum==passages[i]->cnum &&
        vnum!=pvnum
    ) nvers++;
    pvnum=vnum;
  }
  return nvers;
}


void getBooks(Book ***books,size_t *nbooks,Passage **passages,size_t npassages) {
  size_t i;
  size_t bnum=0;
  size_t pbnum=0;
  for(i=0;i<npassages;i++) {
    bnum=passages[i]->bnum;
    if(bnum!=pbnum) {
      Book_Append(books,nbooks,Book_New(passages[i]->bname,passages[i]->bnum));
    }
    pbnum=bnum;
  }
}


int main(void) {

  Passage **passages=NULL;
  size_t npassages=0;

  char *bname=NULL;
  size_t bnum=0;
  size_t cnum=0,vnum=0;
  char *text=NULL;

  size_t nbook=0;
  size_t nchap=0;
  size_t nvers=0;

  size_t pbnum=0;
  size_t pcnum=0;
  size_t pvnum=0;

  Book **books=NULL;
  size_t nbooks=0;

  Info **infos=NULL;
  size_t ninfos=0;

  Info *info=NULL;

  size_t i,j;

  ezxml_t bible = ezxml_parse_file("adb.xml"),book,chap,vers;

  for (book = ezxml_child(bible, "BIBLEBOOK"); book; book = book->next) {
    for (chap = ezxml_child(book, "CHAPTER"); chap; chap = chap->next) {
      for (vers = ezxml_child(chap, "VERS"); vers; vers = vers->next) {
        bname =strdup(ezxml_attr(book,"bname"));
        bnum = atoi(ezxml_attr(book,"bnumber"));
        cnum = atoi(ezxml_attr(chap,"cnumber"));
        vnum = atoi(ezxml_attr(vers,"vnumber"));
        text = strdup(ezxml_txt(vers));

        Passage_Append(&passages,&npassages,Passage_New(bname,bnum,cnum,vnum,text));
        free(bname);
        bname=NULL;

        free(text);
        text=NULL;

      }
    }
  }
  ezxml_free(book);




  getBooks(&books,&nbooks,passages,npassages);



  for(i=0;i<nbooks;i++) {

    info=Info_New();

    info->bname=strdup(books[i]->bname);

    info->bnum=books[i]->bnum;

    info->nchap=countChapters(passages,npassages,info->bnum);

    info->nvers=malloc(sizeof(*(info->nvers))*info->nchap);

    for(j=0;j<info->nchap;j++) {
      info->nvers[j]=countVerses(passages,npassages,info->bnum,j+1);
    }

    Info_Append(&infos,&ninfos,info);

  }

  Infos_Print(infos,ninfos);

  Infos_Free(&infos,&ninfos);

  Books_Free(&books,&nbooks);


  Passages_Free(&passages,&npassages);

  return 0;
}



