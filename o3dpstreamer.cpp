#include "o3dpstreamer.h"
#include <QDebug>

O3DPStreamer::O3DPStreamer(QString file, QIODevice::OpenModeFlag flag, QObject *parent ):QObject(parent)
{

    voxel_dimension = QVector<float>(3,0.0);
    gridSize = QVector<long>(3,0.0);
    bboxSize = QVector<double>(6,0.0);
    num_mat = 0;
    Flag =flag;
    f = new QFile(file);
    if (flag == QIODevice::WriteOnly){flag = QIODevice::Append;}
    f->open(flag);
    grid = QVector<QByteArray>();
    readHeader();

}

O3DPStreamer::O3DPStreamer(QString file, QIODevice::OpenModeFlag flag, QVector<long> gridsize,QVector<double> bbox, QObject *parent):QObject(parent)
{
    voxel_dimension = QVector<float>(3,0.0);
    gridSize = gridsize;
    bboxSize = bbox;
    num_mat = 0;
    Flag =flag;
    f = new QFile(file);
    if (flag == QIODevice::WriteOnly){flag = QIODevice::Append;}
    f->open(flag);
    grid = QVector<QByteArray>(gridsize[2]);
}

bool O3DPStreamer::open(QIODevice::OpenModeFlag flag){
    Flag =flag;
    if (flag == QIODevice::WriteOnly){flag = QIODevice::Append;}
    return f->open(flag);
}
void O3DPStreamer::close(){
    f->close();
}


void O3DPStreamer::readHeader(){
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

    if (Flag == QIODevice::ReadOnly && f->isOpen()) {
        f->seek(0);
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
    }
    nheaderbytes = headercount;
}
QByteArray O3DPStreamer::readLayer(int layer){
    if ( (gridSize[2]<layer) || (layer<0)){return QByteArray();}

    // else load the layer and return it
    if (Flag ==QIODevice::ReadOnly && f->isOpen()) {
        //f->seek(nheaderbytes+layer*gridSize[1]*gridSize[0]);
        QByteArray bytelayer( gridSize[1]*gridSize[0],0);
        bytelayer = f->read( gridSize[1]*gridSize[0] );
        return bytelayer;
    }
    return QByteArray();
}

bool O3DPStreamer::writeHeader(){
    bool worked=false;
    if( Flag == QIODevice::WriteOnly && f->isOpen()){

        int grid_size_unit = 4;
        int bbox_unit = 8;
        int n_mat_len = 4;

        QString cookie("#OpenFab3DP V1.0 Binary");

        f->write(cookie.toStdString().c_str(),cookie.length());
        for (int i=0; i<this->gridSize.length();i++){
            long Iarr[1] = { gridSize.at(i) };
            char *arr = (char*) Iarr;
            f->write(arr,grid_size_unit);
        }

        qDebug()<<"wrote gridsize";
        for (int i=0; i<this->bboxSize.length();i++){
            double darr[1] = { bboxSize.at(i) };
            char *arr = (char*) darr;
            f->write(arr,bbox_unit);
        }

        //mat
        uint32_t Iarr[1] = { num_mat };
        char *arr = (char*) Iarr;
        worked = f->write(arr,n_mat_len)>0;
    }
    return worked;
}
bool O3DPStreamer::writeLayer(QByteArray Layer){
    if( Flag == QIODevice::WriteOnly  && f->isOpen()){
        bool worked = f->write(Layer)==Layer.length();
        return worked;
    }
    return false;
}
