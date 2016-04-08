#include "o3dpfile.h"
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QDebug>
#include <qendian.h>

#include <iostream>
#include <fstream>

O3DPFile::O3DPFile(QString file,QObject *parent) : QObject(parent)
{
    voxel_dimension = QVector<float>(3,0.0);
    gridSize = QVector<long>(3,0.0);
    bboxSize = QVector<double>(6,0.0);
    num_mat = 0;
    f = new QFile(file);
    grid = QVector<QByteArray>();
    readHeader();

}

O3DPFile::O3DPFile(QString file, QVector<long> gridsize,QVector<double> bbox, QObject *parent) : QObject(parent)
{
    voxel_dimension = QVector<float>(3,0.0);
    gridSize = gridsize;
    bboxSize = bbox;
    num_mat = 0;
    f = new QFile(file);
    grid = QVector<QByteArray>(gridsize[2]);
    readHeader();

}


void O3DPFile::readHeader(){
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

    if (f->open(QIODevice::ReadOnly) ){
        QByteArray cookarray = f->read(cookie.length());
        QString readcookie = QString::fromStdString(cookarray.toStdString());
        qDebug()<<"read cookie: "<<readcookie;

        QByteArray temp;
        // load int32 grid_val
        for(int i=0;i<n_grid_size_units;i++){
            temp = f->read(grid_size_unit);
            long grid_val = *(long*)temp.data();
            this->gridSize[i]=grid_val;
        }
        // load float64
        for(int i=0;i<n_bbox_units;i++){
            temp = f->read(bbox_unit);
            double bbox_val;
            bbox_val = *(double*)temp.data();
            this->bboxSize[i]=bbox_val;
        }
        // load num mat
        temp = f->read(n_mat_len);
        num_mat = *(int*)temp.data();

        qDebug()<<"Gridsize: "<<this->gridSize;
        qDebug()<<"bbox:" <<this->bboxSize;
        qDebug()<<"nmat:"<< this->num_mat;

        for(int i=0;i<3;i++){
            voxel_dimension[0+i] = (bboxSize[3+i]-bboxSize[0+i])/gridSize[i];
        }
        f->close();
    }
    nheaderbytes = headercount;

}


bool O3DPFile::setLayer(int layer_index,QByteArray layer){
    if (layer.length() != gridSize[0]*gridSize[1]){return false;}

    if (grid.length()>layer_index){
        grid[layer_index] = layer;
        return true;
    }
    return false;
}


QByteArray O3DPFile::layer(int layer){
    // if a layer is in memory return it
    if (grid.length()>layer){return grid[layer];}

    // else load the layer and return it
    if (f->open(QIODevice::ReadOnly) ){
        f->seek(nheaderbytes);
        f->seek(layer*gridSize[1]*gridSize[0]);
        QByteArray bytelayer( gridSize[1]*gridSize[0],0);
        bytelayer = f->read( gridSize[1]*gridSize[0] );
        f->close();
        return bytelayer;
    }
    return QByteArray();
}

void O3DPFile::readAll(){
    readHeader();
    if (f->open(QIODevice::ReadOnly) ){
        f->seek(nheaderbytes);
        grid = QVector<QByteArray>(gridSize[2]);

        for(int z=0; z<this->gridSize[2]; z++){
            qDebug()<<"z:"<<z;
            QByteArray layer( gridSize[1]*gridSize[0],0);
            layer = f->read( gridSize[1]*gridSize[0] );
            grid[z] = layer;
        }

        qDebug()<<"Read";
        f->close();
    }


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


QFile fw(file);
if( fw.open( QFile::WriteOnly ) ){

    int grid_size_unit = 4;
    int bbox_unit = 8;
    int n_mat_len = 4;

    QString cookie("#OpenFab3DP V1.0 Binary");

    fw.write(cookie.toStdString().c_str(),cookie.length());
    for (int i=0; i<this->gridSize.length();i++){
        long Iarr[1] = { gridSize.at(i) };
        char *arr = (char*) Iarr;
        fw.write(arr,grid_size_unit);
    }

    qDebug()<<"wrote gridsize";
    for (int i=0; i<this->bboxSize.length();i++){
        double darr[1] = { bboxSize.at(i) };
        char *arr = (char*) darr;
        fw.write(arr,bbox_unit);
    }

    //mat
    uint32_t Iarr[1] = { num_mat };
    char *arr = (char*) Iarr;
    fw.write(arr,n_mat_len);

    qDebug()<<"wrote header";
    for(int z=0; z<this->gridSize.at(2); z++){
        //qDebug()<<"z";
        QByteArray layer = grid.at(z);
        fw.write(layer);
    }
    qDebug()<<"wrote grid";
    fw.close();
}

}

QByteArray invertArray(QByteArray array){
    QByteArray newarray(array.length(),0);
    for(int i=0; i<array.length(); i++){
        newarray[array.length()-i]= array.at(i);
    }
    return newarray;

}


long qbytearrayToLong(QByteArray buf){
    long  l =  (unsigned char)buf.at(0) |
              ( (unsigned char)buf.at(1) << 8L)  |
              ( (unsigned char)buf.at(2) << 16L) |
              ( (unsigned char)buf.at(3) << 24L);
    return l;
}
