#include "backend_lib.h"
#include "validate_util.h"
#include "filesys.h"
#include "assimilator_plankton.h"

using std::string;

vector< vector<string> > result_file_names;

int process_output(task_t task) {
    char output_filename[65535];
    char command[65535];

    // Формирование имени итогового выходного файла
    //
    sprintf(output_filename, "%s/%s_%d_%s_%d.%s", config.project_path("dir/%s", task.login), task.app_name, task.id, task.name, task.timestamp, task.extension);
    log_messages.printf(MSG_NORMAL, "output_filename: %s\n", output_filename);
    sprintf(command, "mencoder -oac copy -ovc copy -o %s", output_filename);
    // Открывать результаты и доклеивать в итоговый файл
    //
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(command, "%s %s", command, result_file_names[i][0].c_str());
    }
    log_messages.printf(MSG_NORMAL, "FINAL COMMAND: %s\n", command);
    return system(command);
}

int process_background(task_t task) {
    char output_filename[65535];
    // Переложить все бекграунды в папку к пользователю
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/%s_%d_%s_%d_%d_%d", config.project_path("dir/%s", task.login), task.app_name, task.id, task.name, task.timestamp, i, task.size);
        boinc_rename(result_file_names[i][1].c_str(), output_filename);
    }
    return 0;
}

int process_log(task_t task) {
    char output_filename[65535];
    // Переложить все логи в папку к пользователю
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/%s_%d_%s_%d_%d_%d", config.project_path("dir/%s", task.login), task.app_name, task.id, task.name, task.timestamp, i, task.size);
        boinc_rename(result_file_names[i][2].c_str(), output_filename);
    }
    return 0;
}

int rmerge(task_t task, vector<RESULT> results) {

    // Собрать имена всех файлов для всех результатов
    for (unsigned int i = 0; i < task.size; i++) {
        get_output_file_paths(results[i], result_file_names[i]);
    }

    // Обработать выходные файлы
    process_output(task);
    process_background(task);
    process_log(task);
    return 0;
}
