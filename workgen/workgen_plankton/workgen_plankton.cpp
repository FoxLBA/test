#include "boinc_db.h"
#include "error_numbers.h"
#include "backend_lib.h"
#include "parse.h"
#include "util.h"
#include "filesys.h"

#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"
#include "str_util.h"

#include <my_global.h>
#include <mysql.h>

#include "plankton.h"

#define REPLICATION_FACTOR 1
#define SLEEP_INTERVAL 10
#define INFILES_COUNT 3
#define MAX_TASKS 5

using namespace std;

// globals
//
char* wu_template;
DB_APP app;
FILE* input_file;

char full_input_filename[255];
char input_dir_string[255];

int timestamp;
int current_part = 0;
int total_parts = 0;

MYSQL *frontend_db;
MYSQL *backend_db;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;

char* db_taskID;
char* db_uid;
char* db_login;
char* db_filename;
char* db_background;
char* db_par1;
char* db_par2;
char* db_localID;

char infiles[INFILES_COUNT][255];
DB_WORKUNIT wu;


int split_input(const char* db_login, const char* db_filename) {
    char split[255];

    log_messages.printf(MSG_NORMAL, "Found new file \"%s/%s\", processing...\n", db_login, db_filename);
    // Путь до ожидающего обработки файла
    sprintf(full_input_filename, "%s/%s/holograms/%s", source_path, db_login, db_filename);
    log_messages.printf(MSG_NORMAL, "Full path: %s\n", full_input_filename);

    // Формирование имени папки для нарезок
    //
    // sprintf(buff, "%s/%s/holograms/%s", source_path, db_login, db_filename);
    strncpy(input_dir_string, full_input_filename, strlen(full_input_filename)-4);  //FIXME

    log_messages.printf(MSG_NORMAL, "full_input_filename: %s\n", full_input_filename);
    log_messages.printf(MSG_NORMAL, "input_dir_string: %s\n", input_dir_string);

    // Запуск скрипта на разделение видеофайла (файл full_input_filename, складывать в input_dir_string)
    //
    sprintf(split, "%s/ffsplit.sh %s", config.project_path("bin"), full_input_filename);
    log_messages.printf(MSG_NORMAL, "==SCRIPT STARTING==: %s\n", split);
    // вызов скрипта
    return system(split)>>8;
}

int process_input(const char* input_filename, const char* db_filename) {
    log_messages.printf(MSG_NORMAL, "Processing input: %s\n", input_filename);
    char output_filename[255];
    char full_output_filename[255];
    char basename[255];
    char extension[255];
    int retval;

    // Чтение строки, формирование путей и имён
    //
    // Имя воркюнита
    sscanf(db_filename, "%[^.].%[^.]", basename, extension);
    sprintf(output_filename, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, db_taskID, db_uid, db_localID, timestamp, current_part, total_parts, extension);
    config.download_path(output_filename, full_output_filename);

    sprintf(full_input_filename, "%s/%s", input_dir_string, input_filename);
    log_messages.printf(MSG_NORMAL, "From full filepath: %s\n", full_input_filename);
    log_messages.printf(MSG_NORMAL, "To full filepath: %s\n", full_output_filename);

    retval = boinc_copy(full_input_filename, full_output_filename);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Error [%d] renaming file %s to %s\n", retval, full_input_filename, full_output_filename);
    } else {
        log_messages.printf(MSG_NORMAL, "File successfully renamed\n");
        retval = boinc_delete_file(full_input_filename);
        if (retval)
            log_messages.printf(MSG_CRITICAL, "Error [%d] removing file %s\n", retval, full_input_filename);
    }
/*
    // encrypt file
    if (encrypt_file(full_output_filename) == 0) {
        log_messages.printf(MSG_NORMAL, "File succesfully encrypted\n");
    }
*/
    strcpy(wu.name, output_filename);
    strcpy(infiles[0], output_filename);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");

    return 0;
}

int process_background(const char* filename) {
    log_messages.printf(MSG_NORMAL, "Processing background: %s\n", filename);
    char full_output_filename[255];
    char outname[255];
    char basename[255];
    char extension[255];
    int retval;

    sscanf(filename, "%[^.].%[^.]", basename, extension);

    // Путь до бекграунда
    sprintf(full_input_filename, "%s/%s/backgrounds/%s.%s", source_path, db_login, basename, extension);
    log_messages.printf(MSG_NORMAL, "From full background path: %s\n", full_input_filename);
    sprintf(outname, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, db_taskID, db_uid, db_localID, timestamp, current_part, total_parts, extension);

    config.download_path(outname, full_output_filename);
    log_messages.printf(MSG_NORMAL, "To full background path: %s\n", full_output_filename);

    retval = boinc_copy(full_input_filename, full_output_filename);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Error [%d] renaming file %s to %s\n", retval, full_input_filename, full_output_filename);
    } else {
        log_messages.printf(MSG_NORMAL, "File successfully renamed\n");
    }
/*
    // encrypt background
    if (encrypt_file(full_output_filename) == 0) {
        log_messages.printf(MSG_NORMAL, "Background succesfully encrypted\n");
    }
*/
    strcpy(infiles[1], outname);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");
    return 0;
}

int process_config(const char* par1, const char* par2) {
    char in_par1[255];
    char in_par2[255];
    strcpy(in_par1, par1);
    strcpy(in_par2, par2);
    log_messages.printf(MSG_NORMAL, "Processing config: %s %s\n", in_par1, in_par2);
    FILE* configfile;
    int i=0;
    char path[255];
    char filename[255];
    // Имя конфига
    sprintf(filename, "%s_%s_%s_config_%d_%d_%d.cfg", app.name, db_taskID, db_uid, timestamp, current_part, total_parts);

    // Путь до конфига
    config.download_path(filename, path);
    log_messages.printf(MSG_NORMAL, "To full config path: %s\n", path);

    // Здесь парсятся поля, открывается файл path и в него пишется текст конфига
    configfile = fopen(path, "w");
    if (configfile==NULL) {
        printf("config open error\n");
    }

    // Парсинг полей
    while (in_par1[i]!='\0' || in_par2[i]!='\0') {
        if (in_par1[i]=='&') {
            in_par1[i]='\n';
        }
        if (in_par2[i]=='&') {
            in_par2[i]='\n';
        }
        i++;
    }

    // Запись конфига. Возможно потребуется замена path на filename
    fprintf(configfile, "[Main parameters]\n%s\nmaxFr=100000\n\n[Search parameters]\n%s", in_par1, in_par2); //FIXME maxFr=100000
    fclose(configfile);
/*
    // encrypt config
    if (encrypt_file(path) == 0) {
        log_messages.printf(MSG_NORMAL, "Configfile succesfully encrypted\n");
    }
*/
    strcpy(infiles[2], filename);
    for (int i=0; i<3; i++) {
        log_messages.printf(MSG_NORMAL, "infiles[%d]=%s\n", i, infiles[i]);
    }
    log_messages.printf(MSG_NORMAL, "============\n");

    return 0;
}

// create one new job
//
int make_job(char* db_taskID) {
    int tid;
    tid = atoi(db_taskID);
    // Fill in the job parameters
    //
    wu.appid = app.id;
    wu.batch = tid;
    wu.rsc_fpops_est = 1e12;
    wu.rsc_fpops_bound = 1e14;
    wu.rsc_memory_bound = 1e8;
    wu.rsc_disk_bound = 2*1e10;
    wu.delay_bound = 86400;
    wu.min_quorum = REPLICATION_FACTOR;
    wu.target_nresults = REPLICATION_FACTOR;
    wu.max_error_results = REPLICATION_FACTOR*4;
    wu.max_total_results = REPLICATION_FACTOR*8;
    wu.max_success_results = REPLICATION_FACTOR*4;

    const char* in[] = {infiles[0], infiles[1], infiles[2]};
    log_messages.printf(MSG_NORMAL, "Creating work\n\n");
    return create_work(
        wu,
        wu_template,
        "templates/result.xml",
        config.project_path("templates/result.xml"),
        in,
        INFILES_COUNT,
        config
    );
}

int get_running_count() {

    char buff[255];
    sprintf(buff, "select count(taskID) from task where status=%d AND del<>1", PLANKTON_STATUS_IN_PROGRESS);
    mysql_query(frontend_db, buff);
    result = mysql_store_result(frontend_db);
    row = mysql_fetch_row(result);
    log_messages.printf(MSG_NORMAL, "Tasks currently running: %s\n", row[0]);
    return atoi(row[0]);
}

int task_is_empty(const char* par1) {

    char* str;
    char* token;
    char* saveptr;
    double value;

    str = (char*)malloc(strlen(par1) + 1);
    if (str) {
        strcpy(str, par1);
    } else {
        log_messages.printf(MSG_CRITICAL, "%s\n", "Unable to copy parameter string");
    }

    token = strtok_r(str, "=", &saveptr);
    while (token) {
        token = strtok_r(NULL, "&", &saveptr);
        value = atof(token);
        if (value != 0) {
            free(str);
            return 0;
        }
        token = strtok_r(NULL, "=", &saveptr);
    }

    // printf("skipping task\n");
    free(str);
    return 1;
}

int cancel_wu() {
    // char* c_task_id;
    // char buff[255];
    // log_messages.printf(MSG_NORMAL, "Trying to find task where STATUS=3 or DEL=1...\n");
    // sprintf(buff, "select taskID from task where status=%d or del=1", PLANKTON_STATUS_PAUSED);
    // mysql_query(frontend_db, buff);
    // result = mysql_store_result(frontend_db);
    // while ((row = mysql_fetch_row(result))) {
    //     c_task_id = row[0];
    //     // Ставится соответствие taskID и filename планктона воркюнитам из таблицы workunit
    //     // в результате имеется начальный и конечный id
    //     // ВАЖНО: id ву и результатов могут не совпадать!
    //     // sprintf(buff, "update result set server_state=5, outcome=5 where server_state=2 and batch=%s", c_task_id); // SERVER STATE FIXME
    //     sprintf(buff, "update result set server_state=5, outcome=5 where batch=%s", c_task_id); // SERVER STATE FIXME
    //     mysql_query(backend_db, buff);
    //     sprintf(buff, "update workunit set error_mask=error_mask|16 where batch=%s", c_task_id);
    //     mysql_query(backend_db, buff);
    // }

    log_messages.printf(MSG_NORMAL, "Cleaning up paused/deleted tasks\n");
    char query[65535];
    sprintf(query, "CREATE TEMPORARY TABLE plankton.batches AS (SELECT DISTINCT batch FROM plankton.workunit WHERE batch NOT IN (SELECT taskID FROM dihm1.task) OR batch IN (SELECT taskID FROM dihm1.task WHERE status=3 OR del=1));");
    mysql_query(backend_db, query);
    sprintf(query, "UPDATE plankton.result SET server_state=5, outcome=5  WHERE batch IN (SELECT batch FROM plankton.batches);");
    mysql_query(backend_db, query);
    sprintf(query, "UPDATE plankton.workunit SET error_mask=error_mask|16 WHERE batch IN (SELECT batch FROM plankton.batches);");
    mysql_query(backend_db, query);
    sprintf(query, "DROP TABLE plankton.batches;");
    mysql_query(backend_db, query);

    return 0;
}

void main_loop() {
    char buff[255];
    char input_filename[255];
    int retval;
    int running_tasks;

    check_stop_daemons();
    // Сканируем базу каждые SLEEP_INTERVAL секунд
    //
    while (1) {
        cancel_wu();
        running_tasks = get_running_count();
        if (running_tasks < MAX_TASKS) {  //FIXME
            log_messages.printf(MSG_NORMAL, "Scanning database for pending files...\n");
            //запрос на все ожидающие файлы
            sprintf(buff, "SELECT taskID, uid, login, hol_source, bg_source, par1, par2, localID FROM task inner join user ON uid=id WHERE status=%d AND del <> '1' AND hol_source <> '' ORDER BY taskID LIMIT %d", PLANKTON_STATUS_WAITING_QUEUE, MAX_TASKS-running_tasks);
            mysql_query(frontend_db, buff);
            result = mysql_store_result(frontend_db);
            while ((row = mysql_fetch_row(result))) {
                db_taskID       = row[0];
                db_uid          = row[1];
                db_login        = row[2];
                db_filename     = row[3];
                db_background   = row[4];
                db_par1         = row[5];
                db_par2         = row[6];
                db_localID      = row[7];

                if (task_is_empty(db_par1)) {
                    log_messages.printf(MSG_NORMAL, "Skipping empty taks %s\n", db_taskID);
                } else {
                    total_parts = split_input(db_login, db_filename);

                    if (total_parts < 1) {
                        log_messages.printf(MSG_CRITICAL, "Total parts less than 1, pausing task\n");
                        sprintf(buff,"update status=%d where taskID=%i\n", PLANKTON_STATUS_PAUSED, atoi(db_taskID));
                        mysql_query(frontend_db, buff);
                    } else {
                        log_messages.printf(MSG_NORMAL, "Total parts=%i\n", total_parts);
                        // Открыть папку для сканирования нарезок
                        DIRREF input_dir = dir_open(input_dir_string);
                        vector<string> input_vector;
                        log_messages.printf(MSG_NORMAL, "Scanning directory %s for files\n", input_dir_string);
                        while (!(dir_scan(input_filename, input_dir, sizeof(input_filename)))) {
                            input_vector.push_back(input_filename);
                        }
                        sort(input_vector.begin(), input_vector.end());

                        timestamp = time(0);
                        current_part = 0;
                        for (unsigned int i = 0; i < input_vector.size(); i++) {
                            current_part++;
                            wu.clear();
                            retval = process_input(input_vector[i].c_str(), db_filename);
                            if (retval) { log_messages.printf(MSG_CRITICAL, "Can't process input: %d\n", retval); }
                            retval = process_background(db_background);
                            if (retval) {log_messages.printf(MSG_CRITICAL, "Can't process background: %d\n", retval); }
                            retval = process_config(db_par1, db_par2);
                            if (retval) { log_messages.printf(MSG_CRITICAL, "Can't process config: %d\n", retval); }
                            retval = make_job(db_taskID);
                            if (retval) {
                                log_messages.printf(MSG_CRITICAL, "Can't create job: %d\n", retval);
                                exit(1); // need new "invalid" status
                            }
                            memset(infiles, 0, sizeof(infiles));
                        }

                        retval = boinc_rmdir(input_dir_string);
                        if (retval) {
                            log_messages.printf(MSG_CRITICAL, "Can't remove directory %s: %d\n", input_dir_string, retval);
                            exit(1); // need new "invalid" status
                        }
                        memset(input_dir_string, 0 , sizeof(input_dir_string));

                        // Изменение статуса файла в БД планктон
                        //
                        log_messages.printf(MSG_NORMAL, "row[0] (taskID): %s\n", db_taskID);
                        sprintf(buff, "UPDATE task SET status=%d, startDate=NOW() WHERE taskID=%s\n", PLANKTON_STATUS_IN_PROGRESS, db_taskID);
                        log_messages.printf(MSG_NORMAL, "buff for query (status update): %s", buff);
                        mysql_query(frontend_db, buff);
                    }
                }
            }
        }
        sleep(SLEEP_INTERVAL);
    }
}

void usage(char* name) {
    fprintf(stderr,
        "Usage: %s [OPTION]...\n\n"
        "Options:\n"
        "  [ -d X ]                    Sets debug level to X.\n"
        "  [ -h | --help ]             Shows this help text.\n",
        name
    );
}

int main(int argc, char** argv) {
    int i, retval;

    for (i=1; i<argc; i++) {
        if (is_arg(argv[i], "d")) {
            if (!argv[++i]) {
                log_messages.printf(MSG_CRITICAL, "%s requires an argument\n\n", argv[--i]);
                usage(argv[0]);
                exit(1);
            }
            int dl = atoi(argv[i]);
            log_messages.set_debug_level(dl);
            if (dl == 4) g_print_queries = true;
        } else if (is_arg(argv[i], "h") || is_arg(argv[i], "help")) {
            usage(argv[0]);
            exit(0);
        } else {
            log_messages.printf(MSG_CRITICAL, "unknown command line argument: %s\n\n", argv[i]);
            usage(argv[0]);
            exit(1);
        }
    }

    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL,
            "Can't parse config.xml: %s\n", boincerror(retval)
        );
        exit(1);
    }

    retval = boinc_db.open(config.db_name, config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db\n");
        exit(1);
    }
    if (app.lookup("where name='test-plankton'")) {
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc(config.project_path("templates/wu.xml"), wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    // инициализация подключения к БД dims
    frontend_db = mysql_init(NULL);
    if (frontend_db == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(frontend_db), mysql_error(frontend_db));
        exit(1);
    }
    // подключение к БД dims
    if (mysql_real_connect(frontend_db, "127.0.0.1", "boincadm", "password!stronk!", "dihm1", 0, NULL, 0) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error dihm1 %u: %s\n", mysql_errno(frontend_db), mysql_error(frontend_db));
        exit(1);
    }
    // инициализация подключения к БД нижнего уровня
    backend_db = mysql_init(NULL);
    if (backend_db == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(backend_db), mysql_error(backend_db));
        exit(1);
    }
    // подключение к БД нижнего уровня
    if (mysql_real_connect(backend_db, "127.0.0.1", "boincadm", "password!stronk!", "plankton", 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error plankton %u: %s\n", mysql_errno(backend_db), mysql_error(backend_db));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");
    main_loop();
}
