/*
**
** Date 			Author 			Notes
** -----------------------------------------------
** 2020-06-07 	Remy Shi 		the first version
**
*/

#define LOG_TAG	"JSON_INIT"
#include <rtthread.h>
#include "cJSON.h"
#include <string.h>
#include <dfs_posix.h>
#include <stdio.h>
#include <ulog.h>

static const char original_params_json[] = { \
	"{ \
		\"sampling_rate\":3000, \
		\"sampling_nums\":8192, \
		\"lowpass_filter\":12000, \
		\"piezo_sen\":50.0, \
		\"bat_pwr_cal_factor\":0.99 \
	}" \
};

static const char original_time_json[] = { \
	"{ \
		\"alarm_cycle\":300, \
		\"bat_pwr_up_cycle\":3600, \
		\"temp_up_cycle\":300, \
		\"vib_eigen_up_cycle\":300, \
		\"time_domain_data_up_cycle\":7200, \
		\"freq_domain_data_up_cycle\":7200, \
		\"log_file_up_cycle\":86400, \
		\"cmd_monitor_cycle\":1800, \
		\"cmd_monitor_window_time\":20000 \
	}" \
};

static const char original_thresholds_json[] = { \
	"{ \
		\"bat_pwr_threshold\":10, \
		\"temp_threshold\":1000.1, \
		\"vel_rms_threshold\":1000.1, \
		\"acce_rms_threshold\":1000.1 \
	}" \
};

static const char original_prod_info_json[] = { \
	"{ \
		\"prod_model\":\"DVT200\", \
		\"prod_hw_version\":\"V1.0\", \
		\"prod_sw_version\":\"V1.0\", \
		\"prod_sn\":null, \
		\"nb_model\":null, \
		\"nb_hw_version\":null, \
		\"nb_sw_version\":null, \
		\"nb_imei\":null, \
		\"nb_imsi\":null, \
		\"nb_service_provider\":null, \
		\"nb_iccid\":null \
	}" \
};

struct original_jsons_stru{
	char *original_name;
	char *fs_name;
	char *date;
};

static const struct original_jsons_stru original_jsons[] = { \
	{"original_params_json", "params.json", (char *)original_params_json},
	{"original_time_json", "time.json", (char *)original_time_json},
	{"original_thresholds_json", "thresholds.json", (char *)original_thresholds_json},
	{"original_prod_info_json", "prod_info.json", (char *)original_prod_info_json}
};

static uint32_t size_of_str(char *str)
{
	uint32_t size = 0;
	char *str_copy = str;
	if(NULL == str_copy){
		return size;
	}
	while(*str_copy != '\0'){
			size++;
			str_copy++;
	}
	return size;
}

static int original_jsons_init_to_fs(void)
{
	cJSON *cjson;
	char *json_str = RT_NULL;
	char json_name[20];
	int fd = -1;
	for(uint8_t i=0; i<(sizeof(original_jsons)/sizeof(struct original_jsons_stru)); i++){
		// 生成文件绝对路径
		sprintf(json_name, "/%s", original_jsons[i].fs_name);
		// 检查文件是否存在
		fd = open(json_name, O_RDONLY);
		if(fd >= 0){
			LOG_I("File(%s) is already initialized", json_name);
			close(fd);
		}
		else{
			// 将原始json格式化并最小化
			cjson = cJSON_Parse(original_jsons[i].date);
			json_str = cJSON_Print(cjson);
			cJSON_Minify(json_str);
			LOG_D("%s", json_str);
			// 将原始json写入文件系统
			fd = open(json_name, O_WRONLY | O_CREAT);
			if(fd >= 0){
				write(fd, json_str, size_of_str(json_str));
				close(fd);
				LOG_I("=== Initialize %s to %s", original_jsons[i].original_name, json_name);
			}
			else{
				LOG_E("=== Open file(%s) failed", json_name);
			}
			cJSON_Delete(cjson);
			rt_free(json_str);
		}
	}
	return 0;
}
INIT_APP_EXPORT(original_jsons_init_to_fs);


static const char *json_name_list[] = {
		"original_params_json",
		"original_time_json",
		"original_thresholds_json",
		"original_prod_info_json",
};

static void show_origin_json_help(void)
{

	rt_kprintf("Usage: show_origin_json [json_name]/all\n");
	rt_kprintf("json_name list:\n");
	for(uint8_t i=0; i<sizeof(json_name_list)/sizeof(char*); i++){
			rt_kprintf("\t- %s\n", json_name_list[i]);
	}
}

static int show_origin_json(uint8_t argc, char **argv)
{
	if(argc < 2){
		show_origin_json_help();
	}
	else{
		cJSON *json_file;
		char *json_print;
		if(!strcmp(argv[1], "original_params_json")){
			json_file = cJSON_Parse(original_params_json);
		}
		else if(!strcmp(argv[1], "original_time_json")){
			json_file = cJSON_Parse(original_time_json);
		}
		else if(!strcmp(argv[1], "original_thresholds_json")){
			json_file = cJSON_Parse(original_thresholds_json);
		}
		else if(!strcmp(argv[1], "original_prod_info_json")){
			json_file = cJSON_Parse(original_prod_info_json);
		}
		else if(!strcmp(argv[1], "all")){
			for(uint8_t i=0; i<sizeof(original_jsons)/sizeof(struct original_jsons_stru); i++){
				json_file = cJSON_Parse(original_jsons[i].date);
				json_print = cJSON_Print(json_file);
				LOG_I("========== %s", original_jsons[i].original_name);
				rt_kprintf("%s\n", json_print);
			}
			cJSON_Delete(json_file);
			rt_free(json_print);
			return 0;
		}
		else{
			rt_kprintf(">>> Bad json_name\n");
			show_origin_json_help();
			return 0;
		}
		json_print = cJSON_Print(json_file);
		rt_kprintf("%s\n", json_print);
		cJSON_Delete(json_file);
		rt_free(json_print);
	}
	return 1;
}
MSH_CMD_EXPORT(show_origin_json, Show original json);
