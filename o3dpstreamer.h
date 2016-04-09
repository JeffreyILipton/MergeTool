#ifndef O3DPSTREAMER_H
#define O3DPSTREAMER_H

#include <QObject>
#include <QIODevice>
#include <QVector>
#include <QFile>

class O3DPStreamer : public QObject
{
    Q_OBJECT
public:
    O3DPStreamer(QString file, QIODevice::OpenModeFlag flag = QIODevice::ReadOnly, QObject *parent = 0);
    O3DPStreamer(QString file, QIODevice::OpenModeFlag flag, QVector<long> gridsize,QVector<double> bbox, QObject *parent =0);

    bool open(QIODevice::OpenModeFlag flag);
    void close();

    void readHeader();
    QByteArray readLayer(int layer);

    bool writeHeader();
    bool writeLayer(QByteArray Layer);

public:
    QIODevice::OpenModeFlag Flag;
    QFile* f;
    int nheaderbytes;
    QVector<QByteArray> grid;
    QVector<long> gridSize;
    QVector<double> bboxSize;
    quint32 num_mat;
    QVector<float> voxel_dimension;
};

#endif // O3DPSTREAMER_H
