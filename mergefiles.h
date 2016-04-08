#ifndef MERGEFILES_H
#define MERGEFILES_H

#include "o3dpfile.h"

void mergeFiles(QString XMLFile);


struct fileWithLocation{
    O3DPFile o3dp;
    QVector<int> indices;
}

#endif // MERGEFILES_H
