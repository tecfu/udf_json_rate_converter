/**
 * mysql> SHOW VARIABLES LIKE 'plugin_dir';
 * 
 * RETURNS i.e. 
 * /usr/lib/mysql/plugin/
 * 
 * $ mv plugin.so /usr/lib/mysql/plugin/
 * 
 * 
 * create function udf_json_high_rate returns real soname 'udf_json_rate_converter.so';
 * 
 * drop function udf_json_high_rate
 * 
 * select udf_json_high_rate();
 * select udf_json_high_rate('[{"q":1,"p":19.00},{"q":21,"p":18.5},{"q":30,"p":13.5}]');
 * select udf_json_high_rate('[{"q":25,"p":24},{"q":15,"p":20},{"q":20,"p":15},{"q":45,"p":15},{"q":50,"p":12.5}]');
 * 
 * 
 * create function udf_json_low_rate returns real soname 'udf_json_rate_converter.so';
 * SELECT * FROM mysql.func;
 * 
 * drop function udf_json_low_rate
 * 
 * select udf_json_low_rate();
 * select udf_json_low_rate('[{"q":1,"p":19.00},{"q":21,"p":18.5},{"q":30,"p":13.5}]');
 */

#include <stdio.h>
#include <string.h>
#include "jansson.h"

#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <ctype.h>



/*
 * 
 */
my_bool udf_json_high_rate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{  
    if (args->arg_count != 1)
    {
      //WILL NOT BUILD WITH THESE
      strcpy(message,"Must pass argument to function");
      //fprintf(stderr, "Wrong argument.");
      return 1;
    }
    
    initid->decimals = 2;
    return 0;
}


double udf_json_high_rate(
    UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,
    char *is_null, char *error
){  
    
    //GET ARGS[0] AS STRING
    const char *json_str=args->args[0];

    int i;
    json_t *root;
    json_error_t jsonerror;
    root = json_loads(json_str,JSON_DISABLE_EOF_CHECK,&jsonerror);
   
    double high_rate = -1;

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jsonerror.line, jsonerror.text);
        return 0;
    }
    if(!json_is_array(root))
    {
        fprintf(stderr, "error: root is not an array\n");
        json_decref(root);
        return 0;
    }

    for(i = 0; i < json_array_size(root); i++)
    {
        json_t *data, *p;

        double rate;
        data = json_array_get(root, i);
        if(!json_is_object(data))
        {
            fprintf(stderr, "error: commit data %d is not an object\n", i + 1);
            json_decref(root);
            return 0;
        }
        
        p = json_object_get(data, "p");
        if(!json_is_real(p) && !json_is_integer(p))
        {
            fprintf(stderr, "error: p is not a number @ price point '%d'\n", i + 1);
            json_decref(root);
            return 0;
        }
        
        if(json_is_real(p)){
            rate = json_real_value(p);
        }
        
        if(json_is_integer(p)){
            rate = json_integer_value(p);
        }
        
        if(high_rate < rate){
            high_rate = rate;
        }
        
        //fprintf(stdout, "%.2f\n",rate);
    }
    json_decref(root);
    
    //PRINT HIGH RATE
    //fprintf(stdout, "%.2f\n",high_rate);

    return high_rate;
}



my_bool udf_json_low_rate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{  
    if (args->arg_count != 1)
    {
      //WILL NOT BUILD WITH THESE
      strcpy(message,"Must pass argument to function.");
      //fprintf(stderr, "Wrong argument.");
      return 1;
    }
    
    initid->decimals = 2;
    return 0;
}

double udf_json_low_rate(
    UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,
    char *is_null, char *error
)
{
    //GET ARGS[0] AS STRING
    const char *json_str=args->args[0];
    //const char *json_str;
    //return 0;

    int i;
    json_t *root;
    json_error_t jsonerror;
    root = json_loads(json_str,JSON_DISABLE_EOF_CHECK,&jsonerror);
   
    //STARTING VALUE
    double low_rate = -1;

/**/ 
    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jsonerror.line, jsonerror.text);
        return 0;
    }
    if(!json_is_array(root))
    {
        fprintf(stderr, "error: root is not an array\n");
        json_decref(root);
        return 0;
    }

    for(i = 0; i < json_array_size(root); i++)
    {
        json_t *data, *p;

        double rate;
        data = json_array_get(root, i);
        if(!json_is_object(data))
        {
            fprintf(stderr, "error: commit data %d is not an object\n", i + 1);
            json_decref(root);
            return 0;
        }
        
        p = json_object_get(data, "p");
        if(!json_is_real(p) && !json_is_integer(p))
        {
            fprintf(stderr, "error: p is not a number @ point '%d'\n", i + 1);
            json_decref(root);
            return 0;
        }
        
        if(json_is_real(p)){
            rate = json_real_value(p);
        }
        
        if(json_is_integer(p)){
            rate = json_integer_value(p);
        }
        
        if(low_rate == -1){
            low_rate = rate;
        }
        if(rate < low_rate){
            low_rate = rate;
        }
        
        //fprintf(stdout, "%.2f\n",rate);
    }
    json_decref(root);
    
    //PRINT HIGH RATE
    //fprintf(stdout, "%.2f\n",high_rate);

    return low_rate;
}



/**
 * 
 * @param initid
 * @param args
 * @param message
 * @return 
 * 
 * create function udf_is_json returns integer soname 'udf_json_rate_converter.so';
 */
my_bool udf_is_json_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{  
    if (args->arg_count != 1)
    {
      strcpy(message,"Must pass argument to function check_json");
      return 1;
    }

    return 0;
}



long udf_is_json(
    UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length,
    char *is_null, char *error
) 
{
    //GET ARGS[0] AS STRING
    char *json_str= NULL;
    json_str = args->args[0];
    
    long r;
    
    //ADD EOF [JANSSON DECODE WILL FAIL IF DOES NOT EXIST]
    //strcat(json_str,"\0");
    
    json_t *root;
    json_error_t jsonerror;
    root = json_loads(json_str,JSON_DISABLE_EOF_CHECK,&jsonerror);
    
    if(!root)
    {
        long long    l;
        l = -(args->lengths[0]);

        unsigned long argLength = args->lengths[0];
        char *stringText = args->args[0];

        //CHECK IF INPUT IS EMPTY
        if(stringText[0] == '\0'){
            fprintf(stderr, "%s", "udf_is_json : no string to decode");
            return l;
        }
        else{
            char* myValue = NULL;
            myValue = (char *)malloc(argLength);

            strncpy(myValue, json_str,argLength);

            fprintf(stderr, "jansson json decoding error: on line %d: %s\n", jsonerror.line, stringText);
            fprintf(stderr, "args->args[%d] = >>%s<<\n\n", 0, myValue);

            r = l;
        }
    }
    else{
        r = 1;
    }

    return r;
}