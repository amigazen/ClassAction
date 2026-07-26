void Gcode(char *name,char *code)
{

char l1,l2,c1,c2,c3,v1,v2,v3;
char l[2],c[3],v[3];
int  cn=0;
int i;


sprintf(l,"%02d",strlen(name)+45);
l1=l[0];
l2=l[1];

for(i=0;i<strlen(name);i++)
	cn+=name[i];

srand(1995+cn);

cn=cn%240;

sprintf(c,"%03d",cn);
c1=c[0];
c2=c[1];
c3=c[2];

code[0]=rand()%10+'0';
code[1]=l1;
code[2]=rand()%10+'0';
code[3]=l2;
code[4]=rand()%10+'0';
code[5]=c1;
code[6]=rand()%10+'0';
code[7]=c2;
code[8]=rand()%10+'0';
code[9]=c3;

cn=0;
for(i=0;i<10;i++) cn+=code[i];

sprintf(v,"%03d",cn%240);

v1=v[0];
v2=v[1];
v3=v[2];
code[10]=rand()%10+'0';
code[11]=v1;
code[12]=rand()%10+'0';
code[13]=v2;
code[14]=rand()%10+'0';
code[15]=v3;
code[16]=0;
}

int main(argc,char **argv)
{

char code[20];

Gcode(argv[1],code);

printf("Dear %s,\n\nYour Licence Code is : %s\n",argv[1],code);

}

