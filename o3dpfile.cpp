#include "o3dpfile.h"
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QDebug>
#include <qendian.h>

#include <iostream>
#include <fstream>

O3DPFile::O3DPFile(QObject *parent) : QObject(parent)
{
    voxel_dimension = QVector<float>(3,0.0);
    gridSize = QVector<long>(3,0.0);
    bboxSize = QVector<double>(6,0.0);
    num_mat = 0;
}


void O3DPFile::read(QString file){
    QString cookie = "#OpenFab3DP V1.0 Binary";
    int headercount = cookie.length();
    int n_grid_size_units = 3;
    int grid_size_unit = 4;
    int n_bbox_units = 6;
    int bbox_unit = 8;
    int n_mat_len = 4;

    headercount += n_grid_size_units*grid_size_unit;
    headercount += n_bbox_units*bbox_unit;
    headercount += n_mat_len;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QByteArray cookarray = f.read(cookie.length());
    QString readcookie = QString::fromStdString(cookarray.toStdString());
    qDebug()<<"read cookie: "<<readcookie;

    QByteArray temp;
    // load int32 grid_val
    for(int i=0;i<n_grid_size_units;i++){
        temp = f.read(grid_size_unit);
        long grid_val = qbytearrayToLong(temp);
        this->gridSize[i]=grid_val;
    }
    // load float64
    for(int i=0;i<n_bbox_units;i++){
        temp = f.read(bbox_unit);
        double bbox_val;
        bbox_val = temp.toDouble();
        this->bboxSize[i]=bbox_val;
    }
    // load num mat
    temp = f.read(n_mat_len);
    int tempint= temp.toInt();
    num_mat = tempint;

    qDebug()<<"Gridsize: "<<this->gridSize;
    qDebug()<<"bbox:" <<this->bboxSize;


    for(int i=0;i<3;i++){
        voxel_dimension[0+i] = (bboxSize[3+i]-bboxSize[0+i])/gridSize[i];
    }
    for(int z=0; z<this->gridSize[2]; z++){

        QVector< QVector<quint8>>cols(gridSize[1]);
        for(int y=0; y<this->gridSize[1]; y++){

            QVector<quint8>row(gridSize[0],1);
            cols[y] = row;
        }
        grid.append(cols);
    }

    for(int z=0; z<this->gridSize[2]; z++){
        qDebug()<<"z:"<<z;
        for(int y=0; y<this->gridSize[1]; y++){
            for(int x=0; x<this->gridSize[0]; x++){
                temp = f.read(1);
                grid[z][y][x]=temp.at(0);
            }
        }
    }


/*
 *  QVector< QVector< QVector<quint8> > > grid;
    QVector<qint32> gridSize;
    QVector<double> bboxSize;
    quint32 num_mat;
    QVector<float> voxel_dimension;
*/


}
void O3DPFile::write(QString file){
/*%O3DP file format header spec
file = fopen(filename, 'w');
fwrite(file, cookie, 'char');   %used as ID by DLL
fwrite(file, gridSize, 'int32'); %3 element array (voxel counts) [XvoxelDim, YvoxelDim, ZvoxelDim] MUST match the sizes of the matrices written for each slice.
fwrite(file, bboxSize, 'float64');  %6 element array (mm) [bboxXfirst,bboxYfirst,bboxZfirst,bboxXend,bboxYend,bboxZend] Probably not used by the Objet, but space in the file is expected by Twister DLL
fwrite(file, numMaterials, 'uint32'); %leave unchanged'''
'''fwrite(file, slice(:,:,z));  #fwrite(fileID,A) write the elements of array A as 8-bit unsigned integers to a binary file in column order. '''
*/


QFile f(file);
if( f.open( QFile::WriteOnly ) ){
    QDataStream out(&f);
    out <<"#OpenFab3DP V1.0 Binary";
    for (int i=0; i<this->gridSize.length();i++){
        //out<<this->gridSize.at(i);
    }

    for (int i=0; i<this->bboxSize.length();i++){
        out<<this->bboxSize.at(i);
    }
    out<<this->num_mat;

    for(int z=0; z<this->gridSize.at(2); z++){
        for(int y=0; z<this->gridSize.at(1); y++){
            for(int x=0; x<this->gridSize.at(0); y++){
                out<<this->grid[z][y][x];
            }
        }
    }
    f.close();
}

}


long qbytearrayToLong(QByteArray buf){
    long  l =  (unsigned char)buf.at(0) |
              ( (unsigned char)buf.at(1) << 8L)  |
              ( (unsigned char)buf.at(2) << 16L) |
              ( (unsigned char)buf.at(3) << 24L);
    return l;
}
