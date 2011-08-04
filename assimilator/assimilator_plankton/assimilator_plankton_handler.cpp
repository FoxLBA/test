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
    sprintf(output_filename, "%s/%s_%d_%d.%s", config.project_path("tmp/%s/holograms", task.login), task.name, task.id, task.timestamp, task.extension);
    log_messages.printf(MSG_NORMAL, "output_filename: %s\n", output_filename);
    sprintf(command, "mencoder -msglevel all=1 -oac copy -ovc copy -o %s", output_filename);
    // Открывать результаты и доклеивать в итоговый файл
    //
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(command, "%s %s", command, result_file_names[i][0].c_str());
    }
    log_messages.printf(MSG_NORMAL, "FINAL COMMAND: %s\n", command);
    return system(command);
}

int process_background(task_t task) {
    int retval;
    char output_filename[65535];

    // Переложить все бекграунды в папку к пользователю
    boinc_mkdir(config.project_path("tmp/%s/backgrounds", task.login));
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/background_%s_%d_%d_%d_%d", config.project_path("tmp/%s/backgrounds", task.login), task.name, task.id, task.timestamp, i+1, task.size);
//        boinc_make_dirs("", output_filename);
        log_messages.printf(MSG_NORMAL, "Saving background [%d] to: %s\n", i+1, output_filename);
        retval = boinc_copy(result_file_names[i][1].c_str(), output_filename);
        if (retval) {
            log_messages.printf(MSG_CRITICAL, "Error [%d] copying background file %s to %s\n", retval, result_file_names[i][1].c_str(), output_filename);
        }
    }
    return retval;
}

int process_log(task_t task) {
    int retval;
    char output_filename[65535];

    // Переложить все логи в папку к пользователю
    boinc_mkdir(config.project_path("tmp/%s/logs", task.login));
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/log_%s_%d_%d_%d_%d", config.project_path("tmp/%s/logs", task.login), task.name, task.id, task.timestamp, i+1, task.size);
        log_messages.printf(MSG_NORMAL, "Saving log [%d] to: %s\n", i+1, output_filename);
        retval = boinc_copy(result_file_names[i][2].c_str(), output_filename);
        if (retval) {
            log_messages.printf(MSG_CRITICAL, "Error [%d] copying log file %s to %s\n", retval, result_file_names[i][2].c_str(), output_filename);
        }
    }
    return retval;
}

int handle_result(task_t task, vector<RESULT> results) {
    int retval;
    vector<string> single;

    // Собрать имена всех файлов для всех результатов
    for (unsigned int i = 0; i < task.size; i++) {
        get_output_file_paths(results[i], single);
        for (unsigned int i = 0; i < 3; i++) {
            log_messages.printf(MSG_NORMAL, "Path [%d]: %s\n", i, single[i].c_str());
        }
        result_file_names.push_back(single);
    }

    // Обработать выходные файлы
    log_messages.printf(MSG_NORMAL, "Processing output\n");
    retval = process_output(task);
    log_messages.printf(MSG_NORMAL, "Processing background\n");
    retval = process_background(task);
    log_messages.printf(MSG_NORMAL, "Processing log\n");
    retval = process_log(task);

    result_file_names.clear();
    return retval;
}
