#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>



int main()
{
	int in ;
	int shmid1 , shmid2 , shmid3 ;
	printf("%s","Input the matrix dimension:" );
	scanf("%d", &in);


	key_t key1;
	key_t key2;
	key_t key3;
	shmid1 = shmget(key1, sizeof(unsigned int) * in * in  , IPC_CREAT |0666);
	shmid2 = shmget(key2 , sizeof(unsigned int) *in *in , IPC_CREAT |0666);	
	shmid3 = shmget(key3 , sizeof(unsigned int) *in *in , IPC_CREAT |0666);	
	
	unsigned int *exp1 =(unsigned int *)shmat(shmid1 , 0 , 0);	
	unsigned int i1 , j1,temp1=0 ;
	for(i1=0; i1<in ; i1++)
	{
		for(j1=0 ; j1<in ;j1++)
		{

			exp1[i1*in + j1] = temp1;
			temp1++;
		}
	}
	shmdt(exp1);
	
	unsigned int *exp2 =(unsigned int *)shmat(shmid2 , 0 , 0);
	unsigned int i2 , j2,temp2=0 ;
	for(i2=0; i2<in ; i2++)
	{
		for(j2=0 ; j2<in ;j2++)
		{
			exp2[i2*in + j2] = temp2;
			temp2++;
		}
	}
	shmdt(exp2);
	
		pid_t pidarray;
		int iofpid = 0 ;

		int processnum_current=1;
		int processid;
		for(;processnum_current<=16;processnum_current++)
		{

			struct timeval start ,end;
			gettimeofday(&start, 0);
			for(iofpid=0 ; iofpid<processnum_current ;iofpid++)
			{
			
					pidarray = fork();
					if(pidarray==0)
					{
						processid=iofpid+1;
						break;
					}
			
		
			}
		
			if(pidarray==0)
			{
				unsigned int *matrix_A =(unsigned int *)shmat(shmid1 , 0 , SHM_RDONLY);
				unsigned int *matrix_B =(unsigned int *)shmat(shmid2 , 0 , SHM_RDONLY); 
				unsigned int *matrix_C =(unsigned int *)shmat(shmid3 , 0 , 0);

				

				int row , col , size=in ;
				int times = size / processnum_current;
				row = (processid-1) * size * times;
				if(processid != processnum_current )
				{
					unsigned int tmp1=0;
					int i3 =0 ; 
		
					for(i3=0; i3<times ; i3++)
					{
						int j3=0 ,t1;
						for(t1=0 ; t1<size ;t1++)
						{
							for(j3=0 ; j3<size ; j3++)
								tmp1 += matrix_A[row+j3]*matrix_B[j3*size+t1];
							matrix_C[row+t1]=tmp1;
							tmp1=0;
						}

						row+=size;
					}	
				}
				else 
				{

					unsigned int tmp2=0;
					int i4 =0 ; 
					int last = size - times * (processnum_current-1);
		
					for(i4=0; i4<last ; i4++)
					{


						int j4=0 ,t2;
						for(t2=0 ; t2<size ;t2++)
						{							

							for(j4=0 ; j4<size ; j4++)
								{
									tmp2 += matrix_A[row + j4] * matrix_B[j4 * size + t2];
								}
							matrix_C[row + t2]=tmp2;
							tmp2=0;
						}
			
						row+=size;
					}
				}

				shmdt(matrix_A);
				shmdt(matrix_B);
				shmdt(matrix_C);

				exit(0);
			}
			else 
			{
				int t3;
				for(t3=0; t3<processnum_current ; t3++)
				wait(NULL);
				unsigned int *temp = (unsigned int*)shmat(shmid3 , 0 , SHM_RDONLY);
				unsigned int sum=0 , i5=0;
				for(; i5<in*in;i5++)
				sum+=temp[i5];	
				gettimeofday(&end, 0);

				int sec = end.tv_sec - start.tv_sec;
				int  usec = end.tv_usec - start.tv_usec;
				printf("Multiplying matrices using %d process \n",processnum_current );
				printf("Elapsed time %f sec ", sec*1.0+(usec/1000000.0));
				printf("Checksum: %u\n" ,(unsigned  int)sum);
				if(processnum_current==16)
					{
						shmctl(shmid1 , IPC_RMID , NULL);
						shmctl(shmid2 , IPC_RMID , NULL);
						shmctl(shmid3 , IPC_RMID , NULL);
					}
				

			}
		}
		
}