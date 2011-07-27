#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>

template <class T>
class BlockingQueue
{

private:
    QQueue<T> queue;
    QMutex mutex;
    QWaitCondition notEmpty;

public:

    /* constructor */
    inline BlockingQueue() {}

    /* destructor */
    inline ~BlockingQueue() {}

    inline void enqueue(const T &t)
    {
        // entrar RC
        QMutexLocker locker(&mutex);
        // adiciona à queue
        queue.enqueue(t);
        // notify
        notEmpty.wakeAll();
    }

    inline T dequeue()
    {
        T t;

        // entrar RC
        mutex.lock();
        // verificar elementos
        if(queue.empty())
            notEmpty.wait(&mutex);

        t = queue.dequeue();

        // sair RC
        mutex.unlock();

       return t;
    }

    inline bool isEmpty()
    {
        QMutexLocker locker(&mutex);
        return queue.empty();
    }

};

#endif // BLOCKINGQUEUE_H
