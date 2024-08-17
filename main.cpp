//
//  !!! Dit bestand moet je in principe NIET wijzigen !!!
//

#include  <QApplication>
#include  "src/SchaakGUI.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SchaakGUI w;
    w.show();

    return a.exec();
}
