#include "mergefiles.h"

void mergeFiles(QString XMLFile){
    QVector<fileWithLocation> files;
    QString outname;
    QVector<long> outGridSize;
    QVector<double> outBBox;

/// Parse XML into
///  - fileWithLocations
///  - a fixed gridsize

    O3DPFile OutFile(outname,outGridSize,outBBox);

    for( int z=0; z<outGridSize[2]; z++){
        QByteArray layer( outGridSize[1]*outGridSize[0],0);
        for(int f=0; f<files.length();f++){
            int file_layer_number = z-files.at(f).indices[2];
            // if the file is in this layer then add the values
            if (file_layer_number>0 && file_layer_number<files.at(f).o3dp.gridSize[2]){
                // get the layer
                QByteArray fileLayer = files.at(f).o3dp.layer(file_layer_number);

                // for each row write out
                for (int y=0;y<files.at(f).o3dp.gridSize[1];y++){
                    ///  *-------->
                    ///  |   |
                    ///  |---x
                    /// \|/
                    int indexInLayer = (y+files.at(f).indices[1])*outGridSize[0]+
                                       files.at(f).indices[0];
                    int len = files.at(f).o3dp.gridSize[0];
                    QByteArray col = fileLayer.mid(y*len,len);
                    layer.replace(indexInLayer,len,col);
                }
            }
        }
        OutFile.grid[z] = layer;
    }

    OutFile.write(outname);
}
