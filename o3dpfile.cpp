#include "o3dpfile.h"
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QDebug>

O3DPFile::O3DPFile(QObject *parent) : QObject(parent)
{
    voxel_dimension = QVector<float>(3,0.0);
    gridSize = QVector<qint32>(3,0.0);
    bboxSize = QVector<double>(6,0.0);
    num_mat = 0;
}


void O3DPFile::read(QString file){
    QString cookie = "#OpenFab3DP V1.0 Binary";
    int headercount = cookie.length();
    int n_grid_size_units = 3;
    int n_bbox_units = 6;
    int n_mat_len = 4;

    headercount += n_grid_size_units*4;
    headercount += n_bbox_units*8;
    headercount += n_mat_len;

    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QDataStream in(&f);
    QString readcookie;
    in>>readcookie;
    qDebug()<<"read cookie: "<<readcookie;

    // load int32 grid_val
    for(int i=0;i<n_grid_size_units;i++){
        qint32 grid_val;
        in>>grid_val;
        this->gridSize[i]=grid_val;
    }
    // load float64
    for(int i=0;i<n_bbox_units;i++){
        double bbox_val;
        in>>bbox_val;
        this->bboxSize[i]=bbox_val;
    }
    // load num mat
    in>>num_mat;

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
                in>>grid[z][y][x];
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
        out<<this->gridSize.at(i);
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
