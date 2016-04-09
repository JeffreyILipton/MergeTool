#ifndef MERGEFILES_H
#define MERGEFILES_H

#include "o3dpfile.h"
#include "o3dpstreamer.h"

void mergeFiles(QString XMLFile);


struct fileWithLocation{
    O3DPStreamer* o3dp;
    QVector<int> indices;
};

#endif // MERGEFILES_H
