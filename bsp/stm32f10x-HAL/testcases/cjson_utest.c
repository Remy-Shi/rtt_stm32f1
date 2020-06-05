/*
*
* Date 			Author 			Notes
* -----------------------------------------------
* 2020-06-05 	Remy Shi 		the first version
*
*/
#define LOG_TAG	"cjson_utest"
#include <rtthread.h>
#include "utest.h"
#include "cJSON.h"

static void cjson_utest(void)
{
	/*********************** Declare variables ***********************/
	cJSON *json_file;
	cJSON *json_item;
	cJSON *json_obj;
	char *json_print;
	rt_uint32_t old_total_mem, old_used_mem, old_max_alloc_mem;
	rt_uint32_t new_total_mem, new_used_mem, new_max_alloc_mem;
	rt_memory_info(&old_total_mem, &old_used_mem, &old_max_alloc_mem);
	char *empty_json = "{}";
	json_file = cJSON_Parse(empty_json);
	// Add item to the JSON file
	cJSON_AddNullToObject(json_file, "NULL");
	cJSON_AddTrueToObject(json_file, "TRUE");
	cJSON_AddFalseToObject(json_file, "FALSE");
	cJSON_AddNumberToObject(json_file, "NUMBER", 10);
	cJSON_AddStringToObject(json_file, "STRING", "10");
	int int_array[] = {1, 2, 3, 4, 5};
	json_item = cJSON_CreateIntArray(int_array, sizeof(int_array)/sizeof(int));
	cJSON_AddItemToObject(json_file, "INT_ARRAY", json_item);
	float float_array[] = {1.1, 2.2, 3.3, 4.4, 5.5};
	json_item = cJSON_CreateFloatArray(float_array, sizeof(float_array)/sizeof(float));
	cJSON_AddItemToObject(json_file, "FLOAT_ARRAY", json_item);
	char *string_array[] = {"str_1", "str_2", "str_3"};
	json_item = cJSON_CreateStringArray((const char **)string_array, 3);
	cJSON_AddItemToObject(json_file, "STR_ARRAY", json_item);
	json_obj = cJSON_Duplicate(json_file, 1);
	cJSON_AddItemToObject(json_file, "JSON_OBJ", json_obj);
	// print the JSON file
	json_print = cJSON_Print(json_file);
	LOG_D("\n|============================== RAW JSON ==============================|");
	LOG_D("\n%s\n", json_print);
	LOG_D("\n|============================ MINIFY JSON =============================|");
	cJSON_Minify(json_print); // Minify JSON in order to store
	LOG_D("\n%s\n", json_print); // 'json_print' may be used to write to a file
	cJSON_Delete(json_file);
	
	json_file = cJSON_Parse(json_print); // 'json_print' may read from a file
	// Get item from the JSON file and check it
	json_item = cJSON_GetObjectItem(json_file, "NULL");
	uassert_int_equal(json_item->type, cJSON_NULL);
	json_item = cJSON_GetObjectItem(json_file, "TRUE");
	uassert_int_equal(json_item->type, cJSON_True);
	json_item = cJSON_GetObjectItem(json_file, "FALSE");
	uassert_int_equal(json_item->type, cJSON_False);
	json_item = cJSON_GetObjectItem(json_file, "NUMBER");
	uassert_int_equal(json_item->type, cJSON_Number);
	uassert_int_equal(json_item->valueint, 10);
	json_item = cJSON_GetObjectItem(json_file, "STRING");
	uassert_int_equal(json_item->type, cJSON_String);
	uassert_str_equal(json_item->valuestring, "10");
	json_item = cJSON_GetObjectItem(json_file, "INT_ARRAY");
	uassert_int_equal(json_item->type, cJSON_Array);
	uint8_t array_index = 0;
	json_item = cJSON_GetArrayItem(json_item, array_index);
	do{
		uassert_int_equal(json_item->type, cJSON_Number);
		uassert_int_equal(json_item->valueint, int_array[array_index]);
		array_index++;
		json_item = json_item->next;
	}while(RT_NULL != json_item);
	json_item = cJSON_GetObjectItem(json_file, "FLOAT_ARRAY");
	uassert_int_equal(json_item->type, cJSON_Array);
	array_index = 0;
	json_item = cJSON_GetArrayItem(json_item, array_index);
	do{
		uassert_int_equal(json_item->type, cJSON_Number);
		uassert_int_equal((float)json_item->valuedouble, float_array[array_index]);
		array_index++;
		json_item = json_item->next;
	}while(RT_NULL != json_item);
	json_item = cJSON_GetObjectItem(json_file, "STR_ARRAY");
	uassert_int_equal(json_item->type, cJSON_Array);
	array_index = 0;
	json_item = cJSON_GetArrayItem(json_item, array_index);
	do{
		uassert_int_equal(json_item->type, cJSON_String);
		uassert_str_equal(json_item->valuestring, string_array[array_index]);
		array_index++;
		json_item = json_item->next;
	}while(RT_NULL != json_item);
	/************************* Free variables ************************/
	cJSON_Delete(json_file);
	rt_free(json_print);
	rt_memory_info(&new_total_mem, &new_used_mem, &new_max_alloc_mem);
	uassert_int_equal(old_used_mem, new_used_mem);
}

static rt_err_t cjson_utest_init(void)
{
	return RT_EOK;
}

static rt_err_t cjson_utest_cleanup(void)
{
	return RT_EOK;
}

static void cjson_utestcase(void)
{
	UTEST_UNIT_RUN(cjson_utest);
}
UTEST_TC_EXPORT(cjson_utestcase, 		\
				"cjson_tc",			\
				cjson_utest_init, 		\
				cjson_utest_cleanup,	\
				10);
