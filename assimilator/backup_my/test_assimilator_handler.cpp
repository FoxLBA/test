#include "backend_lib.h"
#include "validate_util.h"
#include "filesys.h"
#include "test_assimilator.h"

int rmerge(task_t task, vector<RESULT> results) {
    FILE *output;
    FILE *result;
    char output_filename[255];
    std::string result_file_name;
    char buff[65535];
    int retval;

//    retval = boinc_mkdir(config.project_path("sample_results"));
//    if (retval) {
//        log_messages.printf(MSG_NORMAL,"Can't create output folder: %s\n", config.project_path("sample_results"));
//        return(1);
//    }
    sprintf(output_filename, "%s_%s_%d", task.app_name, task.name, task.timestamp);
    //открытие файла
    output = fopen(config.project_path("sample_results/%s/%s", task.login, output_filename), "a+");
    if (!output) {
        log_messages.printf(MSG_CRITICAL,"Output file open failed: %s\n", output_filename);
        return(1);
    }

    for (unsigned int i = 0; i < task.size; i++) {
        get_output_file_path(results[i], result_file_name);
        result = fopen(result_file_name.c_str(), "r");
        if (!result) {
            log_messages.printf(MSG_CRITICAL,"Result file open failed: %s\n", result_file_name.c_str());
            return(1);
        }
        fgets(buff, sizeof(buff), result);
        fputs(buff, output);
        fclose(result);
    }

    fclose(output);
    return 0;
}
