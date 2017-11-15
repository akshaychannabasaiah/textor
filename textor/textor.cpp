                                                                     
                                                                     
                                                                     
                                             
                                                                                                                                                                               

#include "stdafx.h"
#include<iostream>
#include<GL/glut.h>
#include<string.h>
#include<fstream>
using namespace std;
#define MAX_LINES 100
#define MAX_LEN 130
#define V 20
float R=0,G=0,B=0;

fstream fil,filelist;
char filename[50],filearray[10][50];

//class is used as a structure to store the strings that r entered
class text
{
public:
    int end;
    char str[MAX_LEN];
    //int selx[MAX_LEN];
    //int sely;
    text()
    {
        end=0;
    }
};
text T[MAX_LINES];
text BUF[MAX_LINES];
text D;

//structure used for undo(implemented as a linked list)
struct listforundo
{
    text T[MAX_LINES];
    int i,curx,cury,TH;
    void* font;
    float R,G,B;
    listforundo* next;
    listforundo* prev;
}*start,*ptr,*last,*curptr,*temp;
void SetCursor(int,int);

//Declaration of global variables
int i=0,j=0,posx=100,posy=100,oldx,oldy,newx,newy,valid,startpos,startrow,endpos,endrow,ibuf=0,curx=100,cury=100,cnt=0,SELECT=0,TH=1,listlen=0;
int xcor,ycor,xind,yind,SAVE=0,fi=0,OPEN=0,fl=0,ech=0,NEW=0,TITLE=1;
char ch;
void* font=GLUT_BITMAP_9_BY_15;
GLfloat curz;
int w=glutBitmapWidth(font,'a');

//to clear the filename so as to take a new filename while saving a new file
void clearname(char *str)
{
	for(fi=0;fi<strlen(str);fi++)
		str[fi]='\0';
	fi=0;
}

//func to push change of state of the record onto a stack
void push(listforundo* ptr)
{
    while(curptr!=last && last!=NULL && curptr!=NULL)
    {
        temp=last;
        last=last->prev;
        last->next=NULL;
        delete temp;
    }
    if(start==NULL)
    {
        start=last=ptr;
        listlen++;
    }
    else if(start==last)
    {
        start->next=ptr;
        ptr->prev=start;
        last=ptr;
        listlen++;
    }
    else if(listlen==10)
    {
        temp=start;
        start=start->next;
        delete temp;
        ptr->prev=last;
        last->next=ptr;
        last=ptr;
    }
    else
    {
        ptr->prev=last;
        last->next=ptr;
        last=ptr;
    }
    curptr=last;
}

//func to create a node for every change in the state of the record
void RecordChange()
{
    ptr=new listforundo;
    ptr->curx=curx;
    ptr->cury=cury;
    ptr->font=font;
    ptr->i=i;
    ptr->B=B;
    ptr->R=R;
    ptr->G=G;
    ptr->TH=TH;
    ptr->next=NULL;
    ptr->prev=NULL;
    for(int k=0;k<MAX_LINES;k++)
        ptr->T[k]=T[k];
    push(ptr);
}

//func to implement undo
void undo()
{
    if(start==NULL)
    {
    }
    else if(start==curptr)
    {
    }
    else
    {
        curptr=curptr->prev;
        curx=curptr->curx;
        cury=curptr->cury;
        font=curptr->font;
        R=curptr->R;
        G=curptr->G;
        B=curptr->B;
        TH=curptr->TH;
        for(int k=0;k<MAX_LINES;k++)
        T[k]=curptr->T[k];
    }
}

//function to implement redo
void redo()
{
    if(curptr==last)
    {
    }
    else if(start==NULL)
    {
    }
    else
    {
        curptr=curptr->next;
        curx=curptr->curx;
        cury=curptr->cury;
        font=curptr->font;
        i=curptr->i;
        R=curptr->R;
        G=curptr->G;
        B=curptr->B;
        TH=curptr->TH;
        for(int k=0;k<MAX_LINES;k++)
        T[k]=curptr->T[k];
    }
}

//func to extract the list of files created using the same editor stored in "file list.txt" file
void file_extract()
{
	int k=0;
	filelist.open("file list.txt",ios::in);
	filelist.seekg(0);
	while(!filelist.eof())
		filelist.getline(filearray[k++],50);
	fl=k;
	filelist.close();
}

//func to implement save as option
void save_as_file(char* str)
{
	
    fil.open(str,ios::out|ios::trunc);
    fil.seekp(0);
        for(int k=0;k<MAX_LINES;k++)
        {
            D=T[k];
            fil.write((char*)&D,sizeof(D));
        }
    fil.close();
    filelist.open("file list.txt",ios::out|ios::out|ios::app);
    filelist<<str;
    filelist<<"\n";
    filelist.close();
}

//func to implement save option
void save_file()
{
	
    if(filename[0]!='\0')
    {
        fil.open(filename,ios::out|ios::trunc);
        fil.seekp(0);
          for(int k=0;k<MAX_LINES;k++)
            {
                D=T[k];
                fil.write((char*)&D,sizeof(D));
            }
        fil.close();
    }
	else
		SAVE=1;
}

//func to open any given file from a list of files
void open_file(char* str)
{
    int k=0;
    fil.open(str,ios::in);
    fil.seekg(0);
    while(!fil.eof())
    {
            fil.read((char*)&D,sizeof(D));
            T[k]=D;
            i=k++;
       
    }
    fil.close();
    SetCursor(100,100);
}

//to clear the contents of d class structure(for copying new text)
void clearall(text *C)
{
    for(int q=0;q<=MAX_LINES;q++)
    {
        for(int z=0;z<MAX_LEN;z++)
        {
            C[q].str[z]='\0';
        }
    }
}

//func used to clear the whole text record-for creating a new record
void MEGACLEAR()
{
	clearall(T);
	clearall(BUF);
	i=0;ibuf=0;
	curx=100;cury=100;
	cnt=0;SELECT=0;NEW=0;
	SAVE=0;OPEN=0;R=G=B=0;
	xcor=ycor=xind=yind=0;
	oldx=oldy=newx=newy=0;
	fi=0;listlen=0;ch='\0';
	while(start!=NULL)
	{
		temp=start;
		start=start->next;
		//start->prev=NULL;
		delete temp;
	}
}

//theme1-backgrounds
void theme1()
{
float x=0.0,y=5500.0;
  
    glColor3f(0,0,1.0);
     //to draw vertical stripes  
     for(;x<5500.0;x+=10)
     {
         if(x>=90.0)
             y=70.0;
         else
             y=5500.0;
      
         glBegin(GL_POLYGON);
         
            glVertex2f(x,0.0);
            glVertex2f(x,y);
            glVertex2f(x+5,y);
            glVertex2f(x+5,0.0);
         
        glEnd();
        
     }
	 //to draw a horizontal line as d border on top
            glBegin(GL_POLYGON);
            glVertex2f(80,70.0);
            glVertex2f(5500,70);
            glVertex2f(5500,75);
            glVertex2f(80,75.0);
           glEnd();
}

//backgroungs-theme2
void theme2()
{
float x,y;
    glColor3f(0.903,0.829,0.882);
    glBegin(GL_POLYGON);//to draw the blue L shaped polygon 
        glVertex2f(0,0);
        glVertex2f(0,5500);
        glVertex2f(90,5500);
        glVertex2f(90,70);
        glVertex2f(5500,70);
        glVertex2f(5500,0);
    glEnd();
  

        glColor3f(1.0,0.7,0.7);
  
       for(y=70.0;y<5500.0;y+=200)//to draw the triangles on the left
       {
         glBegin(GL_TRIANGLES);
            glVertex2f(90,y);
            glVertex2f(0,y+100);
            glVertex2f(90,y+200);
        glEnd();
        
        }
	   //triangles on the top
       for(x=90.0;x<5500.0;x+=200)
       {
         glBegin(GL_TRIANGLES);
            glVertex2f(x,70.0);
            glVertex2f(x+100,0.0);
            glVertex2f(x+200,70.0);
        glEnd();
        }
       glColor3f(0.6,1.0,1.0);
	   //to draw inner triangles on left
        for(y=70.0;y<5500.0;y+=200)
       {
         glBegin(GL_TRIANGLES);
            glVertex2f(0,y+130);
            glVertex2f(90,y+200);
            glVertex2f(0,y+270);
        glEnd();
        }
		//to draw inner triangles on the top
        for(x=90.0;x<5500.0;x+=200)
       {
         glBegin(GL_TRIANGLES);
            glVertex2f(x+130,0.0);
            glVertex2f(x+200,70.0);
            glVertex2f(x+270,0.0);
        glEnd();
      
        }
		//to draw triangle on the top-left corner
        glBegin(GL_POLYGON);
            glVertex2f(0,140.0);
            glVertex2f(90,70.0);
            glVertex2f(140,0.0);
            glVertex2f(0,0.0);
        glEnd();

}

//backgrounds=theme3
void theme3()
{
    float x=0.0,y=5500.0;
    //to draw the l shaped polygon
    glColor3f(0.933,0.879,0.768);
    glBegin(GL_POLYGON);
        glVertex2f(0,0);
        glVertex2f(0,5500);
        glVertex2f(90,5500);
        glVertex2f(90,70);
        glVertex2f(5500,70);
        glVertex2f(5500,0);
    glEnd();
     //to draw overlapping boxes of diff color  
    glColor3f(1.0,0.709,0.578);
    for(x=0.0;x<=5500;x+=130)
    {
        glBegin(GL_POLYGON);
        glVertex2f(x+20,10);
        glVertex2f(x+20,50);
        glVertex2f(x+80,50);
        glVertex2f(x+80,10);
        glEnd();
    }
    glColor3f(1.0,0.71,0.71);
    for(x=0.0;x<=5500;x+=130)
    {
        glBegin(GL_POLYGON);
        glVertex2f(x+50,20);
        glVertex2f(x+50,60);
        glVertex2f(x+110,60);
        glVertex2f(x+110,20);
        glEnd();
    }
  
    glColor3f(1.0,0.709,0.578);
    for(y=70.0;y<=5500;y+=130)
    {
        glBegin(GL_POLYGON);
        glVertex2f(10,y);
        glVertex2f(50,y);
        glVertex2f(50,y+60);
        glVertex2f(10,y+60);
        glEnd();
    }
    glColor3f(1.0,0.71,0.71);
    for(y=100.0;y<=5500;y+=130)
    {
        glBegin(GL_POLYGON);
        glVertex2f(40,y);
        glVertex2f(80,y);
        glVertex2f(80,y+60);
        glVertex2f(40,y+60);
        glEnd();
    }
}


 void DrawCursor() //to draw cursor
{
    cnt++;
    if(curz==0.0&&cnt==200)
    {
        curz=1.0;
        cnt=0;
    }
    else if(curz==1.0&&cnt==200)
    {
        curz=0.0;
        cnt=0;
    }
    glColor3f(curz,curz,curz);
    glBegin(GL_LINES);
    glVertex2f(curx,cury+8);
    glVertex2f(curx,cury-15);
    glEnd();
    glFlush();
    glColor3f(0,0,0);
}

void IndexToCoOrdinate(int x,int y)//conversion of given indices to respective co ordinates
{
    ycor=100+(V*y);
    xcor=100+(w*x);
}

void CoOrdinateToIndex(int x, int y)//conversion of given coordinates to respective indices
{
    int k,l;
    if(x<=100&&y<=100)
    {
        xind=0;
        yind=0;
       xcor=100;
        ycor=100;
        return;
    }
    k=((x-100)%w);
    if(k<0)  //this means that the cursor placed left of the leftmost character
    {
        xcor=100;
        xind=0;
    }
    else
    {
        xcor=x-k;
        xind=(xcor-100)/w;
   
    }
 
    l=((y-100)%V);
    if(l<0)   //cursor placed above the topmost line
    {
        ycor=100;
        yind=0;
    }
    else
    {
        ycor=y-l;
        yind=(ycor-100)/V;
    }
 
    if(yind>i)
    {
        yind=i;
        ycor=100+(i*V);
    }
    if(xind>T[yind].end)
    {
        xind=T[yind].end;
        xcor=100+(T[yind].end*w);
    }
}

void SetCursor(int x, int y)//to set the cursor at various positions wen needed
{
    //EraseCursor()
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(curx,cury+8);
    glVertex2f(curx,cury-15);
    glEnd();
    glFlush();
    glColor3f(0,0,0);
    //end of EraseCursor()
    curx=x;
    cury=y;
    //draw cursor again to avoid unwanted flicker
    glColor3f(curz,curz,curz);
    glBegin(GL_LINES);
    glVertex2f(curx,cury+8);
    glVertex2f(curx,cury-15);
    glEnd();
    glFlush();
    glColor3f(0,0,0);
}
 
void drawBitmapText(text *T,float x,float y,float z)//to draw the text onto the frame buffer
{
    char *c,string[MAX_LEN];
    int m,k;
    int p=x;
    glRasterPos3f(x, y,z);
 
    for(int row=0;row<MAX_LINES;row++)
    {
        strcpy(string,T[row].str);
        for (c=string; *c != '\0'; c++)
            {
                glutBitmapCharacter(font, *c);
             }
        y+=V;
        p=x;
        glRasterPos3f(x, y,z);
    }
}

//to determine the start n the ending position during selection
void positions()
{
    int temp;
     //SELECT=0;
    CoOrdinateToIndex(oldx,oldy+V);
    startpos=xind;
    startrow=yind;
    CoOrdinateToIndex(newx,newy+V);
    endpos=xind;
    endrow=yind;
    if(startrow>endrow)
    {
        temp=endrow;
        endrow=startrow;
        startrow=temp;
        temp=endpos;
        endpos=startpos;
        startpos=temp;
    }
    else if(startrow==endrow)
    {
        if(startpos>endpos)
        {
           temp=endpos;
        endpos=startpos;
        startpos=temp;
        }
    }
    endpos=endpos-1;
}

  void select()//to draw a polygon over the selected text to indicate selection ends
  {
      int ox,oy,nx,ny,diff,p,q,k;
      positions();
    IndexToCoOrdinate(startpos,startrow); //finding the coordinates
    ox=xcor;
    oy=ycor;
    endpos=endpos+1;
    IndexToCoOrdinate(endpos,endrow);
    nx=xcor;
    ny=ycor;

    glColor4f(1.0,1.0,0.0,0.0);
    if(startrow==endrow)//for selecting in the same line
    {
        diff=nx-ox;
 
        glBegin(GL_POLYGON);
         glVertex2f(ox,oy+8);
         glVertex2f(nx,oy+8);
         glVertex2f(nx,oy-15);
         glVertex2f(ox,oy-15);
        glEnd();
        glFlush();
    }
    else
    {
        IndexToCoOrdinate(T[startrow].end,startrow);// for selectin in d startrow
        p=xcor;
        glBegin(GL_POLYGON);
         glVertex2f(ox,oy+8);
         glVertex2f(p,oy+8);
         glVertex2f(p,oy-15);
         glVertex2f(ox,oy-15);
        glEnd();
        glFlush();
        for( k=startrow+1;k<endrow;k++)//for selecting all d intermediate rows fully
        {
            IndexToCoOrdinate(T[k].end,k);
            p=xcor;
            q=ycor;
            glBegin(GL_POLYGON);
            glVertex2f(100,q+8);
            glVertex2f(p,q+8);
            glVertex2f(p,q-15);
            glVertex2f(100,q-15);
            glEnd();
            glFlush();
        }

            glBegin(GL_POLYGON); //for selecting in d endrow
            glVertex2f(100,ny+8);
            glVertex2f(nx,ny+8);
            glVertex2f(nx,ny-15);
            glVertex2f(100,ny-15);
            glEnd();
        glFlush();

    }
}


void copy()//to copy the selected text to a buffer
{
 
    int k,jbuf=0;
    clearall(BUF);
    positions();
  
     if(startrow==endrow)
    {
        for(k=startpos;k<=endpos;k++)//for copying text that is selected within a line
        {
            BUF[ibuf].end=jbuf;
            BUF[ibuf].str[jbuf++]=T[startrow].str[k];
        }
    }
    else
    {
        for(k=startpos;k<=T[startrow].end;k++)//copying the first line from start position to the end of line
        {
            BUF[ibuf].end=jbuf;
            BUF[ibuf].str[jbuf++]=T[startrow].str[k];
        }
        for(k=startrow+1;k<endrow;k++)//for copying lines that are selected completely
        {
            BUF[++ibuf].end=strlen(T[k].str);
            strcpy(BUF[ibuf].str,T[k].str);
        }
        ibuf++;
        jbuf=0;
        for(k=0;k<=endpos;k++)//for copying text of the last line
        {
            BUF[ibuf].end=jbuf;
            BUF[ibuf].str[jbuf++]=T[endrow].str[k];
        }
    }
     for(int p=0;p<=ibuf;p++)
    {
        for(int q=0;q<=BUF[p].end;q++)
        {
            printf("%c",BUF[p].str[q]);
        }
        printf("\n");
    }

}


void DeleteText(int x1,int y1,int x2,int y2)//for deleting text between specified positions
{
   int sr,temp;
   int diff,k,l;

     positions();//to determine the start n end positions
    if(startrow==endrow)
    {
        diff=endpos-startpos;
        for(k=startpos,l=endpos+1;l<MAX_LEN;k++,l++)//for deleting text that is selected within a line
        {
            T[startrow].str[k]=T[startrow].str[l];
        }
        while(k<=MAX_LEN-1)
        {
            T[startrow].str[k++]='\0';
        }
        T[startrow].end-=diff;
    }
    else
    {
        for(k=startpos;k<MAX_LEN;k++)//deleting the first line from start position to the end of line
        {
            T[startrow].str[k]='\0';
        }
        T[startrow].end=startpos;
        diff=endpos;
        sr=startrow;
        for(k=startpos,l=endpos+1;l<=MAX_LEN;k++,l++)//for deleting the last line of selected text
        {
            T[sr].str[k]=T[endrow].str[l];
            T[sr].end=k;
            if(k==MAX_LEN)
            {
                sr++;
                k=0;
            }
        }
       for(k=startrow+1,l=endrow+1;l<=MAX_LINES;k++,l++)//for deleting lines completely selected
        {
            strcpy(T[k].str,T[l].str);
            T[k].end=T[l].end;
        }
        while(k<=MAX_LINES)
        {
             for(int z=0;z<MAX_LEN;z++)
            {
                T[k].str[z]='\0';
            }
            k++;
        }
    }
    endrow=startrow;
    endpos=startpos;
    IndexToCoOrdinate(startpos,startrow);
    SetCursor(xcor,ycor);
}

//to cut the selected text from the text record
void cut()
{
    int m,n,p,q;
     positions();
     copy();//to copy
     CoOrdinateToIndex(newx,newy+V);//to find the coordinates
     IndexToCoOrdinate(xind,yind);
     SetCursor(xcor,ycor);
     DeleteText(oldx,oldy,newx,newy);//to delete
     SELECT=0;
}

void InsertText(unsigned char str,int x,int y)//to insert a character
{
    char c;
    int k,l,m=0,z;
    if(str==13)
    {
        for(k=MAX_LINES;k>y+1;k--)//shifting rows
        {
            strcpy(T[k].str,T[k-1].str);
            T[k].end=T[k-1].end;
        }
        for(int z=0;z<MAX_LEN;z++)//making the (y+1)th row null
        {
            T[y+1].str[z]='\0';
        }
        for(int z=x;z<T[y].end;z++)//copying the characters in yth row after the position where enter key is pressed, to the next line (y+1)th row
        {
            T[y+1].str[m++]=T[y].str[z];
        }
        T[y].end=x;//updating the end values
        T[y+1].end=m;
        for(int z=x;z<MAX_LEN;z++)//replacing the copied characters in yth row
        {
            T[y].str[z]='\0';
        }
     
    }
    else
    {
        T[y].end+=1;
        for(k=T[y].end-1,l=T[y].end;k>=x;k--,l--)//shifting characters ahead
        {
            T[y].str[l]=T[y].str[k];
        }
        T[y].str[x]=str;
    }
}


void InsertText(char *str,int x,int y)//to insert a string
{
    char c;
    int k,l,m=0;
    T[y].end+=strlen(str);
    for(k=T[y].end-strlen(str),l=T[y].end;k>=x;k--,l--)//shifting characters ahead
        {
            T[y].str[l]=T[y].str[k];
        }
    for (k=x; k<=l; k++)//inserting new characters
        {
            T[y].str[k]=str[m++];       
        }
}

void paste()//to paste the contents of BUF in the selected line
{
  int m,n,p,k,l;
 CoOrdinateToIndex(curx,cury);
 if(ibuf==0)//if buffer contains jus 1 line of data
 {
     InsertText(BUF[0].str,xind,yind);
     IndexToCoOrdinate(T[yind].end,yind);
     SetCursor(xcor,ycor);
 }
 else
 {
     m=xind;
     n=yind;
     p=0;
     for(int k=yind;k<=yind+ibuf;k++)
     {
         InsertText(BUF[p].str,m,k);//to insert the text at the positon specifed
         l=strlen(BUF[p].str);
         if(p==ibuf)
             break;
         InsertText(13,m+l,k);//to insert ENTER so tat multiple lines can be pasted
         m=100;
         p++;
     }
     i=i+ibuf;//updating i
     IndexToCoOrdinate(l,yind+ibuf);
     SetCursor(xcor,ycor);
     }
}

//to implement backspace
void backspace()
{
       int m,n,p,q,k,l;
        m=curx;
        n=cury;
        if(xind==0&&yind==0)
        {
            SetCursor(100,100);
        }
        else if(SELECT==1)//for selected text
        {
            CoOrdinateToIndex(newx,newy+V);
            IndexToCoOrdinate(xind,yind);
            SetCursor(xcor,ycor);
            DeleteText(oldx,oldy,newx,newy);
            SELECT=0;
        }
        else if(xind==0 && yind!=0)//wen the cursor is at the begining of a line
        {
            i--;
            IndexToCoOrdinate(T[yind-1].end,yind-1);
            p=xcor;
            q=ycor;
            strcat(T[yind-1].str,T[yind].str);
            T[yind-1].end+=strlen(T[yind].str);
            for(l=yind;l<MAX_LINES-1;l++)//for deleting lines completely selected
            {
                strcpy(T[l].str,T[l+1].str);
                T[l].end=T[l+1].end;
            }
             while(l<MAX_LINES)//replacing the previous pos of shifted lines
            {
                 for(int z=0;z<MAX_LEN;z++)
                  {
                      T[l].str[z]='\0';
                 }
                 T[l].end=0;
            l++;
            }
            SetCursor(p,q);
        }
        else    
        {
			//for deleting a character
            for( k=xind-1,l=xind;l<=MAX_LEN;l++,k++)
            {
                T[yind].str[k]=T[yind].str[l];
            }
            T[yind].str[k]='\0';
            T[yind].end-=1;
            IndexToCoOrdinate(xind-1,yind);
            SetCursor(xcor,ycor);
            printf("%d %d\n",xind-1,yind);
        }
}


void init()//to set d background colour
{
    clearall(T);
    RecordChange();
    glClearColor(1.0,1.0,1.0,1.0);
}


void reshape(int w,int h)//reshape function
{
 
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,w,h,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

//to display text on screen
void bittext(char *str,int x,int y,int z)
{
	      char* p;
		 glRasterPos3i(x,y,z);
		 for (p=str; *p != '\0'; p++)
         {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
		 }	
}

void title()	// to draw the starting screen
{	
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.4,0.4,0.4,0.0);

	glColor3f(0,0.8,0.8);	
	for(int x=0,y=5500;x<200.0;x+=10) //to draw the stripes on the left 
	{
         glBegin(GL_POLYGON);
            glVertex2f(x,0.0);
            glVertex2f(x,y);
            glVertex2f(x+5,y);
            glVertex2f(x+5,0.0);
        glEnd();
     }
	for(int x=1150,y=5500;x<5500.0;x+=10)//to draw the stripes on the right
	{     
         glBegin(GL_POLYGON);
         
            glVertex2f(x,0.0);
            glVertex2f(x,y);
            glVertex2f(x+5,y);
            glVertex2f(x+5,0.0);
         
        glEnd();        
     }

	glColor3f(1.0,1.0,1.0);
	bittext("B M S   I N S T I T U T E   O F   T E C H N O L O G Y",450,70,0);

	glColor3f(0.8,0.8,0.8);
	bittext("Department  of  Computer  Science  and  Engineering",450,100,0);
	bittext("Doddabballapur main rd, Bangalore- 560 064",480,120,0);

	glColor3f(1.0,1.0,0.6);
	bittext("A Computer Graphics & Visualization Mini Project report on",420,190,0);	
	
	glColor3f(1.0,1.0,0.4);
	bittext(" * * G R A P H I C A L   I M P L E M E N T A T I O N   O F   T E X T   E D I T O R * *",320,220,0);

	glColor3f(1.0,0.8,0.7);
	bittext("Submitted by:",600,300,0);
	
	glColor3f(1.0,1.0,1.0);
	bittext("AAKANKSHA PARAMESHWAR",300,340,0);
	bittext("AKSHAY CHANNABASAIAH",770,340,0);

	glColor3f(0.0,0.9,0.9);
	bittext("( 1BY10CS002 )",350,370,0);
	bittext("( 1BY10CS009 )",820,370,0);

	glColor3f(1.0,0.8,0.7);
	bittext("Under the guidance of:",570,460,0);

	glColor3f(1.0,1.0,1.0);
	bittext(" Mr. MUNESHWARA M.S.",300,500,0);
	bittext(" Ms. AMBIKA G.N ",820,500,0);

	glColor3f(1,0.8,0.7);
	bittext("Assistant Professor,",300,530,0.0);
	bittext("Assistant Professor,",820,530,0.0);

	glColor3f(0.0,0.9,0.9);
	bittext("Dept. of CS&E",300,550,0.0);
	bittext("Dept. of CS&E",820,550,0.0);

	glColor3f(1.0,1.0,0.4);
	bittext("Press ANY KEY to view the project",520,650,0);
	glFlush();
}

void save_disp()  //to display save window 
{
		 bittext("TO SAVE THE FILE ENTER THE FILE NAME BELOW",300,300,0);
		 bittext(filename,350,350,0);
}

void open_disp()  //to display open file window
{
	    int l,m;
	     char* p;
		 bittext("THE LIST OF EXISTING FILES ARE:: CHOOSE FROM THEM::",300,300,0);
		  m=320;
		  file_extract();//extract list of files
		 for(l=0;l<=fl;l++)
		 {
			 bittext(filearray[l],350,m,0);
			 m=m+20;
		 }
		 bittext("ENTER THE NAME OF THE FILE TO BE OPENED",300,m,0);
		 bittext(filename,350,m+50,0);
}


void render(void)//display func
{
    int cnt;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if(TH==1) theme1();
    else if(TH==2) theme2();
    else if(TH==3) theme3();
    if(TITLE==1)
	{
		title();
	}
	else if(SAVE==1)
	{
		glColor3f(0,0,0);
		save_disp();
	}
	else if (OPEN==1)
	{
		glColor3f(0,0,0);
		open_disp();
	}
	else
	{
		 if(SELECT==1 )
         {
         select();
    
		 }
	    else
        DrawCursor();
		glColor3f(R,G,B);
        drawBitmapText(T,posx,posy,0);
	}
    glutSwapBuffers();
}


 void keyboard(unsigned char key, int x, int y)
{
	if(TITLE==1)
	{
		TITLE=0;
		glClearColor(1.0,1.0,1.0,1.0);
		return;
	}
    int m,p,q,k,l;
    CoOrdinateToIndex(curx,cury);//to find the current indices
  
    if (key==13)//Enter key
    {
       if(SAVE==1) //for saving a file
		 {
			 SAVE=0;
			 save_as_file(filename);
			 CoOrdinateToIndex(oldx,oldy);
			 IndexToCoOrdinate(xind,yind);
			 SetCursor(xcor,ycor);
	    }
	   else if(OPEN==1) //for opening a file
		 {
			 OPEN=0;
			 open_file(filename);
			 SetCursor(100,100);
	    }
	   else      //insert enter
		{
		i++;
        InsertText(key,xind,yind);
        SetCursor(100,cury+V);
	   }
     }
    else if(key==9)//TAB key
    {
        SetCursor(curx,cury);
        InsertText("     ",xind,yind);   
        // w=glutBitmapWidth(font,T[yind].str[xind]);
        SetCursor((curx+5*w),cury);
    }
    else if(key==8 || key==127)//backspace key
    {
		if(SAVE==1 || OPEN==1) //to edit file name while entering
		{
			filename[fi--]='\0';
		}
		else
		   backspace();
    }
    else if(xind==MAX_LEN-1)
    {
        i++;
        SetCursor(100,cury+V);
        CoOrdinateToIndex(curx,cury+V);
        InsertText(key,xind,yind);
        SetCursor(curx+w,cury);
    }
    else
    {
		if(SAVE==1 || OPEN==1)//to take filename as input
		{
			filename[fi++]=key;
            filename[fi]='\0';

		}
		else        
		{
         //take normal single character input
        InsertText(key,xind,yind);
        SetCursor(curx+w,cury);
		}
    }
    RecordChange();
}

void Mymouse(int b,int s,int x,int y)   //mouse func
{
    if(b==GLUT_LEFT_BUTTON && s==GLUT_DOWN)
    {
        valid=1;
        oldx=x;
        oldy=y;
        SELECT=0;
        CoOrdinateToIndex(x,y+V);
        IndexToCoOrdinate(xind,yind);
        SetCursor(xcor,ycor);
    }
}

void movef(int x,int y)  //func to record movement of the mouse after a click
{
    if(valid)
    {
        newx=x;
        newy=y;
        SELECT=1;
    }
}

void main_menu(int id)
{
    glutPostRedisplay();
}
void formatoptions(int id)
{
    glutPostRedisplay();
}

void fileoptions(int id)
{
    switch(id)
    {
     case 11: MEGACLEAR();
             break;
     case 12:clearname(filename);
		     OPEN=1;
             break;
     case 13:save_file();
             break;
     case 14: clearname(filename);
		     SAVE=1;
             break;
     case 15:exit(0);
            break;
    }
    glutPostRedisplay();
}

void editoptions(int id)
{
    switch(id)
    {
        case 21: undo();
             break;
        case 22: redo();
              break;
        case 23: cut();           
                RecordChange();
              break;
        case 24: copy();
             break;
        case 25: paste();           
                RecordChange();
              break;
        case 26: backspace();           
                RecordChange();
              break;
         }
    glutPostRedisplay();
}

void sizeoptions(int id)
{
    switch(id)
    {
     case 31: font=GLUT_BITMAP_8_BY_13;
              break;
     case 32: font=GLUT_BITMAP_9_BY_15;
              break;
    }
     w=glutBitmapWidth(font,'a');
    glutPostRedisplay();   
    RecordChange();
}

void colouroptions(int id)
{
    switch(id)
    {
     case 41: R=1.0;//RED
              G=0.0;
              B=0.0;
              break;

    case 42: R=0.0; //LIME GREEN
              G=1.0;
              B=0.0;
              break;

    case 43: R=0.0; //NAVY BLUE
              G=0.0;
              B=0.5;
              break;

    case 44:  R=0.6;//GREY
              G=0.6;
              B=0.6;
              break;

    case 45:  R=0.8;//PURPLE
              G=0.5;
              B=0.8;
              break;

    case 46: R=0.0;//BLACK
              G=0.0;
              B=0.0;
              break;
    }
    glutPostRedisplay();
}

void backgrounds(int id)
{
    switch(id)
    {
        case 51: TH=1;
              break;
        case 52:TH=2;
             break;
        case 53:TH=3;
             break;
    }
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{
        printf("%d %s",argc,argv);
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
        glutInitWindowSize(5500,5500);
        glutInitWindowPosition(0,0);
        glutCreateWindow("Textor");
        glutSetCursor( GLUT_CURSOR_TEXT );//changing the cursor view
        init();

        glutDisplayFunc(render);
        glutKeyboardFunc(keyboard);
        glutMouseFunc(Mymouse);
        glutMotionFunc(movef);
        glutIdleFunc(render);
        glutReshapeFunc(reshape);

        int file_menu=glutCreateMenu(fileoptions);
        glutAddMenuEntry("New",11);
        glutAddMenuEntry("Open",12);
        glutAddMenuEntry("Save",13);
        glutAddMenuEntry("Save As",14);
        glutAddMenuEntry("Quit",15);

        int edit_menu=glutCreateMenu(editoptions);
        glutAddMenuEntry("Undo",21);
        glutAddMenuEntry("Redo",22);
        glutAddMenuEntry("Cut",23);
        glutAddMenuEntry("Copy",24);
        glutAddMenuEntry("Paste",25);
        glutAddMenuEntry("Delete",26);

        int size_menu=glutCreateMenu(sizeoptions);
        glutAddMenuEntry("bitmap size 8",31);
        glutAddMenuEntry("bitmap size 9",32);
        

        int colour_menu=glutCreateMenu(colouroptions);
        glutAddMenuEntry("Red",41);
        glutAddMenuEntry("Lime Green",42);
        glutAddMenuEntry("Navy Blue",43);
        glutAddMenuEntry("Grey",44);
        glutAddMenuEntry("Purple",45);
        glutAddMenuEntry("Black",46);

        int back_menu=glutCreateMenu(backgrounds);
        glutAddMenuEntry("Theme 1",51);
        glutAddMenuEntry("Theme 2",52);
        glutAddMenuEntry("Theme 3",53);

        int format_menu=glutCreateMenu(formatoptions);
        glutAddSubMenu("Change Size",size_menu);
        glutAddSubMenu("Change Colour",colour_menu);

        glutCreateMenu(main_menu);
        glutAddSubMenu("File",file_menu);
        glutAddSubMenu("Edit", edit_menu);
        glutAddSubMenu("Format",format_menu);
        glutAddSubMenu("Change Backgrounds",back_menu);
        glutAttachMenu(GLUT_RIGHT_BUTTON);

        // enter the main loop
        glutMainLoop();
    return 0;
}