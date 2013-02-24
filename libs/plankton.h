#include <tomcrypt.h>
#include <ctime>

#include <mysql.h>

#include "util.h"
#include "filesys.h"
#include "boinc_db.h"
#include "boinc_api.h"
#include "parse.h"
#include "error_numbers.h"
#include "str_util.h"
#include "validate_util.h"
#include "sched_config.h"
#include "sched_util.h"
#include "sched_msgs.h"

using std::vector;

const char* source_path="/mnt/boinc/source";
const char* result_path="/mnt/boinc/results";
const char* user_path="/mnt/boinc/user";

struct task_t {
    char app_name[255];
    unsigned int id;
    unsigned int uid;
    char login[255];
    char name[255];
    char extension[255];
    unsigned int timestamp;
    unsigned int size;
};

extern int encrypt_file(const char *infile);
extern int decrypt_file(const char *infile);
extern int handle_result(task_t task, vector<RESULT> results);


#define PLANKTON_STATUS_DONE                    0
#define PLANKTON_STATUS_IN_PROGRESS             1
#define PLANKTON_STATUS_WAITING_QUEUE           2
#define PLANKTON_STATUS_PAUSED                  3
#define PLANKTON_STATUS_WAITING_RECORDING       4
#define PLANKTON_STATUS_WAITING_TRANSFER        5
#define PLANKTON_STATUS_RECORDING_ERROR         6
#define PLANKTON_STATUS_RECORDING_IN_PROGRESS   7

#define PLANKTON_PROCESSING_MODE_RECORD         0x00000001
#define PLANKTON_PROCESSING_MODE_COMPUTE        0x00000002
