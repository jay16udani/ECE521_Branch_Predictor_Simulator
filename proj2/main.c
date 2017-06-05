#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int policy_counter=0;
int btb_hits=0,btb_misses=0,btb_accesses=0,btb_mispredictions=0,prev=0;

struct table
{
    int counter;
};

struct gtable
{
    int counter;
};

struct btable
{
    int counter;
};

struct htable
{
    unsigned long long local_reg;
};

typedef struct BTB
{
    int valid;
    unsigned long tag;
    //int dirty;
    int policycounter;
}BTB;

void makebtb(int,int,unsigned long long,int,char,BTB btb[][32]);
int btbpolicy(BTB btb[][32],unsigned long long,int);

int main(int argc, char *argv[])
{
    setbuf(stdout,NULL);

    int x,tablesize,predictions=0,mispredictions=0,total_branches=0,i,assoc,bits_btb,j;
    unsigned long long address,index_number;
    char t_n='0';
    if(strcasecmp(argv[1],"bimodal")==0)
    {
        int y;
        tablesize=atoi(argv[2]);
        x=pow(2,tablesize);
        assoc=atoi(argv[4]);
        bits_btb=atoi(argv[3]);
        y=pow(2,bits_btb);

        struct table values[x];

        BTB btb[4100][32];

        for(i = 0;i<y;i++)
        {
            for(j=0;j<assoc;j++)
            {
                btb[i][j].valid = 0;
                //btb[i][j].dirty = 0;
                btb[i][j].tag=0;
                btb[i][j].policycounter=0;
            }
        }

        for(i=0;i<x;i++)
        {
            values[i].counter=2;
        }
        FILE *fp;
        fp=fopen(argv[5],"r");//reading trace file

        fseek(fp,0,SEEK_SET);
        if(fp==NULL)
        {
            printf("\nFile does not contain any data: Exiting\n");
            exit(1);
        }

        printf("Command Line:\n");
        printf("./sim_bp %s %s %s %s %s\n",argv[1],argv[2],argv[3],argv[4],argv[5]);
        //printf("\nFinal Bimodal Table Contents:\n");

        while(1)
        {

//            printf("Hey inside super loop..\n");
            if(feof(fp))
                {
                    break;
                }
            total_branches+=1;
            fscanf(fp,"%llx %c\n",&address,&t_n);

            if(assoc!=0)
            makebtb(x,assoc,address,bits_btb,t_n,btb);      //calling BTB

            /*decoding index*/
            index_number=address>>2;
            index_number=index_number<<(64-tablesize);
            index_number=index_number>>(64-tablesize);

            if(btb_misses-prev==1)
            {
                prev+=1;
            }

            else if(values[index_number].counter>=2)
            {
                predictions+=1;
                if(t_n=='t')
                {
                     if(values[index_number].counter==3)
                        values[index_number].counter=3;
                     else values[index_number].counter+=1;
                }
                else
                {
                    mispredictions+=1;
                    if(values[index_number].counter==0)
                        values[index_number].counter=0;
                    else values[index_number].counter-=1;
                }
            }

            else
            {
                predictions+=1;
                if(t_n=='t')
                {
                     mispredictions+=1;
                     if(values[index_number].counter==3)
                        values[index_number].counter=3;
                     else values[index_number].counter+=1;
                }
                else
                {

                    if(values[index_number].counter==0)
                        values[index_number].counter=0;
                    else values[index_number].counter-=1;
                }
            }
        }
        if(assoc!=0)
        {
            printf("\nFinal BTB Tag Array Contents {valid, pc}: \n");
            for(i=0;i<y;i++)
            {printf("Set     %d:   ",i);
		for(j=0;j<assoc;j++)
		{                
			printf("{%d, 0x  %lx}  ",btb[i][j].valid,btb[i][j].tag);
		}
		printf("\n");
            }
        }

       if(assoc!=0) printf("\n\nFinal Bimodal Table Contents:\n");
	else printf("\nFinal Bimodal Table Contents:\n");
        for(i=0;i<x;i++)
        {
            printf("table[%d]: %d\n",i,values[i].counter);
        }

        printf("\nFinal Branch Predictor Statistics:\n");
        printf("a. Number of branches: %d\n",total_branches);
        printf("b. Number of predictions from the branch predictor: %d\n",predictions);
        printf("c. Number of mispredictions from the branch predictor: %d\n",mispredictions);
        printf("d. Number of mispredictions from the BTB: %d\n",btb_mispredictions);
        printf("e. Misprediction Rate: %.2f percent\n",((((float)mispredictions+(float)btb_mispredictions)/(float)total_branches))*100);
    //printf("BTB hits: %d",btb_hits);
    }

    else if(strcasecmp(argv[1],"gshare")==0)
    {
        unsigned long long ghr=0,temp=0,temp1=0;
        int h,y;
        unsigned long long b=pow(2,63);
        //printf("%u\n",b);
        h=atoi(argv[3]);
        tablesize=atoi(argv[2]);
        x=pow(2,tablesize);
        struct table values[x];
        assoc=atoi(argv[5]);
        bits_btb=atoi(argv[4]);
        y=pow(2,bits_btb);

        BTB btb[4100][32];

        for(i = 0;i<y;i++)
        {
            for(j=0;j<assoc;j++)
            {
                btb[i][j].valid = 0;
                //btb[i][j].dirty = 0;
                btb[i][j].tag=0;
                btb[i][j].policycounter=0;
            }
        }


        for(i=0;i<x;i++)
        {
            values[i].counter=2;
        }
        FILE *fp;
        fp=fopen(argv[6],"r");//reading trace file

        fseek(fp,0,SEEK_SET);
        if(fp==NULL)
        {
            printf("\nFile does not contain any data: Exiting\n");
            exit(1);
        }

        printf("Command Line:\n");
        printf("./sim_bp %s %s %s %s %s %s\n",argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
        //printf("\nFinal GShare Table Contents:\n");
        while(1)
        {
            if(feof(fp))
                {
                    break;
                }
            total_branches+=1;
            fscanf(fp,"%llx %c\n",&address,&t_n);
            if(assoc!=0)
            makebtb(x,assoc,address,bits_btb,t_n,btb);      //calling BTB
            /*decoding index*/
            temp=address>>(2+tablesize-h);
            temp=temp<<(64-h);
            temp=temp>>(64-h);
            temp=temp^ghr;

            temp1=address<<(64-(tablesize-h+2));
            temp1=temp1>>(64-(tablesize-h+2));
            temp1=temp1>>2;

            temp=temp<<(tablesize-h);

            index_number=temp | temp1;
            //printf("%ld",index_number);

            if(btb_misses-prev==1)
            {
                prev+=1;
            }
            else
            {
                ghr=ghr>>1;
                if(values[index_number].counter>=2)
                {
                    predictions+=1;
                    if(t_n=='t')
                    {
                         ghr=ghr<<(64-h);
                         ghr=ghr | b;
                         ghr=ghr>>(64-h);

                         if(values[index_number].counter==3)
                            values[index_number].counter=3;
                         else values[index_number].counter+=1;
                    }
                    else
                    {
                        mispredictions+=1;
                        if(values[index_number].counter==0)
                            values[index_number].counter=0;
                        else values[index_number].counter-=1;
                    }
                }

                else
                {
                    predictions+=1;
                    if(t_n=='t')
                    {
                         ghr=ghr<<(64-h);
                         ghr=ghr | b;
                         ghr=ghr>>(64-h);
                         mispredictions+=1;
                         if(values[index_number].counter==3)
                            values[index_number].counter=3;
                         else values[index_number].counter+=1;
                    }
                    else
                    {

                        if(values[index_number].counter==0)
                            values[index_number].counter=0;
                        else values[index_number].counter-=1;
                    }
                }
            }
        }

        if(assoc!=0)
        {
            printf("\nFinal BTB Tag Array Contents {valid, pc}: \n");
            for(i=0;i<y;i++)
            {printf("Set     %d:   ",i);
		for(j=0;j<assoc;j++)
		{                
			printf("{%d, 0x  %lx}  ",btb[i][j].valid,btb[i][j].tag);
		}
		printf("\n");
            }
        }

        if(assoc!=0) printf("\n\nFinal GShare Table Contents:\n");
	else printf("\nFinal GShare Table Contents:\n");
        for(i=0;i<x;i++)
        {
            printf("table[%d]: %d\n",i,values[i].counter);
        }
        printf("\nFinal GHR Contents: 0x       %llx \n",ghr);
        printf("\nFinal Branch Predictor Statistics:\n");
        printf("a. Number of branches: %d\n",total_branches);
        printf("b. Number of predictions from the branch predictor: %d\n",predictions);
        printf("c. Number of mispredictions from the branch predictor: %d\n",mispredictions);
        printf("d. Number of mispredictions from the BTB: %d\n",btb_mispredictions);
        printf("e. Misprediction Rate: %.2f percent\n",(((float)mispredictions+(float)btb_mispredictions)/(float)total_branches)*100);
    }

    else if(strcasecmp(argv[1],"hybrid")==0)
    {
        unsigned long long ghr=0,temp=0,temp1=0,g_index_number,b_index_number;
        int h,gtablesize,btablesize,g,b1,b_temp_index,g_temp_index,y;
        unsigned long long b=pow(2,63);

        h=atoi(argv[4]);
        tablesize=atoi(argv[2]);
        gtablesize=atoi(argv[3]);
        btablesize=atoi(argv[5]);

        x=pow(2,tablesize);
        g=pow(2,gtablesize);
        b1=pow(2,btablesize);

        assoc=atoi(argv[7]);
        bits_btb=atoi(argv[6]);
        y=pow(2,bits_btb);

        BTB btb[4100][32];

        for(i = 0;i<y;i++)
        {
            for(j=0;j<assoc;j++)
            {
                btb[i][j].valid = 0;
                //btb[i][j].dirty = 0;
                btb[i][j].tag=0;
                btb[i][j].policycounter=0;
            }
        }

        struct table values[x];
        struct gtable gvalues[g];
        struct btable bvalues[b1];
        for(i=0;i<x;i++)
        {
            values[i].counter=1;
        }

        for(i=0;i<g;i++)
        {
            gvalues[i].counter=2;
        }

        for(i=0;i<b1;i++)
        {
            bvalues[i].counter=2;
        }

        FILE *fp;
        fp=fopen(argv[8],"r");//reading trace file

        fseek(fp,0,SEEK_SET);
        if(fp==NULL)
        {
            printf("\nFile does not contain any data: Exiting\n");
            exit(1);
        }

        printf("Command Line:\n");
        printf("./sim_bp %s %s %s %s %s %s %s %s\n",argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8]);
        //printf("\nFinal Hybrid Table Contents:\n");
        while(1)
        {
            if(feof(fp))
                {
                    break;
                }
            total_branches+=1;
            fscanf(fp,"%llx %c\n",&address,&t_n);

            if(assoc!=0)
            makebtb(x,assoc,address,bits_btb,t_n,btb);      //calling BTB

            /*decoding gshare index*/
            temp=address>>(2+gtablesize-h);
            temp=temp<<(64-h);
            temp=temp>>(64-h);
            temp=temp^ghr;

            temp1=address<<(64-(gtablesize-h+2));
            temp1=temp1>>(64-(gtablesize-h+2));
            temp1=temp1>>2;

            temp=temp<<(gtablesize-h);

            g_index_number=temp | temp1;

            /*decoding bimodal index*/
            b_index_number=address>>2;
            b_index_number=b_index_number<<(64-btablesize);
            b_index_number=b_index_number>>(64-btablesize);

            /*decoding chooser index*/
            index_number=address>>2;
            index_number=index_number<<(64-tablesize);
            index_number=index_number>>(64-tablesize);

            g_temp_index=gvalues[g_index_number].counter;
            b_temp_index=bvalues[b_index_number].counter;

            if(btb_misses-prev==1)
                prev+=1;
            else
            {
                ghr=ghr>>1;                 //global history register update

                if(values[index_number].counter>=2)     //selecting gshare predictor
                {
                    if(gvalues[g_index_number].counter>=2)
                {
                    //g_temp_index=gvalues[g_index_number].counter;
                    predictions+=1;
                    if(t_n=='t')
                    {
                         ghr=ghr<<(64-h);
                         ghr=ghr | b;
                         ghr=ghr>>(64-h);
                         if(gvalues[g_index_number].counter==3)
                            gvalues[g_index_number].counter=3;
                         else gvalues[g_index_number].counter+=1;
                    }
                    else
                    {
                        mispredictions+=1;
                        if(gvalues[g_index_number].counter==0)
                            gvalues[g_index_number].counter=0;
                        else gvalues[g_index_number].counter-=1;
                    }
                }

                else
                {
                    predictions+=1;
                    if(t_n=='t')
                    {
                         ghr=ghr<<(64-h);
                         ghr=ghr | b;
                         ghr=ghr>>(64-h);
                         mispredictions+=1;
                         if(gvalues[g_index_number].counter==3)
                            gvalues[g_index_number].counter=3;
                         else gvalues[g_index_number].counter+=1;
                    }
                    else
                    {

                        if(gvalues[g_index_number].counter==0)
                            gvalues[g_index_number].counter=0;
                        else gvalues[g_index_number].counter-=1;
                    }
                }
                }

                else                                            //selecting bimodal predictor
                {
                    if(bvalues[b_index_number].counter>=2)
                    {
                        predictions+=1;
                        if(t_n=='t')
                        {
                             ghr=ghr<<(64-h);
                             ghr=ghr | b;
                             ghr=ghr>>(64-h);
                             if(bvalues[b_index_number].counter==3)
                                bvalues[b_index_number].counter=3;
                             else bvalues[b_index_number].counter+=1;
                        }
                        else
                        {
                            mispredictions+=1;
                            if(bvalues[b_index_number].counter==0)
                                bvalues[b_index_number].counter=0;
                            else bvalues[b_index_number].counter-=1;
                        }
                    }

                    else
                    {
                        predictions+=1;
                        if(t_n=='t')
                        {
                             ghr=ghr<<(64-h);
                             ghr=ghr | b;
                             ghr=ghr>>(64-h);
                             mispredictions+=1;
                             if(bvalues[b_index_number].counter==3)
                                bvalues[b_index_number].counter=3;
                             else bvalues[b_index_number].counter+=1;
                        }
                        else
                        {

                            if(bvalues[b_index_number].counter==0)
                                bvalues[b_index_number].counter=0;
                            else bvalues[b_index_number].counter-=1;
                        }
                    }
                }


                /*updating the chooser table*/
                if(t_n=='t' && ((g_temp_index>=2 && b_temp_index>=2) || (g_temp_index<2 && b_temp_index<2)))
                    values[index_number].counter+=0;
                else if(t_n=='n' && ((g_temp_index>=2 && b_temp_index>=2) || (g_temp_index<2 && b_temp_index<2)))
                        values[index_number].counter+=0;
                else if(t_n=='t' && g_temp_index>=2 && b_temp_index<2)
                {
                    if(values[index_number].counter==3)
                        values[index_number].counter=3;
                    else values[index_number].counter+=1;
                }

                else if(t_n=='n' && g_temp_index<2 && b_temp_index>=2)
                {
                    if(values[index_number].counter==3)
                        values[index_number].counter=3;
                    else values[index_number].counter+=1;
                }

                else if(t_n=='t' && g_temp_index<2 && b_temp_index>=2)
                {
                    if(values[index_number].counter==0)
                            values[index_number].counter=0;
                    else values[index_number].counter-=1;
                }

                else if(t_n=='n' && g_temp_index>=2 && b_temp_index<2)
                {
                    if(values[index_number].counter==0)
                            values[index_number].counter=0;
                    else values[index_number].counter-=1;
                }
            }


        }
        if(assoc!=0)
        {
            printf("\nFinal BTB Tag Array Contents {valid, pc}: \n");
            for(i=0;i<y;i++)
            {printf("Set     %d:   ",i);
		for(j=0;j<assoc;j++)
		{                
			printf("{%d, 0x  %lx}  ",btb[i][j].valid,btb[i][j].tag);
		}
		printf("\n");
            }
        }

        if(assoc!=0) printf("\n\nFinal Bimodal Table Contents:\n");
	else printf("\nFinal Bimodal Table Contents:\n");
        for(i=0;i<b1;i++)
        {
            printf("table[%d]: %d\n",i,bvalues[i].counter);
        }

        printf("\nFinal GShare Table Contents:\n");
        for(i=0;i<g;i++)
        {
            printf("table[%d]: %d\n",i,gvalues[i].counter);
        }
        printf("\nFinal GHR Contents: 0x       %llx \n",ghr);
        printf("\nFinal Chooser Table Contents:\n");
        for(i=0;i<x;i++)
        {
            printf("Choice table[%d]: %d\n",i,values[i].counter);
        }

        printf("\nFinal Branch Predictor Statistics:\n");
        printf("a. Number of branches: %d\n",total_branches);
        printf("b. Number of predictions from the branch predictor: %d\n",predictions);
        printf("c. Number of mispredictions from the branch predictor: %d\n",mispredictions);
        printf("d. Number of mispredictions from the BTB: %d\n",btb_mispredictions);
        printf("e. Misprediction Rate: %.2f percent\n",(((float)mispredictions+(float)btb_mispredictions)/(float)total_branches)*100);
    }

    else if(strcasecmp(argv[1],"yehpatt")==0)
    {
        int h,p,y,z;
        unsigned long long b=pow(2,63);

        h=atoi(argv[2]);
        p=atoi(argv[3]);
        x=pow(2,h);
        y=pow(2,p);

        assoc=atoi(argv[5]);
        bits_btb=atoi(argv[4]);
        z=pow(2,bits_btb);


        BTB btb[4100][32];

        for(i = 0;i<z;i++)
        {
            for(j=0;j<assoc;j++)
            {
                btb[i][j].valid = 0;
                //btb[i][j].dirty = 0;
                btb[i][j].tag=0;
                btb[i][j].policycounter=0;
            }
        }

        struct htable hvalues[x];
        struct table values[y];

        for(i=0;i<x;i++)
        {
            hvalues[i].local_reg=0;
        }

        for(i=0;i<y;i++)
        {
            values[i].counter=2;
        }

        FILE *fp;
        fp=fopen(argv[6],"r");//reading trace file

        fseek(fp,0,SEEK_SET);
        if(fp==NULL)
        {
            printf("\nFile does not contain any data: Exiting\n");
            exit(1);
        }

        printf("Command Line:\n");
        printf("./sim_bp %s %s %s %s %s %s\n",argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);

        while(1)
        {
            if(feof(fp))
                {
                    break;
                }
            total_branches+=1;
            fscanf(fp,"%llx %c\n",&address,&t_n);

            if(assoc!=0)
            makebtb(y,assoc,address,bits_btb,t_n,btb);      //calling BTB

            /*decoding history index*/
            index_number=address>>2;
            index_number=index_number<<(64-h);
            index_number=index_number>>(64-h);

            if(btb_misses-prev==1)
                prev+=1;
            else if(values[hvalues[index_number].local_reg].counter>=2)
                {
                    predictions+=1;
                    if(t_n=='t')
                    {
                         if(values[hvalues[index_number].local_reg].counter==3)
                            values[hvalues[index_number].local_reg].counter=3;
                         else values[hvalues[index_number].local_reg].counter+=1;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg>>1;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg<<(64-p);
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg | b;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg>>(64-p);
                    }
                    else
                    {
                        mispredictions+=1;
                        if(values[hvalues[index_number].local_reg].counter==0)
                            values[hvalues[index_number].local_reg].counter=0;
                        else values[hvalues[index_number].local_reg].counter-=1;
                        hvalues[index_number].local_reg=hvalues[index_number].local_reg>>1;
                    }
                }

                else
                {
                    predictions+=1;
                    if(t_n=='t')
                    {
                         mispredictions+=1;
                         if(values[hvalues[index_number].local_reg].counter==3)
                            values[hvalues[index_number].local_reg].counter=3;
                         else values[hvalues[index_number].local_reg].counter+=1;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg>>1;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg<<(64-p);
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg | b;
                         hvalues[index_number].local_reg=hvalues[index_number].local_reg>>(64-p);
                    }
                    else
                    {

                        if(values[hvalues[index_number].local_reg].counter==0)
                            values[hvalues[index_number].local_reg].counter=0;
                        else values[hvalues[index_number].local_reg].counter-=1;
                        hvalues[index_number].local_reg=hvalues[index_number].local_reg>>1;
                    }
                }
            }

        if(assoc!=0)
        {
            printf("\nFinal BTB Tag Array Contents {valid, pc}: \n");
            for(i=0;i<z;i++)
            {printf("Set     %d:   ",i);
		for(j=0;j<assoc;j++)
		{                
			printf("{%d, 0x  %lx}  ",btb[i][j].valid,btb[i][j].tag);
		}
		printf("\n");
            }
        }
        if(assoc!=0) printf("\n\nFinal History Table Contents: \n");
	else printf("\nFinal History Table Contents: \n");
        for(i=0;i<x;i++)
        {
            printf("table[%d]: 0x %llx\n",i,hvalues[i].local_reg);
        }

        printf("\nFinal Prediction Table Contents: \n");
        for(i=0;i<y;i++)
        {
            printf("table[%d]: %d\n",i,values[i].counter);
        }

        printf("\nFinal Branch Predictor Statistics:\n");
        printf("a. Number of branches: %d\n",total_branches);
        printf("b. Number of predictions from the branch predictor: %d\n",predictions);
        printf("c. Number of mispredictions from the branch predictor: %d\n",mispredictions);
        printf("d. Number of mispredictions from the BTB: %d\n",btb_mispredictions);
        printf("e. Misprediction Rate: %.2f percent\n",(((float)mispredictions+(float)btb_mispredictions)/(float)total_branches)*100);
    }
    return 0;
}

void makebtb(int x,int assoc,unsigned long long address,int tablesize,char t_n,BTB btb[][32])
{
    //printf("Hey inside makebtb func\n");
    unsigned long long tag=0,index=0;
    int counter=0,counter2=0,rep_candidate=0;
    int i;

    btb_accesses+=1;
    policy_counter+=1;



    /*decoding tag*/
    tag=address;
    //printf("%lx\n",tag);
    /*decoding index*/
    index=(address<<(64-(tablesize+2)));
    index=index>>(64-(tablesize+2));
    index=index>>2;
    //printf("%lx\n",index);
    //printf("%d",btb[index][0].valid);
    for(i=0;i<assoc;i++)
    {

        if(btb[index][i].valid==1 && (tag==btb[index][i].tag))        //handling btb hits
        {
            //printf("heya\n");
            btb_hits+=1;
            counter=1;
            btb[index][i].policycounter=policy_counter;
            //btb[index][i].dirty=1;
            break;
        }
    }
    if(counter==0)          //handling btb misses
    {
        btb_misses+=1;
        if(t_n=='t')
            btb_mispredictions+=1;
        for(i=0;i<assoc;i++)
        {
            if(btb[index][i].valid==0)
            {
                rep_candidate=i;
                break;
            }
            else
                counter2+=1;
        }
        if(counter2==assoc)
        {
            rep_candidate=btbpolicy(btb,index,assoc);
        }

        /*if(btb[index][rep_candidate].dirty==1 && btb[index][rep_candidate].valid==1)
        {
            L1writebacks+=1;
            if(L2cache_size!=0)
            L2writecache(L1cache,L2cache,rep_candidate);
        }
        if(L2cache_size!=0)
        L2readcache(L2cache);*/

        btb[index][rep_candidate].tag=tag;
        btb[index][rep_candidate].policycounter=policy_counter;
        //btb[index][rep_candidate].dirty=1;
        btb[index][rep_candidate].valid=1;
    }
    return;
}

int btbpolicy(BTB btb[][32],unsigned long long index,int assoc)
{
    //printf("Hey inside btbpolicy func\n");
    int i,lru=0,lruway=0;

    lru=btb[index][0].policycounter;
    for(i=1;i<assoc;i++)
    {

        if(btb[index][i].policycounter<lru)
        {
            lru=btb[index][i].policycounter;
            lruway=i;
        }

    }
    return lruway;
}
