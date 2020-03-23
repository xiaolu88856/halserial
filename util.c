#include "util.h"

// Name: file_read
// Func: read file info to buf
// Input: path -- file path
//        buf  -- file buf
// Output: NULL--error
//
char* file_read(char* path)
{
    FILE* fp;
    int fp_size;
    char* buf;
    
    fp = fopen(path,"rb");
    if(!fp)
    {
        printf("cannot find config file: %s!\n",path);
        return NULL;
    }
    // calculate file size
    fseek(fp,0,SEEK_END);
    fp_size = ftell(fp);
    rewind(fp);

    // read file
    buf = (char*)malloc(sizeof(char)*fp_size);
    if(buf == NULL)
    {
        printf("file buffer malloc error!\n");
        fclose(fp);
        return NULL;
    }

    int result = fread(buf,1,fp_size,fp);
    if(result != fp_size)
    {
        printf("file read error\n");
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return buf;
}

dtu_model_t modelstring_to_model(char* model)
{
    if(strcmp(model,"S3100C2E1BD") == 0)
    {
        return DTU_NISE_R2E1;
    }   
    if(strcmp(model,"S3100C4E2BD") == 0)
    {
        return DTU_NISE_R4E2;
    }
    if(strcmp(model,"S3100C8E2BD") == 0)
    {
        return DTU_NISE_R8E2;
    }
    if(strcmp(model,"S3100C16E2BJ") == 0)
    {
        return DTU_NISE_R16E2;
    }
    if(strcmp(model,"S20C4") == 0)
    {
        return DTU_S20_R4E2;
    }
    if(strcmp(model,"S20C8") == 0)
    {
        return DTU_S20_R8E2;
    }
    if(strcmp(model,"PC-WIN") == 0)
    {
        return DTU_PC_WIN;
    }
    if(strcmp(model,"PC-LINUX") == 0)
    {
        return DTU_PC_LINUX;
    }
    return DTU_MODEL_NULL;
}



dtu_com_t comstring_to_com(const char* com)
{
    if(strcmp(com,"COM1") == 0)
        return DTU_COM1;
    if(strcmp(com,"COM2") == 0)
        return DTU_COM2;
    if(strcmp(com,"COM3") == 0)
        return DTU_COM3;
    if(strcmp(com,"COM4") == 0)
        return DTU_COM4;
    if(strcmp(com,"COM5") == 0)
        return DTU_COM5;
    if(strcmp(com,"COM6") == 0)
        return DTU_COM6;
    if(strcmp(com,"COM7") == 0)
        return DTU_COM7;
    if(strcmp(com,"COM8") == 0)
        return DTU_COM8;
    if(strcmp(com,"COM9") == 0)
        return DTU_COM9;
    if(strcmp(com,"COM10") == 0)
        return DTU_COM10;
    if(strcmp(com,"COM11") == 0)
        return DTU_COM11;
    if(strcmp(com,"COM12") == 0)
        return DTU_COM12;
    if(strcmp(com,"COM13") == 0)
        return DTU_COM13;
    if(strcmp(com,"COM14") == 0)
        return DTU_COM14;
    if(strcmp(com,"COM15") == 0)
        return DTU_COM15;
    if(strcmp(com,"COM16") == 0)
        return DTU_COM16;
    
    return DTU_COM1;
}


format_t formatstring_to_format(const char* format)
{
    if(strcmp(format,"N81") == 0)
        return FORMAT_N81;
    if(strcmp(format,"E81") == 0)
        return FORMAT_E81;
    if(strcmp(format,"O81") == 0)
        return FORMAT_O81;
    if(strcmp(format,"N82") == 0)
        return FORMAT_N82;
    
    return FORMAT_N81;
}