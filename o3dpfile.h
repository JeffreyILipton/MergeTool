#ifndef O3DPFILE_H
#define O3DPFILE_H

#include <QObject>
#include <QVector>

class O3DPFile : public QObject
{
    Q_OBJECT
public:
    explicit O3DPFile(QObject *parent = 0);


public slots:
    void read(QString file);
    void write(QString file);

public:
    QVector<QVector<quint8>> grid;
    QVector<long> gridSize;
    QVector<double> bboxSize;
    quint32 num_mat;
    QVector<float> voxel_dimension;
};


#endif // O3DPFILE_H
