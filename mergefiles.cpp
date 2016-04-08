#include "mergefiles.h"
#include <QXmlStreamReader>
#include <QMap>
#include <QDebug>
#include <algorithm>

void mergeFiles(QString XMLFile){
    QString outname;
    QMap<int,O3DPFile*> o3dpfileMap;
    QVector<fileWithLocation> files;
    QVector<long> outGridSize(3,0);
    QVector<float> voxDims(3,0);
    QVector<double> outBBox(6,0);
    quint32 nmat = 0;

/// Parse XML into
///  - fileWithLocations
///  - a fixed gridsize
///  - bbox
///  - outfilename

    QFile f(XMLFile);
    f.open(QIODevice::ReadOnly);



    QXmlStreamReader xml(&f);
    while(!xml.atEnd() && !xml.hasError()){
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument){
            //we don’t want any of this data, it isn’t any element
            //we need.
            continue;
        }

        if(token == QXmlStreamReader::StartElement) {
            if(xml.name() == "files"){
                continue;
            }
            if(xml.name() == "filename"){
                outname = xml.readElementText();
                qDebug()<<"outname: "<<outname;
            }
            if(xml.name() == "inputfile"){
                int id = xml.attributes().value("id").toInt();
                QString filename = xml.readElementText();
                O3DPFile* input = new O3DPFile(filename);
                o3dpfileMap[id] = input;
            }
            if (xml.name() == "insertfile"){
                int id = xml.attributes().value("id").toInt();
                QVector<int> indecies(3,0.0);
                indecies[0] = xml.attributes().value("x").toFloat();
                indecies[1] = xml.attributes().value("y").toFloat();
                indecies[2] = xml.attributes().value("z").toFloat();

                fileWithLocation loc;
                loc.o3dp = o3dpfileMap[id];
                loc.indices = indecies;
                nmat = std::max(nmat,loc.o3dp->num_mat);

                files.append(loc);
            }
            if(xml.name() == "gridsize"){
                outGridSize[0] = xml.attributes().value("x").toLong();
                outGridSize[1] = xml.attributes().value("y").toLong();
                outGridSize[2] = xml.attributes().value("z").toLong();
            }
            if(xml.name() == "voxeldims"){
                voxDims[0] = xml.attributes().value("x").toFloat();
                voxDims[1] = xml.attributes().value("y").toFloat();
                voxDims[2] = xml.attributes().value("z").toFloat();
            }
        }
    }
    if (xml.hasError()){qDebug()<<"Error:"<<xml.error();}

    for(int i=0;i<3;i++){
        outBBox[3+i] = outGridSize[i]*voxDims[i];
    }



    O3DPFile OutFile(outname,outGridSize,outBBox);
    OutFile.num_mat=nmat;
    OutFile.writeHeader();
    for( int z=0; z<outGridSize[2]; z++){
        QByteArray layer( outGridSize[1]*outGridSize[0],0);

        for(int f=0; f<files.length();f++){
            int file_layer_number = z-files.at(f).indices[2];
            // if the file is in this layer then add the values
            if( (file_layer_number > -1) &&
                (file_layer_number < files.at(f).o3dp->gridSize[2]) ){
                // get the layer
                QByteArray fileLayer = files.at(f).o3dp->layer(file_layer_number);

                // for each row write out
                for (int y=0;y<files.at(f).o3dp->gridSize[1];y++){
                    ///  *-------->
                    ///  |   |
                    ///  |---x
                    /// \|/
                    int indexInLayer = (y+files.at(f).indices[1])*outGridSize[0]+
                                       files.at(f).indices[0];
                    int len = files.at(f).o3dp->gridSize[0];
                    QByteArray col = fileLayer.mid(y*len,len);
                    layer.replace(indexInLayer,len,col);
                }
            }
        }
        //OutFile.grid[z] = layer;
        qDebug()<<"layer:"<<z<<" : "<<OutFile.writeLayer(layer);
    }

    //qDebug()<<"generated";
    //OutFile.writeAll(outname);
    qDebug()<<"done writing";
}



