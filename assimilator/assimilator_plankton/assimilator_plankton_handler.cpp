#include "backend_lib.h"
#include "validate_util.h"
#include "filesys.h"
#include "assimilator_plankton.h"

using std::string;

int process_output () {

    return 0;
}

int process_background () {

    return 0;
}

int process_log () {

    return 0;
}

int rmerge(task_t task, vector<RESULT> results) {
    char output_filename[65535];
    vector<string> result_file_names;
    char command[65535];

    // Формирование имени итогового выходного файла
    //
    sprintf(output_filename, "%s/%s_%d_%s_%d.%s", config.project_path("dir/%s", task.login), task.app_name, task.tid, task.name, task.timestamp, task.extension);
    log_messages.printf(MSG_NORMAL, "output_filename: %s\n", output_filename);
    sprintf(command, "mencoder -oac copy -ovc copy -o %s", output_filename);
    // Открывать результаты и доклеивать в итоговый файл
    //
    for (unsigned int i = 0; i < task.size; i++) {
        get_output_file_paths(results[i], result_file_names);
        sprintf(command, "%s %s", command, result_file_name.c_str());
    }
    log_messages.printf(MSG_NORMAL, "FINAL COMMAND: %s\n", command);
    system(command);
    return 0;
}
