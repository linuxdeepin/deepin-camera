/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */


#ifndef MajorImageProcessingThread_H
#define MajorImageProcessingThread_H

#include <QThread>
#include <QImage>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

#ifdef __cplusplus
extern "C" {
#endif
#include "LPF_V4L2.h"
#include "stdlib.h"
#include "malloc.h"
#include "stdio.h"
#include "math.h"

#ifdef __cplusplus
}
#endif
class MajorImageProcessingThread : public QThread
{
    Q_OBJECT
public:
    MajorImageProcessingThread();

    QImage majorImage;
    void stop();
    void init();

protected:
    void run();

private:
    volatile int majorindex;
    volatile bool stopped;

signals:
    void SendMajorImageProcessing(QImage image, int result);

};

#endif // MajorImageProcessingThread_H
