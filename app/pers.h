#ifndef PERS_H
#define PERS_H
#include <QDateTime>
#include <QRandomGenerator>

struct Data {
    QDateTime date;
    double arr[4];
};

enum class Result { //возврат результата
    NONE,           //ничего не делать
    SHORT,
    LONG,
    CLOSE
};

class Pers { //особь
public:
    Pers();
    ~Pers();

    void persStart(QRandomGenerator *r); //старновый метод со случайными генами
    void persStart(QRandomGenerator *r, const Pers &pers); //старновый метод с заданными генами
    void mutation();                                       //мутация
    void cross(const Pers &pers);                       //скрещивание
    QString brainGet();                                 //получить мозг
    Result make(const QList<Data *> &data, int &param); //сделать выбор на основе данных
    uint getScore();                                    //получить баланс
    void upScoreLong(uint size, bool test);
    void downScoreLong(uint size, bool test);
    void upScoreShort(uint size, bool test);
    void downScoreShort(uint size, bool test);
    void reset();                 //перезапустить особь
    void setBrain(QStringList b); //редактор мозга
    void calcScore();             //посчитать баланс
    uint getDrop();

    static const int countBrain = 256; //количество значений в мозге

private:
    Result makePrivate(const QList<Data *> &data, int count, int &param);
    int getMark(int mark);
    void upScore(uint size);
    void downScore(uint size);

    const int sizeBrain = 256; //размер мозга
    const uint balance = 200;  //начальный баланс
    const uint maxDrop = 200;  //максимальная просадка
    const uint sizeFall = 50;  //количество ошибок подряд
    const uint minusScore = 1; //убавление балов
    const uint plusScore = 1;  //прибавление балов
    const uint tr = 1;         //комиссия
    const uint minUp = 5;      //минимальный прирост

    int mark = 0;                 //метка в мозге
    int *brain;                   //мозг
    uint score = balance;         //общей баланс
    uint scoreLong = balance;     //баланс на покупку
    uint scoreShort = balance;    //баланс на продажу
    uint maxScore = balance;      //максимальный баланс
    uint maxScoreLong = balance;  //максимальный баланс на покупку
    uint maxScoreShort = balance; //максимальный баланс на продажу
    uint fallLong = 0;            //ошибки подряд на покупку
    uint fallShort = 0;           //ошибки подряд на продажу
    uint drop = 0;                //просадка
    QRandomGenerator *random;
};

#endif // PERS_H
