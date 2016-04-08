#include <QCoreApplication>
#include <o3dpfile.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    O3DPFile f("Fingers.o3dp");
    f.readAll();
    f.write("test.o3dp");
    return a.exec();
}
