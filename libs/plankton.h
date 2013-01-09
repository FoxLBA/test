#include <tomcrypt.h>

#include "error_numbers.h"
#include "parse.h"
#include "util.h"
#include "filesys.h"

#include "sched_msgs.h"
#include "str_util.h"

const char* source_path="/mnt/boinc/source";
const char* result_path="/mnt/boinc/results";

extern int encrypt_file(const char *infile);
extern int decrypt_file(const char *infile);

#define PLANKTON_STATUS_DONE                    0
#define PLANKTON_STATUS_IN_PROGRESS             1
#define PLANKTON_STATUS_WAITING_QUEUE           2
#define PLANKTON_STATUS_PAUSED                  3
#define PLANKTON_STATUS_WAITING_RECORDING       4
#define PLANKTON_STATUS_WAITING_TRANSFER        5
#define PLANKTON_STATUS_RECORDING_ERROR         6
#define PLANKTON_STATUS_RECORDING_IN_PROGRESS   7
