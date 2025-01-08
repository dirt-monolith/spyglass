#include "spyglass.h"

void process_data(int value)
{
    SPYGLASS_LOG_INFO("Processing data...");
    value = 0;
    SPYGLASS_LOG_WARN("Data might be incomplete: %d", value);
}

int main()
{
    spyglass_log_config config = 
    {
        .active_flags = SPYGLASS_LOG_CFG_USE_COLOR | SPYGLASS_LOG_CFG_SHOW_TIME,
        .active_levels = SPYGLASS_LOG_ERROR | SPYGLASS_LOG_WARNING | SPYGLASS_LOG_INFO
    };
    spyglass_log_init(&config);

    const int value = 42;
    SPYGLASS_LOG_INFO("Program started with value: %d", value);
    process_data(value);
    SPYGLASS_LOG_ERROR("Failed to process item: %d: %s", value, "invalid input");
    return 0;
}