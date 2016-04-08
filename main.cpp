#include <QCoreApplication>
#include <o3dpfile.h>
#include <QDebug>
#include <mergefiles.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    O3DPFile f("fingertip-solid.o3dp");
//    f.readHeader();
//    QByteArray b = f.layer(120);
//    qDebug()<< b.length();
    //f.readAll();
    //f.write("test.o3dp");

    mergeFiles("process.xml");
    return a.exec();
}
