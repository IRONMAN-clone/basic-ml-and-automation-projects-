// Very simple calculation automation
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define I32_PER_KEY_TEST 6

typedef enum {
    KEY,
    VAL
}State;

// For now only supports integer operations
typedef struct pair {
    int keylen,vallen;
    char **key;
    char **val;
}Pair;

typedef enum {
    PLUS,
    MIN,
    DIV,  // coming soon along with user-defined
    MUL,  // ''      ''   ''   ''    ''  ''
    USER_DEFINED, // Coming soon
    UNDEFINED
}IntrOps;

int training_data_key_amount(const char* filepath,char *mark_val_frm){
    FILE *training_data = fopen(filepath,"r");
    int keys = 0;

    // State machine
    State this_state = KEY; 

    char *buf = (char*)malloc(sizeof(char)*256);
    while(fgets(buf,256,training_data) != NULL)
    {
        char *tokenizer = strtok(buf,mark_val_frm);
        while(tokenizer != NULL)
        { 
            if(this_state == KEY)
            {
                keys++;
                this_state = VAL;
            }
            else if(this_state == VAL)
            {
                this_state = KEY;
            }
            tokenizer = strtok(NULL,mark_val_frm);
        }       
    }

    fclose(training_data);
    return keys;
}

int training_data_val_amount(const char* filepath,char *mark_val_frm){
    FILE *training_data = fopen(filepath,"r");
    int vals = 0;

    // State machine
    State this_state = KEY; 

    char *buf = (char*)malloc(sizeof(char)*256);
    while(fgets(buf,256,training_data) != NULL)
    {
        char *tokenizer = strtok(buf,mark_val_frm);
        while(tokenizer != NULL)
        { 
            if(this_state == KEY)
            {
                this_state = VAL;
            }
            else if(this_state == VAL)
            {
                vals++;
                this_state = KEY;
            }
            tokenizer = strtok(NULL,mark_val_frm);
        }       
    }

    fclose(training_data);
    return vals;
}

Pair read_pair_from_data(const char* filepath,char *mark_val_frm) {
    FILE *training_data = fopen(filepath,"r");
    Pair parsed_pair;
    parsed_pair.keylen = 0;
    parsed_pair.vallen = 0;
    
    /* Initialization of pairs */
    int keys_found = training_data_key_amount(filepath,mark_val_frm);
    int vals_found = training_data_val_amount(filepath,mark_val_frm);

    parsed_pair.key = (char**)malloc(sizeof(char*)*keys_found);
    parsed_pair.val = (char**)malloc(sizeof(char*)*vals_found);
    for(int i = 0; i < keys_found; i++)
        parsed_pair.key[i] = (char*)malloc(sizeof(char)*256);
    for(int j = 0; j < vals_found; j++)
        parsed_pair.val[j] = (char*)malloc(sizeof(char)*256);
    /* Initialization of pairs */

    // State machine
    State this_state = KEY; 

    char *buf = (char*)malloc(sizeof(char)*256);
    while(fgets(buf,256,training_data) != NULL)
    {
        char *tokenizer = strtok(buf,mark_val_frm);
        while(tokenizer != NULL)
        { 
            if(this_state == KEY)
            {
                int n = 0;
                char *tmp = (char*)malloc(sizeof(char)*strlen(tokenizer)+1);
                for(; n < strlen(tokenizer); n++)
                    tmp[n] = tokenizer[n];
                tmp[n++] = '\0';

                parsed_pair.key[parsed_pair.keylen] = tmp;
                parsed_pair.keylen++;

                this_state = VAL;
            }
            else if(this_state == VAL)
            {
                int n = 0;
                char *tmp = (char*)malloc(sizeof(char)*strlen(tokenizer)+1);
                for(; n < strlen(tokenizer); n++)
                    tmp[n] = tokenizer[n];
                tmp[n++] = '\0';

                parsed_pair.val[parsed_pair.vallen] = tmp;
                parsed_pair.vallen++;

                this_state = KEY;
            }
            tokenizer = strtok(NULL,mark_val_frm);
        }       
    }

    fclose(training_data);
    return parsed_pair;
}

IntrOps train_input_data(Pair train)
{
    IntrOps opFound = UNDEFINED;

    int **key_as_i32 = (int**)malloc(sizeof(int*)*train.keylen);
    int i32_global_push = 0;
    int *i32_family_push = (int*)malloc(sizeof(int)*train.keylen);
    int *val_as_i32 = (int*)malloc(sizeof(int)*train.vallen);

    // Key has a length limit per training data test
    for(int i = 0; i < train.keylen; i++)
        key_as_i32[i] = (int*)malloc(sizeof(int)*I32_PER_KEY_TEST);

    for(int n = 0; n < train.vallen; n++)
    {
        // we are assuming that the value is a single i32 integer
        val_as_i32[n] = atoi(train.val[n]);
    } 

    for(int j = 0; j < train.keylen; j++)
    {
        i32_family_push[j] = 0;
        char *training_data = train.key[j];
        
        char delim[] = {',','\0','\n',' '};
        char *tokenizer = strtok(training_data,delim);

        while(tokenizer != NULL)
        {
            int n = 0;
            char *tmp = (char*)malloc(sizeof(char)*strlen(tokenizer)+1);
            for(; n < strlen(tokenizer); n++)
                tmp[n] = tokenizer[n];

            tmp[n++] = '\0';
            key_as_i32[i32_global_push][i32_family_push[j]] = atoi(tmp);
            i32_family_push[j]++;

            tokenizer = strtok(NULL,delim);
        }
        i32_global_push++;
    } 


    // TODO: Fix the training
    // Here training the data begins
    // Basic: Assuming that the same operation has been done in all the datas   
    int p = 0;  
    int cases_matched = 0; // [matched == n] [failed == 0] [found!]  
    for(; p < train.vallen; p++)
    {
        int __expop__ = val_as_i32[p];
        int res1 = 0;
        int res2 = 1;

        int n1 = 0;
        for(; n1 < i32_family_push[p]; n1++){
            res1 += key_as_i32[p][n1];
            res2 *= key_as_i32[p][n1];        
        }

        if(res1 == __expop__)
        {
            opFound = PLUS;
            cases_matched++;
        }
        else if(res2 == __expop__) 
        {
            opFound = MUL;
            cases_matched++;
        }

    }   
    if(cases_matched != p)
    {
        opFound = UNDEFINED;
    }
    return opFound;
}


void produce_output(Pair qp,const char* opfile,IntrOps opcode)
{
    FILE *result = fopen(opfile,"w");


    if(opcode == PLUS)
    {   
        for(int j = 0; j < qp.keylen; j++)
        {
            int this_res = 0;
            char *lit = qp.key[j];
            char *prsv = (char*)malloc(sizeof(char)*256);

            for(int x = 0; x < strlen(qp.key[j]); x++)
            {
                prsv[x] = qp.key[j][x];
            }

            char *buf = (char*)malloc(sizeof(char)*(strlen(lit)+1));
            char delim[] = {',','\0','\n',' '};
            buf = strtok(lit,delim);
            while(buf != NULL)
            {
                this_res += atoi(buf);
                buf = strtok(NULL,delim);
            }
            char res_write[strlen(lit)+1];
            sprintf(res_write,"%d",this_res);
            
            fprintf(result,"[ ");
            fprintf(result,prsv);
            fprintf(result,"]");
            fprintf(result,"\n");
            fprintf(result,res_write);
            fprintf(result,"\n\n");
        }
    }
    if(opcode == MUL)
    {   
        for(int j = 0; j < qp.keylen; j++)
        {
            int this_res = 1;
            char *lit = qp.key[j];
            char *prsv = (char*)malloc(sizeof(char)*256);

            for(int x = 0; x < strlen(qp.key[j]); x++)
            {
                prsv[x] = qp.key[j][x];
            }

            char *buf = (char*)malloc(sizeof(char)*(strlen(lit)+1));
            char delim[] = {',','\0','\n',' '};
            buf = strtok(lit,delim);
            while(buf != NULL)
            {
                this_res *= atoi(buf);
                buf = strtok(NULL,delim);
            }
            char res_write[strlen(lit)+1];
            sprintf(res_write,"%d",this_res);
            
            fprintf(result,"[ ");
            fprintf(result,prsv);
            fprintf(result,"]");
            fprintf(result,"\n");
            fprintf(result,res_write);
            fprintf(result,"\n\n");
        }
    }
    printf("Succesfully produced output based on training!\n");
    fclose(result);
}


int main(int argc,char *argv[])
{
    fflush(stdout);

    int argcx = 2;
    char *argvx = "./t1.txt";
    char *q1 = "./q1.txt";

    if(argcx < 2) {
        printf("No path provided for training data and analysis\n");
        exit(1);
    }
    else {
        Pair inuse = read_pair_from_data(argvx,":");   
        // TODO: Train the 'inuse data and produce output based on it
        IntrOps fnd = train_input_data(inuse);
        if(fnd == UNDEFINED) 
        {
            printf("ERROR: Undefined operation!\n");
            exit(1);
        }
        else{
            Pair q_Pair = read_pair_from_data(q1,":");
            produce_output(q_Pair,"a1.txt",fnd);
        }
    }   

    return 0;
}