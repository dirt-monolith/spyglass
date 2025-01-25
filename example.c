#include "spyglass.h"

void process_data(__attribute__((unused)) int value)
{
    SPYGLASS_LOG_INFO("Processing data...");
    value = 0;
    SPYGLASS_LOG_WARN("Data might be incomplete: %d", value);
}

int main()
{
    const int value = 42;
    SPYGLASS_LOG_INFO("Program started with value: %d", value);
    process_data(value);
    SPYGLASS_LOG_ERROR("Failed to process item: %d: %s", value, "invalid input");
    return 0;
}