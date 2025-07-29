#include <QtTest>
#include <QObject>

class MyTest : public QObject {
    Q_OBJECT
private slots:
    void test_example() {
        QVERIFY(1 + 1 == 2);
    }
};

QTEST_MAIN(MyTest)
#include "test_main.moc"