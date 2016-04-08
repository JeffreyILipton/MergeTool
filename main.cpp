#include <QCoreApplication>
#include <o3dpfile.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    O3DPFile f;
    f.read("fingertip-solid.o3dp");
    f.write("test.o3dp");
    return a.exec();
}
