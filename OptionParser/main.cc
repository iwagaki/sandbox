#include <cstdio>
#include "OptionParserMain.h"

OPTION_INT    g_numOfData("num-of-data", "Number of data", 123);
OPTION_DOUBLE g_ratioOfData("ratio-of-data", "Ratio of data", 0.123);

int main(int argc, char** argv) {
    OptionParser::parse(argc, argv);

    printf("g_numOfData = %d\n", (int)g_numOfData);
    printf("g_ratioOfData = %lf\n", (double)g_ratioOfData);

    return 0;
}
