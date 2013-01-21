#include "plankton.h"



#define SLEEP_INTERVAL 1

// #include <my_global.h>
// #include <mysql.h>
MYSQL *frontend_db;
MYSQL_RES *mysql_result;
MYSQL_ROW row;
char buff[255];
char *rez;
char frmt[10]="%H:%i:%S";

bool update_db = true;
int sleep_interval = SLEEP_INTERVAL;
int g_argc;
char** g_argv;

int update_plankton(task_t& task, APP_VERSION& app_version) {     // FIXME
    sprintf(buff, "select startDate from task where taskId=%d", task.id);
    mysql_query(frontend_db, buff);
    mysql_result = mysql_store_result(frontend_db);
    while ((row = mysql_fetch_row(mysql_result))) {
        rez = row[0];
    }
    sprintf(buff, "UPDATE task SET status=0, calcID=1, calcTime=TIMEDIFF(CURTIME(), DATE_FORMAT('%s', '%s')), ver=%d WHERE taskID=%d\n", rez, frmt, app_version.version_num, task.id);//"UPDATE tasks SET status=0 WHERE taskID=%d\n"
    mysql_query(frontend_db, buff);
    log_messages.printf(MSG_NORMAL, "Plankton updated\n");
    return 0;
}

int update_plankton_percent(vector<RESULT> result, task_t& task) {
    log_messages.printf(MSG_NORMAL, "[%d_%s] %d/%d\n", task.uid, task.name, (int)result.size(), task.size);
    sprintf(buff, "UPDATE task SET percent=%d WHERE taskID=%d\n", (int)result.size() * 100 / task.size, task.id);
    log_messages.printf(MSG_NORMAL, "Percent update command: UPDATE task SET percent=%d WHERE taskID=%d\n", (int)result.size() * 100 / task.size, task.id);
    mysql_query(frontend_db, buff);
    return 0;
}

int main_loop(APP& app) {
    DB_WORKUNIT wu;
    DB_RESULT canonical_result, result;
    DB_APP_VERSION app_version;     // http://boinc.berkeley.edu/doxygen/server/html/classDB__APP__VERSION.html
    char buf[256];
    char buf2[256];
    int retval;
    task_t task;


    while(1) {
        check_stop_daemons();

        sprintf(buf, "WHERE appid=%d AND assimilate_state=%d AND error_mask<>16", app.id, ASSIMILATE_READY);
        // Заполнение полей текущего ворк юнита
        retval = wu.enumerate(buf);
        if (retval) {
            if (retval != ERR_DB_NOT_FOUND) {
                log_messages.printf(MSG_DEBUG, "DB connection lost, exiting\n");
                exit(0);
            }
        }

        // Заполнение полей текущего задания
        sscanf(wu.name, "%[^_]_%d_%d_%[^_]_%d_%*d_%d.%[^_]", task.app_name, &task.id, &task.uid, task.name, &task.timestamp, &task.size, task.extension);
        sprintf(buf, "SELECT login FROM user WHERE id=%d", task.uid);
        mysql_query(frontend_db, buf);
        mysql_result = mysql_store_result(frontend_db);
        if ((row = mysql_fetch_row(mysql_result)) != NULL) {
            strcpy(task.login, row[0]);
        }

        // Создание списка результатов задания
        vector<RESULT> results;
        if (strlen(task.name) > 0) {
            sprintf(buf, "INNER JOIN workunit ON result.id = workunit.canonical_resultid WHERE workunit.name like \"%%_%d_%d_%s_%%\" and workunit.assimilate_state=%d and workunit.error_mask<>16", task.id, task.uid, task.name, ASSIMILATE_READY);
            while (!result.enumerate(buf)) {
                results.push_back(result);
            }
        }

        // Склеивание заданий
        if ((results.size() == task.size) && (task.size != 0)) {
            log_messages.printf(MSG_NORMAL,"[%d_%s] Assimilating task\n", task.uid, task.name);
            retval = handle_result(task, results);
            if (retval) {
                log_messages.printf(MSG_CRITICAL,"[%d_%s] Assimilation failed\n", task.uid, task.name);
            } else {
                // Обновление записей в базе
                if (update_db) {
                    sprintf(buf, "assimilate_state=%d, transition_time=%d", ASSIMILATE_DONE, (int)time(0));
                    sprintf(buf2, "appid=%d and assimilate_state=%d and name like \"%%_%d_%d_%s_%%\"", app.id, ASSIMILATE_READY, task.id, task.uid, task.name);
                    wu.update_fields_noid(buf, buf2);
                    boinc_db.commit_transaction();
                    // Обновление планктона
                    update_plankton(task, app_version);
                    update_plankton_percent(results, task);
                }
                log_messages.printf(MSG_NORMAL,"[%d_%s] Task assimilated\n", task.uid, task.name);

                //Очистка всех структур
                wu.clear();
                memset(&task, 0, sizeof(task));
                results.clear();
            }
        } else {
            if (results.size()) {
                update_plankton_percent(results, task);
            }
        }
        sleep(SLEEP_INTERVAL);
    }
}

int main(int argc, char** argv) {
    // Инициализация подключения к dims
    frontend_db = mysql_init(NULL);
    if (frontend_db == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(frontend_db), mysql_error(frontend_db));
        exit(1);
    }
    // Подключение к БД dims
    if (mysql_real_connect(frontend_db, "localhost", "boincadm", "password!stronk!", "dihm1", 0, NULL, 0) == NULL) {
        log_messages.printf(MSG_CRITICAL, "Error %u: %s\n", mysql_errno(frontend_db), mysql_error(frontend_db));
        exit(1);
    }

    int retval;
    DB_APP app;     // http://boinc.berkeley.edu/doxygen/server/html/classDB__APP.html
    int i;
    char buf[256];

    strcpy(app.name, "");
    check_stop_daemons();
    g_argc = argc;
    g_argv = argv;
    for (i=1; i<argc; i++) {
         if (is_arg(argv[i], "sleep_interval")) {
            sleep_interval = atoi(argv[++i]);
        } else if (is_arg(argv[i], "d") || is_arg(argv[i], "debug_level")) {
            int dl = atoi(argv[++i]);
            log_messages.set_debug_level(dl);
            if (dl ==4) g_print_queries = true;
        } else if (is_arg(argv[i], "app")) {
            strcpy(app.name, argv[++i]);
        } else if (is_arg(argv[i], "dont_update_db")) {
            // This option is for testing your assimilator.  When set,
            // it ensures that the assimilator does not actually modify
            // the assimilate_state of the workunits, so you can run
            // your assimilator over and over again without affecting
            // your project.
            update_db = false;
        } else if (is_arg(argv[i], "help") || is_arg(argv[i], "h")) {
//            usage(argv);
        } else {
            log_messages.printf(MSG_CRITICAL, "Unrecognized arg: %s\n", argv[i]);
//            usage(argv);
        }
    }

    if (!strlen(app.name)) {
//        usage(argv);
    }

    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't parse config.xml: %s\n", boincerror(retval));
        exit(1);
    }

    log_messages.printf(MSG_NORMAL, "Starting\n");

    retval = boinc_db.open(config.db_name, config.db_host, config.db_user, config.db_passwd);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't open DB\n");
        exit(1);
    }
    sprintf(buf, "where name='%s'", app.name);
    retval = app.lookup(buf);
    if (retval) {
        log_messages.printf(MSG_CRITICAL, "Can't find app\n");
        exit(1);
    }
    install_stop_signal_handler();

    main_loop(app);
}
