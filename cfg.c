#include "cfg.h"
#include "cJSON.h"

#define CFG_INI_PATH "./cfg/cfg.ini"
#define CFG_DTU_PATH "./cfg/dtu.json"

cfg_serial_t cfg_serial[MAX_SERIAL_NUM];
dtu_model_t dtu_model;

char* get_ini_key_string(char *title,char *key,char *filename)  
{  
    FILE *fp;  
    int  flag = 0;  
    char sTitle[32], *wTmp,*ret;  
    static char sLine[1024];  
    ret = (char*)malloc(32);
    memset(ret,0,32);
  
    sprintf(sTitle, "[%s]", title);  
    if(NULL == (fp = fopen(filename, "r"))) {  
        perror("fopen");  
        return NULL;  
    }

    // printf("open ok!\n");  
  
    while (NULL != fgets(sLine, 1024, fp)) {  
        // printf("%s\n",sLine);
        // 这是注释行  
        if (0 == strncmp("//", sLine, 2)) continue;  
        if ('#' == sLine[0])              continue;  
  
        wTmp = strchr(sLine, '=');  
        if ((NULL != wTmp) && (1 == flag)) 
        {  
            if (0 == strncmp(key, sLine, strlen(key))) 
            { // 长度依文件读取的为准  
                sLine[strlen(sLine) - 1] = '\0';  
                fclose(fp);  
                // printf("%s\n",wTmp+1);
                // 忽略空格
                sscanf(wTmp + 1,"%s",ret);

                return ret;  
            }  
        } 
        else 
        {  
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) 
            { // 长度依文件读取的为准  
                // printf("find title:%s\n",sTitle);  
                flag = 1; // 找到标题位置  
            }  
        }

        memset(sLine,0,sizeof(sLine));  
    }  
    fclose(fp);  
    return NULL;  
}  

int get_ini_key_int(char *title,char *key,char *filename)  
{  
    char* strval = get_ini_key_string(title, key, filename);;
    int intval = atoi(strval);
    free(strval); 

    return intval;  
} 

int cfg_read_ini(void)
{
    cfg_serial[1].map_port = get_ini_key_int("serial","COM2",CFG_INI_PATH);
    cfg_serial[2].map_port = get_ini_key_int("serial","COM3",CFG_INI_PATH);
    cfg_serial[3].map_port = get_ini_key_int("serial","COM4",CFG_INI_PATH);
    cfg_serial[4].map_port = get_ini_key_int("serial","COM5",CFG_INI_PATH);
    cfg_serial[5].map_port = get_ini_key_int("serial","COM6",CFG_INI_PATH);
    cfg_serial[0].map_port = get_ini_key_int("serial","COM1",CFG_INI_PATH);
    cfg_serial[6].map_port = get_ini_key_int("serial","COM7",CFG_INI_PATH);
    cfg_serial[7].map_port = get_ini_key_int("serial","COM8",CFG_INI_PATH);
    cfg_serial[8].map_port = get_ini_key_int("serial","COM9",CFG_INI_PATH);
    cfg_serial[9].map_port = get_ini_key_int("serial","COM10",CFG_INI_PATH);
    cfg_serial[10].map_port = get_ini_key_int("serial","COM11",CFG_INI_PATH);
    cfg_serial[11].map_port = get_ini_key_int("serial","COM12",CFG_INI_PATH);
    cfg_serial[12].map_port = get_ini_key_int("serial","COM13",CFG_INI_PATH);
    cfg_serial[13].map_port = get_ini_key_int("serial","COM14",CFG_INI_PATH);
    cfg_serial[14].map_port = get_ini_key_int("serial","COM15",CFG_INI_PATH);
    cfg_serial[15].map_port = get_ini_key_int("serial","COM16",CFG_INI_PATH);
    return 0;
}


int cfg_read_dtu(void)
{
    char* cfg_buf = (char*)file_read(CFG_DTU_PATH);
    if(cfg_buf == NULL)
    {
        return -1;
    }

    cJSON *root = cJSON_Parse(cfg_buf);
    if (root == NULL)
    {
        printf("json parsing dtu cfg null\n");
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return -1;
    }

    // parsing model
    cJSON* basic = cJSON_GetObjectItem(root,"basic");
    cJSON* system = cJSON_GetObjectItem(basic,"system");
    cJSON* model = cJSON_GetObjectItem(system,"model");
    if(model == NULL)
    {
        printf("parsing basic.sys.model NULL!\n");
        return -1;
    }
    dtu_model = modelstring_to_model(model->valuestring);

    // parsing ports
    cJSON* nodes = cJSON_GetObjectItem(root,"collect_nodes");
    cJSON* node = NULL;
    cJSON_ArrayForEach(node,nodes)
    {
        // port
        cJSON* port = cJSON_GetObjectItem(node,"port");
        cJSON* type = cJSON_GetObjectItem(port,"type");
        
        dtu_com_t com = comstring_to_com(type->valuestring);
        cfg_serial[(int)com].enable = 1;
        memcpy(cfg_serial[(int)com].com,type->valuestring,strlen(type->valuestring));
        cfg_serial[(int)com].com_id = com;

        cJSON* type_setting = cJSON_GetObjectItem(port,"setting");
        cJSON* format = cJSON_GetObjectItem(type_setting,"format");
        cJSON* baudrate = cJSON_GetObjectItem(type_setting,"baudrate");
        cfg_serial[(int)com].baudrate = atoi(baudrate->valuestring);
        cfg_serial[(int)com].format = formatstring_to_format(format->valuestring);
    }

    return 0;
}

void cfg_init(void)
{
    dtu_model = DTU_MODEL_NULL;
    memset(cfg_serial,0,sizeof(cfg_serial));

    printf("parsing cfg.ini\n");
    cfg_read_ini();

    printf("parsing dtu.json\n");
    cfg_read_dtu();

    int i = 0;  
    for(i=0;i<MAX_SERIAL_NUM;i++)
    {
        cfg_serial[i].fd = -1;
        printf("enable:%d,com:%s,id:%d,baudrate:%d,format:%d\n",
            cfg_serial[i].enable,cfg_serial[i].com,cfg_serial[i].com_id,
            cfg_serial[i].baudrate,cfg_serial[i].format);
    }
}