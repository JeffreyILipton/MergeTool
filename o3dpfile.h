#ifndef O3DPFILE_H
#define O3DPFILE_H

#include <QObject>
#include <QVector>
#include <QFile>

class O3DPFile : public QObject
{
    Q_OBJECT
public:
    O3DPFile(QString file, QObject *parent = 0);
    O3DPFile(QString file, QVector<long> gridsize,QVector<double> bbox, QObject *parent =0);

    void readHeader();
    bool setLayer(int layer_index,QByteArray layer);
    QByteArray layer(int layer);

public slots:
    void readAll();
    void writeAll(QString file);
    void writeHeader();
    bool writeLayer(QByteArray Layer);

public:
    QFile* f;
    int nheaderbytes;
    QVector<QByteArray> grid;
    QVector<long> gridSize;
    QVector<double> bboxSize;
    quint32 num_mat;
    QVector<float> voxel_dimension;
};


#endif // O3DPFILE_H
