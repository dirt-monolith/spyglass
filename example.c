#include "spyglass.h"

int main()
{
    spyglass_log_print(SPYGLASS_LOG_INFO, "Starting program");
    spyglass_log_print(SPYGLASS_LOG_WARNING, "This is a warning");
    spyglass_log_print(SPYGLASS_LOG_ERROR, "Something went wrong!");
    spyglass_log_print(5, NULL);
    return 0;
}