//client

#include "sharedFunction.h"

int verificarSintax(char** transformacoes, int nTransf){//antes de enviar para server passar por isto
    int count=0;
    for (int i = 0; i < nTransf; i++){
        for (int j = 0; j < TRANS_NR; j++){
            if (!strcmp(transformacoes[i], transformacoesNome[j])){
                count++;
                break;
            }
        }
    }
    return count;
}

int main(int argc, char const *argv[]){
    int fd, n;
    int pid=getpid();

    if(argc==1){// ./sdstore
        char* info = "./sdstore status\n./sdstore proc-file priority input-filename output-filename transformation-id-1 transformation-id-2 ...\n";
        write(STDOUT_FILENO, info, strlen(info));
    }
    else if(argc==2 && !strcmp(argv[1],"status")){// ./sdstore status
        if ((fd=open("tmp/fifoWrite", O_WRONLY))==ERROR){
            perror("error opening fifoWrite");
            return ERROR;
        }
        char* buffer = malloc(1024);//make it non static
        sscanf(buffer, "%d status\n", pid);//pid deste processo antes do comando
        write(fd, buffer, strlen(buffer));//meter o \n ??????
        
        if ((fd=open("tmp/fifoRead", O_RDONLY))==ERROR){
            perror("error opening fifoRead");
            return ERROR;
        }

        while((n = read(fd,buffer,1024))>0){      
            write(STDOUT_FILENO,buffer,n);
        }

        //close(fd);

        free(buffer);
        
        if(n==ERROR){
            perror("error reading from fifo");
            return ERROR;
        }
    }
    else if(argc>=5 && !strcmp(argv[1], "proc-file")){// ./sdstore proc-file file-in file-out transf1...
        if(verificarSintax(&(argv[1]), argc-1)==argc-1){
            if ((fd=open("tmp/fifoWrite", O_WRONLY))==ERROR){
                perror("error opening fifoWrite");
                return ERROR;
            }
            char* buffer = malloc(1024);
            buffer="";
            for (int i = 1; i < argc; i++){
                buffer = concatStrings(buffer, argv[i]);
            }

            sscanf(buffer, "%d %s\n", pid, buffer);//pid deste processo antes do comando
            
            write(fd, buffer, strlen(buffer));
            
            if ((fd=open("tmp/fifoRead", O_RDONLY))==ERROR){
                perror("error opening fifoRead");
                return ERROR;
            }

            //wait pelas respostas ??

        }
        else{
            perror("syntax error in the transformations");
            return ERROR;
        }
    }
    else{
        perror("error on the input arguments");
        return ERROR;
    }

    return 1;
}
