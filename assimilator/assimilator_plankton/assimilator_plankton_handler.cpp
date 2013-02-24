#include "plankton.h"
// #include "assimilator_plankton.h"

using std::string;

vector< vector<string> > result_file_names;

int process_output(task_t task) {
    char output_filename[65535];
    char preview_filename[65535];
    char command[65535];
    int retval;
/*
    //decrypt
    for (unsigned int i = 0; i < task.size; i++) {
        log_messages.printf(MSG_NORMAL, "Decrypting video file: %s\n", result_file_names[i][0].c_str());
        decrypt_file(result_file_names[i][0].c_str());
    }
*/
    // Формирование имени итогового выходного файла
    //
    sprintf(output_filename, "%s/%s/result_%s.%s", result_path, task.login, task.name, task.extension);
    log_messages.printf(MSG_NORMAL, "output_filename: %s\n", output_filename);
    sprintf(command, "mencoder -msglevel all=1 -oac copy -ovc copy -o %s", output_filename);
    // Открывать результаты и доклеивать в итоговый файл
    //
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(command, "%s %s", command, result_file_names[i][0].c_str());
    }
    log_messages.printf(MSG_NORMAL, "FINAL COMMAND: %s\n", command);
    retval=system(command);
    if (!retval) {
        sprintf(preview_filename, "%s/%s/res_%s.flv", user_path, task.login, task.name);
        log_messages.printf(MSG_NORMAL, "preview_filename: %s\n", preview_filename);
        sprintf(command, "ffmpeg.exe -b 200000 -i %s %s", output_filename, preview_filename);
        log_messages.printf(MSG_NORMAL, "FINAL COMMAND: %s\n", command);
        retval=system(command);
    };
    return retval;
}

int process_background(task_t task) {
    int retval;
    char output_filename[65535];
    char output_dir[65535];
/*
    //decrypt
    for (unsigned int i = 0; i < task.size; i++) {
        log_messages.printf(MSG_NORMAL, "Decrypting background file: %s\n", result_file_names[i][1].c_str());
        decrypt_file(result_file_names[i][1].c_str());
    }
*/
    // Переложить все бекграунды в папку к пользователю
    sprintf(output_dir, "%s/%s/backgrounds", result_path, task.login);
    // boinc_mkdir(config.project_path("tmp/%s/backgrounds", task.login));
    boinc_mkdir(output_dir);
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/background_%s_%d_%d_%d_%d", output_dir, task.name, task.id, task.timestamp, i+1, task.size);
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
    char output_dir[65535];
/*
    //decrypt
    for (unsigned int i = 0; i < task.size; i++) {
        log_messages.printf(MSG_NORMAL, "Decrypting log file: %s\n", result_file_names[i][2].c_str());
        decrypt_file(result_file_names[i][2].c_str());
    }
*/
    // Переложить все логи в папку к пользователю
    sprintf(output_dir, "%s/%s/logs", result_path, task.login);
    // boinc_mkdir(config.project_path("tmp/%s/logs", task.login));
    boinc_mkdir(output_dir);
    for (unsigned int i = 0; i < task.size; i++) {
        sprintf(output_filename, "%s/log_%s_%d_%d_%d_%d", output_dir, task.name, task.id, task.timestamp, i+1, task.size);
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
