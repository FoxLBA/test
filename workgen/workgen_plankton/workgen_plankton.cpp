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


#define REPLICATION_FACTOR 1
#define SLEEP_INTERVAL 10
#define INFILES_COUNT 3

// globals
//
char* wu_template;
DB_APP app;
FILE *input_file;
DIRREF input_dir;
char input_filename[255];
char full_input_filename[255];

int timestamp;
int current_part = 0;
int total_parts = 0;

MYSQL *conn;
MYSQL_RES *result;
MYSQL_ROW row;
MYSQL_FIELD *field;

char *db_taskID;
char *db_login;
char *db_filename;
char *db_background;
char *db_par1;
char *db_par2;

const char* infiles[INFILES_COUNT];
DB_WORKUNIT wu;

char input_dir_string[255];

int split_input() {
    char split[255];

    log_messages.printf(MSG_NORMAL, "Found new file \"%s/%s\", processing...\n", db_login, db_filename);
    // Путь до ожидающего обработки файла
    sprintf(full_input_filename, "%s/%s/%s", config.project_path("dir"), db_login, db_filename);
    log_messages.printf(MSG_NORMAL, "Full path: %s\n", full_input_filename);

    // Формирование имени папки для нарезок
    //
    strncpy(input_dir_string, full_input_filename, strlen(full_input_filename)-4);

    log_messages.printf(MSG_NORMAL, "full_input_filename 13: %s\n", full_input_filename);
    log_messages.printf(MSG_NORMAL, "input_dir_string 13: %s\n", input_dir_string);

    // Запуск скрипта на разделение видеофайла (файл full_input_filename, складывать в input_dir_string)
    //
    sprintf(split, "%s/ffsplit.sh %s", config.project_path("bin"), full_input_filename);
    log_messages.printf(MSG_NORMAL, "==SCRIPT STARTING==: %s\n", split);
    // вызов скрипта
    return system(split)>>8;
}

int process_input(char *filename) {
    char name[255], path[255];
    char newname[255];
    char oldname[255];
    char basename[255];
    char extension[255];

    // Чтение строки, формирование путей и имён
    //
    // Имя воркюнита
    sscanf(db_filename, "%[^.].%[^.]", basename, extension);
    sprintf(name, "%s_%s_%s_%s_%d_%d_%d.%s", app.name, db_taskID, db_login, basename, timestamp, current_part, total_parts, extension);
    config.download_path(name, path);
    sprintf(newname, "%s", path);

    log_messages.printf(MSG_NORMAL, "newname: %s\n", newname);
    log_messages.printf(MSG_NORMAL, "oldname input_dir_string: %s\n", input_dir_string);
    log_messages.printf(MSG_NORMAL, "oldname input_filename: %s\n", input_filename);

    sprintf(oldname, "%s/%s", input_dir_string, input_filename);
    log_messages.printf(MSG_NORMAL, "oldname: %s\n", oldname);

    if (boinc_rename(oldname, newname))
        log_messages.printf(MSG_CRITICAL, "Error renaming file %s\n", oldname);
    else
        log_messages.printf(MSG_NORMAL, "File successfully renamed\n");

    strcpy(wu.name, name);
    infiles[0] = name;

    return 0;
}

int process_background(char *filename) {
    char path[255];
    // Путь до бекграунда
    sprintf(full_input_filename, "%s/%s/%s", config.project_path("dir"), db_login, filename);
    sprintf(full_input_filename, "%s_%d", full_input_filename, current_part);
    log_messages.printf(MSG_NORMAL, "Full background path: %s\n", full_input_filename);

    config.download_path(filename, path);
    log_messages.printf(MSG_NORMAL, "Moving background to: %s\n", path);

    infiles[1] = filename;
    return boinc_copy(full_input_filename, path);
}

int process_config(char *par1, char *par2) {
	FILE *config;
	int i=0;
    char path[255];
    char filename[255];
    // Имя конфига
    sprintf(filename, "%s_%s_%s_%d.cfg", app.name, db_taskID, db_filename, current_part);

    // Путь до конфига
    config.download_path(filename, path);
    log_messages.printf(MSG_NORMAL, "Writing config to: %s\n", path);

    // Здесь парсятся поля, открывается файл path и в него пишется текст конфига
	config = fopen(path, "w");
	{
		printf("config open error\n");
	}
	
	// Парсинг полей
	while (par1[i]!='\0' || par2[i]!='\0') {
		if (par1[i]=='&') { 
			par1[i]='\n';
		}
		if (par2[i]=='&') { 
			par2[i]='\n';
		}
		i++;
	}
	
	// Запись конфига. Возможно потребуется замена path на filename
	fprintf(config, "Filename=%s\n[Main parameters]\n%s\n\n[Search parameters]\n%s", path, par1, par2);

    infiles[2] = filename;
    return 0;
}

// create one new job
//
int make_job() {

    // Fill in the job parameters
    //
    wu.clear();
    wu.appid = app.id;
    wu.rsc_fpops_est = 1e12;
    wu.rsc_fpops_bound = 1e14;
    wu.rsc_memory_bound = 1e8;
    wu.rsc_disk_bound = 1e8;
    wu.delay_bound = 86400;
    wu.min_quorum = REPLICATION_FACTOR;
    wu.target_nresults = REPLICATION_FACTOR;
    wu.max_error_results = REPLICATION_FACTOR*4;
    wu.max_total_results = REPLICATION_FACTOR*8;
    wu.max_success_results = REPLICATION_FACTOR*4;

    process_input(db_filename);
    process_background(db_background);
    process_config(db_par1, db_par2);

    // Register the job with BOINC
    //
    return create_work(
        wu,
        wu_template,
        "templates/result",
        config.project_path("templates/result"),
        infiles,
        INFILES_COUNT,
        config
    );
}

void main_loop() {
    char buff[255];
    int retval;
    check_stop_daemons();
    // Сканируем базу каждые SLEEP_INTERVAL секунд
    //
    while (1) {
        log_messages.printf(MSG_NORMAL, "Scanning database for pending files...\n");
        //запрос на все ожидающие файлы
        mysql_query(conn, "select taskID, login, filename, background, par1, par2 from tasks inner join users on uid=id where status = '2'");
        result = mysql_store_result(conn);
        // Подсчёт количества столбцов. Пока не используется
        //
        //num_fields = mysql_num_fields(result);
        while ((row = mysql_fetch_row(result))) {
            db_taskID =     row[0];
            db_login =      row[1];
            db_filename =   row[2];
            db_background = row[3];
            db_par1 =       row[4];
            db_par2 =       row[5];

            total_parts = split_input();

            // Открыть папку для сканирования нарезок
            input_dir = dir_open(input_dir_string);
            timestamp = time(0);
            current_part = 0;
            while (!(dir_scan(input_filename, input_dir, sizeof(input_filename)))) {
                current_part++;
                retval = make_job();
            }

            // Изменение статуса файла в БД планктон
            //
            log_messages.printf(MSG_NORMAL, "row[0] (taskID): %s\n", db_taskID);
            sprintf(buff, "UPDATE tasks SET status=1 WHERE taskID=%s\n", db_taskID);
            log_messages.printf(MSG_NORMAL, "buff for query (status update): %s", buff);
            mysql_query(conn, buff);

            // Удаление исходного файла будет происходить только после изменения в БД
            //
            //log_messages.printf(MSG_NORMAL, "Processing complete, deleting file \"%s\"\n", input_filename);
            //boinc_delete_file(full_input_filename);
        }
        sleep(SLEEP_INTERVAL);
    }
}

void usage(char *name) {
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

    retval = boinc_db.open(
        config.db_name, config.db_host, config.db_user, config.db_passwd
    );
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "can't open db\n");
        exit(1);
    }
    if (app.lookup("where name='test-ffmpeg'")) { //FIXME
        log_messages.printf(MSG_CRITICAL, "can't find app\n");
        exit(1);
    }
    if (read_file_malloc(config.project_path("templates/wu"), wu_template)) {
        log_messages.printf(MSG_CRITICAL, "can't read WU template\n");
        exit(1);
    }

    //инициализация подключения к базе данных
    conn = mysql_init(NULL);
    if (conn == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }
    //подключение к БД
    if (mysql_real_connect(conn, "localhost", "root", "password!stronk!", "plankton", 0, NULL, 0) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");
    main_loop();
}
