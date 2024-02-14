#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRandomGenerator>

#include <pers.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    struct Graph {
        QVector<double> x;
        QVector<double> y;
        double max;
        double min;
    };
    void readData();                      //прочитать данные
    void readSave();                      //загрузить популяцию
    void writeSave();                     //сохранить популяцию
    void test();                          //тесты
    void newPers(int score);              //новая популяция
    void selection();                     //отбор
    void makeTest(Pers *pers, bool test); //тестовый прогон
    void paintTest(double minX, double maxX, double minY, double maxY); //рисование графиков
    void paintGraph();
    void bargainClose(Pers *pers, int &make, int raz, bool test);
    void bargainLong(Pers *pers, int &make, int raz, double &startVal, double val, bool test);
    void bargainShort(Pers *pers, int &make, int raz, double &startVal, double val, bool test);

    const int startPers = 64; //начальное количество особей
    const int endPers = 8;    //конечное количество особей
    const int resSize = 1; //коэфициент возвращаемого результата
    const int valRate = 10000; //коэфициент данных
    const int sprad = 1;       //спрэд

    Ui::MainWindow *ui;
    QList<Data *> dataList;                //данные для обучения
    QList<Data *> testDataList;            //данные для теста
    QList<Pers *> persList;                //основной список
    QList<Pers *> persListCopy;            //временный список
    QList<Graph *> graphList;              //графики
    QList<QPair<double, double>> minYList; //минимумы
    QList<QPair<double, double>> maxYList; //максимумы
    QRandomGenerator *random;              //рандомайзер
    Graph *grTest = new Graph();
    Graph *graph = new Graph();
    bool flag = false;
    double mMinX;
    double mMaxX;
    double mMinY;
    double mMaxY;
};

#endif // MAINWINDOW_H
