#include <cstdio>
#include "OptionParserMain.h"

int& g_numOfData = OptionParser::createOption(123, "num-of-data", "Number of data");
double& g_ratioOfData = OptionParser::createOption(0.123, "ratio-of-data", "Ratio of data");

int main(int argc, char** argv) {
    OptionParser::parse(argc, argv);

    printf("g_numOfData = %d\n", g_numOfData);
    printf("g_ratioOfData = %lf\n", g_ratioOfData);

    return 0;
}
