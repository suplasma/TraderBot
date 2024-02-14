#include "pers.h"

#include <QDateTime>
#include <QDebug>

Pers::Pers() {}

Pers::~Pers()
{
    delete brain;
}

void Pers::persStart(QRandomGenerator *r)
{
    random = r;
    brain = new int[sizeBrain];

    for (int i = 0; i < sizeBrain; ++i) {
        brain[i] = random->generate() % countBrain;
    }
}

void Pers::persStart(QRandomGenerator *r, const Pers &pers)
{
    random = r;
    brain = new int[sizeBrain];

    for (int i = 0; i < sizeBrain; ++i) {
        brain[i] = pers.brain[i];
    }
}

void Pers::mutation()
{
    for (int i = 0; i < sizeBrain; ++i)
        if (random->generate() % 100 < 14)
            brain[i] = random->generate() % countBrain;
}

void Pers::cross(const Pers &pers)
{
    for (int i = 0; i < sizeBrain; ++i)
        if (random->generate() % 2 == 0)
            brain[i] = pers.brain[i];
}

QString Pers::brainGet()
{
    QString str;
    for (int i = 0; i < sizeBrain; ++i)
        str += QString::number(brain[i]) + " ";
    str += "\n";
    return str;
}

Result Pers::make(const QList<Data *> &data, int &param)
{
    mark = 0;
    return makePrivate(data, 0, param);
}

uint Pers::getScore()
{
    return score;
}

void Pers::upScore(uint size)
{
    if (score > 0) {
        score += plusScore * size - tr;
    }
    if (maxScore < score)
        maxScore = score;
}

void Pers::downScore(uint size)
{
    if (score > (minusScore * size + tr)) {
        score -= minusScore * size + tr;
    } else
        score = 0;

    if (drop < (maxScore - score))
        drop = maxScore - score;
}

void Pers::upScoreLong(uint size, bool test)
{
    if (test) {
        upScore(size);
        return;
    }
    if (size < minUp)
        ++fallLong;
    else
        fallLong = 0;
    if (scoreLong != 0)
        scoreLong += plusScore * size - tr;
    if (maxScoreLong < scoreLong)
        maxScoreLong = scoreLong;
}

void Pers::downScoreLong(uint size, bool test)
{
    if (test) {
        downScore(size);
        return;
    }
    ++fallLong;
    if (scoreLong > (minusScore * size + tr) && fallLong < sizeFall &&
        maxScoreLong - maxDrop <= scoreLong) {
        scoreLong -= minusScore * size + tr;
    } else {
        scoreLong = 0;
    }
}

void Pers::upScoreShort(uint size, bool test)
{
    if (test) {
        upScore(size);
        return;
    }
    if (size < minUp)
        ++fallShort;
    else
        fallShort = 0;
    if (scoreShort != 0)
        scoreShort += plusScore * size - tr;

    if (maxScoreShort < scoreShort)
        maxScoreShort = scoreShort;
}

void Pers::downScoreShort(uint size, bool test)
{
    if (test) {
        downScore(size);
        return;
    }
    ++fallShort;
    if (scoreShort > (minusScore * size + tr) && fallShort < sizeFall &&
        maxScoreShort - maxDrop <= scoreShort) {
        scoreShort -= minusScore * size + tr;
    } else {
        scoreShort = 0;
    }
}

void Pers::reset()
{
    score = balance;
    scoreShort = balance;
    scoreLong = balance;
    maxScoreLong = balance;
    maxScoreShort = balance;
    maxScore = balance;
    fallLong = 0;
    fallShort = 0;
    drop = 0;
}

void Pers::setBrain(QStringList b)
{
    for (int i = 0; i < b.size(); ++i)
        brain[i] = b.value(i).toInt();
}

void Pers::calcScore()
{
    if (scoreLong > balance && scoreShort > balance) {
        score = scoreLong + scoreShort - (2 * balance);
    } else
        score = 0;
}

uint Pers::getDrop()
{
    return drop;
}

Result Pers::makePrivate(const QList<Data *> &data, int count, int &param)
{
    if (count == sizeBrain) {
        return Result::NONE;
    }

    switch (brain[mark]) {
        case 0: //переход в заданное место
            mark = getMark(brain[getMark(mark + 1)]);
            return makePrivate(data, count + 1, param);
            break;

        case 1: //безусловный переход
            mark = getMark(mark + brain[getMark(mark + 1)]);
            return makePrivate(data, count + 1, param);
            break;

        case 2: //сравнение двух заданных свеч
            data.value(brain[getMark(mark + 1)])->arr[0] <
                    data.value(brain[getMark(mark + 2)])->arr[0]
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);

            return makePrivate(data, count + 1, param);
            break;

        case 3:
            data.value(brain[getMark(mark + 1)])->arr[1] <
                    data.value(brain[getMark(mark + 2)])->arr[1]
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);

            return makePrivate(data, count + 1, param);
            break;

        case 4:
            data.value(brain[getMark(mark + 1)])->arr[2] <
                    data.value(brain[getMark(mark + 2)])->arr[2]
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);

            return makePrivate(data, count + 1, param);
            break;

        case 5:
            data.value(brain[getMark(mark + 1)])->arr[3] <
                    data.value(brain[getMark(mark + 2)])->arr[3]
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);

            return makePrivate(data, count + 1, param);
            break;

        case 6: //проверка свечи
            data.value(brain[getMark(mark + 1)])->arr[0] <
                    data.value(brain[getMark(mark + 1)])->arr[3]
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);

            return makePrivate(data, count + 1, param);
            break;

        case 7: //любая проверка двух свеч
            data.value(brain[getMark(mark + 1)])->arr[brain[getMark(mark + 3)] % 4] <
                    data.value(brain[getMark(mark + 2)])->arr[brain[getMark(mark + 4)] % 4]
                ? mark = getMark(brain[getMark(mark + 5)])
                : mark = getMark(brain[getMark(mark + 6)]);

            return makePrivate(data, count + 1, param);
            break;

        case 8: //проверка свечи в заданном диапозоне
            data.value(brain[getMark(mark + 1)])->arr[brain[getMark(mark + 2)] % 4] <
                    1. + ((double) brain[getMark(mark + 3)] / 50)
                ? mark = getMark(brain[getMark(mark + 4)])
                : mark = getMark(brain[getMark(mark + 5)]);

            return makePrivate(data, count + 1, param);
            break;

        case 9: param = brain[getMark(mark + 1)]; return Result::LONG;

        case 10: return Result::CLOSE;

        case 11: param = brain[getMark(mark + 1)]; return Result::SHORT;

        case 12: //сравнение дня недели
            data.value(brain[getMark(mark + 1)])->date.date().dayOfWeek() ==
                    (brain[getMark(mark + 2)] % 7 + 1)
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);
            return makePrivate(data, count + 1, param);
            break;

        case 13: //получение дня недели
            mark = getMark(brain[getMark(
                mark + data.value(brain[getMark(mark + 1)])->date.date().dayOfWeek() + 1)]);
            return makePrivate(data, count + 1, param);
            break;

        case 14: //сравнение часа
            data.value(brain[getMark(mark + 1)])->date.time().hour() ==
                    (brain[getMark(mark + 2)] % 24)
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);
            return makePrivate(data, count + 1, param);
            break;

        case 15: //сравнение минут
            data.value(brain[getMark(mark + 1)])->date.time().minute() ==
                    (brain[getMark(mark + 2)] % 60)
                ? mark = getMark(brain[getMark(mark + 3)])
                : mark = getMark(brain[getMark(mark + 4)]);
            return makePrivate(data, count + 1, param);
            break;

        default: return Result::NONE;
    }

    qCritical() << Q_FUNC_INFO;
    throw this;
}

int Pers::getMark(int mark)
{
    return mark % sizeBrain;
}
