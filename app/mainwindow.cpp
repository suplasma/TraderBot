#include "mainwindow.h"

#include <QDebug>
#include <QFile>

#include <iostream>
#include <thread>

#include <math.h>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->qcustomplot->setInteraction(QCP::iRangeDrag, true);
    ui->qcustomplot->setInteraction(QCP::iRangeZoom, true);

    readData();

    random = new QRandomGenerator();
    random->seed(QDateTime::currentDateTime().toMSecsSinceEpoch());

    for (int i = persList.size(); i < startPers; ++i) {
        Pers *pers = new Pers();
        pers->persStart(random);
        persList.append(pers);
    }

    readSave();
    on_pushButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (flag) {
        flag = false;
        ui->pushButton->setEnabled(false);
    } else {
        flag = true;
        ui->pushButton->setText("Сохранить");
        paintGraph();
        std::thread th([&]() {
            while (flag) {
                QString str;
                uint score = 0;
                uint max = 0;
                for (int numPers = 0; numPers < startPers; ++numPers) {
                    int raz = 0;
                    int make = 0;
                    int param;
                    double startVal = 0;
                    int rand = random->generate() %
                               (170000 /*dataList.size() - Pers::countBrain - 10000*/);
                    for (int i = rand; i < rand + 30000; ++i) {
                        Result res = persList.value(numPers)->make(
                            dataList.mid(i, Pers::countBrain), param);
                        double price = make != 0
                                           ? startVal
                                           : dataList.value(i + Pers::countBrain - 1)->arr[3];
                        raz = (dataList.value(i + Pers::countBrain)->arr[3] - price) * valRate;
                        switch (res) {
                            case Result::LONG: {
                                bargainLong(persList.value(numPers), make, raz, startVal,
                                            dataList.value(i + Pers::countBrain - 1)->arr[3],
                                            false);
                                break;
                            }

                            case Result::SHORT: {
                                bargainShort(persList.value(numPers), make, raz, startVal,
                                             dataList.value(i + Pers::countBrain - 1)->arr[3],
                                             false);
                                break;
                            }

                            case Result::CLOSE: {
                                bargainClose(persList.value(numPers), make, raz, false);
                                break;
                            }

                            case Result::NONE: {
                                break;
                            }
                        }
                    }
                    //                    bargainClose(persList.value(numPers), make, raz,
                    //                    false);

                    persList.value(numPers)->calcScore();
                    score += persList.value(numPers)->getScore();
                    str += QString::number(persList.value(numPers)->getScore()) + " ";

                    if (max < persList.value(numPers)->getScore())
                        max = persList.value(numPers)->getScore();
                }

                qDebug() << str << "\nscore" << score << "\nmax" << max << endl;
                if (score == 0)
                    exit(0);

                newPers(score);
                selection();

                for (int numPers = 0; numPers < endPers; ++numPers) {
                    persList.value(numPers)->reset();
                }

                for (int numPers = 0; numPers < (startPers - endPers); ++numPers) {
                    persList.append(persListCopy.takeFirst());
                }
            }
            writeSave();
            test();
            paintTest(mMinX, mMaxX, mMinY, mMaxY);
            ui->pushButton->setText("Продолжить");
            ui->pushButton->setEnabled(true);
        });

        th.detach();
    }
}

void MainWindow::readData()
{
    int x = 0;

    QFile file("../data/data.txt");
    file.open(QFile::ReadOnly);
    file.readLine();
    while (!file.atEnd()) {
        QStringList list = QString(file.readLine()).split(",");
        Data *data = new Data();

        data->date = QDateTime::fromString(list.value(2) + list.value(3), "yyyyMMddhhmmss");
        data->arr[0] = list.value(4).toDouble();
        data->arr[1] = list.value(5).toDouble();
        data->arr[2] = list.value(6).toDouble();
        data->arr[3] = list.value(7).toDouble();

        dataList.append(data);

        if (graph->x.isEmpty()) {
            graph->x.append(x++);
            graph->y.append(data->arr[0]);
            graph->max = data->arr[0];
            graph->min = data->arr[0];
        }
        graph->x.append(x++);
        graph->y.append(data->arr[3]);

        if (graph->max < data->arr[3])
            graph->max = data->arr[3];

        if (graph->min > data->arr[3])
            graph->min = data->arr[3];
    }
    file.close();

    qDebug() << dataList.size();

    x = 0;

    QFile fileTest("../data/dataTest.txt");
    fileTest.open(QFile::ReadOnly);
    fileTest.readLine();
    while (!fileTest.atEnd()) {
        QStringList list = QString(fileTest.readLine()).split(",");
        Data *data = new Data();

        data->date = QDateTime::fromString(list.value(2) + list.value(3), "yyyyMMddhhmmss");
        data->arr[0] = list.value(4).toDouble();
        data->arr[1] = list.value(5).toDouble();
        data->arr[2] = list.value(6).toDouble();
        data->arr[3] = list.value(7).toDouble();

        testDataList.append(data);

        if (grTest->x.isEmpty()) {
            grTest->x.append(x++);
            grTest->y.append(data->arr[0]);
            grTest->max = data->arr[0];
            grTest->min = data->arr[0];
        }
        grTest->x.append(x++);
        grTest->y.append(data->arr[3]);

        if (grTest->max < data->arr[3])
            grTest->max = data->arr[3];

        if (grTest->min > data->arr[3])
            grTest->min = data->arr[3];
    }

    fileTest.close();
    qDebug() << testDataList.size();
}

void MainWindow::readSave()
{
    QFile file("../data/save.txt");
    if (file.open(QFile::ReadOnly)) {
        int row = 0;
        while (!file.atEnd()) {
            persList.value(row)->setBrain(QString(file.readLine()).split(" "));
            ++row;
        }
        file.close();
    }
}

void MainWindow::writeSave()
{
    QFile file("../data/save.txt");
    file.open(QFile::WriteOnly);
    for (int numPers = 0; numPers < startPers; ++numPers)
        file.write(persList.value(numPers)->brainGet().toLatin1());
    file.close();
}

void MainWindow::test()
{
    while (graphList.size() > 0)
        delete graphList.takeLast();
    mMaxX = mMaxY = INT_MIN;
    mMinX = mMinY = INT_MAX;

    uint max = 0;

    for (int numPers = 0; numPers < endPers; ++numPers) {
        makeTest(persList.value(numPers), true);
        qDebug() << QString("unit %1 score: %2 max drop: %3")
                        .arg(numPers + 1)
                        .arg(persList.value(numPers)->getScore())
                        .arg(persList.value(numPers)->getDrop());
        if (max < persList.value(numPers)->getScore())
            max = persList.value(numPers)->getScore();
        persList.value(numPers)->reset();
    }

    qDebug() << "max score:" << max << endl;
}

void MainWindow::newPers(int score)
{
    if (score == 0) {
        for (int i = 0; i < (startPers - endPers); ++i) {
            Pers *pers = new Pers();
            pers->persStart(random);
            persListCopy.append(pers);
        }
        return;
    }

    for (int i = 0; i < (startPers - endPers); ++i) {
        int rand = random->generate() % score;
        int num = 0;
        while (rand >= 0) {
            rand -= persList.value(num)->getScore();
            ++num;
        }

        int rand2 = random->generate() % score;
        int num2 = 0;
        while (rand2 >= 0) {
            rand2 -= persList.value(num2)->getScore();
            ++num2;
        }
        Pers *pers = new Pers();
        pers->persStart(random, *persList.value(num - 1));
        pers->cross(*persList.value(num2 - 1));
        pers->mutation();
        persListCopy.append(pers);
    }
}

void MainWindow::selection()
{
    for (int i = 0; i < (startPers - endPers); ++i) {
        uint min = persList.first()->getScore();
        int mini = 0;
        for (int num = 1; num < startPers - i; ++num) {
            if (min > persList.value(num)->getScore()) {
                min = persList.value(num)->getScore();
                mini = num;
            }
        }
        delete persList.takeAt(mini);
    }
}

void MainWindow::makeTest(Pers *pers, bool test)
{
    int param;
    int make = 0;
    double startVal = 0;
    Graph *gr = new Graph();
    gr->x.append(0);
    gr->y.append(pers->getScore());
    for (int i = 0; i < testDataList.size() - Pers::countBrain; ++i) {
        Result res = pers->make(testDataList.mid(i, Pers::countBrain), param);
        double price =
            make != 0 ? startVal : testDataList.value(i + Pers::countBrain - 1)->arr[3];
        int raz = (testDataList.value(i + Pers::countBrain)->arr[3] - price) * valRate;
        switch (res) {
            case Result::LONG: {
                bargainLong(pers, make, raz, startVal,
                            testDataList.value(i + Pers::countBrain - 1)->arr[3], test);
                break;
            }

            case Result::SHORT: {
                bargainShort(pers, make, raz, startVal,
                             testDataList.value(i + Pers::countBrain - 1)->arr[3], test);
                break;
            }
            case Result::CLOSE: {
                bargainClose(pers, make, raz, test);
                break;
            }
            case Result::NONE: {
                break;
            }
        }
        if (!test)
            pers->calcScore();
        gr->x.append(i - 1);
        gr->y.append(pers->getScore());
        if (mMinY > pers->getScore()) {
            mMinY = pers->getScore();
        }
        if (mMaxY < pers->getScore()) {
            mMaxY = pers->getScore();
        }
        if (mMinX > i)
            mMinX = i;
        if (mMaxX < i)
            mMaxX = i;
    }

    graphList.append(gr);
}

void MainWindow::paintTest(double minX, double maxX, double minY, double maxY)
{
    ui->qcustomplot->clearGraphs();
    for (int i = 0; i < graphList.size(); ++i) {
        ui->qcustomplot->addGraph();
        ui->qcustomplot->graph(i)->setData(graphList.value(i)->x, graphList.value(i)->y);
        ui->qcustomplot->graph(i)->setPen(QPen(QColor(0, 0, 255, 127), 1.0));
    }

    ui->qcustomplot->addGraph();
    QVector<double> y;

    for (int i = 0; i < grTest->y.size(); ++i) {
        y.append((grTest->y.at(i) - grTest->min) / (grTest->max - grTest->min) * (maxY - minY) +
                 minY);
    }

    ui->qcustomplot->graph(0)->setData(grTest->x, y);
    ui->qcustomplot->graph(0)->setPen(QPen(QColor(0, 255, 0, 255), 1.0));

    ui->qcustomplot->xAxis->setRange(minX, maxX);
    ui->qcustomplot->yAxis->setRange(minY, maxY);

    ui->qcustomplot->replot();
    qDebug() << endl << "max:" << maxY << "min:" << minY << endl;
}

void MainWindow::paintGraph()
{
    ui->qcustomplot->clearGraphs();

    ui->qcustomplot->xAxis->setRange(0, graph->x.size());
    ui->qcustomplot->yAxis->setRange(graph->min, graph->max);

    ui->qcustomplot->addGraph();

    ui->qcustomplot->graph(0)->setData(graph->x, graph->y);
    ui->qcustomplot->graph(0)->setPen(QPen(QColor(0, 255, 0, 127), 1.0));

    ui->qcustomplot->replot();
}

void MainWindow::bargainClose(Pers *pers, int &make, int raz, bool test)
{
    if (make == 1) {
        if (raz > sprad)
            pers->upScoreLong(raz - sprad, test);
        else
            pers->downScoreLong(abs(raz) + sprad, test);
    } else if (make == -1) {
        if (raz < -sprad)
            pers->upScoreShort(-raz - sprad, test);
        else
            pers->downScoreShort(abs(raz) + sprad, test);
    }

    make = 0;
}

void MainWindow::bargainLong(Pers *pers, int &make, int raz, double &startVal, double val,
                             bool test)
{
    if (make == 0) {
        make = 1;
        startVal = val;
    } else if (make == -1) {
        if (raz < -sprad)
            pers->upScoreShort(-raz - sprad, test);
        else
            pers->downScoreShort(abs(raz) + sprad, test);

        make = 1;
        startVal = val;
    }
}

void MainWindow::bargainShort(Pers *pers, int &make, int raz, double &startVal, double val,
                              bool test)
{
    if (make == 0) {
        make = -1;
        startVal = val;
    } else if (make == 1) {
        if (raz > sprad)
            pers->upScoreLong(raz - sprad, test);
        else
            pers->downScoreLong(abs(raz) + sprad, test);

        make = -1;
        startVal = val;
    }
}
